#pragma once

#include "mathlib.h"
#include <iostream>
//#include "shaders.h"
//#include "Assets\flower.h"

//•	A function to convert 2 dimensional coordinates to a 1 dimensional coordinate.
unsigned int ConvertTo1D(int x, int y, unsigned int _width)
{
	return (y * _width + x);
}

void ClearZ()
{
	for (unsigned int i = 0; i < NUM_PIXELS; i++)
	{
		Z_Buffer[i] = 1.0f;
	}
}

//•	A function to draw a pixel(fill a certain pixel with a specific color)
void SetPixel(unsigned int _color, unsigned int _x, unsigned int _y)
{
	if ( _x > RASTER_WIDTH || _y > RASTER_HEIGHT  )
	{
		std::cout << "Bounds breach on _x: " << _x << " _y: " << _y << '\n';
	//	system("pause");
	}
	else
	{
	
		  Backbuffer[ConvertTo1D(_x, _y, RASTER_WIDTH)] = _color;
	}
}

void SetPixelZ(unsigned int _color, unsigned int _x, unsigned int _y, float z)
{
	if (_x > RASTER_WIDTH || _y > RASTER_HEIGHT ||
		_x < 0 || _y < 0)
	{
		std::cout << "Bounds breach on _x: " << _x << " _y: " << _y << '\n';
		//	system("pause");
	}
	else
	{
		if (z < Z_Buffer[ConvertTo1D(_x, _y, RASTER_WIDTH)])
		{
			
			
			Z_Buffer[ConvertTo1D(_x, _y, RASTER_WIDTH)] = z;
			Backbuffer[ConvertTo1D(_x, _y, RASTER_WIDTH)] = _color;
			
			
		}
	}
}


//•	A function to clear the color buffer to a solid color of your choice.
void ClearBuffer()
{
	for (int i = 0; i < NUM_PIXELS; i++)
	{
		Backbuffer[i] = 0xFF095919;
	}
}

void Clear_Z_and_Buffer()
{
	for (int i = 0; i < NUM_PIXELS; i++)
	{
		Backbuffer[i] = 0xFF105919;
		Z_Buffer[i] = 1.0f;
	}
}



void SetPixel(unsigned int _color, unsigned int _position)
{
	Backbuffer[_position] = _color;

}

void SetPixel(unsigned int _x, unsigned int _y, unsigned int _a, unsigned int _r, unsigned int g, unsigned int b)
{

	Backbuffer[ConvertTo1D(_x, _y, RASTER_WIDTH)] = (_a << 24) | (_r << 16) | (_r << 8) | b;
}

void SetPixel(A_PIXEL _color, unsigned int _x, unsigned int _y)
{

	Backbuffer[ConvertTo1D(_x, _y, RASTER_WIDTH)] = _color.color;
}
//•	A function to BLIT(Block Image Transfer)



unsigned int ConvertLerp(unsigned int _original, unsigned int _color)
{
	// A R G B
	unsigned  int oldA = (_original & 0xFF000000) >> 24;
	unsigned  int oldR = (_original & 0x00FF0000) >> 16;
	unsigned  int oldG = (_original & 0x0000FF00) >> 8;
	unsigned  int oldB = (_original & 0x000000FF);
	// 0x A R G B 
	unsigned  int newA = (_color & 0xFF000000) >> 24;
	unsigned  int newR = (_color & 0x00FF0000) >> 16;
	unsigned  int newG = (_color & 0x0000FF00) >> 8;
	unsigned  int newB = (_color & 0x000000FF);

	float R = (float)newA / 255.0f;

	unsigned int return_colorA = (unsigned int)((float)((int)newA - (int)oldA) * R + (float)oldA);
	unsigned int return_colorR = (unsigned int)((float)((int)newR - (int)oldR) * R + (float)oldR);
	unsigned int return_colorG = (unsigned int)((float)((int)newG - (int)oldG) * R + (float)oldG);
	unsigned int return_colorB = (unsigned int)((float)((int)newB - (int)oldB) * R + (float)oldB);

	return ((return_colorA << 24) | (return_colorR << 16) | (return_colorG << 8) | return_colorB);
}



//void BLIT(unsigned int draw_x, unsigned int draw_y, unsigned int BBW, unsigned BBH, RECTa tile, unsigned int imgW, unsigned int imgh)
//{
//	for (int j = 0; j < tile.hight; j++)
//	{
//		for (int i = 0; i < tile.width; i++)
//		{
//			unsigned int index = ConvertTo1D(tile.left + i, tile.top + j, imgW);
//		//	unsigned int color = ConvertFromBGRAtoARGB(flower_pixels[index]);
//
//			index = ConvertTo1D(draw_x + i, draw_y + j, BBW);
//			//color = ConvertFromBGRAtoARGB(color);
//			
//			Backbuffer[index] = ConvertLerp(Backbuffer[index], color);
//		}
//	}
//}


//void BLITfire(unsigned int draw_x, unsigned int draw_y, unsigned int BBW, unsigned BBH, RECTa tile, unsigned int imgW, unsigned int imgh)
//{
//	for (int j = 0; j < tile.hight; j++)
//	{
//		for (int i = 0; i < tile.width; i++)
//		{
//			unsigned int index = ConvertTo1D(tile.left + i, tile.top + j, imgW);
//			unsigned int color = ConvertFromBGRAtoARGB(fire_02_pixels[index]);
//
//			index = ConvertTo1D(draw_x + i, draw_y + j, BBW);
//
//			color = ConvertLerp(Backbuffer[index], color);
//			Backbuffer[index] = color;
//		}
//	}
//}


