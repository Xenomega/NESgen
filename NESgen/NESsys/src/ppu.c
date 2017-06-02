#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "game_base.h"
#include "tests.h"

// ---------------------------------
// Functions
// ---------------------------------
/*
 * Initializes the PPU.
 */
void ppu_init()
{
	// Set our default values.
	ppuCycles = 0;
	currentScanline = 0;
	spritesOnCurrentLine = 0;
	vramWriteToggle = TRUE;
	vramReadValue = 0;
	scrollX = 0;
	scrollY = 0;
	oamReadWriteAddress = 0;
	vramReadWriteAddress = 0;
	frameCount = 0;
	forceGreyscale = FALSE;
	memset(&ppuStatus, 0, sizeof(ppuStatus));
	memset(&ppuCtrl, 0, sizeof(ppuCtrl));
	memset(&ppuMask, 0, sizeof(ppuMask));
	memset(&physicalNameTables, 0, sizeof(physicalNameTables));
	memset(&objectAttributeMemory, 0, sizeof(objectAttributeMemory));
	universalBackgroundColor = 0;
	memset(&backgroundColorPalette, 0, sizeof(backgroundColorPalette));
	memset(&spriteColorPalette, 0, sizeof(spriteColorPalette));
	memset(&colorPaletteUnused, 0, sizeof(colorPaletteUnused));
	ppuCtrl.masterSlaveSelect = MASTERSLAVE_UNSET;

	// Nametable indexes are as follows: top-left, top-right, bottom-left, bottom-right.
	if (mirroringType == HORIZONTAL)
	{
		// Horizontal mirrors left side tables onto right.
		nameTables[0] = nameTables[1] =  (physicalNameTables);
		nameTables[2] = nameTables[3] =  (physicalNameTables + 1);
	}
	else if (mirroringType == VERTICAL)
	{
		// Vertical mirrors top tables onto bottom.
		nameTables[0] = nameTables[2] =  (physicalNameTables);
		nameTables[1] = nameTables[3] =  (physicalNameTables + 1);
	}
	else if (mirroringType == ONE_SCREEN)
	{
		error("One screen mirroring not supported.");
	}

	// Copy CHR-ROM into memory.
	BYTE* patternTablePtr = (BYTE*)patternTables;
	UINT copySize = min(chrRomSize, PATTERN_TABLE_SIZE * 2);
	memcpy(patternTablePtr, chrRom, copySize);

	debug_log("Picture Processing Unit (PPU) initialized...\n");
}
/*
 * Obtains a packed version of the status flags.
 */
BYTE ppu_get_status()
{
	BYTE status = 0;
	if (ppuStatus.spriteOverflow) status |= (1 << 5);
	if (ppuStatus.sprite0Hit) status |= (1 << 6);
	if (ppuStatus.verticalBlanking) status |= (1 << 7);
	vramWriteToggle = TRUE;
	return status;
}
/*
 * Sets the PPUCTRL register values accordingly.
 */
void ppu_set_ctrl(BYTE data)
{
	if(ppuMask.showBackground || ppuCtrl.masterSlaveSelect != MASTERSLAVE_SLAVE)
		ppuCtrl.baseNameTableIndex = data & 3; // 1st + 2nd bit.
	ppuCtrl.vramAddrIncrements32 = ((data >> 2) & 1); // 3rd bit, determines increment of 1/32.
	ppuCtrl.spritePatternTableIndex = (data >> 3) & 1; // 4th bit
	ppuCtrl.backgroundPatternTableIndex = (data >> 4) & 1; // 5th bit
	ppuCtrl.doubleSpriteHeight = (data >> 5) & 1; // 6th bit
	if(ppuCtrl.masterSlaveSelect == MASTERSLAVE_UNSET) // only set once.
		ppuCtrl.masterSlaveSelect = (data >> 6) & 1; // 7th bit
	ppuCtrl.executeNMIonVBLANK = (data >> 7) & 1; // 8th bit
}
/*
 * Sets the PPUMASK register values accordingly.
 */
