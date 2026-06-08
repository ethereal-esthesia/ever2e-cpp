/*************************************************************************
 *                                                                       *
 *   Copyright (C) 2012 Shane Reilly                                     *
 *                                                                       *
 *   This file is part of the Ever2e Application.                        *
 *                                                                       *
 *   This file is free software: it may be redistributed and/or          *
 *   modified under the terms of the GNU General Public License as       *
 *   published by the Free Software Foundation, either version 3 of the  *
 *   License, or any later version.                                      *
 *                                                                       *
 *   This file is distributed in the hope that it will be useful, but    *
 *   without any warranty including the implied warranty of              *
 *   merchantability or fitness for a particular purpose. See the        *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   A copy of the GNU General Public License should accompany this      *
 *   file. If it does not, it may be found at                            *
 *   <http://www.gnu.org/licenses/>.                                     *
 *                                                                       *
 *   Shane Reilly                                                        *
 *   shane@cursorcorner.net                                              *
 *                                                                       *
 *************************************************************************/


#include "recorder.h"

#include <atomic>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>


#ifdef EVER2E_ENABLE_FFMPEG

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
}


using namespace std;


namespace
{

string avErrorString( int error )
{
	char buffer[AV_ERROR_MAX_STRING_SIZE];
	av_strerror(error, buffer, sizeof(buffer));
	return string(buffer);
}

bool setOptionIfPresent( void* options, const char* name, const char* value )
{
	int ret = av_opt_set(options, name, value, 0);
	return ret>=0 || ret==AVERROR_OPTION_NOT_FOUND;
}

bool setOptionIfPresent( void* options, const char* name, int64_t value )
{
	int ret = av_opt_set_int(options, name, value, 0);
	return ret>=0 || ret==AVERROR_OPTION_NOT_FOUND;
}

}


struct MediaRecorder::Impl
{
	struct VideoBuffer
	{
		explicit VideoBuffer( size_t byteCount )
			: pts(0),
			  pixels(byteCount)
		{
		}

		int64_t pts;
		vector<Uint8> pixels;
	};

	struct AudioChunk
	{
		int64_t pts;
		vector<Sint16> samples;
	};

	struct WorkItem
	{
		enum Type { VIDEO, AUDIO };

		Type type;
		unique_ptr<VideoBuffer> video;
		AudioChunk audio;
	};

	explicit Impl( const Config& config )
		: config(config),
		  formatContext(NULL),
		  videoContext(NULL),
		  audioContext(NULL),
		  videoStream(NULL),
		  audioStream(NULL),
		  videoFrame(NULL),
		  packet(NULL),
		  videoFrameBytes((size_t)config.width*(size_t)config.height*4),
		  videoPitch(config.width*4),
		  nextVideoPts(0),
		  nextAudioPts(0),
		  audioScratchPts(0),
		  allocatedVideoBuffers(0),
		  stopping(false),
		  accepting(false),
		  failed(false),
		  headerWritten(false),
		  droppedVideoFrames(0),
		  droppedAudioSamples(0)
	{
		audioScratch.reserve((size_t)this->config.audioChunkSamples);
	}

	~Impl()
	{
		stop();
		closeOutput();
	}

	bool start()
	{
		if( accepting.load() )
			return true;
		if( !openOutput() )
			return false;
		accepting.store(true);
		worker = thread(&Impl::workerMain, this);
		cerr << "Recording video to " << config.path << " ("
			 << config.width << "x" << config.height << " FFV1, mono PCM "
			 << config.audioSampleRate << " Hz)\n";
		return true;
	}

	void stop()
	{
		if( !accepting.load() && !worker.joinable() )
			return;

		flushAudioScratch();
		accepting.store(false);
		{
			lock_guard<mutex> lock(queueMutex);
			stopping = true;
		}
		queueCondition.notify_one();
		if( worker.joinable() )
			worker.join();

		size_t droppedVideo = droppedVideoFrames.load();
		size_t droppedAudio = droppedAudioSamples.load();
		if( droppedVideo>0 || droppedAudio>0 ) {
			cerr << "Recording dropped " << droppedVideo << " video frame(s)"
				 << " and " << droppedAudio << " audio sample(s)"
				 << " to keep emulation responsive.\n";
		}
	}

