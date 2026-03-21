#include "card16bit.h"
#include <algorithm>


using namespace std;


PeripheralCard16bit::PeripheralCard16bit()
{
	setSlotPattern(0x00);
	idTag = "card16";
}

void PeripheralCard16bit::putMem16b( Uint8 offset, Uint8 byte )
{
	assert( offset<16 );
	slotRom256b[offset] = byte;
}

Uint8 PeripheralCard16bit::getMem16b( Uint8 offset )
{
	assert( offset<16 );
	return slotRom256b[offset];
}

Uint8 PeripheralCard16bit::getMem256b( Uint8 offset )
{
	return slotRom256b[offset];
}

Uint8 PeripheralCard16bit::getMem2k( Uint16 offset )
{
	assert( offset<2048 );
	return expRom2k[offset];
}

void PeripheralCard16bit::setSlotPattern( Uint8 value )
{
	slotRom256b.fill(value);
	expRom2k.fill(value);
}

void PeripheralCard16bit::setIdTag( const std::string& id )
{
	idTag = id;
}

const std::string& PeripheralCard16bit::getIdTag() const
{
	return idTag;
}
