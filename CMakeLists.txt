cmake_minimum_required(VERSION 3.21)

project(ever2e-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

option(EVER2E_ENABLE_FFMPEG "Enable FFmpeg-backed video/audio recording" ON)

find_package(SDL3 CONFIG QUIET)
if(TARGET SDL3::SDL3)
  set(EVER2E_SDL_TARGET SDL3::SDL3)
else()
  if(NOT UNIX)
    message(FATAL_ERROR "SDL3 not found via CMake config and pkg-config fallback is unavailable on this platform.")
  endif()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)
  set(EVER2E_SDL_TARGET PkgConfig::SDL3)
endif()

set(EVER2E_FFMPEG_TARGET "")
if(EVER2E_ENABLE_FFMPEG)
  find_package(PkgConfig QUIET)
  if(PkgConfig_FOUND)
    pkg_check_modules(FFMPEG QUIET IMPORTED_TARGET libavformat libavcodec libavutil)
  endif()
  if(TARGET PkgConfig::FFMPEG)
    set(EVER2E_FFMPEG_TARGET PkgConfig::FFMPEG)
  else()
    message(STATUS "FFmpeg development libraries not found; video recording support will be disabled.")
  endif()
endif()

file(GLOB_RECURSE EVER2E_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB EVER2E_TEST_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/microcode/*.cpp")

list(REMOVE_ITEM EVER2E_SRC "${CMAKE_SOURCE_DIR}/src/ever2e.cpp")

add_library(ever2e_core STATIC ${EVER2E_SRC})
target_include_directories(ever2e_core PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(ever2e_core PUBLIC ${EVER2E_SDL_TARGET})
if(EVER2E_FFMPEG_TARGET)
  target_link_libraries(ever2e_core PUBLIC ${EVER2E_FFMPEG_TARGET})
  target_compile_definitions(ever2e_core PUBLIC EVER2E_ENABLE_FFMPEG=1)
endif()

if(MSVC)
  target_compile_options(ever2e_core PRIVATE /W4)
else()
  target_compile_options(ever2e_core PRIVATE -Wall -Wextra)
endif()

add_executable(ever2e "${CMAKE_SOURCE_DIR}/src/ever2e.cpp")
target_include_directories(ever2e PRIVATE "${CMAKE_SOURCE_DIR}/third_party/CLI/include")
target_link_libraries(ever2e PRIVATE ever2e_core)

add_executable(microcode_tests ${EVER2E_TEST_SRC})
add_executable(dsk_to_nib "${CMAKE_SOURCE_DIR}/tools/dsk_to_nib.cpp")

target_include_directories(microcode_tests PRIVATE "${CMAKE_SOURCE_DIR}/tests/microcode")
target_link_libraries(microcode_tests PRIVATE ever2e_core)

if(BUILD_TESTING)
  add_test(NAME microcode_tests COMMAND microcode_tests)
  set_tests_properties(microcode_tests PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()