	bool isRunning() const
	{
		return accepting.load() && !failed.load();
	}

	void captureVideoFrame( SDL_Surface* source, int sourceX, int sourceY )
	{
		if( !accepting.load() || failed.load() || source==NULL )
			return;

		int64_t pts = nextVideoPts++;
		unique_ptr<VideoBuffer> buffer = acquireVideoBuffer();
		if( buffer==NULL ) {
			droppedVideoFrames++;
			return;
		}
		buffer->pts = pts;

		if( !copyFrame(source, sourceX, sourceY, *buffer) ) {
			recycleVideoBuffer(std::move(buffer));
			droppedVideoFrames++;
			return;
		}

		WorkItem item;
		item.type = WorkItem::VIDEO;
		item.video = std::move(buffer);
		enqueueWork(std::move(item));
	}

	void captureAudioSample( Sint16 sample )
	{
		if( !accepting.load() || failed.load() )
			return;

		if( audioScratch.empty() )
			audioScratchPts = nextAudioPts;
		audioScratch.push_back(sample);
		nextAudioPts++;
		if( (int)audioScratch.size()>=config.audioChunkSamples )
			flushAudioScratch();
	}

	size_t getDroppedVideoFrames() const
	{
		return droppedVideoFrames.load();
	}

	size_t getDroppedAudioSamples() const
	{
		return droppedAudioSamples.load();
	}

private:

	Config config;
	AVFormatContext* formatContext;
	AVCodecContext* videoContext;
	AVCodecContext* audioContext;
	AVStream* videoStream;
	AVStream* audioStream;
	AVFrame* videoFrame;
	AVPacket* packet;

	size_t videoFrameBytes;
	int videoPitch;
	int64_t nextVideoPts;
	int64_t nextAudioPts;
	int64_t audioScratchPts;

	deque<unique_ptr<VideoBuffer> > freeVideoBuffers;
	deque<WorkItem> workQueue;
	vector<Sint16> audioScratch;
	size_t allocatedVideoBuffers;

	mutable mutex queueMutex;
	condition_variable queueCondition;
	thread worker;
	bool stopping;
	atomic<bool> accepting;
	atomic<bool> failed;
	bool headerWritten;
	atomic<size_t> droppedVideoFrames;
	atomic<size_t> droppedAudioSamples;

	bool openOutput()
	{
		int ret = avformat_alloc_output_context2(&formatContext, NULL, "matroska", config.path.c_str());
		if( ret<0 || formatContext==NULL ) {
			cerr << "Unable to create recording output: " << avErrorString(ret) << "\n";
			return false;
		}

		if( !openVideoStream() || !openAudioStream() )
			return false;

		if( !(formatContext->oformat->flags & AVFMT_NOFILE) ) {
			ret = avio_open(&formatContext->pb, config.path.c_str(), AVIO_FLAG_WRITE);
			if( ret<0 ) {
				cerr << "Unable to open recording file \"" << config.path << "\": "
					 << avErrorString(ret) << "\n";
				return false;
			}
		}

		ret = avformat_write_header(formatContext, NULL);
		if( ret<0 ) {
			cerr << "Unable to write recording header: " << avErrorString(ret) << "\n";
			return false;
		}
		headerWritten = true;
		return true;
	}

	bool openVideoStream()
	{
		const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_FFV1);
		if( codec==NULL ) {
			cerr << "FFV1 encoder not found in libavcodec.\n";
			return false;
		}

		videoStream = avformat_new_stream(formatContext, NULL);
		if( videoStream==NULL ) {
			cerr << "Unable to allocate recording video stream.\n";
			return false;
		}

		videoContext = avcodec_alloc_context3(codec);
		if( videoContext==NULL ) {
			cerr << "Unable to allocate FFV1 encoder context.\n";
			return false;
		}

