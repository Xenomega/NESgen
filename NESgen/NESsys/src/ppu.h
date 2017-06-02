// References:
// https://wiki.nesdev.com/w/index.php/PPU_registers
// https://wiki.nesdev.com/w/index.php/PPU_memory_map
// https://wiki.nesdev.com/w/index.php/PPU_palettes
// https://wiki.nesdev.com/w/index.php/PPU_rendering
// https://wiki.nesdev.com/w/index.php/PPU_attribute_tables

#ifndef PPU_H_
#define PPU_H_
#include "NESsys.h"

// ---------------------------------
// Picture Processing Unit (PPU)
// ---------------------------------
#define RESOLUTION_WIDTH			256
#define RESOLUTION_HEIGHT			240
#define PATTERN_TABLE_TILE_WIDTH	8
#define PATTERN_TABLE_TILE_HEIGHT	8
#define RESOLUTION_TILES_WIDTH		(RESOLUTION_WIDTH / PATTERN_TABLE_TILE_WIDTH)
#define RESOLUTION_TILES_HEIGHT		(RESOLUTION_HEIGHT / PATTERN_TABLE_TILE_HEIGHT)
#define PATTERN_TABLE_SIZE			0x1000
#define NAME_TABLE_SIZE				0x400
#define OAM_TABLE_COUNT				0x40
#define SPRITE_WIDTH				8
#define SPRITE_HEIGHT				8
#define SCANLINES_PER_FRAME			262
#define SCANLINES_PER_VBLANK		240
#define PPU_CYCLES_PER_SCANLINE		341
#define PPU_CYCLES_PER_FRAME		(SCANLINES_PER_FRAME * PPU_CYCLES_PER_SCANLINE)
#define PPU_CYCLES_PER_VBLANK		(SCANLINES_PER_VBLANK * PPU_CYCLES_PER_SCANLINE)

enum MIRRORINGTYPE { HORIZONTAL, VERTICAL, FOUR_SCREEN, ONE_SCREEN };
enum MASTERSLAVEMODE { MASTERSLAVE_SLAVE, MASTERSLAVE_MASTER, MASTERSLAVE_UNSET };

/*
 * PPU control register, used to control PPU execution.
 */
struct PPUCTRL
{
	BYTE baseNameTableIndex; // 0-3 index into patternTables
	BOOL vramAddrIncrements32; // increment per read/write. index into [1,32].
	BYTE spritePatternTableIndex; // 0/1 index into patternTables. (only for 8x8 sprites)
	BYTE backgroundPatternTableIndex; // 0/1 index into patternTables
	BOOL doubleSpriteHeight; // 8x16 instead of 8x8?
	enum MASTERSLAVEMODE masterSlaveSelect;
	BOOL executeNMIonVBLANK; // execute every vertical blank?
};
/*
 * PPU mask register, flags used to set PPU rendering settings.
 */
struct PPUMASK
{
	BOOL greyscale; // renders in greyscale.
	BOOL showBackgroundLeft; // shows background on first 8 pixel columns on left
	BOOL showSpritesLeft; // shows sprites on first 8 pixel columns on left
	BOOL showBackground; // show/hide background
	BOOL showSprites; // show/hide sprites
	BOOL emphasizeRed;
	BOOL emphasizeGreen;
	BOOL emphasizeBlue;
};
/*
 * PPU status register, flags that indicates PPU statuses.
 */
struct PPUSTATUS
{
	BOOL verticalBlanking;
	BOOL sprite0Hit;
	BOOL spriteOverflow;
};
/*
 * Object Attribute Memory (dictates sprite information)
 */
struct OAMEntry
{
	BYTE Y;
	BYTE ID; // for 8x16 sprites, uses bit 0 for pattern table index.
	BYTE attributes;
	BYTE X;
};
struct NAMETABLE
{
	BYTE cells[(RESOLUTION_WIDTH * RESOLUTION_HEIGHT) / (8 * 8)]; // 8x8 cells that make up the screen
	BYTE attributes[64]; // controls palette for 4x4 cells above. 2 bits for every 2x2 top-left, top-right, bottom-left, bottom-right.
};
USHORT nameTableSize;
struct COLORPALETTE
{
	BYTE paletteColorIndex[3];
};