void ppu_set_mask(BYTE data)
{
	// All 8 bits in order from lowest to highest.
	ppuMask.greyscale = data & 1;
	ppuMask.showBackgroundLeft = (data >> 1) & 1;
	ppuMask.showSpritesLeft = (data >> 2) & 1;
	ppuMask.showBackground = (data >> 3) & 1;
	ppuMask.showSprites = (data >> 4) & 1;
	ppuMask.emphasizeRed = (data >> 5) & 1;
	ppuMask.emphasizeGreen = (data >> 6) & 1;
	ppuMask.emphasizeBlue = (data >> 7) & 1;
}
/*
 * Sets the OAMADDR register which describes an address to read/write data from/to in OAM internal memory.
 */
void ppu_set_oamaddr(BYTE data)
{
	oamReadWriteAddress = data;
}
/*
 * Obtains the byte at OAMADDR within the OAM internal memory.
 */
BYTE ppu_get_oamdata()
{
	BYTE* dataArray = (BYTE*)objectAttributeMemory;
	return dataArray[oamReadWriteAddress];
}
/*
 * Sets the byte at OAMADDR within the OAM internal memory and increments OAMADDR.
 */
void ppu_set_oamdata(BYTE data)
{
	BYTE* dataArray = (BYTE*)objectAttributeMemory;
	dataArray[oamReadWriteAddress++] = data;
}
/*
 * Copies the entire OAM internal memory to the given CPU memory page number.
 */
void ppu_oam_dma(BYTE pageNumber)
{
	// Copy a memory page from CPU memory to OAM memory.
	BYTE* dataArray = (BYTE*)objectAttributeMemory;
	USHORT cpuMemAddr = pageNumber * MEMORY_PAGE_SIZE;
	for(USHORT i = 0; i < MEMORY_PAGE_SIZE; i++)
		dataArray[i] = cpu_read8(cpuMemAddr + i);
}
/*
 * Sets either X or Y scroll value depending on the PPU high/low toggle.
 */
void ppu_set_ppuscroll(BYTE data)
{
	// Set the appropriate scroll value based off of our write toggle
	if(!vramWriteToggle)
	{
		scrollY = data;
		if(scrollY >= RESOLUTION_HEIGHT)
			scrollY = 0;
	}
	else
	{
		scrollX = data;
	}

	// Toggle our write toggle.
	vramWriteToggle = !vramWriteToggle;
}
/*
 * Sets the either the high or low byte of the PPUADDR register (depending on PPU high/low toggle), used to read/write memory from PPU
 */
void ppu_set_ppuaddr(BYTE data)
{
	// Set the appropriate high/low byte for the PPU address based off write toggle
	if(vramWriteToggle)
		vramReadWriteAddress = (USHORT)data << 8;
	else
		vramReadWriteAddress = (vramReadWriteAddress & 0xFF00) | (USHORT)data;
	vramWriteToggle = !vramWriteToggle;
	// TODO: Check this. PPUSCROLL and PPUADDR use the same internal register and must be cleared after use, but we implement separately.
}
/*
 * Reads a byte from the PPU memory space at the given PPUADDR.
 */
BYTE ppu_get_data()
{
	// If we read in our palette memory range, we return that value immediately.
	// Otherwise we read to an internal memory buffer and return the previous buffer value.
	BYTE result = 0;
	USHORT vramRWAddrNM = ppu_get_non_mirrored_addr(vramReadWriteAddress);
	if(vramRWAddrNM >= PALETTE_ADDRS_START && vramRWAddrNM < PALETTE_ADDRS_END)
	{
		result = ppu_read8(vramReadWriteAddress);
	}
	else
	{
		result = vramReadValue;
		vramReadValue = ppu_read8(vramReadWriteAddress);
	}

	// Increment our address with our provided increment value.
	vramReadWriteAddress += ppuCtrl.vramAddrIncrements32 ? 32 : 1;
	return result;
}
/*
 * Writes a byte to the PPU memory space at the given PPUADDR.
 */