void Bresenham(unsigned int _x_start, unsigned int _y_start, unsigned int _x_end, unsigned int _y_end)
{

	unsigned int CurrY = _y_start;
	float Slope = (float)(_y_end - _y_start) / (float)(_x_end - _x_start);
	float  Error = 0;

	for (unsigned int x = _x_start; x < _x_end; x++)
	{
//		SetPixel(0x00FF0000, x, CurrY);
		Error += Slope;
		if (Error > 0.5f)
		{
			CurrY++;
			Error -= 1.0f;
		}
	}
}

void Midpoint(unsigned int _x_start, unsigned int _y_start, unsigned int _x_end, unsigned int _y_end)
{
	unsigned int CurrY = _y_start;
	float d = ((float)_y_start - (float)_y_end) * ((float)_x_start + 1.0f) + (float)((float)_x_end - (float)_x_start) * ((float)_y_start + 0.5f) + (float)(_x_start * _y_end) - (float)(_x_end * _y_start);
	for (unsigned int x = _x_start; x < _x_end; x++)
	{
//		SetPixel(0x0000FF00, x, CurrY);

		if (d < 0.0f)
		{
			CurrY++;
			d = d + (float)(_x_end - _x_start) + (float)((float)_y_start - (float)_y_end);
		}
		else
			d = d + (float)((float)_y_start - (float)_y_end);
	}
}



void Parametric(unsigned int _x_start, unsigned int _y_start, unsigned int _x_end, unsigned int _y_end, MY_VERTEX _start, MY_VERTEX _end)
{
	float ratio = 0;
	float CurrY = 0;
	if (_x_start == 0)  _x_start = 1;
	float Z = 0.0f;
	int dx = _x_end - _x_start;
	int dy = _y_end - _y_start;
	int tp = Max(abs(dx), abs(dy));

	if (dx || dy)
	{

		unsigned int new_color;
		if (abs(dx) >= abs(dy))
		{
			float d_y = float(dy) / float(dx);
			float y = _y_start + 0.5f;
			if (dx > 0)
				for (int x = _x_start; x <= int(_x_end); x++)
				{
				new_color = ConvertLerp(_start.color, _end.color, dx);
				Z = ConvertLerp(_start.Z, _end.Z, dx);
				SetPixel(new_color, floor(x), (unsigned int)(floor(y)));
				y += d_y;
				}
			else
				for (int x = _x_start; x >= int(_x_end); x--)
				{
				new_color = ConvertLerp(_start.color, _end.color, dx);
				Z = ConvertLerp(_start.Z, _end.Z, dx);
				SetPixel(new_color, floor(x), (unsigned int)(floor(y)));
				y -= d_y;
				}
		}
		else  {
			float d_x = float(dx) / float(dy);
			float x = _x_start + 0.5f;
			if (dy > 0)
				for (int y = _y_start; y <= int(_y_end); y++)
				{
				new_color = ConvertLerp(_start.color, _end.color, dx);
				Z = ConvertLerp(_start.Z, _end.Z, dx);
				SetPixel(new_color, floor(x), (unsigned int)(floor(y)));
				x += d_x;
				}
			else

				for (int y = _y_start; y >= int(_y_end); y--)
				{
				new_color = ConvertLerp(_start.color, _end.color, dx);
				Z = ConvertLerp(_start.Z, _end.Z, dx);
				SetPixel(new_color, floor(x), (unsigned int)(floor(y)));
				x -= d_x;
				}
		}
	}



}


void RandomParametric2(unsigned int _x_start, unsigned int _y_start, unsigned int _x_end, unsigned int _y_end)
{
	float ratio = 0;
	float CurrY = 0;
	float Currx = 0;
	if (_x_start == 0)  _x_start = 1;
	unsigned int new_color;

	int dx = abs(((int)_x_end - (int)_x_start));
	int dy = abs(((int)_y_end - (int)_y_start));

	int tp = Max(dx, dy);
	float r;
	int x;
	int y;
	for (unsigned int i = 0; i < tp; i++)
	{
		r = i / float(tp);
		x = (_x_end - _x_start) * r + _x_start;
		y = (_y_end - _y_start) * r + _y_start;
		new_color = ConvertLerp(0xFFFF00FF, 0xFF00FFFF, dy);
	//	SetPixel(new_color, floor(x), (unsigned int)(floor(y)));
	}

}

SCREEN_XY CartesianToScreen(MY_VERTEX _screenXY)
{
	SCREEN_XY _tempXY;

	_tempXY.X = ((_screenXY.X + 1) * RASTER_WIDTH) / 2;
	_tempXY.Y = ((1 - _screenXY.Y ) * RASTER_HEIGHT / 2);
	
	return _tempXY;
}

unsigned int CartesianToScreen_X(float X)
{

	return ((X + 1) * RASTER_WIDTH) / 2;
}
unsigned int CartesianToScreen_Y(float Y)
{

	return ((Y) * RASTER_HEIGHT / 2);
}


// new
MY_VERTEX ScreenToCartesian(SCREEN_XY _screenXY)
{
	MY_VERTEX cart;

	cart.X = _screenXY.X - RASTER_WIDTH / 2;
	cart.Y =  RASTER_HEIGHT / 2 - _screenXY.Y;

	return cart;
}
// only in 2d space 
void NDCToRaster(float & out_x, float & out_y, float x, float y)
{
	out_x = floorf((RASTER_WIDTH * ( x + 1 )) *0.5f );
	out_y = floorf(RASTER_HEIGHT * 0.5f - y * RASTER_HEIGHT * 0.5f);
}

