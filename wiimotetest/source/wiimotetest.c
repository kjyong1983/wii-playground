//code by WinterMute
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <wiiuse/wpad.h>
 
static GXRModeObj *rmode = NULL;
 
//-----------------------------------------------------------------------------------
//doreload - a flag that tells the program to quit looping and exit the program to the HBChannel.
//dooff - a flag that tells the program to quit looping and properly shutdown the system.
int doreload=0, dooff=0;
 
//Calling the function will end the while loop and properly exit the program to the HBChannel. 
void reload(void) {
	doreload=1;
}
 
//Calling the function will end the while loop and then properly shutdown the system 
//with SYS_ResetSystem(SYS_SHUTDOWN,0,0) (see the last if statement).
void shutdown(void) {
	dooff=1;
}
 
//Draw a square on the screen (May draw rectangles as well, I am uncertain). 
//*xfb - framebuffer
//*rmode - the current video mode (# lines,progressive or interlaced, NTSC or PAL etc.) see libogc/gc/ogc/gx_struct.h
// for the definition
//w - Width of screen (Used as scale factor in converting fx to pixel coordinates)
//h - Height of screen (Used as scale factor in converting fy to pixel coordinates)
//fx - X coordinate to draw on the screen (0-w)
//fy - Y coordinate to draw on the screen (!unsure!-h)
//color - the color of the rectangle (Examples: COLOR_YELLOW, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_BLACK, COLOR_WHITE)
void drawdot(void *xfb, GXRModeObj *rmode, float w, float h, float fx, float fy, u32 color) {
 
	//*fb - !unsure!
	//px - !unsure!
	//py - !unsure!
	//x - !unsure!
	//y - !unsure!
 
	u32 *fb;
	int px,py;
	int x,y;
 
 
	fb = (u32*)xfb;
 
	// y = 화면 좌표의 y * 프레임 버퍼의 높이 / 화면의 높이
	y = fy * rmode->xfbHeight / h;
	// x = 화면 좌표의 x * 프레임 버퍼의 너비 / 화면의 너비 / 2
	x = fx * rmode->fbWidth / w / 2;
 
	// px, py는 픽셀을 그리기 위한 너비와 높이.
	// -4~+4, -2~+2로 4*8의 픽셀을 그림.
	for(py=y-4; py<=(y+4); py++) {
		// 화면을 벗어나면 그리지 않음.
		if(py < 0 || py >= rmode->xfbHeight)
				continue;
		for(px=x-2; px<=(x+2); px++) {
			// 화면을 벗어나면 그리지 않음.
			if(px < 0 || px >= rmode->fbWidth/2)
				continue;
			// 프레임 버퍼의 해당 배열값에 너비를 2로 나눈 것에 대해 다시 2를 곱한 후에 색을 넣어줌.
			fb[rmode->fbWidth/VI_DISPLAY_PIX_SZ*py + px] = color;
		}
	}
 
}

// 콜백으로 프레임마다 호출, evctr는 프레임 갯수를 반영.
int evctr = 0;
void countevs(int chan, const WPADData *data) {
	evctr++;
}

void print_wiimote_connection_status(int wiimote_connection_status) {
	switch(wiimote_connection_status) {
		case WPAD_ERR_NO_CONTROLLER:
			printf(" Wiimote not connected\n");
			break;
		case WPAD_ERR_NOT_READY:
			printf(" Wiimote not ready\n");
			break;
		case WPAD_ERR_NONE:
			printf(" Wiimote ready\n");
			break;
		default:
			printf(" Unknown Wimote state %d\n",wiimote_connection_status);
	}
}

void print_wiimote_buttons(WPADData *wd) {
	printf(" Buttons down:\n ");
	if(wd->btns_h & WPAD_BUTTON_A) printf("A ");
	if(wd->btns_h & WPAD_BUTTON_B) printf("B ");
	if(wd->btns_h & WPAD_BUTTON_1) printf("1 ");
	if(wd->btns_h & WPAD_BUTTON_2) printf("2 ");
	if(wd->btns_h & WPAD_BUTTON_MINUS) printf("MINUS ");
	if(wd->btns_h & WPAD_BUTTON_HOME) printf("HOME ");
	if(wd->btns_h & WPAD_BUTTON_PLUS) printf("PLUS ");
	printf("\n ");
	if(wd->btns_h & WPAD_BUTTON_LEFT) printf("LEFT ");
	if(wd->btns_h & WPAD_BUTTON_RIGHT) printf("RIGHT ");
	if(wd->btns_h & WPAD_BUTTON_UP) printf("UP ");
	if(wd->btns_h & WPAD_BUTTON_DOWN) printf("DOWN ");
	printf("\n");
}