void ppu_set_data(BYTE data)
{
	// Write the given data.
	ppu_write8(vramReadWriteAddress, data);

	// Increment our address with our provided increment value.
	vramReadWriteAddress += ppuCtrl.vramAddrIncrements32 ? 32 : 1;
}
/*
 * Obtains the color for the given color index in our palette.
 * (Adjusts according to monochrome/greyscale flag in PPUMASK)
 */
UINT ppu_get_color(BYTE paletteColorIndex)
{
	// Retrieve the color (with a bounded index, ignore all upper bits out of range).
	UINT paletteColor = palette[paletteColorIndex & 0x3F];

	// Handle PPUMASK's greyscale flag as well.
	if(ppuMask.greyscale || forceGreyscale)
	{
		// Average the color components for greyscale
		paletteColor = ((paletteColor >> 8) & 0xFF) + ((paletteColor >> 16) & 0xFF) + ((paletteColor >> 24) & 0xFF);
		paletteColor = ((paletteColor / 3) * 0x01010100) | 0xFF;
	}

	return paletteColor;
}

/*
 * Updates the framebuffer by drawing the appropriate sprite layer on it.
 * The given priority is either 0 (top layer), 1 (behind background) and is determined in OAM Entry attributes.
 */
void ppu_draw_sprites(BYTE priority)
{
	/*
	 * References:
	 * https://wiki.nesdev.com/w/index.php/PPU_OAM
	 */
	// Calculate our sprite height based off of our flags.
	BYTE spriteHeight = SPRITE_HEIGHT;
	if(ppuCtrl.doubleSpriteHeight)
		spriteHeight *= 2;

	// Items earlier in the OAM table are drawn in front.
	for(int spriteIndex = OAM_TABLE_COUNT - 1; spriteIndex >= 0; spriteIndex--)
	{
		// Obtain all relevant values for rendering.
		struct OAMEntry oamEntry = objectAttributeMemory[spriteIndex];
		BYTE y = oamEntry.Y + 1; // y is off by 1.
		BYTE x = oamEntry.X;

		// If the current sprite isn't on this scanline, skip to the next sprite.
		if(y > currentScanline || (y + spriteHeight <= currentScanline))
			continue;

		// Obtain our flags from our attributes byte
		BYTE paletteIndex = oamEntry.attributes & 3;
		BYTE spritePriority = (oamEntry.attributes >> 5) & 1;

		// If we're not rendering with the appropriate priority, skip to the next sprite.
		if(spritePriority != priority)
			continue;

		// At this point we know we'll render the sprite on this scanline.
		spritesOnCurrentLine++;

		// If we already have 8 sprites drawn, stop drawing
		// TODO: This is supposed to be implemented in this fashion, yet gives undesired effects..? Used documentation was likely wrong.
		//if(spritesOnCurrentLine > 8)
		//	return;

		// Obtain our remaining flags.
		BOOL flipHorizontal = (oamEntry.attributes >> 6) & 1;
		BOOL flipVertical = (oamEntry.attributes >> 7);

		// Depending on if we have a double-height sprite or not, our drawing will differ.
		// Double height sprites are actually two sprites, but flipping them takes into account the total height.
		// So first determine which line to draw.
		BYTE spriteID = oamEntry.ID;
		UINT spriteLineY;
		if(!flipVertical)
			spriteLineY = currentScanline - y;
		else
			spriteLineY = (y + spriteHeight) - (currentScanline + 1);


		// Next determine the offset of our tile in our pattern table.
		// 8x8 sprites use PPUCTRL for pattern table index, 8x16 use bit 0 of ID.
		UINT patternTableIndex = ppuCtrl.spritePatternTableIndex;
		if(ppuCtrl.doubleSpriteHeight)
		{
			// Pattern table index comes from bit 0 of our ID for 8x16 tiles.
			patternTableIndex = spriteID & 1;
			// Our ID is the 7 upper bits only then.
			spriteID &= ~1;

			// If we are rendering an 8x16 sprite, and it's flipped, it's with respect to total height, as mentioned.
			// If our line for this sprite is in the other sprite,
			if(spriteLineY >= 8)
			{
				spriteID++;
				spriteLineY -= 8;
			}
		}

		// Our ID is our index, each 8x8 pixel tile is represented as 0x10 bytes (look at background rendering method for more info).
		UINT patternTableTileOffset = (spriteID * 0x10) + spriteLineY;

    	// Obtain the low and high byte from the pattern table which describe the colors for the 8x8 tile.
    	// Each 8x1 row comes from 2 bytes (16 bits). Each pixel is 2 bits, one bit from each byte.
		assert(patternTableTileOffset < PATTERN_TABLE_SIZE, "Pattern Table access out of bounds.");
    	BYTE tileColorLowByte = patternTables[patternTableIndex][patternTableTileOffset];
    	BYTE tileColorHighByte = patternTables[patternTableIndex][patternTableTileOffset + 8];

    	// Loop for every pixel to render.
    	for(int spritePixelX = 0; spritePixelX < SPRITE_WIDTH; spritePixelX++)
    	{
    		// If our X coordinate we're drawing to is outside of the frameBuffer, stop.
    		// If we're rendering first 8 pixels of screen and we're not meant to for sprites, stop.
    		if(x + spritePixelX >= RESOLUTION_WIDTH)
    			continue;
    		else if((x + spritePixelX) < 8 && !ppuMask.showSpritesLeft)
    			continue;

    		// Our color index is two bits, a high bit from high byte, low bit from low byte.
    		// The higher bits are for lower pixel indexes.
    		// We take into account our flip horizontal flag here to flip the color to obtain.
    		BYTE pixelIndex = flipHorizontal ? (7 - spritePixelX) : spritePixelX;
    		BYTE colorIndex = (tileColorLowByte >> (7 - pixelIndex)) & 1;
    		colorIndex |= ((tileColorHighByte >> (7 - pixelIndex)) & 1) << 1;

    		// Color offset 0 means transparent, anything else indexes into the target sprite color palettes colors.
    		if(colorIndex > 0)
    		{
    			// If we're rendering our first sprite's scanline, set our flag.
    			if(spriteIndex == 0)
    				ppuStatus.sprite0Hit = TRUE;

    			// Read the color index which indexes into the NES internal color palette, and obtain the color.
    			BYTE paletteColorIndex = spriteColorPalette[paletteIndex].paletteColorIndex[colorIndex - 1] & 0x3F;
    			UINT rgbaColor =  ppu_get_color(paletteColorIndex);

				// Determine our position in the frame buffer
				UINT frameBufferRowOffset = currentScanline * RESOLUTION_WIDTH;
				UINT frameBufferPixelOffset = frameBufferRowOffset + x + spritePixelX;

				// Output our pixel.
				assert(sizeof(internalFrameBuffer) > frameBufferPixelOffset, "Internal framebuffer drawing out of bounds.");
				internalFrameBuffer[frameBufferPixelOffset] = rgbaColor;
    		}
    	}
	}
}
/*
 * Updates the framebuffer by drawing the background layer onto it.
 */