//β = ImplicitLineEquation(B, line AC)
//γ = ImplicitLineEquation(C, line BA)
//α = ImplicitLineEquation(A, line CB)
//b = ImplicitLineEquation(P, line AC)
//y = ImplicitLineEquation(P, line BA)
//a = ImplicitLineEquation(P, line CB)
//Pβγα = (b / β, y / γ, a / α)
//Could we use the Barycentric coordinates to detect if we are inside or outside the triangle ? If so… how ?

bool PointInTriangle(MY_VERTEX A, MY_VERTEX B, MY_VERTEX C, MY_VERTEX P)
{

	MY_VERTEX u;
	u.X = B.X - A.X;
	u.Y = B.Y - A.Y;
	u.Z = B.Z - A.Z;
	u.Z = 0.0f;
	MY_VERTEX v;
	v.X = C.X - A.X;
	v.Y = C.Y - A.Y;
	v.Z = C.Z - A.Z;
	v.Z = 0.0f;
	MY_VERTEX w;
	w.X = P.X - A.X;
	w.Y = P.Y - A.Y;
	w.Z = P.Z - A.Z;
	w.Z = 0.0f;

	MY_VERTEX vCrossW = Vector_Cross(v, w);
	MY_VERTEX vCrossU = Vector_Cross(v, u);

	if ((Vector_Dot(vCrossW, vCrossU)) < 0.0f)
	{
		return false;
	}
	MY_VERTEX uCrossW = Vector_Cross(u, w);
	MY_VERTEX uCrossV = Vector_Cross(u, v);

	if ((Vector_Dot(uCrossW, uCrossV)) < 0.0f)
	{
		return false;
	}
	float d = Vector_Length(uCrossV);
	float r = Vector_Length(vCrossW) / d;
	float t = Vector_Length(uCrossW) / d;
	
	return (r + t <= 1.0f);
}


unsigned int BarycentricColor(MY_VERTEX V1, MY_VERTEX V2, MY_VERTEX V3, float * agb)
{
	// vertex 1 
	unsigned int original_V1_A		= (V1.color & 0xFF000000) >> 24;
	unsigned int original_V1_R		= (V1.color & 0x00FF0000) >> 16;
	unsigned int original_V1_G		= (V1.color & 0x0000FF00) >> 8;
	unsigned int original_V1_B		= (V1.color & 0x000000FF) >> 0;
	// vertex 2
	unsigned int original_V2_A		= (V2.color & 0xFF000000) >> 24;
	unsigned int original_V2_R		= (V2.color & 0x00FF0000) >> 16;
	unsigned int original_V2_G		= (V2.color & 0x0000FF00) >> 8;
	unsigned int original_V2_B		= (V2.color & 0x000000FF) >> 0;
	// vertex 3
	unsigned int original_V3_A		= (V3.color & 0xFF000000) >> 24;
	unsigned int original_V3_R		= (V3.color & 0x00FF0000) >> 16;
	unsigned int original_V3_G		= (V3.color & 0x0000FF00) >> 8;
	unsigned int original_V3_B		= (V3.color & 0x000000FF) >> 0;

	unsigned int new_color;
	
	unsigned int A = agb[0] * original_V3_A + agb[1] * original_V1_A + agb[2] * original_V2_A;
	unsigned int R = agb[0] * original_V3_R + agb[1] * original_V1_R + agb[2] * original_V2_R;
	unsigned int G = agb[0] * original_V3_G + agb[1] * original_V1_G + agb[2] * original_V2_G;
	unsigned int B = agb[0] * original_V3_B + agb[1] * original_V1_B + agb[2] * original_V2_B;

	new_color = (A << 24) | (R << 16) | (G << 8) | B;

	return new_color;

}
float ImplicitLineEquation(MY_VERTEX Point, MY_VERTEX _A, MY_VERTEX _C)
{
	float A = _A.Y - _C.Y;
	float B = _C.X - _A.X;
	float C = (_A.X * _C.Y) - (_A.Y * _C.X);
	
	return (A * Point.X + B * Point.Y + C );
}




void PointInTriangleBarycentric(MY_VERTEX P, MY_VERTEX A, MY_VERTEX B, MY_VERTEX C, float * agb)
{
//	β = ImplicitLineEquation(B, line AC)
	float _B = ImplicitLineEquation(B, A, C);
//γ = ImplicitLineEquation(C, line BA)
	float _Y = ImplicitLineEquation(C, B, A);
//	α = ImplicitLineEquation(A, line CB)
	float _A = ImplicitLineEquation(A, C, B);
//	b = ImplicitLineEquation(P, line AC)
	float b = ImplicitLineEquation(P, A, C);
//	y = ImplicitLineEquation(P, line BA)
	float y = ImplicitLineEquation(P, B, A);
//	a = ImplicitLineEquation(P, line CB)
	float a = ImplicitLineEquation(P, C, B);

	agb[0] = b / _B;
	agb[1] = y / _Y;
	agb[2] = a / _A;
}



