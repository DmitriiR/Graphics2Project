#pragma once

#include "defines.h"
#include <math.h>

MY_VERTEX MultiplyVertexByMatrix3x3(MY_VERTEX multiplyMe, MY_MATRIX_3X3 SV_WorldMatrix)
{
	MY_VERTEX temp;
	temp.X = multiplyMe.X * SV_WorldMatrix.e[0][0] + multiplyMe.Y * SV_WorldMatrix.e[1][0] + multiplyMe.Z * SV_WorldMatrix.e[2][0];
	temp.Y = multiplyMe.X * SV_WorldMatrix.e[0][1] + multiplyMe.Y * SV_WorldMatrix.e[1][1] + multiplyMe.Z * SV_WorldMatrix.e[2][1];
	temp.Z = multiplyMe.X * SV_WorldMatrix.e[0][2] + multiplyMe.Y * SV_WorldMatrix.e[1][2] + multiplyMe.Z * SV_WorldMatrix.e[2][2];
	
	temp.color = multiplyMe.color;
	temp.W = multiplyMe.W;
	return temp;

}


MY_VERTEX MultiplyVertexByMatrix(MY_VERTEX multiplyMe, MY_MATRIX_4X4 SV_WorldMatrix)
{
	MY_VERTEX temp;
	temp.X = multiplyMe.X * SV_WorldMatrix.e[0][0] + multiplyMe.Y * SV_WorldMatrix.e[1][0] + multiplyMe.Z * SV_WorldMatrix.e[2][0] + multiplyMe.W * SV_WorldMatrix.e[3][0];
	temp.Y = multiplyMe.X * SV_WorldMatrix.e[0][1] + multiplyMe.Y * SV_WorldMatrix.e[1][1] + multiplyMe.Z * SV_WorldMatrix.e[2][1] + multiplyMe.W * SV_WorldMatrix.e[3][1];
	temp.Z = multiplyMe.X * SV_WorldMatrix.e[0][2] + multiplyMe.Y * SV_WorldMatrix.e[1][2] + multiplyMe.Z * SV_WorldMatrix.e[2][2] + multiplyMe.W * SV_WorldMatrix.e[3][2];
	temp.W = multiplyMe.X * SV_WorldMatrix.e[0][3] + multiplyMe.Y * SV_WorldMatrix.e[1][3] + multiplyMe.Z * SV_WorldMatrix.e[2][3] + multiplyMe.W * SV_WorldMatrix.e[3][3];
	temp.color = multiplyMe.color;
	temp.u = multiplyMe.u;
	temp.v = multiplyMe.v;
	return temp;
}

//GetIncreasingRadiansOverTime


MY_MATRIX_4X4 BuidRotationMatrixOnAxisZ(float _rot)
{
	MY_MATRIX_4X4 tempMatrix;
	
	tempMatrix.e[0][0] = cos(_rot); tempMatrix.e[0][1] = -sin(_rot);	tempMatrix.e[0][2] = 0.0f;
	tempMatrix.e[1][0] = sin(_rot); tempMatrix.e[1][1] = cos(_rot);		tempMatrix.e[1][2] = 0.0f;
	tempMatrix.e[2][0] = 0.0f;		tempMatrix.e[2][1] = 0.0f;			tempMatrix.e[2][2] = 0.0f;
	tempMatrix.e[2][0] = 0.0f;		tempMatrix.e[2][1] = 0.0f;			tempMatrix.e[2][2] = 1.0f;
	return tempMatrix;
}

MY_MATRIX_4X4 RotateOnY(MY_MATRIX_4X4 M, float _rot)
{
	//MY_MATRIX_4X4 tempMatrix;

	M.e[0][0] =  cos(_rot);
	M.e[2][0] =  -sin(_rot);
	M.e[1][1] =  1.0f;
	M.e[0][2] =  sin(_rot);
	M.e[2][2] =  cos(_rot);

	//tempMatrix.e[0][0] = M.e[0][0] * cos(_rot);  tempMatrix.e[0][1] = M.e[0][1] * 0.0f;	        tempMatrix.e[0][2] = M.e[0][2] * sin(_rot); tempMatrix.e[0][3] = M.e[0][3] * 0.0f;
	//tempMatrix.e[1][0] = M.e[1][0] * 0.0f;		 tempMatrix.e[1][1] = M.e[1][1] * 1.0f;		    tempMatrix.e[1][2] = M.e[1][2] * 0.0f;		tempMatrix.e[1][3] = M.e[1][3] * 0.0f;
	//tempMatrix.e[2][0] = M.e[2][0] * -sin(_rot); tempMatrix.e[2][1] = M.e[2][1] * 0.0f;			tempMatrix.e[2][2] = M.e[2][2] * cos(_rot); tempMatrix.e[2][3] = M.e[2][3] * 0.0f;
	//tempMatrix.e[3][0] = M.e[3][0] * 0.0f;		 tempMatrix.e[3][1] = M.e[3][1] * 0.0f;			tempMatrix.e[3][2] = M.e[3][2] * 1.0f;		tempMatrix.e[3][3] = M.e[3][3] * 1.0f;


	return M;
}

