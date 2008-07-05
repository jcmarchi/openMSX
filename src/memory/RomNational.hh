// $Id$

#ifndef ROMNATIONAL_HH
#define ROMNATIONAL_HH

#include "RomBlocks.hh"

namespace openmsx {

class SRAM;

class RomNational : public Rom16kBBlocks
{
public:
	RomNational(MSXMotherBoard& motherBoard, const XMLElement& config,
	            std::auto_ptr<Rom> rom);
	virtual ~RomNational();

	virtual void reset(const EmuTime& time);
	virtual byte peekMem(word address, const EmuTime& time) const;
	virtual byte readMem(word address, const EmuTime& time);
	virtual const byte* getReadCacheLine(word address) const;
	virtual void writeMem(word address, byte value, const EmuTime& time);
	virtual byte* getWriteCacheLine(word address) const;

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	const std::auto_ptr<SRAM> sram;
	int sramAddr;
	byte control;
	byte bankSelect[4];
};
REGISTER_MSXDEVICE(RomNational, "RomNational");

} // namespace openmsx

#endif