float PointInTriangleBarycentricforZ(MY_VERTEX P, MY_VERTEX A, MY_VERTEX B, MY_VERTEX C)
{
	//	β = ImplicitLineEquation(B, line AC)
	float _B = ImplicitLineEquation(B, A, C);
	//γ = ImplicitLineEquation(C, line BA)
	float _Y = ImplicitLineEquation(C, B, A);
	//	α = ImplicitLineEquation(A, line CB)
	float _A = ImplicitLineEquation(A, C, B);
	//	b = ImplicitLineEquation(P, line AC)
	float b = ImplicitLineEquation(P, A, C);
	//	y = ImplicitLineEquation(P, line BA)
	float y = ImplicitLineEquation(P, B, A);
	//	a = ImplicitLineEquation(P, line CB)
	float a = ImplicitLineEquation(P, C, B);

	float ratioAC = (b / _B);
	float ratioBA = y / _Y;
	float ratioCB = a / _A;
	
	//return (1 - ratioAC - ratioBA);

	//float new_z = (ratioAC * A.Z + ratioBA * B.Z + ratioCB * C.Z);
	//float new_z = (ratioAC * A.Z + ratioBA * B.Z + ratioCB * C.Z);
	float new_z = ( ratioAC  + ratioBA  + ratioCB );

	return new_z;
	

//	def GetInterpolatedZ(triangle, u, v) :
		    
//	 float z0 = 1.0 / triangle[0].z
//	float	z1 = 1.0 / triangle[1].z
//	float	z2 = 1.0 / triangle[2].z
//			z = z0 + u * (z1 - z0) + v * (z2 - z0)
//			return 1.0 / z

}

//float ImplicitLineEquationZ(MY_VERTEX Point, MY_VERTEX _A, MY_VERTEX _C)
//{

	    //def GetInterpolatedZ(triangle, u, v) :
	//	fooat z0 = 1.0 / triangle[0].z
//		z1 = 1.0 / triangle[1].z
//		z2 = 1.0 / triangle[2].z
//		z = z0 + u * (z1 - z0) + v * (z2 - z0)
//		return 1.0 / z


//	(Y1 – Y2)x + (X2 – X1)y + X1Y2 – Y1X2 = 0
//	D = Implicit Result / Gradient Length


//	float Distance = sqrtf((_C.X - _A.X) * (_C.X - _A.X) + (_C.Y - _A.Y) * (_C.Y - _A.Y) + (_C.Z - _A.Z) * (_C.Z - _A.Z));

	


//	float A = _A.Y - _C.Y;
//	float B = _C.X - _A.X;
//	float C = (_A.X * _C.Y) - (_A.Y * _C.X);

//	return (A * Point.Z + B * Point.Z + C);
//}
	// Compute barycentric coordinates (u, v, w) for
	// point p with respect to triangle (a, b, c)


MY_VERTEX FindBarycentricXYZ(MY_VERTEX p, MY_VERTEX a, MY_VERTEX b, MY_VERTEX c)
	{
	MY_VERTEX result;
	
	    MY_VERTEX v0 = Vector_Subtract(b, a); 
		MY_VERTEX v1 = Vector_Subtract(c, a); 
		MY_VERTEX v2 = Vector_Subtract(p, a); 

		float d00 = Vector_Dot(v0, v0);
		float d01 = Vector_Dot(v0, v1);
		float d11 = Vector_Dot(v1, v1);
		float d20 = Vector_Dot(v2, v0);
		float d21 = Vector_Dot(v2, v1);

		float denom = (d00 * d11) - (d01 * d01);
		result.X = (d11 * d20 - d01 * d21) / denom;
		result.Y = (d00 * d21 - d01 * d20) / denom;
		result.Z = 1.0f - result.X - result.Y;
		result.W = p.W; // can we do thids ?
		return result;

}


// Draws a line using one of the line equations.
//void DrawLine(const MY_VERTEX &start, const MY_VERTEX &end)
//{
//	// Copy input data and send through shaders
//	MY_VERTEX copy_start;
//	copy_start.X = start.X;
//	copy_start.Y= start.Y;
//	copy_start.Z = start.Z;
//	copy_start.W = start.W;
//	copy_start.color = start.color;
//	MY_VERTEX copy_end;
//	copy_end.X = end.X;
//	copy_end.Y = end.Y;
//	copy_end.Z = end.Z;
//	copy_end.W = end.W;
//	copy_end.color = end.color;
//
//	// Use vertex shader to modify incoming copies only.
//	if (VertexShader)
//	{
//		VertexShader(copy_start);
//		VertexShader(copy_end);
//	}
//	// original plotting variables adapted to use new cartesian data
//	SCREEN_XY screen_start = CartesianToScreen(copy_start);
//	SCREEN_XY screen_end = CartesianToScreen(copy_end);
//
////	if (PixelShader)
////		PixelShader(A_PIXEL{0xFFFFFF00});
//		
//	Parametric(screen_start.X, screen_start.Y, screen_end.X, screen_end.Y, copy_start, copy_end);
//	
//
//
//}

