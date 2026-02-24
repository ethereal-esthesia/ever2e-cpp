#include "card16bit.h"


using namespace std;


PeripheralCard16bit::PeripheralCard16bit()
{
	/// STUB ///
	slotRom256b = NULL;
	expRom2k = NULL;
}

void putMem16b( Uint8 offset, Uint8 byte )
{
	/// STUB ///
	assert( offset<16 );
}

Uint8 PeripheralCard16bit::getMem16b( Uint8 offset )
{
	/// STUB ///
	assert( offset<16 );
	return 0;
}

Uint8 PeripheralCard16bit::getMem256b( Uint8 offset )
{
	/// STUB ///
	assert( offset<256 );
	return 0;
}

Uint8 PeripheralCard16bit::getMem2k( Uint16 offset )
{
	/// STUB ///
	assert( offset<2048 );
	return 0;
}