		videoContext->width = config.width;
		videoContext->height = config.height;
		videoContext->time_base = (AVRational){1, config.fps};
		videoContext->framerate = (AVRational){config.fps, 1};
		videoContext->pix_fmt = AV_PIX_FMT_BGR0;
		videoContext->level = 3;
		videoContext->thread_count = 0;
		videoContext->thread_type = FF_THREAD_SLICE;
		videoContext->slices = 8;
		if( formatContext->oformat->flags & AVFMT_GLOBALHEADER )
			videoContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		if( videoContext->priv_data!=NULL &&
			(!setOptionIfPresent(videoContext->priv_data, "coder", "range_tab") ||
			 !setOptionIfPresent(videoContext->priv_data, "context", (int64_t)1) ||
			 !setOptionIfPresent(videoContext->priv_data, "slicecrc", (int64_t)0)) ) {
			cerr << "Unable to set FFV1 encoder options.\n";
			return false;
		}

		int ret = avcodec_open2(videoContext, codec, NULL);
		if( ret<0 ) {
			cerr << "Unable to open FFV1 encoder: " << avErrorString(ret) << "\n";
			return false;
		}

		ret = avcodec_parameters_from_context(videoStream->codecpar, videoContext);
		if( ret<0 ) {
			cerr << "Unable to copy FFV1 stream parameters: " << avErrorString(ret) << "\n";
			return false;
		}
		videoStream->time_base = videoContext->time_base;

		videoFrame = av_frame_alloc();
		if( videoFrame==NULL ) {
			cerr << "Unable to allocate FFV1 video frame.\n";
			return false;
		}
		videoFrame->format = videoContext->pix_fmt;
		videoFrame->width = videoContext->width;
		videoFrame->height = videoContext->height;
		ret = av_frame_get_buffer(videoFrame, 32);
		if( ret<0 ) {
			cerr << "Unable to allocate FFV1 video frame buffer: " << avErrorString(ret) << "\n";
			return false;
		}

		packet = av_packet_alloc();
		if( packet==NULL ) {
			cerr << "Unable to allocate recording packet.\n";
			return false;
		}