//void DrawTriangle(const MY_VERTEX &V0, const MY_VERTEX &V1,  const MY_VERTEX &V2)
//{
//	MY_VERTEX copy_V0;
//	copy_V0.X = V0.X;
//	copy_V0.Y = V0.Y;
//	copy_V0.Z = V0.Z;
//	copy_V0.W = V0.W;
//	copy_V0.color = V0.color;
//	
//	MY_VERTEX copy_V1;
//	copy_V1.X = V1.X;
//	copy_V1.Y = V1.Y;
//	copy_V1.Z = V1.Z;
//	copy_V1.W = V1.W;
//	copy_V1.color = V1.color;
//
//	MY_VERTEX copy_V2;
//	copy_V2.X = V2.X;
//	copy_V2.Y = V2.Y;
//	copy_V2.Z = V2.Z;
//	copy_V2.W = V2.W;
//	copy_V2.color = V2.color;
//	
//	// Use vertex shader to modify incoming copies only.
//	if (VertexShader)
//	{
//		VertexShader(copy_V0);
//		VertexShader(copy_V1);
//		VertexShader(copy_V2);
//	}
//	// original plotting variables adapted to use new cartesian data
//	SCREEN_XY screen_start = CartesianToScreen(copy_V0);
//	SCREEN_XY screen_end = CartesianToScreen(copy_V1);
//	Parametric(abs(screen_start.X), abs(screen_start.Y), abs(screen_end.X), abs(screen_end.Y), copy_V0, copy_V1);
//
//	 screen_start = CartesianToScreen(copy_V1);
//	 screen_end = CartesianToScreen(copy_V2);
//	Parametric(abs(screen_start.X), abs(screen_start.Y), abs(screen_end.X), abs(screen_end.Y), copy_V1, copy_V2);
//
//	 screen_start = CartesianToScreen(copy_V2);
//	 screen_end = CartesianToScreen(copy_V0);
//	Parametric(abs(screen_start.X), abs(screen_start.Y), abs(screen_end.X), abs(screen_end.Y), copy_V2, copy_V0);
//
//	//if (PixelShader)
//	//	PixelShader(start.color); 
//	}