MY_MATRIX_4X4 RotateOnX(MY_MATRIX_4X4 M, float _rot)
{
	//MY_MATRIX_4X4 tempMatrix;

	M.e[0][0] = 1.0f;
	M.e[1][1] = cos(_rot);
	M.e[1][2] = sin(_rot);
	M.e[2][1] = -sin(_rot);
	M.e[2][2] = cos(_rot);
	//tempMatrix.e[0][0] = M.e[0][0] * cos(_rot);  tempMatrix.e[0][1] = M.e[0][1] * 0.0f;	        tempMatrix.e[0][2] = M.e[0][2] * sin(_rot); tempMatrix.e[0][3] = M.e[0][3] * 0.0f;
	//tempMatrix.e[1][0] = M.e[1][0] * 0.0f;		 tempMatrix.e[1][1] = M.e[1][1] * 1.0f;		    tempMatrix.e[1][2] = M.e[1][2] * 0.0f;		tempMatrix.e[1][3] = M.e[1][3] * 0.0f;
	//tempMatrix.e[2][0] = M.e[2][0] * -sin(_rot); tempMatrix.e[2][1] = M.e[2][1] * 0.0f;			tempMatrix.e[2][2] = M.e[2][2] * cos(_rot); tempMatrix.e[2][3] = M.e[2][3] * 0.0f;
	//tempMatrix.e[3][0] = M.e[3][0] * 0.0f;		 tempMatrix.e[3][1] = M.e[3][1] * 0.0f;			tempMatrix.e[3][2] = M.e[3][2] * 1.0f;		tempMatrix.e[3][3] = M.e[3][3] * 1.0f;

//			[1, 0, 0, 0]
//	Rx =	[0, cos(r), sin(r), 0]
//			[0, -sin(r), cos(r), 0]
//			[0, 0, 0, 1]
//
	return M;
}

MY_MATRIX_4X4 RotateOnZ(MY_MATRIX_4X4 M, float _rot)
{
	//MY_MATRIX_4X4 tempMatrix;

	M.e[0][0] = cos(_rot);
	M.e[0][1] = sin(_rot);
	M.e[1][0] = -sin(_rot);
	M.e[1][1] = cos(_rot);
	M.e[2][2] = 1.0f;
	//tempMatrix.e[0][0] = M.e[0][0] * cos(_rot);  tempMatrix.e[0][1] = M.e[0][1] * 0.0f;	        tempMatrix.e[0][2] = M.e[0][2] * sin(_rot); tempMatrix.e[0][3] = M.e[0][3] * 0.0f;
	//tempMatrix.e[1][0] = M.e[1][0] * 0.0f;		 tempMatrix.e[1][1] = M.e[1][1] * 1.0f;		    tempMatrix.e[1][2] = M.e[1][2] * 0.0f;		tempMatrix.e[1][3] = M.e[1][3] * 0.0f;
	//tempMatrix.e[2][0] = M.e[2][0] * -sin(_rot); tempMatrix.e[2][1] = M.e[2][1] * 0.0f;			tempMatrix.e[2][2] = M.e[2][2] * cos(_rot); tempMatrix.e[2][3] = M.e[2][3] * 0.0f;
	//tempMatrix.e[3][0] = M.e[3][0] * 0.0f;		 tempMatrix.e[3][1] = M.e[3][1] * 0.0f;			tempMatrix.e[3][2] = M.e[3][2] * 1.0f;		tempMatrix.e[3][3] = M.e[3][3] * 1.0f;

//			[cos(r), sin(r), 0, 0]
//	Rz =	[-sin(r), cos(r), 0, 0]
//			[0, 0, 1, 0]
//			[0, 0, 0, 1]
	return M;
}