void ppu_draw_background()
{
	/*
	 * Notes:
	 * -Two pattern tables, one describes sprites, the other background (determined by programmer).
	 * -Screen is made up of 8x8 pixel tiles. (Width = 256px = 32 blocks, Height = 240 = 30 blocks).
	 * -Pattern Table (an array of tiles/icons):
	 * 		-Each entry is an 8x8 tile using 16 bytes.
	 * 		-Each 8x1 row is 2 bytes (from the 16 mentioned), the first byte @ rowIndex, the second @ rowIndex+8
	 * 		-Each pixel is 2 bits, the first bit comes from the first byte, the second bit comes from the second byte.
	 * 			-So of those 2 bytes mentioned, one describes all high bits, one describes low bits for the 2 bit representation of a pixel.
	 * 		-That 2-bit number allows for 4 different colors.
	 * -Nametable (canvas indexing tiles and colors to use):
	 * 		-A nametable has 960 cells (32x30 tiles = 256x240 pixels).
	 * 		-A cell is a byte, able to index a 8x8 tile in the pattern table.
	 *	-Attribute table (describes color palette used tiles):
	 *		-64 bytes, each byte describes 4x4 tiles (32x32 pixels).
	 *		-Each 2x2 tile is 2 bits then, describes which palette index to use.
	 */

	/*
	 * References:
	 * https://opcode-defined.quora.com/How-NES-Graphics-Work-Pattern-tables
	 * https://opcode-defined.quora.com/How-NES-Graphics-Work-Nametables
	 * https://wiki.nesdev.com/w/index.php/PPU_attribute_tables
	 */
	// Obtain our name table
	UINT y = currentScanline + scrollY;

    // Each scanline can have a maximum of two nametables it enters
    // (it can enter another nametable horizontally depending on scrollX)
    UINT nameTableIndexes[2];
    nameTableIndexes[0] = ppuCtrl.baseNameTableIndex;
    // If we overflow past our base name table into another via Y coordinate, calculate our indexes.
    if(y >= RESOLUTION_HEIGHT)
    {
    	// We're in the bottom left instead of top right
    	nameTableIndexes[0] += 2;
    	nameTableIndexes[0] %= 4;
    }
    // The second table is the one adjacent to the first.
    nameTableIndexes[1] = ((nameTableIndexes[0] / 2) * 2) + (1 - (nameTableIndexes[0] % 2));

    // Figure out our column numbers (normally 0-32) with respect to wrapping into another nametable (0-64).
    UINT startColumn = scrollX / PATTERN_TABLE_TILE_WIDTH;
    UINT endColumn = startColumn + RESOLUTION_TILES_WIDTH;

    for (UINT columnIndex = startColumn; columnIndex < endColumn + 1; columnIndex++)
    {
    	// TODO: Revisit this, maybe timing is wrong but StarsSE toggles this and it causes a stuttering effect. Not important for now.
    	// If we're not supposed to render the first 8 pixel tiles of the background, and we are the first 8 pixel wide tile, skip.
    	//if(columnIndex == startColumn && !ppuMask.showBackgroundLeft)
    	//	continue;

    	// Determine if we're rendering in the first or second nametable (can be two if we scrolled).
    	UINT nameTableIndexesIndex = columnIndex >= RESOLUTION_TILES_WIDTH;
    	UINT nameTableIndex = nameTableIndexes[nameTableIndexesIndex];
    	UINT columnInNametable = columnIndex % RESOLUTION_TILES_WIDTH;
    	UINT tileIndexY = (y / PATTERN_TABLE_TILE_HEIGHT);

    	// Obtain the offset of the nametable cell which points to an index in the pattern table which describes an 8x8 tile.
    	UINT nameTableCellOffset = (tileIndexY * RESOLUTION_TILES_WIDTH) + columnInNametable;
    	UINT patternTableTileIndex = nameTables[nameTableIndex]->cells[nameTableCellOffset];
    	UINT patternTableTileOffset = (patternTableTileIndex * 0x10) + (y % PATTERN_TABLE_TILE_HEIGHT);

    	// Determine which palette to use by obtaining an attribute byte.
    	// Every attribute byte describes 4x4 tiles (32x32 pixels), we divide by 4 to get from tile indexes to attribute byte positions.
		// Every row is 32 tiles, so 8 attribute bytes. Using column number as X coordinate, we can obtain the appropriate attribute byte.
		// Recall, this byte describes 4x4 tiles, so each 2x2 tile's palette index is 2 bits in order from lowest to highest: top left, top right, bottom left, bottom right.
		// We shift according to which tile we're in (2 * positionIndex) where positionIndex is the index in the positions mentioned above.
		// Since positionIndex can be 0-3, we use 2 bits, the high bit can be determined if we're in the second nametable on the Y axis, the low bit by the same on X axis.
    	BYTE paletteIndex = nameTables[nameTableIndex]->attributes[((tileIndexY / 4) * (32 / 4)) + (columnInNametable / 4)];
		paletteIndex = (BYTE)(paletteIndex >> ((2 * ((columnInNametable % 4) / 2)) | (4 * ((tileIndexY % 4) / 2)))) & 3;

    	// Obtain the low and high byte from the pattern table which describe the colors for the 8x8 tile.
    	// Each 8x1 row comes from 2 bytes (16 bits). Each pixel is 2 bits, one bit from each byte.
    	BYTE tileColorLowByte = patternTables[ppuCtrl.backgroundPatternTableIndex][patternTableTileOffset];
    	BYTE tileColorHighByte = patternTables[ppuCtrl.backgroundPatternTableIndex][patternTableTileOffset + 8];

    	// Determine which pixels of this tile to render.
    	// If we're on the first or last tile, we can be offset by a few pixels.
    	// Tiles are 8 pixels, so determining offset is simple by using scroll value.
    	BYTE startPixel = 0;
        BYTE endPixel = 8;
        if(columnIndex == startColumn)
        	startPixel = scrollX % PATTERN_TABLE_TILE_WIDTH;
        else if(columnIndex == endColumn)
        	endPixel = scrollX % PATTERN_TABLE_TILE_WIDTH;

        // Loop for each pixel in the row to render.
        for (int pixelIndex = startPixel; pixelIndex < endPixel; pixelIndex++)
        {
        	// Our color index is two bits, a high bit from high byte, low bit from low byte.
        	// The higher bits are for lower pixel indexes.
        	BYTE colorIndex = (tileColorLowByte >> (7 - pixelIndex)) & 1;
        	colorIndex |= ((tileColorHighByte >> (7 - pixelIndex)) & 1) << 1;

        	// Color offset 0 means transparent, anything else indexes into the target background color palettes colors.
            if(colorIndex > 0)
            {
            	// Read the color index which indexes into the NES internal color palette, and obtain the color.
            	BYTE paletteColorIndex = backgroundColorPalette[paletteIndex].paletteColorIndex[colorIndex - 1] & 0x3F;
				UINT rgbaColor =  ppu_get_color(paletteColorIndex);

				// Determine our position in the frame buffer
				UINT frameBufferRowOffset = currentScanline * RESOLUTION_WIDTH;
				UINT frameBufferTileOffset = frameBufferRowOffset + (8 * columnInNametable);
				UINT frameBufferPixelOffset;

				// If we are rendering in the second name table, our scroll comes from the other end.
				if (nameTableIndexesIndex == 0)
					frameBufferPixelOffset = frameBufferTileOffset + pixelIndex - scrollX;
				else
					frameBufferPixelOffset = frameBufferTileOffset + pixelIndex + (RESOLUTION_WIDTH - scrollX);

				// Output our pixel.
				assert(sizeof(internalFrameBuffer) > frameBufferPixelOffset, "Internal framebuffer drawing out of bounds.");
				internalFrameBuffer[frameBufferPixelOffset] = rgbaColor;
            }
        }
    }
}
/*
 * Updates the PPU (as if one cycle had occurred).
 */
