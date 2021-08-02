#include "g_debug.h"
#include "g_font.h"

f32 cursorX = 0;
f32 cursorY = 0;

u8 FPS = 0;

extern GRRLIB_texImg *tex_font;

/**
 * This function calculates the number of frames we render each second.
 * @return The number of frames per second.
 */
u8 CalculateFrameRate() {
    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    u32 currentTime = ticks_to_millisecs(gettime());

    frameCount++;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
    return FPS;
}

void print_wiimote_connection_status(int wiimote_connection_status, int channel) {
	switch(wiimote_connection_status) {
		case WPAD_ERR_NO_CONTROLLER:
			// printf("channel %d Wiimote not connected\n", channel);
			Printf("channel %d Wiimote not connected\n", channel);
		break;
		case WPAD_ERR_NOT_READY:
			// printf("channel %d Wiimote not ready\n", channel);
			Printf("channel %d Wiimote not ready\n", channel);
			break;
		case WPAD_ERR_NONE:
			// printf("channel %d Wiimote ready\n", channel);
			Printf("channel %d Wiimote ready\n", channel);
			break;
		default:
			// printf("channel %d Unknown Wiimote state %d\n", channel, wiimote_connection_status);
			Printf("channel %d Unknown Wiimote state %d\n", channel, wiimote_connection_status);
	}
}

void print_wiimote_buttons(WPADData *wd)
{
	Printf(" BUTTONS DOWN:\n ");
	if(wd->btns_h & WPAD_BUTTON_A)
		Printf("A ");
	if(wd->btns_h & WPAD_BUTTON_B) 
		Printf("B ");
	if(wd->btns_h & WPAD_BUTTON_1) 
		Printf("1 ");
	if(wd->btns_h & WPAD_BUTTON_2) 
		Printf("2 ");
	if(wd->btns_h & WPAD_BUTTON_MINUS) 
		Printf("MINUS ");
	if(wd->btns_h & WPAD_BUTTON_HOME) 
		Printf("HOME ");
	if(wd->btns_h & WPAD_BUTTON_PLUS) 
		Printf("PLUS ");
	
	Printf("\n ");

	if(wd->btns_h & WPAD_BUTTON_LEFT) 
		Printf("LEFT ");
	if(wd->btns_h & WPAD_BUTTON_RIGHT) 
		Printf("RIGHT ");
	if(wd->btns_h & WPAD_BUTTON_UP)    
		Printf("UP ");
	if(wd->btns_h & WPAD_BUTTON_DOWN)  
		Printf("DOWN ");
	
	Printf("\n");
}

void SetTextCursorPos(f32 x, f32 y)
{
	cursorX = x;
	cursorY = y;
}

/**
 * @param text Text to draw.
 * @param ... Optional arguments.
**/
void Printf(const char* text, ...)
{
	va_list argp;
    va_start(argp, text);
	GRRLIB_Printf(cursorX, cursorY, tex_font, 0xFFFFFFFF, 1, text, argp);
    va_end(argp);
	cursorY += 20;
}
