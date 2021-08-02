#ifndef _G_DEBUG_H_
#define _G_DEBUG_H_

#include <gccore.h>
#include <ogc/lwp_watchdog.h>   // Needed for gettime and ticks_to_millisecs
#include <wiiuse/wpad.h>
#include <stdarg.h>

u8 CalculateFrameRate();
void print_wiimote_connection_status(int wiimote_connection_status, int channel);
void print_wiimote_buttons(WPADData *wd);
void SetTextCursorPos(f32 x, f32 y);

/**
 * @param text Text to draw.
 * @param ... Optional arguments.
**/
void Printf(const char* text, ...);

#endif