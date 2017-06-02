/*
 ============================================================================
 Name        : NESsys.c
 Author      : David Pokora
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "NESsys.h"

#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "game_base.h"
#include "input.h"
#include "memory.h"
#include "ppu.h"
#include "tests.h"

enum CTXMENU_CPUOPTIONS { CTXMENU_RESUME, CTXMENU_PAUSE, CTXMENU_RESTART };
enum CTXMENU_SPEEDOPTIONS { CTXMENU_SPEED1, CTXMENU_SPEED2, CTXMENU_SPEED5, CTXMENU_SPEED100 };
enum CTXMENU_PPUOPTIONS { CTXMENU_GREYSCALE };
enum CTXMENU_GREYSCALEOPTIONS { CTXMENU_GREYSCALE_ON, CTXMENU_GREYSCALE_OFF };


/*
 * Prints a string onto the screen from -1.0f to 1.0f (translated with respect to the current view matrix)
 */
void WriteString2f(float x, float y, char* s, void* font)
{
	glRasterPos2f(x, y);
	int len = strlen(s);
	for(int i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, s[i]);
	}
}
/*
 * Prints a string onto the screen at the given absolute coordinates.
 */
void WriteStringAbs(int x, int y, char* s, void* font)
{
	// We are given absolute coordinates with 0,0 being top right.
	float fx = ((x - (windowWidth / 2.0f)) / windowWidth) * 2;
	float fy = -((y - (windowHeight / 2.0f)) / windowHeight) * 2;
	WriteString2f(fx,fy, s, font);
}
/*
 * GLUT callback to draw to the screen.
 */
void game_display(void)
{
	// Clear the background with the supplied color.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw the pixels appropriately.
	// TODO: Scaling/positioning.
	glLoadIdentity();
	glRasterPos2f(-1,-1);
	glDrawPixels(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, frameBuffer);

	// Draw an overlay
	char overlayStr[0x100];
	glColor3ub(0, 255, 0);
	snprintf(overlayStr, sizeof(overlayStr), "Speed: %.0f%%", ((double)cpuCyclesLastSecond / CPU_CYCLES_PER_SECOND) * 100);
	WriteStringAbs(0, 12, overlayStr, GLUT_BITMAP_HELVETICA_12);

	glColor3ub(255, 0, 0);
	snprintf(overlayStr, sizeof(overlayStr), "FPS: %i", framesPerSecond);
	WriteStringAbs(0, 24, overlayStr, GLUT_BITMAP_HELVETICA_12);

	glFlush(); // Force changes
}
/*
 * Executes the game on the current thread.
 */
void game_start(void)
{
	do
	{
		// Detect reset and clear reset flag.
		if(cpuRestarting)
			debug_log("REBOOTING...\n");
		cpuRestarting = FALSE;

		// Initialize hardware.
		cpu_init();
		ppu_init();
		framesPerSecond = 60;

		// Enter the game's main entry point (reset interrupt handler).
		debug_log("Entering game's reset interrupt handler.\n");
		game_execute(ID_FUNCTION_RESET);
		debug_log("Exiting game's reset interrupt handler.\n");
	}
	while(cpuRestarting);
}
/*
 * CPU sync callback, happens before/after instructions execute.
 */
void game_on_cpu_sync()
{
	// Occurs when CPU calls sync, which happens before/after instruction executions to call on PPU.
}

/*
 * Window resizing event
 */
void window_resize(int w, int h)
{
	// Set our dimensions
	windowWidth = w;
	windowHeight = h;
}
/*
 * Window closing event
 */
void window_closing(void)
{
	// Exit the application, killing all threads.
	exit(EXIT_SUCCESS);
}
/*
 * Starts a recursive timed loop to redisplay the screen a given amount of times per second.
 * (should only be called once)
 */
void update_game_display_async(int value)
{
	// Base rendering speed off of our FPS count for the previous second (we try to redraw at least once, because this timer needs to keep going).
	glutPostRedisplay();
	glutTimerFunc((int)((1 / (double)max(framesPerSecond, 1)) * 1000), update_game_display_async, value);
}
/*
 * Handle our CPU menu events
 */
