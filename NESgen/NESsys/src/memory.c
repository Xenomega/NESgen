#include <stdio.h>
#include "memory.h"
#include "game_base.h"
#include "input.h"
#include "ppu.h"

// Define memory map to various CPU address spaces.
struct TLBEntry cpuSystemTLB[] =
{
		{0x000, 0x100, zeroPage},
		{0x100, 0x200, stack},
		{0x200, 0x800, ram},
		// 0x0800-0x2000 mirrors 0x0000-0x0800
		{0x6000, 0x8000, sram}
};
UINT cpuSystemTLBSize = sizeof(cpuSystemTLB) / sizeof(struct TLBEntry);

// Define memory map to various PPU address spaces.
struct TLBEntry ppuTLB[] =
{
		{0x0000, 0x2000, (BYTE*)patternTables},
		// 0x2000-0x3000 is mapped to nameTable pointers which change at runtime, so this static table cannot map them (look at mapping function instead).
		// 0x3000-0x3f00 mirrors 0x2000+
		{0x3F00, 0x3F01, &universalBackgroundColor},
		{0x3F01, 0x3F04, (BYTE*)(backgroundColorPalette)},
		{0x3F04, 0x3F05, (BYTE*)(colorPaletteUnused)},
		{0x3F05, 0x3F08, (BYTE*)(backgroundColorPalette + 1)},
		{0x3F08, 0x3F09, (BYTE*)(colorPaletteUnused + 1)},
		{0x3F09, 0x3F0C, (BYTE*)(backgroundColorPalette + 2)},
		{0x3F0C, 0x3F0D, (BYTE*)(colorPaletteUnused + 2)},
		{0x3F0D, 0x3F10, (BYTE*)(backgroundColorPalette + 3)},
		// 0x3F10 mirrors 0x3F00
		{0x3F11, 0x3F14, (BYTE*)(spriteColorPalette)},
		// 0x3F14 mirrors 0x3F04
		{0x3F15, 0x3F18, (BYTE*)(spriteColorPalette + 1)},
		// 0x3F18 mirrors 0x3F08
		{0x3F19, 0x3F1C, (BYTE*)(spriteColorPalette + 2)},
		// 0x3F1C mirrors 0x3F0C
		{0x3F1D, 0x3F20, (BYTE*)(spriteColorPalette + 3)}
};
UINT ppuTLBSize = sizeof(ppuTLB) / sizeof(struct TLBEntry);

// Functions
/*
 * Obtains the real address that the data at the address represents (in case of mirroring memory).
 */
USHORT cpu_get_non_mirrored_addr(USHORT addr)
{
	// The data from 0x0000-0x0800 is mirrored until 0x2000.
	if(addr < 0x2000)
		addr %= 0x800;
	// The PPU registers from 0x2000-0x2008 are mirrored from 0x2008-0x3000
	else if(addr > 0x2008 && addr < 0x3000)
		addr = 0x2000 + (addr % 8);

	return addr;
}
/*
 * Translates a given NES CPU memory address to one within a buffer that was mapped. Should be done on a byte by byte basis.
 * Returns NULL if it is not within our table.
 */
BYTE* cpu_mem_translate(USHORT addr)
{
	// Depending on our memory range, use the appropriate lookup.
	if(IS_CPU_SYSTEM_MEMORY(addr))
	{
		// System lookup table
		for(UINT i = 0; i < cpuSystemTLBSize; i++)
		{
			if(addr >= cpuSystemTLB[i].startAddr && addr < cpuSystemTLB[i].endAddr)
				return (cpuSystemTLB[i].ptr + (addr - cpuSystemTLB[i].startAddr));
		}
	}
	else
	{
		// Game lookup table
		for(UINT i = 0; i < gameTLBSize; i++)
		{
			if(addr >= gameTLB[i].startAddr && addr < gameTLB[i].endAddr)
				return (gameTLB[i].ptr + (addr - gameTLB[i].startAddr));
		}
	}
	return NULL;
}
/*
 * Reads a byte from the given CPU memory address.
 */