MY_MATRIX_4X4 IdentityMatrix()
{
	MY_MATRIX_4X4 tempMatrix;

	tempMatrix.e[0][0] = 1.0f; tempMatrix.e[0][1] = 0.0f; tempMatrix.e[0][2] = 0.0f; tempMatrix.e[0][3] = 0.0f;
	tempMatrix.e[1][0] = 0.0f; tempMatrix.e[1][1] = 1.0f; tempMatrix.e[1][2] = 0.0f; tempMatrix.e[1][3] = 0.0f;
	tempMatrix.e[2][0] = 0.0f; tempMatrix.e[2][1] = 0.0f; tempMatrix.e[2][2] = 1.0f; tempMatrix.e[2][3] = 0.0f;
	tempMatrix.e[3][0] = 0.0f; tempMatrix.e[3][1] = 0.0f; tempMatrix.e[3][2] = 0.0f; tempMatrix.e[3][3] = 1.0f;

	return tempMatrix;


}

MY_MATRIX_4X4 ViewMatrix(float rot)
{
	MY_MATRIX_4X4 tempMatrix;

	tempMatrix.e[0][0] = 1.0f; tempMatrix.e[0][1] = 0.0f;     tempMatrix.e[0][2] = 0.0f;      tempMatrix.e[0][3] = 0.0f;
	tempMatrix.e[1][0] = 0.0f; tempMatrix.e[1][1] = cos(rot); tempMatrix.e[1][2] = -sin(rot); tempMatrix.e[1][3] = 0.0f;
	tempMatrix.e[2][0] = 0.0f; tempMatrix.e[2][1] = sin(rot); tempMatrix.e[2][2] = cos(rot);  tempMatrix.e[2][3] = 0.0f;
	tempMatrix.e[3][0] = 0.0f; tempMatrix.e[3][1] = 0.0f;     tempMatrix.e[3][2] = 0.0f;      tempMatrix.e[3][3] = 1.0f;

	return tempMatrix;

}

MY_MATRIX_4X4 ViewMatrix_RoateX(MY_MATRIX_4X4 M, float rot)
{
		
	MY_MATRIX_4X4 tempMatrix = IdentityMatrix();
		
	 M.e[1][1] =  cos(rot); M.e[1][2] =  (- sin(rot)) ; 
	 
	 M.e[2][1] =  sin(rot); M.e[2][2] =  cos(rot);  
	
	
	return M;

}
MY_MATRIX_4X4 ViewMatrix_RoateY(MY_MATRIX_4X4 M, float rot)
{
//			[cos(r), 0, -sin(r), 0]
//	Ry =	[0, 1, 0, 0]
//			[sin(r), 0, cos(r), 0]
//			[0, 0, 0, 1]

	MY_MATRIX_4X4 tempMatrix = IdentityMatrix();

	M.e[0][0] = cos(rot); M.e[0][2] = -sin(rot);

	M.e[2][0] = sin(rot); M.e[2][2] = cos(rot);


	return M;

}
MY_MATRIX_4X4 ViewMatrix_RoateZ(MY_MATRIX_4X4 M, float rot)
{

//			[cos(r), sin(r), 0, 0]
//	Rz =	[-sin(r), cos(r), 0, 0]
//			[0, 0, 1, 0]
//			[0, 0, 0, 1]

	MY_MATRIX_4X4 tempMatrix = IdentityMatrix();

	M.e[0][0] = cos(rot); M.e[0][1] = sin(rot);

	M.e[1][0] = -sin(rot); M.e[1][1] = cos(rot);


	return M;

}

MY_MATRIX_4X4 Inverse(MY_MATRIX_4X4 M)
{

	MY_MATRIX_3X3 temp;
	// pulls the 3 X 3 matrix
	temp.e[0][0] = M.e[0][0]; temp.e[0][1] = M.e[1][0]; temp.e[0][2] = M.e[2][0];
	temp.e[1][0] = M.e[0][1]; temp.e[1][1] = M.e[1][1]; temp.e[1][2] = M.e[2][1];
	temp.e[2][0] = M.e[0][2]; temp.e[2][1] = M.e[1][2]; temp.e[2][2] = M.e[2][2];
	
	MY_VERTEX tempVertex;
	// negates the X / Y / Z
	tempVertex.X = -1 * M.e[3][0];	tempVertex.Y = -1 * M.e[3][1];	tempVertex.Z = -1 * M.e[3][2]; tempVertex.W = M.e[3][3];
	
	// multiplies the vertes back into into the metrix
	tempVertex = MultiplyVertexByMatrix3x3(tempVertex, temp);

	M.e[0][0] = temp.e[0][0]; M.e[0][1] = temp.e[0][1]; M.e[0][2] = temp.e[0][2]; //M.e[0][3] = temp.e[0][3];
	M.e[1][0] = temp.e[1][0]; M.e[1][1] = temp.e[1][1]; M.e[1][2] = temp.e[1][2]; //M.e[1][3] = temp.e[1][3];
	M.e[2][0] = temp.e[2][0]; M.e[2][1] = temp.e[2][1]; M.e[2][2] = temp.e[2][2]; //M.e[2][3] = temp.e[2][3];
	M.e[3][0] = tempVertex.X; M.e[3][1] = tempVertex.Y; M.e[3][2] = tempVertex.Z; //M.e[3][3] = temp.e[3][3];

	return M;
}