void context_menu_cpu_handler(int menu)
{
	switch(menu)
	{
		case CTXMENU_RESUME:
			cpuPaused = FALSE;
			break;
		case CTXMENU_PAUSE:
			cpuPaused = TRUE;
			break;
		case CTXMENU_RESTART:
			cpuRestarting = TRUE;
			break;

	}
}
/*
 * Handle our speed menu events
 */
void context_menu_speed_handler(int menu)
{
	switch(menu)
	{
		case CTXMENU_SPEED1:
			cpuSpeedMultiplier = 1.0f;
			break;
		case CTXMENU_SPEED2:
			cpuSpeedMultiplier = 2.0f;
			break;
		case CTXMENU_SPEED5:
			cpuSpeedMultiplier = 5.0f;
			break;
		case CTXMENU_SPEED100:
			cpuSpeedMultiplier = 100.0f;
			break;

	}
}
/*
 * Handle our speed menu events
 */
void context_menu_greyscale_handler(int menu)
{
	switch(menu)
	{
		case CTXMENU_GREYSCALE_ON:
			forceGreyscale = TRUE;
			break;
		case CTXMENU_GREYSCALE_OFF:
			forceGreyscale = FALSE;
			break;
	}
}
/*
 * The main entry point for the application.
 */
int main(int argc, char **argv)
{
	// Print a welcome message.
	console_log("%s %.2f by David Pokora\n", APPLICATION_NAME, APPLICATION_VERSION);
	console_log("---------------------------\n");
	console_log("WARNING: This application was converted by a tool with a narrow usage scope.\n");
	console_log("It was created for educational purposes and aims to explore the topic of compilers.\n");
	console_log("As such, it does not aim to support all ROMs, let alone accurately with those that do.\n");
	console_log("You may encounter unexpected behavior.\n");
	console_log("---------------------------\n");

	// If we're in testing mode, run the appropriate tests.
	if(APPLICATION_TESTING_MODE)
	{
		test_all();
		exit(EXIT_SUCCESS);
	}

	// Set up the GUI for the game.
	glutInit(&argc, argv);
	glutCreateWindow(APPLICATION_WINDOW_TEXT);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(256, 240);
	glutInitWindowPosition(50, 50);
	glutDisplayFunc(game_display);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutKeyboardFunc(key_down);
	glutSpecialFunc(special_key_down);
	glutKeyboardUpFunc(key_up);
	glutSpecialUpFunc(special_key_up);
	glutReshapeFunc(window_resize);
	glutCloseFunc(window_closing);

	// Create our context menus
	int ctxMenuCPU = glutCreateMenu(context_menu_cpu_handler);
	glutAddMenuEntry("Resume", CTXMENU_RESUME);
	glutAddMenuEntry("Paused", CTXMENU_PAUSE);
	glutAddMenuEntry("Restart", CTXMENU_RESTART);

	int ctxMenuSpeed = glutCreateMenu(context_menu_speed_handler);
	glutAddMenuEntry("1x", CTXMENU_SPEED1);
	glutAddMenuEntry("2x", CTXMENU_SPEED2);
	glutAddMenuEntry("5x", CTXMENU_SPEED5);
	glutAddMenuEntry("100x", CTXMENU_SPEED100);

	int ctxMenuGreyscale = glutCreateMenu(context_menu_greyscale_handler);
	glutAddMenuEntry("On", CTXMENU_GREYSCALE_ON);
	glutAddMenuEntry("Off", CTXMENU_GREYSCALE_OFF);

	int ctxMenuPPU = glutCreateMenu(NULL);
	glutAddSubMenu("Force Greyscale", ctxMenuGreyscale);

	int windowContextMenu = glutCreateMenu(NULL);
	glutAddSubMenu("CPU", ctxMenuCPU);
	glutAddSubMenu("PPU", ctxMenuPPU);
	glutAddSubMenu("Speed", ctxMenuSpeed);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Add our events for the game.
	onCpuSync = game_on_cpu_sync;

	// Start the game thread
	gameThread = create_thread(game_start);

	// Try to render from framebuffer with the given fps.
	framesPerSecond = 60;
	update_game_display_async(0);

	// Continue processing UI events.
	glutMainLoop();

	return EXIT_SUCCESS;
}
