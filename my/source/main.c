#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

// TODO:
// 버텍스와 텍스트를 동시에 표시하면 화면이 깜빡임.
// 커서가 화면 안에 있으면 보라색으로 깜빡임.
// 그래픽, 텍스트 다 끄면 초록색이 되고 화면 클리어가 안됨.


// debug
bool isDebug = true;
// bool isDebug = false;

// bool useGX = true;
bool useGX = false;

// gx
#define DEFAULT_FIFO_SIZE	(256*1024)
GXColor background = {0, 0, 0, 0xff};
Mtx view;
Mtx44 perspective;
Mtx model, modelview;
f32 yscale;
u32 xfbHeight;

// static void *xfb = NULL;
static void *xfb[2];
static int fbi = 0;

static GXRModeObj *rmode = NULL;

int frames = 0;

// 연결 상태
u32 type;// 뭐에 쓰는지 모름.
int wiimote_connection_status_p1 = 0;
int wiimote_connection_status_p2 = 0;

const int p1Channel = 0; 
const int p2Channel = 1; 
const int p3Channel = 2; 
const int p4Channel = 3; 

void print_wiimote_connection_status(int wiimote_connection_status, int channel);
void drawdot(void *xfb, GXRModeObj *rmode, float w, float h, float fx, float fy, u32 color);
void showCursorInfo(void* screen_buffer, int channel);
void print_wiimote_buttons(WPADData *wd);

// remove implicit declaration
int memalign(int, int);

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	WPAD_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();







	if (useGX)
	{
		// gx setup
		// setup the fifo and then init the flipper
		void *gp_fifo = NULL;
		gp_fifo = (void*) memalign(32,DEFAULT_FIFO_SIZE);
		memset(gp_fifo,0,DEFAULT_FIFO_SIZE);

		GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);

		// clears the bg to color and clears the z buffer
		GX_SetCopyClear(background, 0x00ffffff);

		// other gx setup
		GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
		yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
		xfbHeight = GX_SetDispCopyYScale(yscale);
		GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
		GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
		GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
		GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
		GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

		GX_SetCullMode(GX_CULL_NONE);
		GX_CopyDisp(xfb[fbi],GX_TRUE);
		GX_SetDispCopyGamma(GX_GM_1_0);


		// setup the vertex descriptor
		// tells the flipper to expect direct data
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);

		// setup the vertex attribute table
		// describes the data
		// args: vat location 0-7, type of data, data format, size, scale
		// so for ex. in the first call we are sending position data with
		// 3 values X,Y,Z of size F32. scale sets the number of fractional
		// bits for non float data.
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

		GX_SetNumChans(1);
		GX_SetNumTexGens(0);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

		// setup our camera at the origin
		// looking down the -z axis with y up
		guVector cam = {0.0F, 0.0F, 0.0F},
				up = {0.0F, 1.0F, 0.0F},
				look = {0.0F, 0.0F, -1.0F};

		guLookAt(view, &cam, &up, &look);


		// setup our projection matrix
		// this creates a perspective matrix with a view angle of 90,
		// and aspect ratio based on the display resolution
		f32 w = rmode->viWidth;
		f32 h = rmode->viHeight;
		guPerspective(perspective, 45, (f32)w/h, 0.1F, 300.0F);
		GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
	}
























	if (isDebug)
	{
		// Initialise the console, required for printf
		console_init(xfb[fbi],20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
		// The console understands VT terminal escape codes
		// This positions the cursor on row 2, column 0
		// we can use variables for this with format codes too
		// e.g. printf ("\x1b[%d;%dH", row, column );
		printf("\x1b[2;0H");
		printf("Hello World!");
	}

	WPAD_SetDataFormat(0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(0, rmode->fbWidth, rmode->xfbHeight);

	while(1) {
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);


		// 입력부 관련

		if (isDebug)
		{
			console_init(xfb[fbi],20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
			printf("Hello World!\n");
			printf("frames: %d\n", frames);
	
			// TODO: 2p도 표시할 것
			wiimote_connection_status_p1 = WPAD_Probe(p1Channel, &type);
			print_wiimote_connection_status(wiimote_connection_status_p1, p1Channel);
			WPADData *p1wd = WPAD_Data(p1Channel);
			print_wiimote_buttons(p1wd);
		}

		showCursorInfo(xfb[fbi], 0);




		if (useGX)
		{
			// gx
			// do this before drawing
			GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);

			guMtxIdentity(model);
			guMtxTransApply(model, model, -1.5f,0.0f,-6.0f);
			guMtxConcat(view,model,modelview);
			// load the modelview matrix into matrix memory
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);

			GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
				GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top
				GX_Position3f32(-1.0f,-1.0f, 0.0f);	// Bottom Left
				GX_Position3f32( 1.0f,-1.0f, 0.0f);	// Bottom Right
			GX_End();

			guMtxTransApply(model, model, 3.0f,0.0f,0.0f);
			guMtxConcat(view,model,modelview);
			// load the modelview matrix into matrix memory
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);

			GX_Begin(GX_QUADS, GX_VTXFMT0, 4);			// Draw A Quad
				GX_Position3f32(-1.0f, 1.0f, 0.0f);	// Top Left
				GX_Position3f32( 1.0f, 1.0f, 0.0f);		// Top Right
				GX_Position3f32( 1.0f,-1.0f, 0.0f);	// Bottom Right
				GX_Position3f32(-1.0f,-1.0f, 0.0f);	// Bottom Left
			GX_End();									// Done Drawing The Quad

			// do this stuff after drawing
			GX_DrawDone();
		}









		fbi ^= 1;
		frames++;

		if (useGX)
		{
			GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
			GX_SetColorUpdate(GX_TRUE);
			GX_CopyDisp(xfb[fbi],GX_TRUE);
		}

		// flush
		VIDEO_SetNextFramebuffer(xfb[fbi]);
		VIDEO_Flush();

		// Wait for the next frame
		VIDEO_WaitVSync();

	}

	return 0;
}

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

void showCursorInfo(void* screen_buffer, int channel)
{
	WPADData *wd = WPAD_Data(channel);

	if (wd->ir.valid)
	{
		float theta = wd->ir.angle / 180.0 * M_PI;

		drawdot(screen_buffer, rmode, rmode->fbWidth, rmode->xfbHeight, wd->ir.x, wd->ir.y, COLOR_RED);
		// 파란 점은 위모트의 위쪽을 향함.
		drawdot(screen_buffer, rmode, rmode->fbWidth, rmode->xfbHeight, wd->ir.x + 10*sinf(theta), wd->ir.y - 10*cosf(theta), COLOR_BLUE);
	}
	else
	{
		if (isDebug)
		{
			printf(" No Cursor\n\n");
		}
	}

	// print_wiimote_buttons(wd);
}

void print_wiimote_connection_status(int wiimote_connection_status, int channel) {
	switch(wiimote_connection_status) {
		case WPAD_ERR_NO_CONTROLLER:
			printf("channel %d Wiimote not connected\n", channel);
			break;
		case WPAD_ERR_NOT_READY:
			printf("channel %d Wiimote not ready\n", channel);
			break;
		case WPAD_ERR_NONE:
			printf("channel %d Wiimote ready\n", channel);
			break;
		default:
			printf("channel %d Unknown Wimote state %d\n", channel, wiimote_connection_status);
	}
}

void print_wiimote_buttons(WPADData *wd)
{
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
