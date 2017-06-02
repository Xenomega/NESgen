#include "cpu.h"
#include "input.h"
#include "NESsys.h"

BOOL inputStrobe; // indicates if we are polling states, or allowing the sequential read of bits out of them.
BOOL realtimeStates[2][NES_INPUT_CONTROLLER_BUTTONS]; // two controllers, eight buttons per.
BYTE inputRegisters[2]; // two controllers

/*
 * Set the key state for the given key.
 */
void set_key_state(unsigned char key, BOOL state)
{
	// Controller 1
	if(key == 'w')
		realtimeStates[0][NES_INPUT_KEY_INDEX_UP] = state;
	else if(key == 's')
		realtimeStates[0][NES_INPUT_KEY_INDEX_DOWN] = state;
	else if(key == 'a')
		realtimeStates[0][NES_INPUT_KEY_INDEX_LEFT] = state;
	else if(key == 'd')
		realtimeStates[0][NES_INPUT_KEY_INDEX_RIGHT] = state;
	else if(key == 'm')
		realtimeStates[0][NES_INPUT_KEY_INDEX_A] = state;
	else if(key == 'n')
		realtimeStates[0][NES_INPUT_KEY_INDEX_B] = state;
	else if(key == 'j')
		realtimeStates[0][NES_INPUT_KEY_INDEX_SELECT] = state;
	else if(key == 'k')
		realtimeStates[0][NES_INPUT_KEY_INDEX_START] = state;
	// Controller 2
	else if(key == '3')
		realtimeStates[1][NES_INPUT_KEY_INDEX_A] = state;
	else if(key == '2')
		realtimeStates[1][NES_INPUT_KEY_INDEX_B] = state;
	else if(key == '5')
		realtimeStates[1][NES_INPUT_KEY_INDEX_SELECT] = state;
	else if(key == '6')
		realtimeStates[1][NES_INPUT_KEY_INDEX_START] = state;
}
/*
 * Set the key state for the given key.
 */
void set_special_key_state(int key, BOOL state)
{
	if(state)
	{
		// Conditions where things are done on special key down
		if(key == GLUT_KEY_F5)
			cpuRestarting = TRUE;
	}
	else
	{
		// Conditions where things are done on special key up
	}

	// Controller 2 directions
	if(key == GLUT_KEY_UP)
		realtimeStates[1][NES_INPUT_KEY_INDEX_UP] = state;
	else if(key == GLUT_KEY_DOWN)
		realtimeStates[1][NES_INPUT_KEY_INDEX_DOWN] = state;
	else if(key == GLUT_KEY_LEFT)
		realtimeStates[1][NES_INPUT_KEY_INDEX_LEFT] = state;
	else if(key == GLUT_KEY_RIGHT)
		realtimeStates[1][NES_INPUT_KEY_INDEX_RIGHT] = state;

}
/*
 * Key-down handler for glut.
 */
void key_down(unsigned char key, int x, int y)
{
	set_key_state(key, TRUE);
}
/*
 * Special key-down handler for glut.
 */
void special_key_down(int key, int x, int y)
{
	set_special_key_state(key, TRUE);
}
/*
 * Key-up handler for glut.
 */
void key_up(unsigned char key, int x, int y)
{
	set_key_state(key, FALSE);
}
/*
 * Special key-up handler for glut.
 */
void special_key_up(int key, int x, int y)
{
	set_special_key_state(key, FALSE);
}

/*
 * Refreshes the input registers with the appropriate button states.
 */
void update_input_registers()
{
	// For each controller
	for(int i = 0; i < NES_INPUT_CONTROLLER_COUNT; i++)
	{
		// Set the input register to 0
		inputRegisters[i] = 0;

		// Place a bit for every controller in our input register byte.
		for(int x = 0; x < NES_INPUT_CONTROLLER_BUTTONS; x++)
			inputRegisters[i] |= (realtimeStates[i][x] << x);
	}
}
/*
 * Reads input data for the given controller index.
 */
BYTE read_input(BYTE controllerIndex)
{
	// If our strobe is high, our registers should be update to date.
	if(inputStrobe)
		update_input_registers();

	// Obtain our input status
	BYTE result = inputRegisters[controllerIndex] & 1;

	// If the strobe is low, we shift every read
	if(!inputStrobe)
		inputRegisters[controllerIndex] >>= 1;

	// Return the result
	return result;
}

/*
 * Causes button registers to be updated with the current key states if the strobe bit is set in the provided data.
 */
void write_input_strobe(BYTE data)
{
	// Set the strobe to from the given data appropriately.
	// When strobe is high (1), button registers will continuously reload on read.
	// When it's low (0), they will shift every read, allowing all the button states to be read.
	// So when we go from high to low, update our input registers.
	BOOL newStrobe = data & 1;
	if(inputStrobe && !newStrobe)
		update_input_registers();
	inputStrobe = newStrobe;
}
