#include <grrlib.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wiiuse/wpad.h>

#include "g_defines.h"
#include "g_debug.h"
#include "g_input.h"
#include "g_font.h"

// global setting
bool isDebug = true;

// debug
extern u8 FPS;

// gx
#define DEFAULT_FIFO_SIZE	(256*1024)
GXColor background = {0, 0, 0, 0xff};
Mtx view;
Mtx44 perspective;
Mtx model, modelview;
f32 yscale;
u32 xfbHeight;

// 연결 상태
u32 type;// 뭐에 쓰는지 모름.
int wiimote_connection_status_p1 = 0;
int wiimote_connection_status_p2 = 0;

ir_t irData;
void DrawWiiRemote(int channel);

extern GRRLIB_texImg *tex_font;

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	GRRLIB_Init();

	// This function initialises the attached controllers
	WPAD_Init();
	
	// chan 0
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_0, rmode->fbWidth, rmode->xfbHeight);

	InitFonts();

	// settings
    GRRLIB_Settings.antialias = true;

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    GRRLIB_Camera3dSettings(0.0f,0.0f,13.0f, 0,1,0, 0,0,0);


    float a=0;
    u32 col[3] = {0xFFFFFFFF, 0xAAAAAAFF, 0x666666FF};
    int cubeZ=0;

	while(1) {

		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);

		SetTextCursorPos(0, 0);

		GRRLIB_2dMode();

		// 입력부 관련
		if (isDebug)
		{
			// GRRLIB_Printf(20, 20, tex_font, 0xFFFFFFFF, 1, "HELLO WORLD");
			Printf("Hello World");

			// TODO: 2p도 표시할 것
			wiimote_connection_status_p1 = WPAD_Probe(WPAD_CHAN_0, &type);
			print_wiimote_connection_status(wiimote_connection_status_p1, WPAD_CHAN_0);
			WPADData *p1wd = WPAD_Data(WPAD_CHAN_0);
			print_wiimote_buttons(p1wd);
		}

		DrawWiiRemote(WPAD_CHAN_0);

		GRRLIB_3dMode(0.1, 1000, 45, 0, 0);
        // GRRLIB_ObjectView(0,0, 0, 0,0,0,1,1,1);

		// 카메라가 회전.
        GRRLIB_ObjectView(0,0,cubeZ, a,a*2,a*3,1,1,1);

		// 버텍스 개수 넣고 숫자 맞춰야 함.
        // GX_Begin(GX_QUADS, GX_VTXFMT0, 0);
        // GX_End();


		// guMtxIdentity(model);
		// guMtxTransApply(model, model, -1.5f,0.0f,-6.0f);
		// guMtxConcat(view,model,modelview);
		// // load the modelview matrix into matrix memory
		// GX_LoadPosMtxImm(modelview, GX_PNMTX0);

		// 포맷을 맞춰야 함(위치, 색상)
		// GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
		// 	GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top
        //     GX_Color1u32(col[0]);
		// 	GX_Position3f32(-1.0f,-1.0f, 0.0f);	// Bottom Left
        //     GX_Color1u32(col[0]);
		// 	GX_Position3f32( 1.0f,-1.0f, 0.0f);	// Bottom Right
        //     GX_Color1u32(col[0]);
		// GX_End();

		// guMtxTransApply(model, model, 3.0f,0.0f,0.0f);
		// guMtxConcat(view,model,modelview);
		// // load the modelview matrix into matrix memory
		// GX_LoadPosMtxImm(modelview, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);			// Draw A Quad
			GX_Position3f32(-1.0f, 1.0f, 0.0f);	// Top Left
            GX_Color1u32(col[0]);
			GX_Position3f32( 1.0f, 1.0f, 0.0f);		// Top Right
            GX_Color1u32(col[0]);
			GX_Position3f32( 1.0f,-1.0f, 0.0f);	// Bottom Right
            GX_Color1u32(col[0]);
			GX_Position3f32(-1.0f,-1.0f, 0.0f);	// Bottom Left
            GX_Color1u32(col[0]);
		GX_End();									// Done Drawing The Quad





        // GRRLIB_3dMode(0.1,1000,45,0,0);
        // GRRLIB_ObjectView(0,0,cubeZ, a,a*2,a*3,1,1,1);
        // GX_Begin(GX_QUADS, GX_VTXFMT0, 24);
        //     GX_Position3f32(-1.0f,1.0f,-1.0f);
        //     GX_Color1u32(col[0]);
        //     GX_Position3f32(-1.0f,-1.0f,-1.0f);
        //     GX_Color1u32(col[0]);
        //     GX_Position3f32(1.0f,-1.0f,-1.0f);
        //     GX_Color1u32(col[0]);
        //     GX_Position3f32(1.0f,1.0f,-1.0f);
        //     GX_Color1u32(col[0]);

        //     GX_Position3f32(-1.0f,1.0f,1.0f);
        //     GX_Color1u32(col[0]);
        //     GX_Position3f32(-1.0f,-1.0f,1.0f);
        //     GX_Color1u32(col[0]);
        //     GX_Position3f32(1.0f,-1.0f,1.0f);
        //     GX_Color1u32(col[0]);
        //     GX_Position3f32(1.0f,1.0f,1.0f);
        //     GX_Color1u32(col[0]);

        //     GX_Position3f32(-1.0f,1.0f,1.0f);
        //     GX_Color1u32(col[1]);
        //     GX_Position3f32(1.0f,1.0f,1.0f);
        //     GX_Color1u32(col[1]);
        //     GX_Position3f32(1.0f,1.0f,-1.0f);
        //     GX_Color1u32(col[1]);
        //     GX_Position3f32(-1.0f,1.0f,-1.0f);
        //     GX_Color1u32(col[1]);

        //     GX_Position3f32(-1.0f,-1.0f,1.0f);
        //     GX_Color1u32(col[1]);
        //     GX_Position3f32(1.0f,-1.0f,1.0f);
        //     GX_Color1u32(col[1]);
        //     GX_Position3f32(1.0f,-1.0f,-1.0f);
        //     GX_Color1u32(col[1]);
        //     GX_Position3f32(-1.0f,-1.0f,-1.0f);
        //     GX_Color1u32(col[1]);

        //     GX_Position3f32(-1.0f,1.0f,1.0f);
        //     GX_Color1u32(col[2]);
        //     GX_Position3f32(-1.0f,1.0f,-1.0f);
        //     GX_Color1u32(col[2]);
        //     GX_Position3f32(-1.0f,-1.0f,-1.0f);
        //     GX_Color1u32(col[2]);
        //     GX_Position3f32(-1.0f,-1.0f,1.0f);
        //     GX_Color1u32(col[2]);

        //     GX_Position3f32(1.0f,1.0f,1.0f);
        //     GX_Color1u32(col[2]);
        //     GX_Position3f32(1.0f,1.0f,-1.0f);
        //     GX_Color1u32(col[2]);
        //     GX_Position3f32(1.0f,-1.0f,-1.0f);
        //     GX_Color1u32(col[2]);
        //     GX_Position3f32(1.0f,-1.0f,1.0f);
        //     GX_Color1u32(col[2]);
        // GX_End();

        a+=0.5f;

		// use grrlib render
		GRRLIB_Render();

        FPS = CalculateFrameRate();
	}

	CleanFonts();

    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}

void DrawWiiRemote(int channel)
{
	WPAD_IR(channel, &irData);

	float x = irData.sx;
	float y = irData.sy;

	float r = 4;

	for (float i = x - r; i < x + r; i+=1)
	{
		for (float j = y - r; j < y + r; j+=1)
		{
			GRRLIB_Plot(i, j, GRRLIB_YELLOW);
		}
	}
}