BYTE cpu_read8(USHORT addr)
{
	// Adjust address with respect to mirrored memory.
	USHORT fixedAddr = cpu_get_non_mirrored_addr(addr);

	// If we could translate the address in our lookup, we can simply handle the data.
	BYTE* ptr = cpu_mem_translate(fixedAddr);
	if(ptr != NULL)
		return *ptr;

	// Check for hardware address mappings.
	switch (fixedAddr)
	{
		case PPUSTATUS_REGISTER:
			return ppu_get_status();
		case PPUOAMDATA_REGISTER:
			return ppu_get_oamdata();
		case PPUDATA_REGISTER:
			return ppu_get_data();
		case CONTROLLER1_STATE_REGISTER:
			return read_input(0);
		case CONTROLLER2_STATE_REGISTER:
			return read_input(1);
		case APU_STATUS_REGISTER:
			// TODO: Implement.
			return 0;
		default:
			debug_log("Reading from unexpected CPU memory address 0x%04x...\n", addr);
			return unmappedCPUMemory[addr];
	}
}
/*
 * Reads an unsigned short from the given CPU memory address.
 */
USHORT cpu_read16(USHORT addr)
{
	return (USHORT)(cpu_read8(addr) | (cpu_read8(addr+1) << 8));
}
/*
 * Writes a byte to the given CPU memory address.
 */
void cpu_write8(USHORT addr, BYTE data)
{
	// Adjust address with respect to mirrored memory.
	USHORT fixedAddr = cpu_get_non_mirrored_addr(addr);

	// If it's game memory, we can't write there.
	if(!IS_CPU_SYSTEM_MEMORY(fixedAddr))
		return;

	// If we could translate the address in our lookup, we can simply handle the data.
	BYTE* ptr = cpu_mem_translate(fixedAddr);
	if(ptr != NULL)
	{
		*ptr = data;
		return;
	}

	// Check for hardware address mappings.
	switch(fixedAddr)
	{
		case PPUCTRL_REGISTER:
			ppu_set_ctrl(data);
			break;
		case PPUMASK_REGISTER:
			ppu_set_mask(data);
			break;
		case PPUOAMADDR_REGISTER:
			ppu_set_oamaddr(data);
			break;
		case PPUOAMDATA_REGISTER:
			ppu_set_oamdata(data);
			break;
		case PPUSCROLL_REGISTER:
			ppu_set_ppuscroll(data);
			break;
		case PPUADDR_REGISTER:
			ppu_set_ppuaddr(data);
			break;
		case PPUDATA_REGISTER:
			ppu_set_data(data);
			break;
		case PPUOAMDMA_REGISTER:
			ppu_oam_dma(data);
			break;
		case APU_PULSE1_CHANNEL_REGISTER0:
		case APU_PULSE1_CHANNEL_REGISTER1:
		case APU_PULSE1_CHANNEL_REGISTER2:
		case APU_PULSE1_CHANNEL_REGISTER3:
		case APU_PULSE2_CHANNEL_REGISTER0:
		case APU_PULSE2_CHANNEL_REGISTER1:
		case APU_PULSE2_CHANNEL_REGISTER2:
		case APU_PULSE2_CHANNEL_REGISTER3:
		case APU_TRIANGLE_CHANNEL_REGISTER0:
		case APU_TRIANGLE_CHANNEL_REGISTER1:
		case APU_TRIANGLE_CHANNEL_REGISTER2:
		case APU_NOISE_CHANNEL_REGISTER0:
		case APU_NOISE_CHANNEL_REGISTER1:
		case APU_NOISE_CHANNEL_REGISTER2:
		case APU_DMC_CHANNEL_REGISTER0:
		case APU_DMC_CHANNEL_REGISTER1:
		case APU_DMC_CHANNEL_REGISTER2:
		case APU_DMC_CHANNEL_REGISTER3:
		case APU_CONTROL_REGISTER:
		case APU_FRAME_COUNTER_REGISTER:
			// TODO: Implement all.
			break;
		case CONTROLLER_STROBE_REGISTER:
			write_input_strobe(data);
			break;
		default:
			debug_log("Writing to unexpected CPU memory address 0x%04x...\n", addr);
			unmappedCPUMemory[addr] = data;
	}
}
/*
 * Writes an unsigned short to the given CPU memory address.
 */
