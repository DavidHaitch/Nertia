#ifndef SINE8X28_THINTOP
#define SINE8X28_THINTOP

#define SINE8X28_THINTOP_ANIMATIONFRAMES 28
#define SINE8X28_THINTOP_ANIMATIONRESOLUTION 8

uint8_t maskFrames[SINE8X28_THINTOP_ANIMATIONFRAMES][SINE8X28_THINTOP_ANIMATIONRESOLUTION] =
{
	{255,255, 95,  0,  0,  0,  0,  0 },
	{255,255, 95,  0,  0,  0,  0,  0 },
	{255,255,127, 63,  0,  0,  0,  0 },
	{127,255,255, 95, 63,  0,  0,  0 },
	{ 95,255,255,127, 63,  0,  0,  0 },
	{ 63,127,255,255, 63,  0,  0,  0 },
	{  0, 63,127,255,127, 63,  0,  0 },
	{  0, 63, 95,255,255,127, 63,  0 },
	{  0,  0, 63,127,255,255,127, 63 },
	{  0,  0,  0, 63,127,255,255, 95 },
	{  0,  0,  0, 63, 95,255,255, 95 },
	{  0,  0,  0,  0, 63, 95,159,255 },
	{  0,  0,  0,  0,  0, 63, 95,255 },
	{  0,  0,  0,  0,  0,  0, 63,191 },
	{  0,  0,  0,  0,  0,  0, 63,191 },
	{  0,  0,  0,  0,  0, 63, 95,255 },
	{  0,  0,  0,  0, 63, 95,159,255 },
	{  0,  0,  0, 63, 95,255,255, 95 },
	{  0,  0,  0, 63,127,255,255, 95 },
	{  0,  0, 63,127,255,255,127, 63 },
	{  0, 63, 95,255,255,127, 63,  0 },
	{  0, 63,127,255,127, 63,  0,  0 },
	{ 63,127,255,255, 63,  0,  0,  0 },
	{ 95,255,255,127, 63,  0,  0,  0 },
	{127,255,255, 95, 63,  0,  0,  0 },
	{255,255,127, 63,  0,  0,  0,  0 },
	{255,255, 95,  0,  0,  0,  0,  0 },
	{255,255, 95,  0,  0,  0,  0,  0 }
}
#endif