//void DrawGrid(MY_VERTEX * GridLines)
//{
//	float start = -0.5f;
//	
//
//	for (unsigned int top_index = 0; top_index < 11; top_index++)
//	{
//		GridLines[top_index].X = start;
//		GridLines[top_index].Y = 0.0f;
//		GridLines[top_index].Z = 0.5f;
//		GridLines[top_index].W = 1.0f;
//		GridLines[top_index].color = 0xFFFFFFFF;
//		start += 0.1;
//	}
//	start = -0.5;
//	for (unsigned int bottom_index = 11; bottom_index < 22; bottom_index++)
//	{
//		GridLines[bottom_index].X = start;
//		GridLines[bottom_index].Y = 0.0f;
//		GridLines[bottom_index].Z = -0.5f;
//		GridLines[bottom_index].W = 1.0f;
//		start += 0.1;
//		GridLines[bottom_index].color = 0xFFFFFFFF;
//	}
//	for (unsigned int i = 0; i < 11; i++)
//	{
//	DrawLine(GridLines[i], GridLines[i+11]);
//	}
//	
//	start = -0.5;
//	for (unsigned int left_index = 22; left_index < 33; left_index++)
//	{
//		GridLines[left_index].X = -0.5f;
//		GridLines[left_index].Y = 0.0f;
//		GridLines[left_index].Z = start;
//		GridLines[left_index].W = 1.0f;
//		start += 0.1;
//		GridLines[left_index].color = 0xFFFFFFFF;
//	}
//	start = -0.5;
//	for (unsigned int right_index = 33; right_index < 44; right_index++)
//	{
//		GridLines[right_index].X = 0.5f;
//		GridLines[right_index].Y = 0.0f;
//		GridLines[right_index].Z = start;
//		GridLines[right_index].W = 1.0f;
//		start += 0.1;
//		GridLines[right_index].color = 0xFFFFFFFF;
//	}
//	for (unsigned int i = 22; i < 33; i++)
//	{
//		DrawLine(GridLines[i], GridLines[i + 11]);
//	}
//	
//}
//
//void DrawCude(MY_VERTEX * CubePoints)
//{
//	for (unsigned int i = 0; i < 8; i++)
//	{
//		CubePoints[i].color = 0xFF00FF00;
//	}
//
//	CubePoints[0].X = -0.25f;
//	CubePoints[0].Y = 0.25f;
//	CubePoints[0].Z = -0.25f;
//	CubePoints[0].W = 1.0f;
//
//	CubePoints[1].X = 0.25f;
//	CubePoints[1].Y = 0.25f;
//	CubePoints[1].Z = -0.25f;
//	CubePoints[1].W = 1.0f;
//	DrawLine(CubePoints[0], CubePoints[1]);
//
//	CubePoints[2].X = -0.25f;
//	CubePoints[2].Y = -0.25f;
//	CubePoints[2].Z = -0.25f;
//	CubePoints[2].W = 1.0f;
//
//	CubePoints[3].X = 0.25f;
//	CubePoints[3].Y = -0.25f;
//	CubePoints[3].Z = -0.25f;
//	CubePoints[3].W = 1.0f;
//	DrawLine(CubePoints[2], CubePoints[3]);
//
//	DrawLine(CubePoints[0], CubePoints[2]);
//	DrawLine(CubePoints[1], CubePoints[3]);
//
//	CubePoints[4].X = -0.25f;
//	CubePoints[4].Y = 0.25f;
//	CubePoints[4].Z = 0.25f;
//	CubePoints[4].W = 1.0f;
//
//	CubePoints[5].X = 0.25f;
//	CubePoints[5].Y = 0.25f;
//	CubePoints[5].Z = 0.25f;
//	CubePoints[5].W = 1.0f;
//
//	DrawLine(CubePoints[4], CubePoints[5]);
//	
//	CubePoints[6].X = -0.25f;
//	CubePoints[6].Y = -0.25f;
//	CubePoints[6].Z = 0.25f;
//	CubePoints[6].W = 1.0f;
//
//	CubePoints[7].X = 0.25f;
//	CubePoints[7].Y = -0.25f;
//	CubePoints[7].Z = 0.25f;
//	CubePoints[7].W = 1.0f;
//
//	DrawLine(CubePoints[6], CubePoints[7]);
//	DrawLine(CubePoints[4], CubePoints[6]);
//	DrawLine(CubePoints[5], CubePoints[7]);
//
//	DrawLine(CubePoints[0], CubePoints[4]);
//	DrawLine(CubePoints[2], CubePoints[6]);
//
//	DrawLine(CubePoints[1], CubePoints[5]);
//	DrawLine(CubePoints[3], CubePoints[7]);
//}
//////////////////////-------------------------------------------------------------------
//void DrawTriangleColored(const MY_VERTEX &V0, const MY_VERTEX &V1, const MY_VERTEX &V2)
//{
//	//ClearZ();
//	MY_VERTEX copy_V0;
//	copy_V0.X = V0.X;
//	copy_V0.Y = V0.Y;
//	copy_V0.Z = V0.Z;
//	copy_V0.W = V0.W;
//	copy_V0.u = V0.u;
//	copy_V0.v = V0.v;
//	copy_V0.color = V0.color;
//
//	MY_VERTEX copy_V1;
//	copy_V1.X = V1.X;
//	copy_V1.Y = V1.Y;
//	copy_V1.Z = V1.Z;
//	copy_V1.W = V1.W;
//	copy_V1.u = V1.u;
//	copy_V1.v = V1.v;
//	copy_V1.color = V1.color;
//
//	MY_VERTEX copy_V2;
//	copy_V2.X = V2.X;
//	copy_V2.Y = V2.Y;
//	copy_V2.Z = V2.Z;
//	copy_V2.W = V2.W;
//	copy_V2.u = V2.u;
//	copy_V2.v = V2.v;
//	copy_V2.color = V2.color;
//
//	copy_V0.u = copy_V0.u / copy_V0.W;
//	copy_V0.v = copy_V0.v / copy_V0.W;
//
//	copy_V1.u /= copy_V1.W;
//	copy_V1.v /= copy_V1.W;
//
//
//	// Use vertex shader to modify incoming copies only.
//	if (VertexShader)
//	{
//		VertexShader(copy_V0);
//		VertexShader(copy_V1);
//		VertexShader(copy_V2);
//	}
//	// original plotting variables adapted to use new cartesian data
//	SCREEN_XY screen_start = CartesianToScreen(copy_V0);
//	SCREEN_XY screen_end = CartesianToScreen(copy_V1);
////	Parametric(screen_start.X,screen_start.Y, screen_end.X, screen_end.Y, copy_V0, copy_V1);
//
//
//	screen_start = CartesianToScreen(copy_V1);
//	screen_end = CartesianToScreen(copy_V2);
////	Parametric(screen_start.X, screen_start.Y, screen_end.X, screen_end.Y, copy_V1, copy_V2);
//
//	screen_start = CartesianToScreen(copy_V2);
//	screen_end = CartesianToScreen(copy_V0);
////	Parametric(screen_start.X, screen_start.Y, screen_end.X, screen_end.Y, copy_V2, copy_V0);
//
////	SetPixel(CartesianToScreen(copy_V0).X, CartesianToScreen(copy_V0).Y, copy_V0.color);
////	SetPixel(CartesianToScreen(copy_V1).X, CartesianToScreen(copy_V1).Y, copy_V1.color);
////	SetPixel(CartesianToScreen(copy_V2).X, CartesianToScreen(copy_V2).Y, copy_V2.color);
//
//
//	//	DrawLine(points[0], points[1]);
//	//	DrawLine(points[1], points[2]);
//	//    DrawLine(points[2], points[0]);
//
//
//
//		A_PIXEL pixel;
//		SCREEN_XY ScreenCord;
//	
//		pixel.color = 0xFF080808;
//
//	ScreenCord = CartesianToScreen(copy_V0);
//	copy_V0.X = ScreenCord.X;
//	copy_V0.Y = ScreenCord.Y;
//	copy_V0.u = copy_V0.u / copy_V0.W;
//	copy_V0.v = copy_V0.v / copy_V0.W;
//    copy_V0.color = V0.color;
//	ScreenCord = CartesianToScreen(copy_V1);
//	
//	copy_V1.X = ScreenCord.X;
//	copy_V1.Y = ScreenCord.Y;
//	copy_V1.u /= copy_V1.W;
//	copy_V1.v /= copy_V1.W;
//	
//
//	copy_V1.color = V1.color;
//	ScreenCord = CartesianToScreen(copy_V2);
//	copy_V2.X = ScreenCord.X;
//	copy_V2.Y = ScreenCord.Y;
//	copy_V2.u /= copy_V2.W;
//	copy_V2.v /= copy_V2.W;
//
//	copy_V2.color = V2.color;
//	//points[0] = MultiplyVertexByMatrix(points[0], SV_WorldMatrix);
//	//points[1] = MultiplyVertexByMatrix(points[1], SV_WorldMatrix);
//	//points[2] = MultiplyVertexByMatrix(points[2], SV_WorldMatrix);
//
//
//	//SetPixel(copy_V0.X, copy_V0.Y, copy_V0.color);
//
//
//	int StartX = floorf(Min(copy_V0.X, Min(copy_V1.X, copy_V2.X)));
//	int EndX =   floorf(Max(copy_V0.X, Max(copy_V1.X, copy_V2.X)));
//	int StartY = floorf(Min(copy_V0.Y, Min(copy_V1.Y, copy_V2.Y)));
//	int EndY =   floorf(Max(copy_V0.Y, Max(copy_V1.Y, copy_V2.Y)));
//	
//	
//	for (int y = StartY; y < EndY; y++)
//	{
//		for (int x = StartX; x < EndX; x++)
//		{
//	        
//			float z = 1.0f;
//			float agb[3];
//			SCREEN_XY point;
//			point.X = x;
//			point.Y = y;
//			MY_VERTEX currentpoint;
//			currentpoint = ScreenToCartesian(point);
//		    PointInTriangleBarycentric(MY_VERTEX{ (float)x, (float)y }, copy_V0, copy_V1, copy_V2, agb);
//
//			if ((agb[0] >= 0 && agb[0] <= 1) &&
//				(agb[1] >= 0 && agb[1] <= 1) &&
//				(agb[2] >= 0 && agb[2] <= 1))
//			{
//	
//				pixel.color = BarycentricColor(copy_V0, copy_V1, copy_V2, agb);
//				z = agb[2] * copy_V0.Z + agb[0] * copy_V1.Z + agb[1] * copy_V2.Z;
//
//				float u = agb[2] * copy_V0.u + agb[0] * copy_V1.u + agb[1] * copy_V2.u;
//				float v = agb[2] * copy_V0.v + agb[0] * copy_V1.v + agb[1] * copy_V2.v;
//
//				float w = (1.0f / copy_V0.W) * agb[2] + (1.0f / copy_V1.W) *agb[0] + (1.0f / copy_V2.W) * agb[1];
//
//				u /= w;
//				v /= w;
//
//				if (z > 1 || z < 0.0f)
//				{
//				    std::cout << "z is wrong with " << z << '\n';
//				} 
//				else
//				{
//					if (PixelShader)
//					{
//					 // PS_FlowerShader(pixel, u, v);
//					//  PS_Biliniar(pixel, u, v, z);
//						PS_Biliniar_MIPMapping(pixel, u, v, w);
//					    SetPixelZ(pixel.color, x, y, z);					
//					}
//
//				}
//			}
//		} // inner
//	
//	} // outer
//	
//}
//
//void DrawSide(MY_VERTEX topLeft, MY_VERTEX topRight, MY_VERTEX bottomLeft, MY_VERTEX bottomRight)
//{
//	
////	DrawTriangleColored(bottomLeft, bottomRight, topRight);
////	DrawTriangleColored(topRight, topLeft, bottomLeft);
//	DrawTriangleColored(topRight, bottomLeft, bottomRight);
//	DrawTriangleColored(bottomLeft, topRight, topLeft);
//
//}
//
//void DrawBox(float size)
//{
//
//	MY_VERTEX points[36];
//
//	points[0].X = -size;
//	points[0].Y = size;
//	points[0].Z = -size;
//	points[0].W = 1;
//	points[0].color = 0xFF00FF00;
//	points[0].u = 0.0f;
//	points[0].v = 0.0f;
//
//	points[1].X = size;
//	points[1].Y = size;
//	points[1].Z = -size;
//	points[1].W = 1;
//	points[1].color = 0xFF00FF00;
//	points[1].u = 1.0f;
//	points[1].v = 0.0f;
//
//
//	points[2].X = -size;
//	points[2].Y = -size;
//	points[2].Z = -size;
//	points[2].W = 1;
//	points[2].color = 0xFF00FF00;
//	points[2].u = 0.0f;
//	points[2].v = 1.0f;
//
//	points[3].X = size;
//	points[3].Y = -size;
//	points[3].Z = -size;
//	points[3].W = 1;
//	points[3].color = 0xFF00FF00;
//	        //topLeft,   topRight,  bottomLeft,  bottomRight
//	points[3].u = 1.0f;
//	points[3].v = 1.0f;
//
//	points[4].X = -size;
//	points[4].Y = size;
//	points[4].Z = -size;
//	points[4].W = 1;
//	points[4].color = 0xFFFF0000;
//	points[4].u = 0.0f;
//	points[4].v = 0.0f;
//
//	points[5].X = -size;
//	points[5].Y = size;
//	points[5].Z = size;
//	points[5].W = 1;
//	points[5].color = 0xFF000000;
//	points[5].u = 1.0f;
//	points[5].v = 0.0f;
//
//	points[6].X = -size;
//	points[6].Y = -size;
//	points[6].Z = -size;
//	points[6].W = 1;
//	points[6].color = 0xFFFF0000;
//	points[6].u = 0.0f;
//	points[6].v = 1.0f;
//
//	points[7].X = -size;
//	points[7].Y = -size;
//	points[7].Z = size;
//	points[7].W = 1;
//	points[7].color = 0xFFFF0000;
//	points[7].u = 1.0f;
//	points[7].v = 1.0f;
//
//	points[8].X = size;
//	points[8].Y = size;
//	points[8].Z = size;
//	points[8].W = 1;
//	points[8].color = 0xFFFF0000;
//	points[8].u = 0.0f;
//	points[8].v = 0.0f;
//
//	points[9].X = -size;
//	points[9].Y = size;
//	points[9].Z = size;
//	points[9].W = 1;
//	points[9].color = 0xFFFF0000;
//	points[9].u = 1.0f;
//	points[9].v = 0.0f;
//
//	points[10].X = size;
//	points[10].Y = -size;
//	points[10].Z = size;
//	points[10].W = 1;
//	points[10].color = 0xFF0000FF;
//	points[10].u = 0.0f;
//	points[10].v = 1.0f;
//
//	points[11].X = -size;
//	points[11].Y = -size;
//	points[11].Z = size;
//	points[11].W = 1;
//	points[11].color = 0xFF0000FF;
//	points[11].u = 1.0f;
//	points[11].v = 1.0f;
//
//	points[12].X = size;
//	points[12].Y = size;
//	points[12].Z = -size;
//	points[12].W = 1;
//	points[12].color = 0xFF0000FF;
//	points[12].u = 0.0f;
//	points[12].v = 0.0f;
//
//	points[13].X = size;
//	points[13].Y = size;
//	points[13].Z = size;
//	points[13].W = 1;
//	points[13].color = 0xFF0000FF;
//	points[13].u = 1.0f;
//	points[13].v = 0.0f;
//
//	points[14].X = size;
//	points[14].Y = -size;
//	points[14].Z = -size;
//	points[14].W = 1;
//	points[14].color = 0xFF00FFFF;
//	points[14].u = 0.0f;
//	points[14].v = 1.0f;
//
//	points[15].X = size;
//	points[15].Y = -size;
//	points[15].Z = size;
//	points[15].W = 1;
//	points[15].color = 0xFF00FFFF;
//	points[15].u = 1.0f;
//	points[15].v = 1.0f;
//
//	points[16].X = -size;
//	points[16].Y = size;
//	points[16].Z = size;
//	points[16].W = 1;
//	points[16].color = 0xFFFFFFFF;
//	points[16].u = 0.0f;
//	points[16].v = 0.0f;
//
//	points[17].X = size;
//	points[17].Y = size;
//	points[17].Z = size;
//	points[17].W = 1;
//	points[17].color = 0xFFFFFFFF;
//	points[17].u = 1.0f;
//	points[17].v = 0.0f;
//
//	points[18].X = -size;
//	points[18].Y = size;
//	points[18].Z = -size;
//	points[18].W = 1;
//	points[18].color = 0xFFFFFFFF;
//	points[18].u = 0.0f;
//	points[18].v = 1.0f;
//
//	points[19].X = size;
//	points[19].Y = size;
//	points[19].Z = -size;
//	points[19].W = 1;
//	points[19].color = 0xFFFFFFFF;
//	points[19].u = 1.0f;
//	points[19].v = 1.0f;
//
//	points[20].X = -size;
//	points[20].Y = -size;
//	points[20].Z = size;
//	points[20].W = 1;
//	points[20].color = 0xFFFF00FF;
//	points[20].u = 0.0f;
//	points[20].v = 0.0f;
//
//	points[21].X = size;
//	points[21].Y = -size;
//	points[21].Z = size;
//	points[21].W = 1;
//	points[21].color = 0xFFFF00FF;
//	points[21].u = 1.0f;
//	points[21].v = 0.0f;
//
//	points[22].X = -size;
//	points[22].Y = -size;
//	points[22].Z = -size;
//	points[22].W = 1;
//	points[22].color = 0xFFFF00FF;
//	points[22].u = 0.0f;
//	points[22].v = 1.0f;
//
//	points[23].X = size;
//	points[23].Y = -size;
//	points[23].Z = -size;
//	points[23].W = 1;
//	points[23].color = 0xFFFFFFFF;
//	points[23].u = 1.0f;
//	points[23].v = 1.0f;
//
//	// front
//	DrawSide(points[0], points[1], points[2], points[3]);
//	// left 
//	DrawSide(points[4], points[5], points[6], points[7]);
//	// back 
//	DrawSide(points[8], points[9], points[10], points[11]);
//	// right
//	DrawSide(points[12], points[13], points[14], points[15]);
//	// top
//	DrawSide(points[16], points[17], points[18], points[19]);
//	// bottom
//	DrawSide(points[20], points[21], points[22], points[23]);
//
//
//}