void print_and_draw_wiimote_data(void *screen_buffer) {
	//Makes the var wd point to the data on the wiimote
	WPADData *wd = WPAD_Data(0);
	printf(" Data->Err: %d\n",wd->err);
	printf(" IR Dots:\n");
	int i;
	for(i=0; i<4; i++) {
		if(wd->ir.dot[i].visible) {
			printf(" %4d, %3d\n", wd->ir.dot[i].rx, wd->ir.dot[i].ry);
			drawdot(screen_buffer, rmode, 1024, 768, wd->ir.dot[i].rx, wd->ir.dot[i].ry, COLOR_YELLOW);
		} else {
			printf(" None\n");
		}
	}
	//ir.valid - TRUE is the wiimote is pointing at the screen, else it is false
	if(wd->ir.valid) {
		float theta = wd->ir.angle / 180.0 * M_PI;
	
		//ir.x/ir.y - The x/y coordinates that the wiimote is pointing to, relative to the screen.
		//ir.angle - how far (in degrees) the wiimote is twisted (based on ir)
		printf(" Cursor: %.02f,%.02f\n",wd->ir.x, wd->ir.y);
		printf(" @ %.02f deg\n",wd->ir.angle);
		
		drawdot(screen_buffer, rmode, rmode->fbWidth, rmode->xfbHeight, wd->ir.x, wd->ir.y, COLOR_RED);
		// 파란 점은 위모트의 위쪽을 향함.
		drawdot(screen_buffer, rmode, rmode->fbWidth, rmode->xfbHeight, wd->ir.x + 10*sinf(theta), wd->ir.y - 10*cosf(theta), COLOR_BLUE); 
	} else {
		printf(" No Cursor\n\n");
	}
	if(wd->ir.raw_valid) {
		//ir.z - How far away the wiimote is from the screen in meters
		printf(" Distance: %.02fm\n", wd->ir.z);
		//orient.yaw - The left/right angle of the wiimote to the screen 
		printf(" Yaw: %.02f deg\n", wd->orient.yaw);
	} else {
		printf("\n\n");
	}
	printf(" Accel:\n");
	//accel.x/accel.y/accel.z - analog values for the accelleration of the wiimote
	//(Note: Gravity pulls downwards, so even if the wiimote is not moving, 
	//one(or more) axis will have a reading as if it is moving "upwards")
	printf(" XYZ: %3d,%3d,%3d\n",wd->accel.x,wd->accel.y,wd->accel.z);
	//orient.pitch - how far the wiimote is "tilted" in degrees
	printf(" Pitch: %.02f\n",wd->orient.pitch);
	//orient.roll - how far the wiimote is "twisted" in degrees (uses accelerometer)
	printf(" Roll: %.02f\n",wd->orient.roll);

	print_wiimote_buttons(wd);

	if(wd->ir.raw_valid) {
		for(i=0; i<2; i++) {
			drawdot(screen_buffer, rmode, 4, 4, wd->ir.sensorbar.rot_dots[i].x+2, wd->ir.sensorbar.rot_dots[i].y+2, COLOR_GREEN);
		}
	}

	if(wd->btns_h & WPAD_BUTTON_1) doreload=1;
}
 
int main(int argc, char **argv) {
	void *xfb[2];
	u32 type;
	int fbi = 0;
 
	VIDEO_Init();
	PAD_Init();
	WPAD_Init();
 
	rmode = VIDEO_GetPreferredMode(NULL);
 
	// double buffering, prevents flickering (is it needed for LCD TV? i don't have one to test)
	xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
 
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
 
	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
 
	WPAD_SetDataFormat(0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(0, rmode->fbWidth, rmode->xfbHeight);
 
	while(!doreload && !dooff) {
		CON_Init(xfb[fbi],0,0,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
		//VIDEO_ClearFrameBuffer(rmode,xfb[fbi],COLOR_BLACK);
		printf("\n\n\n");
		WPAD_ReadPending(WPAD_CHAN_ALL, countevs);
		int wiimote_connection_status = WPAD_Probe(0, &type);
		print_wiimote_connection_status(wiimote_connection_status);

		printf(" Event count: %d\n",evctr);
		if(wiimote_connection_status == WPAD_ERR_NONE) {
			print_and_draw_wiimote_data(xfb[fbi]);
		}
		VIDEO_SetNextFramebuffer(xfb[fbi]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		fbi ^= 1;
	}
	if(doreload) return 0;
	if(dooff) SYS_ResetSystem(SYS_SHUTDOWN,0,0);
 
	return 0;
}