void ppu_update()
{
	// Increment another cycle, determine if we finished rendering our current scanline.
	// We control timing through scanline number, rendering a scanline at a time, executing NMI on the appropriate scan line number.
	// NOTE: There is more scanlines/cycles tracked beyond the height, where we perform other actions.
	ppuCycles++;
	if(ppuCycles == PPU_CYCLES_PER_SCANLINE)
	{
		// Increment our scanline number, reset our cycles we're keeping track of
		ppuCycles = 0;
		currentScanline++;

		if(currentScanline < RESOLUTION_HEIGHT)
		{
			// Reset our sprite count for the current line.
			spritesOnCurrentLine = 0;

			// Start by setting the entire scanline color to the background color.
			UINT color = ppu_get_color(universalBackgroundColor);
			for(UINT i = 0; i < RESOLUTION_WIDTH; i++)
				internalFrameBuffer[(currentScanline * RESOLUTION_WIDTH) + i] = color;

			if (ppuMask.showSprites)
			{
				// Draw the sprites that are behind the background.
				ppu_draw_sprites(1);
			}
			if(ppuMask.showBackground)
			{
				// Draw background for this scanline.
				ppu_draw_background();
			}
			if(ppuMask.showSprites)
			{
				// Draw the sprites that are in front of the background.
				ppu_draw_sprites(0);
			}
			ppuStatus.spriteOverflow = spritesOnCurrentLine > 8;
		}
		else if(currentScanline == RESOLUTION_HEIGHT)
		{
			// Copy the final frame (reverse), and render.
			for(UINT y = 0; y < RESOLUTION_HEIGHT; y++)
				for(UINT x = 0; x < RESOLUTION_WIDTH; x++)
					frameBuffer[((RESOLUTION_HEIGHT - (y + 1)) * RESOLUTION_WIDTH)+x] = internalFrameBuffer[(y * RESOLUTION_WIDTH) + x];
			frameCount++;
		}
		else if(currentScanline == SCANLINES_PER_FRAME)
		{
			// We completed a frame, reset some variables.
			currentScanline = 0;
			ppuStatus.sprite0Hit = FALSE;
		}

		// Check if we're to handle our NMI interrupt (V-Blank).
		ppuStatus.verticalBlanking = (currentScanline == SCANLINES_PER_VBLANK);
		if(ppuStatus.verticalBlanking && ppuCtrl.executeNMIonVBLANK)
			interrupts.requestedNMI = TRUE;
	}
}