/*
 * The internal RGBA framebuffer that we draw to.
 */
UINT internalFrameBuffer[RESOLUTION_WIDTH * RESOLUTION_HEIGHT];
/*
 * The final RGBA framebuffer that is a finished frame ready to copy to.
 */
UINT frameBuffer[RESOLUTION_WIDTH * RESOLUTION_HEIGHT];
/*
 * The framecount for the current second.
 */
UINT frameCount;
/*
 * The amount of frames per second.
 */
UINT framesPerSecond;
/*
 * PPU cycle count, reset every full frame render, used to track rendering position.
 */
USHORT ppuCycles;
/*
 * Current scanline number, used to track rendering position.
 */
USHORT currentScanline;
/*
 * Force greyscale (user specified).
 */
BOOL forceGreyscale;
/*
 * PPU control register (written at 0x2000), used to control PPU execution.
 */
struct PPUCTRL ppuCtrl;
/*
 * PPU mask register (written at 0x2001), flags used to set PPU rendering settings.
 */
struct PPUMASK ppuMask;
/*
 * PPU status register (read at 0x2002), flags that indicates PPU statuses.
 */
struct PPUSTATUS ppuStatus;
/*
 * Pattern tables are the tiles used to make up background and sprites.
 */
BYTE patternTables[2][PATTERN_TABLE_SIZE];
/*
 * Nametables lay out backgrounds.
 * Order: (top-left, top-right, bottom-left, bottom-right)
 * Address space has room for 4, but only enough memory for 2, so 2 are always mirrored.
 */
struct NAMETABLE* nameTables[4];
struct NAMETABLE physicalNameTables[2];
/*
 * Object Attribute Memory (internal memory that represents an array of sprites, earlier are more frontward)
 */
struct OAMEntry objectAttributeMemory[OAM_TABLE_COUNT];
/*
 * The address to read/write from/to the PPU memory space, as set by PPUADDR.
 */
USHORT vramReadWriteAddress;
/*
 * The toggle which is set high/low when setting two values (16-bits) through one register (8-bit).
 */
BOOL vramWriteToggle;
/*
 * Internal read buffer for PPUDATA for accesses to certain memory locations.
 * https://wiki.nesdev.com/w/index.php/PPU_registers#PPUDATA
 */
BOOL vramReadValue;
/*
 * Horizontal scroll value as set by PPUSCROLL register.
 */
BYTE scrollX;
/*
 * Vertical scroll value as set by PPUSCROLL register.
 */
BYTE scrollY;
/*
 * The address to read/write from/to the OAM internal memory, as set by the OAMADDR register.
 */
BYTE oamReadWriteAddress;
/*
 * Indicates how many sprites have been crossed on the current scanline.
 */
BYTE spritesOnCurrentLine;
/*
 * The palette color index that the buffer is cleared with on a new frame.
 */
BYTE universalBackgroundColor;
/*
 * Unused space between color palettes, mapped, not actually sequential.
 */
BYTE colorPaletteUnused[3]; // unused bytes between color palettes.
/*
 * Background color palettes which the background references by the nametable's attributes.
 */
struct COLORPALETTE backgroundColorPalette[4];
/*
 * Sprite color palettes which the sprites can extract from the sprite attribute data.
 */
struct COLORPALETTE spriteColorPalette[4];
/*
 * NES color palette stored in internal memory in PPU.
 */
extern UINT palette[];
/*
 * The total count of colors in the palette.
 */
UINT paletteCount;

// ---------------------------------
// Functions
// ---------------------------------
void ppu_init();
BYTE ppu_get_status();
BYTE ppu_get_oamdata();
BYTE ppu_get_data();
void ppu_set_ctrl(BYTE data);
void ppu_set_mask(BYTE data);
void ppu_set_oamaddr(BYTE data);
void ppu_set_oamdata(BYTE data);
void ppu_set_ppuscroll(BYTE data);
void ppu_set_ppuaddr(BYTE data);
void ppu_set_data(BYTE data);
void ppu_oam_dma(BYTE pageNumber);
UINT ppu_get_color(BYTE paletteColorIndex);
void ppu_draw_sprites(BYTE priority);
void ppu_draw_background();
void ppu_update();

#endif /* PPU_H_ */
