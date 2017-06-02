// References:
// https://wiki.nesdev.com/w/index.php/Standard_controller
#ifndef INPUT_H_
#define INPUT_H_
#include "platform.h"

// ---------------------------------
// Input Definitions
// ---------------------------------
// The bit positions of the controls in the input state byte.
#define NES_INPUT_KEY_INDEX_A			0
#define NES_INPUT_KEY_INDEX_B			1
#define NES_INPUT_KEY_INDEX_SELECT		2
#define NES_INPUT_KEY_INDEX_START		3
#define NES_INPUT_KEY_INDEX_UP			4
#define NES_INPUT_KEY_INDEX_DOWN		5
#define NES_INPUT_KEY_INDEX_LEFT		6
#define NES_INPUT_KEY_INDEX_RIGHT		7

#define NES_INPUT_CONTROLLER_COUNT		2
#define NES_INPUT_CONTROLLER_BUTTONS	8



// ---------------------------------
// Functions
// ---------------------------------
void key_down(unsigned char key, int x, int y);
void special_key_down(int key, int x, int y);
void key_up(unsigned char key, int x, int y);
void special_key_up(int key, int x, int y);
BYTE read_input(BYTE controllerIndex);
void write_input_strobe(BYTE data);


#endif /* INPUT_H_ */