void cpu_write16(USHORT addr, USHORT data)
{
	cpu_write8(addr, data & 0xFF);
	cpu_write8(addr + 1, data >> 8);
}
/*
 * Obtains the real address that the data at the address represents (in case of mirroring memory).
 */
USHORT ppu_get_non_mirrored_addr(USHORT addr)
{
	// After this point in memory, all memory before repeats, so we can just reduce it this way.
	addr %= 0x4000;

	// There is also particular mapping for background/color palettes that is more efficient to map here.
	if(addr >= PALETTE_ADDRS_START && addr < PALETTE_ADDRS_END)
	{
		// First we take care of mirroring of our palettes which happens between 0x3F00 and 0x4000.
		addr %= 0x20;
		addr += PALETTE_ADDRS_START;

		// The following must be implemented to mirror universal background color or some particular games will not work as expected.
		// Reference: https://wiki.nesdev.com/w/index.php/PPU_palettes
		if(addr == 0x3F10) addr = 0x3F00;
		else if(addr == 0x3F14) addr = 0x3F04;
		else if(addr == 0x3F18) addr = 0x3F08;
		else if(addr == 0x3F1C) addr = 0x3F0C;
	}
	// Everything between 0x3000-0x3F00 mirrors 0x2000+
	else if(addr >= 0x3000 && addr < 0x3F00)
		addr = (addr - 0x3000) + 0x2000;

	return addr;
}
/*
 * Translates a given NES PPU memory address to one within a buffer that was mapped. Should be done on a byte by byte basis.
 * Returns NULL if it is not within our table.
 */
BYTE* ppu_mem_translate(USHORT addr)
{
	// If it's in our nametable address space, these pointers are set at runtime and so we can't use a static table, so we put the lookup here.
	if(addr >= NAMETABLE_ADDRS_START && addr < NAMETABLE_ADDRS_END)
	{
		UINT nameTableIndex = (addr - NAMETABLE_ADDRS_START) / nameTableSize;
		UINT nameTableOffset = (addr - NAMETABLE_ADDRS_START) % nameTableSize;
		BYTE* ptr = (BYTE*)nameTables[nameTableIndex];
		return (ptr + nameTableOffset);
	}

	// Search through PPU lookup table
	for(UINT i = 0; i < ppuTLBSize; i++)
	{
		if(addr >= ppuTLB[i].startAddr && addr < ppuTLB[i].endAddr)
			return (ppuTLB[i].ptr + (addr - ppuTLB[i].startAddr));
	}
	return NULL;
}
/*
 * Reads a byte from the given PPU memory address.
 */
BYTE ppu_read8(USHORT addr)
{
	// Adjust address with respect to mirrored memory.
	USHORT fixedAddr = ppu_get_non_mirrored_addr(addr);

	// If we could translate the address in our lookup, we can simply handle the data.
	BYTE* ptr = ppu_mem_translate(fixedAddr);
	if(ptr != NULL)
		return *ptr;

	// Check for hardware address mappings.
	switch (fixedAddr)
	{
		default:
			debug_log("Failed to map PPU memory address 0x%04x for reading.\n", addr);
			return 0;
	}
}
/*
 * Reads an unsigned short from the given PPU memory address.
 */
USHORT ppu_read16(USHORT addr)
{
	return (USHORT)(ppu_read8(addr) | (ppu_read8(addr+1) << 8));
}
/*
 * Writes a byte to the given PPU memory address.
 */
void ppu_write8(USHORT addr, BYTE data)
{
	// Adjust address with respect to mirrored memory.
	USHORT fixedAddr = ppu_get_non_mirrored_addr(addr);

	// If we could translate the address in our lookup, we can simply handle the data.
	BYTE* ptr = ppu_mem_translate(fixedAddr);
	if(ptr != NULL)
	{
		*ptr = data;
		return;
	}

	// Check for hardware address mappings.
	switch(fixedAddr)
	{
		default:
			debug_log("Failed to map PPU memory address 0x%04x for writing.\n", addr);
	}
}
/*
 * Writes an unsigned short to the given PPU memory address.
 */
void ppu_write16(USHORT addr, USHORT data)
{
	ppu_write8(addr, data & 0xFF);
	ppu_write8(addr + 1, data >> 8);
}