//void DrawGrid(MY_VERTEX * GridLines)
//{
//	float start = -0.5f;
//
//
//	for (unsigned int top_index = 0; top_index < 11; top_index++)
//	{
//		GridLines[top_index].X = start;
//		GridLines[top_index].Y = 0.0f;
//		GridLines[top_index].Z = 0.5f;
//		GridLines[top_index].W = 1.0f;
//		GridLines[top_index].color = 0xFFFFFFFF;
//		start += 0.1;
//	}
//	start = -0.5;
//	for (unsigned int bottom_index = 11; bottom_index < 22; bottom_index++)
//	{
//		GridLines[bottom_index].X = start;
//		GridLines[bottom_index].Y = 0.0f;
//		GridLines[bottom_index].Z = -0.5f;
//		GridLines[bottom_index].W = 1.0f;
//		start += 0.1;
//		GridLines[bottom_index].color = 0xFFFFFFFF;
//	}
//	for (unsigned int i = 0; i < 11; i++)
//	{
//		DrawLine(GridLines[i], GridLines[i + 11]);
//	}
//
//	start = -0.5;
//	for (unsigned int left_index = 22; left_index < 33; left_index++)
//	{
//		GridLines[left_index].X = -0.5f;
//		GridLines[left_index].Y = 0.0f;
//		GridLines[left_index].Z = start;
//		GridLines[left_index].W = 1.0f;
//		start += 0.1;
//		GridLines[left_index].color = 0xFFFFFFFF;
//	}
//	start = -0.5;
//	for (unsigned int right_index = 33; right_index < 44; right_index++)
//	{
//		GridLines[right_index].X = 0.5f;
//		GridLines[right_index].Y = 0.0f;
//		GridLines[right_index].Z = start;
//		GridLines[right_index].W = 1.0f;
//		start += 0.1;
//		GridLines[right_index].color = 0xFFFFFFFF;
//	}
//	for (unsigned int i = 22; i < 33; i++)
//	{
//		DrawLine(GridLines[i], GridLines[i + 11]);
//	}
//
//}