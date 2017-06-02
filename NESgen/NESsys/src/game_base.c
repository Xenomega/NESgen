#include "game_base.h"

#if APPLICATION_USE_TEMPLATE
#include <stdio.h>

// ---------------------------------
// Functions
// ---------------------------------
BOOL game_execute(USHORT jumpAddress)
{
	UINT test = 0;

	// Switch table for functions here first. ID can be memory offset for compatibility in case of rough runtime calculation support.
	// This is necessary so we can call game_execute(functionID) when we have a JSR
	switch(functionID)
	{
	case ID_FUNCTION_RESET:
		goto FUNCTION_RESET;
	case ID_FUNCTION_NMI:
		goto FUNCTION_NMI;
	case ID_FUNCTION_IRQ:
		goto FUNCTION_IRQ;
	}

	FUNCTION_RESET:
	test++;
	FUNCTION_NMI:
	test++;
	FUNCTION_IRQ:
	test++;
	return FALSE;
}

// ---------------------------------
// Data
// ---------------------------------
enum MIRRORINGTYPE mirroringType = VERTICAL;
struct TLBEntry gameTLB[] =
{
		{0,0,0} // dummy so certain compilers don't complain about this template.
};
UINT gameTLBSize = sizeof(gameTLB) / sizeof(struct TLBEntry);

BYTE chrRom[] = { };
UINT chrRomSize = sizeof(chrRom);
#endif
