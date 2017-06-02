#include "NESsys.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "cpu.h"
#include "game_base.h"
#include "memory.h"
#include "ppu.h"
#include "tests.h"

void fail(const char *fmt, ...)
{
	printf("TEST FAILED: ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}
void test_struct_sizes()
{
	// Some compilers/architectures may align data differently.
	// These structs should align fine, but we have these tests to verify it's as expected.
	assert(sizeof(objectAttributeMemory) == 0x100, "Object Attribute Memory struct size unexpected. Expected %i, but got %i.", 0x100, sizeof(objectAttributeMemory));
	assert(sizeof(physicalNameTables) == 0x800, "Nametables struct size unexpected. Expected %i, but got %i.", 0x800, sizeof(nameTables));
	assert(sizeof(patternTables) == 0x2000, "Pattern Tables struct size unexpected. Expected %i, but got %i.", 0x2000, sizeof(patternTables));
	assert(paletteCount == 64, "PPU Palette Size should be 64 but was %i.", paletteCount);
}
void test_ppu_non_mirrored_addr()
{
	assert(ppu_get_non_mirrored_addr(0x4001) == 0x0001, "Non Mirrored Address #1");
	assert(ppu_get_non_mirrored_addr(0x6001) == 0x2001, "Non Mirrored Address #2");
	assert(ppu_get_non_mirrored_addr(0x3F14) == 0x3F04, "Non Mirrored Address #3");
	assert(ppu_get_non_mirrored_addr(0x3F1C + 0x8000) == 0x3F0C, "Non Mirrored Address #4");
	assert(ppu_get_non_mirrored_addr(0x3000) == 0x2000, "Non Mirrored Address #5");
	assert(ppu_get_non_mirrored_addr(0x3EFF) == 0x2EFF, "Non Mirrored Address #6");
}
void test_ppu_read_write()
{
	ppu_write8(0x6001, 0x07); // mirrored
	assert(ppu_read8(0x6001) == 0x07, "PPU Read/Write Test #1");
	assert(ppu_read8(0x2001) == 0x07, "PPU Read/Write Test #2");
	ppu_write8(0x3000, 0x05);
	assert(ppu_read8(0x2000) == 0x05, "PPU Read/Write Test #3");
	ppu_write8(0x3EFF, 0x05);
	assert(ppu_read8(0x2EFF) == 0x05, "PPU Read/Write Test #4");
	ppu_write8(0x3F10 + 0x8000, 0x03);
	assert(ppu_read8(0x3F00) == 0x03, "PPU Read/Write Test #5");
	assert(ppu_read8(0x3F20) == 0x03, "PPU Read/Write Test #6");
}
void test_cpu_read_write()
{
	cpu_write8(0x500, 0x07); // mirrored
	assert(cpu_read8(0x500 + 0x800) == 0x07, "CPU Read/Write Test #1");
	assert(cpu_read8(0x500 + 0x1800) == 0x07, "CPU Read/Write Test #2");
	cpu_write8(0x20, 0x13); // mirrored
	assert(cpu_read8(0x20 + 0x800) == 0x13, "CPU Read/Write Test #3");
	assert(cpu_read8(0x20 + 0x1800) == 0x13, "CPU Read/Write Test #4");
}
void test_ppu_oamdma_register()
{
	// Write the data (two bytes, writes increment addr)
	cpu_write8(PPUOAMADDR_REGISTER, 0x20);
	cpu_write8(PPUOAMDATA_REGISTER, 0x07);
	cpu_write8(PPUOAMDATA_REGISTER, 0x08);
	// Read the data (two bytes, reads don't increment)
	cpu_write8(PPUOAMADDR_REGISTER, 0x20);
	assert(cpu_read8(PPUOAMDATA_REGISTER) == 0x07, "PPUADDR or PPUDATA Register Write/Read Test #1.");
	assert(cpu_read8(PPUOAMDATA_REGISTER) == 0x07, "PPUADDR or PPUDATA Register Write/Read Test #2.");
	cpu_write8(PPUOAMADDR_REGISTER, 0x21);
	assert(cpu_read8(PPUOAMDATA_REGISTER) == 0x08, "PPUADDR or PPUDATA Register Write/Read Test #3.");

	// Next, we test wrap around writing to OAM.
	// TODO: Check that this actually happens and the addr register is 8-bit as implemented so it wraps around.

	// Write the data (two bytes, writes increment addr)
	cpu_write8(PPUOAMADDR_REGISTER, 0xFF);
	cpu_write8(PPUOAMDATA_REGISTER, 0x01);
	cpu_write8(PPUOAMDATA_REGISTER, 0x02);
	// Read the data (two bytes, reads don't increment)
	cpu_write8(PPUOAMADDR_REGISTER, 0xFF);
	assert(cpu_read8(PPUOAMDATA_REGISTER) == 0x01, "PPUADDR or PPUDATA Register Write/Read Test #4.");
	assert(cpu_read8(PPUOAMDATA_REGISTER) == 0x01, "PPUADDR or PPUDATA Register Write/Read Test #5.");
	cpu_write8(PPUOAMADDR_REGISTER, 0x00);
	assert(cpu_read8(PPUOAMDATA_REGISTER) == 0x02, "PPUADDR or PPUDATA Register Write/Read Test #6.");
}
void test_cpu_flags()
{
	assert(cpu_get_flag(CPU_FLAG_INTERRUPT_DISABLE) == FALSE, "CPU_FLAG_INTERRUPT_DISABLE should've been FALSE, but was TRUE.");
}
void test_chrrom()
{
	// Test all data was copied successfully.
	UINT copySize = min(chrRomSize, PATTERN_TABLE_SIZE * 2);
	for(UINT x = 0; x < copySize; x++)
	{
		UINT bank = x / PATTERN_TABLE_SIZE;
		UINT index = x - (bank * PATTERN_TABLE_SIZE);
		assert(patternTables[bank][index] == chrRom[x], "CHR-ROM improperly loaded into pattern table (offset 0x%02x)", x);
	}
}
void test_all()
{
	// Initialize any needed hardware.
	cpu_init();
	ppu_init();

	test_struct_sizes();
	test_cpu_flags();
	test_ppu_non_mirrored_addr();
	test_ppu_read_write();
	test_cpu_read_write();
	test_ppu_oamdma_register();
	test_chrrom();
	printf("Passed all tests...\n");
}