		return true;
	}

	bool openAudioStream()
	{
		const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_PCM_S16LE);
		if( codec==NULL ) {
			cerr << "PCM S16LE encoder not found in libavcodec.\n";
			return false;
		}

		audioStream = avformat_new_stream(formatContext, NULL);
		if( audioStream==NULL ) {
			cerr << "Unable to allocate recording audio stream.\n";
			return false;
		}

		audioContext = avcodec_alloc_context3(codec);
		if( audioContext==NULL ) {
			cerr << "Unable to allocate recording audio context.\n";
			return false;
		}

		audioContext->sample_rate = config.audioSampleRate;
		audioContext->sample_fmt = AV_SAMPLE_FMT_S16;
		audioContext->time_base = (AVRational){1, config.audioSampleRate};
		av_channel_layout_default(&audioContext->ch_layout, 1);
		if( formatContext->oformat->flags & AVFMT_GLOBALHEADER )
			audioContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		int ret = avcodec_open2(audioContext, codec, NULL);
		if( ret<0 ) {
			cerr << "Unable to open PCM audio encoder: " << avErrorString(ret) << "\n";
			return false;
		}

		ret = avcodec_parameters_from_context(audioStream->codecpar, audioContext);
		if( ret<0 ) {
			cerr << "Unable to copy audio stream parameters: " << avErrorString(ret) << "\n";
			return false;
		}
		audioStream->time_base = audioContext->time_base;
		return true;
	}

	unique_ptr<VideoBuffer> acquireVideoBuffer()
	{
		{
			lock_guard<mutex> lock(queueMutex);
			if( !freeVideoBuffers.empty() ) {
				unique_ptr<VideoBuffer> buffer = std::move(freeVideoBuffers.front());
				freeVideoBuffers.pop_front();
				return buffer;
			}
			if( allocatedVideoBuffers>=(size_t)config.videoQueueFrames )
				return unique_ptr<VideoBuffer>();
			allocatedVideoBuffers++;
		}
		return unique_ptr<VideoBuffer>(new VideoBuffer(videoFrameBytes));
	}

	void recycleVideoBuffer( unique_ptr<VideoBuffer> buffer )
	{
		if( buffer==NULL )
			return;
		lock_guard<mutex> lock(queueMutex);
		freeVideoBuffers.push_back(std::move(buffer));
	}

	bool copyFrame( SDL_Surface* source, int sourceX, int sourceY, VideoBuffer& buffer )
	{
		if( sourceX<0 || sourceY<0 ||
			sourceX+config.width>source->w ||
			sourceY+config.height>source->h ||
			source->pixels==NULL ) {
			return false;
		}

		bool locked = false;
		if( SDL_MUSTLOCK(source) ) {
			if( !SDL_LockSurface(source) )
				return false;
			locked = true;
		}

		const int sourceBytesPerPixel = SDL_BYTESPERPIXEL(source->format);
		const Uint8* sourcePixels =
			(const Uint8*)source->pixels + sourceY*source->pitch + sourceX*sourceBytesPerPixel;
		bool converted = SDL_ConvertPixels(
			config.width,
			config.height,
			source->format,
			sourcePixels,
			source->pitch,
			SDL_PIXELFORMAT_BGRX32,
			buffer.pixels.data(),
			videoPitch);

		if( locked )
			SDL_UnlockSurface(source);
		return converted;
	}

	void enqueueWork( WorkItem item )
	{
		{
			lock_guard<mutex> lock(queueMutex);
			if( stopping || failed.load() )
				return;
			workQueue.push_back(std::move(item));
		}
		queueCondition.notify_one();
	}

	void flushAudioScratch()
	{
		if( audioScratch.empty() )
			return;

		WorkItem item;
		item.type = WorkItem::AUDIO;
		item.audio.pts = audioScratchPts;
		item.audio.samples.swap(audioScratch);
		audioScratch.reserve((size_t)config.audioChunkSamples);
		enqueueWork(std::move(item));
	}

	void workerMain()
	{
		for( ;; ) {
			WorkItem item;
			bool haveItem = false;
			{
				unique_lock<mutex> lock(queueMutex);
				queueCondition.wait(lock, [this] { return stopping || !workQueue.empty(); });
				if( workQueue.empty() && stopping )
					break;
				if( !workQueue.empty() ) {
					item = std::move(workQueue.front());
					workQueue.pop_front();
					haveItem = true;
				}
			}

			if( !haveItem )
				continue;

			bool ok = true;
			if( item.type==WorkItem::VIDEO ) {
				ok = encodeVideoFrame(*item.video);
				recycleVideoBuffer(std::move(item.video));
			}
			else
				ok = encodeAudioChunk(item.audio);

			if( !ok ) {
				failed.store(true);
				accepting.store(false);
				lock_guard<mutex> lock(queueMutex);
				workQueue.clear();
				break;
			}
		}

		flushEncoder(videoContext, videoStream);
		flushEncoder(audioContext, audioStream);
		if( headerWritten ) {
			int ret = av_write_trailer(formatContext);
			if( ret<0 )
				cerr << "Unable to finish recording trailer: " << avErrorString(ret) << "\n";
			headerWritten = false;
		}
	}

	bool encodeVideoFrame( const VideoBuffer& buffer )
	{
		int ret = av_frame_make_writable(videoFrame);
		if( ret<0 ) {
			cerr << "Unable to write recording frame: " << avErrorString(ret) << "\n";
			return false;
		}

		for( int y = 0; y<config.height; y++ ) {
			memcpy(
				videoFrame->data[0] + y*videoFrame->linesize[0],
				buffer.pixels.data() + y*videoPitch,
				(size_t)videoPitch);
		}
		videoFrame->pts = buffer.pts;
		return sendFrame(videoContext, videoFrame, videoStream);
	}

	bool encodeAudioChunk( const AudioChunk& chunk )
	{
		if( chunk.samples.empty() )
			return true;

		AVFrame* frame = av_frame_alloc();
		if( frame==NULL ) {
			cerr << "Unable to allocate recording audio frame.\n";
			return false;
		}

		frame->nb_samples = (int)chunk.samples.size();
		frame->format = audioContext->sample_fmt;
		frame->sample_rate = audioContext->sample_rate;
		frame->pts = chunk.pts;
		int ret = av_channel_layout_copy(&frame->ch_layout, &audioContext->ch_layout);
		if( ret>=0 )
			ret = av_frame_get_buffer(frame, 0);
		if( ret<0 ) {
			cerr << "Unable to allocate recording audio buffer: " << avErrorString(ret) << "\n";
			av_frame_free(&frame);
			return false;
		}

		memcpy(frame->data[0], chunk.samples.data(), chunk.samples.size()*sizeof(Sint16));
		bool ok = sendFrame(audioContext, frame, audioStream);
		av_frame_free(&frame);
		return ok;
	}

	bool sendFrame( AVCodecContext* codecContext, AVFrame* frame, AVStream* stream )
	{
		int ret = avcodec_send_frame(codecContext, frame);
		if( ret<0 ) {
			cerr << "Unable to encode recording frame: " << avErrorString(ret) << "\n";
			return false;
		}
		return receivePackets(codecContext, stream);
	}

	bool flushEncoder( AVCodecContext* codecContext, AVStream* stream )
	{
		if( codecContext==NULL || stream==NULL )
			return true;
		int ret = avcodec_send_frame(codecContext, NULL);
		if( ret==AVERROR_EOF )
			return true;
		if( ret<0 ) {
			cerr << "Unable to flush recording encoder: " << avErrorString(ret) << "\n";
			return false;
		}
		return receivePackets(codecContext, stream);
	}

	bool receivePackets( AVCodecContext* codecContext, AVStream* stream )
	{
		for( ;; ) {
			int ret = avcodec_receive_packet(codecContext, packet);
			if( ret==AVERROR(EAGAIN) || ret==AVERROR_EOF )
				return true;
			if( ret<0 ) {
				cerr << "Unable to receive recording packet: " << avErrorString(ret) << "\n";
				return false;
			}

			av_packet_rescale_ts(packet, codecContext->time_base, stream->time_base);
			packet->stream_index = stream->index;
			ret = av_interleaved_write_frame(formatContext, packet);
			av_packet_unref(packet);
			if( ret<0 ) {
				cerr << "Unable to write recording packet: " << avErrorString(ret) << "\n";
				return false;
			}
		}
	}

	void closeOutput()
	{
		if( videoFrame!=NULL )
			av_frame_free(&videoFrame);
		if( packet!=NULL )
			av_packet_free(&packet);
		if( videoContext!=NULL )
			avcodec_free_context(&videoContext);
		if( audioContext!=NULL )
			avcodec_free_context(&audioContext);
		if( formatContext!=NULL ) {
			if( formatContext->pb!=NULL )
				avio_closep(&formatContext->pb);
			avformat_free_context(formatContext);
			formatContext = NULL;
		}
	}
};


