
#ifndef MEMORY_H_
#define MEMORY_H_
#include "NESsys.h"

// ---------------------------------
// Memory Mapping Unit (MMU)
// ---------------------------------
#define MEMORY_PAGE_SIZE						0x100
#define IS_CPU_SYSTEM_MEMORY(addr)				(addr < 0x8000)
#define PPUCTRL_REGISTER						0x2000
#define	PPUMASK_REGISTER						0x2001
#define PPUSTATUS_REGISTER						0x2002
#define PPUOAMADDR_REGISTER						0x2003
#define PPUOAMDATA_REGISTER						0x2004
#define PPUSCROLL_REGISTER						0x2005
#define PPUADDR_REGISTER						0x2006
#define PPUDATA_REGISTER						0x2007
#define PPUOAMDMA_REGISTER						0x4014
#define APU_PULSE1_CHANNEL_REGISTER0			0x4000
#define APU_PULSE1_CHANNEL_REGISTER1			0x4001
#define APU_PULSE1_CHANNEL_REGISTER2			0x4002
#define APU_PULSE1_CHANNEL_REGISTER3			0x4003
#define APU_PULSE2_CHANNEL_REGISTER0			0x4004
#define APU_PULSE2_CHANNEL_REGISTER1			0x4005
#define APU_PULSE2_CHANNEL_REGISTER2			0x4006
#define APU_PULSE2_CHANNEL_REGISTER3			0x4007
#define APU_TRIANGLE_CHANNEL_REGISTER0			0x4008
#define APU_TRIANGLE_CHANNEL_REGISTER1			0x400A
#define APU_TRIANGLE_CHANNEL_REGISTER2			0x400B
#define APU_NOISE_CHANNEL_REGISTER0				0x400C
#define APU_NOISE_CHANNEL_REGISTER1				0x400E
#define APU_NOISE_CHANNEL_REGISTER2				0x400F
#define APU_DMC_CHANNEL_REGISTER0				0x4010
#define APU_DMC_CHANNEL_REGISTER1				0x4011
#define APU_DMC_CHANNEL_REGISTER2				0x4012
#define APU_DMC_CHANNEL_REGISTER3				0x4013
#define APU_CONTROL_REGISTER					0x4015
#define APU_STATUS_REGISTER						0x4015
#define APU_FRAME_COUNTER_REGISTER				0x4017
#define CONTROLLER_STROBE_REGISTER				0x4016
#define CONTROLLER1_STATE_REGISTER				0x4016
#define CONTROLLER2_STATE_REGISTER				0x4017
#define CROSSES_PAGE_ABSOLUTE(addr1, addr2)		((addr1 & 0xFF00) != (addr2 & 0xFF00))
#define CROSSES_PAGE_RELATIVE(addr, relative)	CROSSES_PAGE_RELATIVE(addr, (addr + relative))
#define PATTERNTABLE_ADDR_START					0x0000
#define PATTERNTABLE_ADDR_END					0x2000
#define NAMETABLE_ADDRS_START					0x2000
#define NAMETABLE_ADDRS_END						0x3000
#define NAMETABLE_SIZE							0x400
#define PALETTE_ADDRS_START						0x3F00
#define PALETTE_ADDRS_END						0x4000

// Translation Lookaside Buffer (TLB) to translate any NES referenced addresses.
struct TLBEntry
{
	UINT startAddr;
	UINT endAddr;
	BYTE* ptr;
};
BYTE* unmappedCPUMemory;

// ---------------------------------
// Functions
// ---------------------------------
USHORT cpu_get_non_mirrored_addr(USHORT addr);
BYTE* cpu_mem_translate(USHORT addr);
BYTE cpu_read8(USHORT addr);
USHORT cpu_read16(USHORT addr);
void cpu_write8(USHORT addr, BYTE data);
void cpu_write16(USHORT addr, USHORT data);

USHORT ppu_get_non_mirrored_addr(USHORT addr);
BYTE* ppu_mem_translate(USHORT addr);
BYTE ppu_read8(USHORT addr);
USHORT ppu_read16(USHORT addr);
void ppu_write8(USHORT addr, BYTE data);
void ppu_write16(USHORT addr, USHORT data);

#endif /* MEMORY_H_ */
