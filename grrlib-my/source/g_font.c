#include "g_font.h"
#include "gfx/courier.h"

GRRLIB_texImg *tex_font;

void InitFonts()
{
	tex_font = GRRLIB_LoadTexture(courier);
    GRRLIB_InitTileSet(tex_font, 14, 27, 32);
}

void CleanFonts()
{
	GRRLIB_FreeTexture(tex_font);
}