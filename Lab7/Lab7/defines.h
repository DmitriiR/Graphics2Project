#pragma once

#define RASTER_WIDTH 500
#define RASTER_HEIGHT 500
#define NUM_STARS 2000
#define LENGTH 0.5f
#define NUM_PIXELS (RASTER_WIDTH * RASTER_HEIGHT)
#define ASPECT_RATIO (RASTER_WIDTH / (float)RASTER_HEIGHT)
#define SAFE_RELEASE(p) { if (p){p->Release(); p = nullptr;} }
#define MOVE_SPEED 0.001f;
unsigned int randomX[NUM_STARS];
unsigned int randomY[NUM_STARS];
unsigned int Backbuffer[NUM_PIXELS];
float Z_Buffer[NUM_PIXELS];



struct MY_VERTEX
{
	float X;
	float Y;
	float Z;
	float W;
	float u;
	float v;
	unsigned int color;
};


struct SIMPLE_VERTEX
{
	float x;
	float y;
	
};

struct SIMPLE_VERTEX_4
{
	float X;
	float Y;
	float Z;
	float W;

	float A;
	float R;
	float G;
	float B;
};

struct A_PIXEL
{
	unsigned int color;
};

struct  MY_MATRIX_3X3
{
	union {
		float e[3][3];
	
		struct 
		{
			float _e00, _e01, _e02;
			float _e10, _e11, _e12;
			float _e20, _e21, _e22;

		}*m;
	};
};

struct  MY_MATRIX_4X4
{
	union {
		float e[4][4];

		struct
		{
			float _e00, _e01, _e02, _e03;
			float _e10, _e11, _e12, _e13;
			float _e20, _e21, _e22, _e23;
			float _e30, _e31, _e32, _e33;

		}*m;
	};
};


struct RECTa 
{
	unsigned int left;
	unsigned int top;
	unsigned int hight;
	unsigned int width;
};

struct SCREEN_XY
{
	int X;
	int Y;
};