MY_MATRIX_4X4 Translate(MY_MATRIX_4X4 SV_WorldMatrix, float CartX,float CartY, float CartZ)
{
	
	//SV_WorldMatrix.e[3][0] = SV_WorldMatrix.e[3][0] * CartX; SV_WorldMatrix.e[3][1] = SV_WorldMatrix.e[3][1] * CartY;  SV_WorldMatrix.e[3][2] = SV_WorldMatrix.e[3][2] * CartZ;
	SV_WorldMatrix.e[3][0] = CartX; SV_WorldMatrix.e[3][1] =  CartY;  SV_WorldMatrix.e[3][2] =  CartZ;

	return SV_WorldMatrix;
}

float DegToRad(float deg)
{
	return (deg * (3.14f / 180.0f));
}

MY_VERTEX Vector_Cross(MY_VERTEX v, MY_VERTEX w)
{
	// TODO LAB 1: Replace with your implementation.
	MY_VERTEX tCross;
	tCross.X = v.Y*w.Z - v.Z*w.Y;
	tCross.Y = v.Z*w.X - v.X*w.Z;
	tCross.Z = v.X*w.Y - v.Y*w.X;
	//tCross.W = 0;
	return tCross;
}

float Vector_Dot(MY_VERTEX v, MY_VERTEX w)
{
	// TODO LAB 1: Replace with your implementation.
	return v.X*w.X + v.Y*w.Y + v.Z * w.Z;// +v.Z*w.Z + v.W*w.W;
}

MY_VERTEX Vector_Add(MY_VERTEX v, MY_VERTEX w)
{
	return{ v.X + w.X, v.Y + w.Y , v.Z + w.Z};
}

MY_VERTEX Vector_Subtract(MY_VERTEX v, MY_VERTEX w)
{
	return{ v.X - w.X, v.Y - w.Y, v.Z - w.Z };
}


float Vector_Length(MY_VERTEX v)
{
	return sqrtf(Vector_Dot(v, v));
}

float Max(float first, float second)
{
	return (((first) > (second)) ? (first) : (second));
}
float Min(float first, float second)
{
	return(((first) < (second)) ? (first) : (second));
}

MY_MATRIX_4X4 Matrix_Matrix_Multiply(MY_MATRIX_4X4 m, MY_MATRIX_4X4 n)
{
	// TODO LAB 2: Replace with your implementation.
	MY_MATRIX_4X4 temp =
	{
		m.e[0][0] * n.e[0][0] + m.e[0][1] * n.e[1][0] + m.e[0][2] * n.e[2][0] + m.e[0][3] * n.e[3][0],
		m.e[0][0] * n.e[0][1] + m.e[0][1] * n.e[1][1] + m.e[0][2] * n.e[2][1] + m.e[0][3] * n.e[3][1],
		m.e[0][0] * n.e[0][2] + m.e[0][1] * n.e[1][2] + m.e[0][2] * n.e[2][2] + m.e[0][3] * n.e[3][2],
		m.e[0][0] * n.e[0][3] + m.e[0][1] * n.e[1][3] + m.e[0][2] * n.e[2][3] + m.e[0][3] * n.e[3][3],

		m.e[1][0] * n.e[0][0] + m.e[1][1] * n.e[1][0] + m.e[1][2] * n.e[2][0] + m.e[1][3] * n.e[3][0],
		m.e[1][0] * n.e[0][1] + m.e[1][1] * n.e[1][1] + m.e[1][2] * n.e[2][1] + m.e[1][3] * n.e[3][1],
		m.e[1][0] * n.e[0][2] + m.e[1][1] * n.e[1][2] + m.e[1][2] * n.e[2][2] + m.e[1][3] * n.e[3][2],
		m.e[1][0] * n.e[0][3] + m.e[1][1] * n.e[1][3] + m.e[1][2] * n.e[2][3] + m.e[1][3] * n.e[3][3],
		
		m.e[2][0] * n.e[0][0] + m.e[2][1] * n.e[1][0] + m.e[2][2] * n.e[2][0] + m.e[2][3] * n.e[3][0],
		m.e[2][0] * n.e[0][1] + m.e[2][1] * n.e[1][1] + m.e[2][2] * n.e[2][1] + m.e[2][3] * n.e[3][1],
		m.e[2][0] * n.e[0][2] + m.e[2][1] * n.e[1][2] + m.e[2][2] * n.e[2][2] + m.e[2][3] * n.e[3][2],
		m.e[2][0] * n.e[0][3] + m.e[2][1] * n.e[1][3] + m.e[2][2] * n.e[2][3] + m.e[2][3] * n.e[3][3],

		m.e[3][0] * n.e[0][0] + m.e[3][1] * n.e[1][0] + m.e[3][2] * n.e[2][0] + m.e[3][3] * n.e[3][0],
		m.e[3][0] * n.e[0][1] + m.e[3][1] * n.e[1][1] + m.e[3][2] * n.e[2][1] + m.e[3][3] * n.e[3][1],
		m.e[3][0] * n.e[0][2] + m.e[3][1] * n.e[1][2] + m.e[3][2] * n.e[2][2] + m.e[3][3] * n.e[3][2],
		m.e[3][0] * n.e[0][3] + m.e[3][1] * n.e[1][3] + m.e[3][2] * n.e[2][3] + m.e[3][3] * n.e[3][3],

	
	};

	
	return temp;
}