MediaRecorder::MediaRecorder( const Config& config )
{
	impl = new Impl(config);
}

MediaRecorder::~MediaRecorder()
{
	delete impl;
}

bool MediaRecorder::start()
{
	return impl->start();
}

void MediaRecorder::stop()
{
	impl->stop();
}

bool MediaRecorder::isRunning() const
{
	return impl->isRunning();
}

void MediaRecorder::captureVideoFrame( SDL_Surface* source, int sourceX, int sourceY )
{
	impl->captureVideoFrame(source, sourceX, sourceY);
}

void MediaRecorder::captureAudioSample( Sint16 sample )
{
	impl->captureAudioSample(sample);
}

size_t MediaRecorder::getDroppedVideoFrames() const
{
	return impl->getDroppedVideoFrames();
}

size_t MediaRecorder::getDroppedAudioSamples() const
{
	return impl->getDroppedAudioSamples();
}

#else

using namespace std;


struct MediaRecorder::Impl
{
	explicit Impl( const Config& config )
		: config(config)
	{
	}

	Config config;
};


MediaRecorder::MediaRecorder( const Config& config )
{
	impl = new Impl(config);
}

MediaRecorder::~MediaRecorder()
{
	delete impl;
}

bool MediaRecorder::start()
{
	cerr << "Video recording support was not built in. Install FFmpeg development libraries and rebuild.\n";
	return false;
}

void MediaRecorder::stop()
{
}

bool MediaRecorder::isRunning() const
{
	return false;
}

void MediaRecorder::captureVideoFrame( SDL_Surface* source, int sourceX, int sourceY )
{
	(void)source;
	(void)sourceX;
	(void)sourceY;
}

void MediaRecorder::captureAudioSample( Sint16 sample )
{
	(void)sample;
}

size_t MediaRecorder::getDroppedVideoFrames() const
{
	return 0;
}

size_t MediaRecorder::getDroppedAudioSamples() const
{
	return 0;
}

#endif