// ---------------------------------
// Data
// ---------------------------------
// RGBA color palette, makes indexing colors easier since we use an RGBA framebuffer.
UINT palette[] = {
	0x808080FF, 0x0000B8FF, 0x3000B8FF, 0x8000A0FF, 0xB80068FF, 0xB00018FF, 0xB00000FF, 0x902400FF,
	0x782800FF, 0x003C00FF, 0x004808FF, 0x003C20FF, 0x002C60FF, 0x000000FF, 0x000400FF, 0x000400FF,
	0xC8C8C8FF, 0x0058F8FF, 0x403CF8FF, 0xB030C8FF, 0xF830A8FF, 0xF83458FF, 0xF83418FF, 0xD04800FF,
	0xC06000FF, 0x387800FF, 0x188410FF, 0x009460FF, 0x0084C0FF, 0x101010FF, 0x080808FF, 0x080808FF,
	0xF8FCF8FF, 0x0094F8FF, 0x6884F8FF, 0xD06CF8FF, 0xF874C8FF, 0xF86C98FF, 0xF87858FF, 0xF89058FF,
	0xF8A030FF, 0xA0BC00FF, 0x50D868FF, 0x48D4A8FF, 0x00D8F8FF, 0x606460FF, 0x080C08FF, 0x080C08FF,
	0xF8FCF8FF, 0x80BCF8FF, 0xB8B8F8FF, 0xD0B8F8FF, 0xF8BCE8FF, 0xF8BCC8FF, 0xF8C4B0FF, 0xF8CCA8FF,
	0xF8D8A0FF, 0xC8E098FF, 0xA8ECB0FF, 0xA8F4E8FF, 0xB0ECF8FF, 0xD8DCD8FF, 0x101010FF, 0x101010FF
};
UINT paletteCount = sizeof(palette) / sizeof(UINT);
USHORT nameTableSize = sizeof(struct NAMETABLE);