MY_MATRIX_4X4 Perspective_Projection_Matrix(float FOV, float zFar,float zNear, float aspect)
{
	MY_MATRIX_4X4 ProjectionMatrix;
	
	//Yscale = cotangent(½ Vertical FOV)
	float YScale = (sin(DegToRad(FOV * 0.5f)) / cos(DegToRad(FOV * 0.5f)));
	float XScale = YScale * (aspect);

	ProjectionMatrix.e[0][0] = XScale; ProjectionMatrix.e[0][1] = 0.0f;   ProjectionMatrix.e[0][2] = 0.0f;                ProjectionMatrix.e[0][3] = 0.0f;
	ProjectionMatrix.e[1][0] = 0.0f;   ProjectionMatrix.e[1][1] = YScale; ProjectionMatrix.e[1][2] = 0.0f;                ProjectionMatrix.e[1][3] = 0.0f;
	ProjectionMatrix.e[2][0] = 0.0f;   ProjectionMatrix.e[2][1] = 0.0f; ProjectionMatrix.e[2][2] = zFar / (zFar - zNear); ProjectionMatrix.e[2][3] = 1.0f;
	ProjectionMatrix.e[3][0] = 0.0f;   ProjectionMatrix.e[3][1] = 0.0f; ProjectionMatrix.e[3][2] = -(zFar * zNear) / (zFar - zNear);       ProjectionMatrix.e[3][3] = 0.0f;
		

	//	PerspectiveProjection
//		[
//			XScale, 0, 0, 0,
//			0, YScale, 0, 0,
//			0, 0, zFar / (zFar – zNear), 1,
//			0, 0, -(zFar * zNear) / (zFar – zNear), 0,
//		]

	return ProjectionMatrix;
}

unsigned int ConvertFromBGRAtoARGB(unsigned int _color)
{
	//				           BGRA to ARGB
	//                         0xB G R A
	unsigned int A = (_color & 0x000000FF);
	unsigned int R = (_color & 0x0000FF00) >> 8;
	unsigned int G = (_color & 0x00FF0000) >> 16;
	unsigned int B = (_color & 0xFF000000) >> 24;

	return ((A << 24) | (R << 16) | (G << 8) | B);
	// argb

}

unsigned int ConvertLerp(unsigned int _original, unsigned int _color, float R)
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


	unsigned int return_colorA = (unsigned int)((float)((int)newA - (int)oldA) * R + (float)oldA);
	unsigned int return_colorR = (unsigned int)((float)((int)newR - (int)oldR) * R + (float)oldR);
	unsigned int return_colorG = (unsigned int)((float)((int)newG - (int)oldG) * R + (float)oldG);
	unsigned int return_colorB = (unsigned int)((float)((int)newB - (int)oldB) * R + (float)oldB);

	return ((return_colorA << 24) | (return_colorR << 16) | (return_colorG << 8) | return_colorB);
}