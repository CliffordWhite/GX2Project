#ifndef _MATH_LIB
#define _MATH_LIB
#include <Windows.h>
#include <math.h>
#define PI		3.14159f

struct Matrix3x3
{
	float x1, y1, z1,
		x2, y2, z2,
		x3, y3, z3;

	Matrix3x3(){};
	Matrix3x3(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _x3, float _y3, float _z3) : x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2), x3(_x3), y3(_y3), z3(_z3){};
};

struct Matrix4x4
{
	float x1, y1, z1, w1,
		x2, y2, z2, w2,
		x3, y3, z3, w3,
		x4, y4, z4, w4;


	Matrix4x4(){};
	Matrix4x4(float _x1, float _y1, float _z1, float _w1, float _x2, float _y2, float _z2, float _w2, float _x3, float _y3, float _z3, float _w3, float _x4, float _y4, float _z4, float _w4) : x1(_x1), y1(_y1), z1(_z1), w1(_w1), x2(_x2), y2(_y2), z2(_z2), w2(_w2), x3(_x3), y3(_y3), z3(_z3), w3(_w3), x4(_x4), y4(_y4), z4(_z4), w4(_w4){};
};

struct VERTEX4
{
	float xyzw[4];
	FLOAT RGBA[4];
};

struct VERTEX
{
	float xyzwuv[6];
	UINT Color;
};
struct SIMPLE_VERTEX
{
	float xy[2];
};

struct SIMPLE_VECTOR
{
	float x;
	float y;
	float z;
};


struct ColorVerts
{
	UINT color;
};

//Used to ReverseColors in  BLIT
UINT InverseColors(UINT _Color)
{
	UINT NewColor = 0; // start blank.

	UCHAR R = (_Color >> 8) & 0xff;
	UCHAR G = (_Color >> 16) & 0xff;
	UCHAR B = (_Color >> 24) & 0xff;
	UCHAR A = (_Color & 0xff);

	NewColor |= (R << 16);
	NewColor |= (G << 8);
	NewColor |= B;
	NewColor |= (A << 24);

	return NewColor;
}

// A function to convert 2 dimensional coordinates to a 1 dimensional coordinate.
UINT Convert2D1D(const UINT _X, const UINT _Y, const UINT _Width)
{
	return _Y * _Width + _X;
}

UINT ColorLerp(UINT _Color1, UINT _Color2, FLOAT Ratio)
{
	UINT C1R = (_Color1 >> 16) & 0xff; // Red shifts 16
	UINT C1G = (_Color1 >> 8) & 0xff; // Green Shifts 8
	UINT C1B = _Color1 & 0xFF; // Blue shifts none
	UINT C1A = (_Color1 >> 24) & 0xFF; // Alpha shifts 24 (not 32)
	//  Updating new colors
	UINT UpdatedR = (_Color2 >> 16) & 0xff;
	UINT UpdatedG = (_Color2 >> 8) & 0xff;
	UINT UpdatedB = (_Color2 & 0xff);
	UINT UpdatedA = (_Color2 >> 24) & 0xff;


	UINT NewColor = 0; // New Color

	NewColor |= ((int)(((int)UpdatedR - (int)C1R) * Ratio) + C1R) << 16;
	NewColor |= ((int)(((int)UpdatedG - (int)C1G) * Ratio) + C1G) << 8;
	NewColor |= (int)(((int)UpdatedB - (int)C1B) * Ratio) + C1B;
	NewColor |= ((int)(((int)UpdatedA - (int)C1A) * Ratio) + C1A) << 24;

	return NewColor;
}

FLOAT ImplicitLineEquation(VERTEX Start, VERTEX End, FLOAT MidPointX, FLOAT MidPointY)//change to vertex's (Start point,end point, point to find)
{
	return ((((FLOAT)(Start.xyzwuv[1]) - (FLOAT)(End.xyzwuv[1]))*MidPointX) + (((FLOAT)(End.xyzwuv[0]) - (FLOAT)(Start.xyzwuv[0]))*MidPointY)
		+ ((FLOAT)(Start.xyzwuv[0])*(FLOAT)(End.xyzwuv[1]))- ((FLOAT)(Start.xyzwuv[1])*(FLOAT)(End.xyzwuv[0])));
}
FLOAT lerp(FLOAT Y1, FLOAT Y2, FLOAT Ratio)
{
	return (Y2 - Y1) * Ratio + Y1;
	//return ((Y1 + Ratio)*Y2 - Y1);
}

FLOAT Degrees_To_Radians(float Deg)
{
	return Deg * PI / 180.0f;
}

FLOAT Radians_To_Degrees(float Rad)
{
	return Rad * 180.0f / PI;
}


VERTEX MultiplyVertexByMatrix3x3(VERTEX multiplyMe, Matrix3x3 SV_WorldMatrix)
{
	VERTEX Temp;

	Temp.xyzwuv[0] = multiplyMe.xyzwuv[0] * SV_WorldMatrix.x1 + multiplyMe.xyzwuv[1] * SV_WorldMatrix.x2 + multiplyMe.xyzwuv[2] * SV_WorldMatrix.x3;

	Temp.xyzwuv[1] = multiplyMe.xyzwuv[0] * SV_WorldMatrix.y1 + multiplyMe.xyzwuv[1] * SV_WorldMatrix.y2 + multiplyMe.xyzwuv[2] * SV_WorldMatrix.y3;

	Temp.xyzwuv[2] = multiplyMe.xyzwuv[0] * SV_WorldMatrix.z1 + multiplyMe.xyzwuv[1] * SV_WorldMatrix.z2 + multiplyMe.xyzwuv[2] * SV_WorldMatrix.z3;

	return Temp;
};

Matrix3x3 Matrix3x3RotationOnZ(float Ratio)
{
	Matrix3x3 ZRotation =
	{
		cos(Ratio), -sin(Ratio), 0,
		sin(Ratio), cos(Ratio), 0,
		0, 0, 1
	};

	return ZRotation;
}

Matrix3x3 Matrix3x3RotationOnY(float Ratio)
{
	Matrix3x3 YRotation =
	{
		cos(Ratio), 0, sin(Ratio),
		0, 1, 0,
		-sin(Ratio), 0, cos(Ratio)
	};

	return YRotation;
}

Matrix3x3 Matrix3x3RotationOnX(float Ratio)
{
	Matrix3x3 XRotation =
	{
		1, 0, 0,
		0, cos(Ratio), -sin(Ratio),
		0, sin(Ratio), cos(Ratio)
	};

	return XRotation;
}

float Matrix3x3_Determinant(float e_11, float e_12, float e_13, float e_21, float e_22, float e_23, float e_31, float e_32, float e_33)
{

	return (e_11*((e_22*e_33) - (e_32*e_23)) - e_12*(e_21*e_33 - e_31*e_23) + e_13*(e_21*e_32 - e_31*e_22));
}

Matrix4x4 Matrix4x4RotationOnZ(float Ratio)
{
	Matrix4x4 ZRotation =
	{ cos(Ratio), -sin(Ratio), 0, 0,
	sin(Ratio), cos(Ratio), 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1 };

	return ZRotation;
}

Matrix4x4 Matrix4x4RotationOnY(float Ratio)
{
	Matrix4x4 YRotation =
	{ cos(Ratio), 0, sin(Ratio), 0,
	0, 1, 0, 0,
	-sin(Ratio), 0, cos(Ratio), 0,
	0, 0, 0, 1 };

	return YRotation;
}

Matrix4x4 Matrix4x4RotationOnX(float Ratio)
{
	Matrix4x4 XRotation =
	{ 1, 0, 0, 0,
	0, cos(Ratio), -sin(Ratio), 0,
	0, sin(Ratio), cos(Ratio), 0,
	0, 0, 0, 1 };

	return XRotation;
}

VERTEX MultiplyVertexByMatrix4x4(VERTEX v, Matrix4x4 m)
{

	VERTEX _Temp = v;
	_Temp.xyzwuv[0] = (m.x1*v.xyzwuv[0]) + (m.x2*v.xyzwuv[1]) + (m.x3*v.xyzwuv[2]) + (m.x4*v.xyzwuv[3]);
	_Temp.xyzwuv[1] = (m.y1*v.xyzwuv[0]) + (m.y2*v.xyzwuv[1]) + (m.y3*v.xyzwuv[2]) + (m.y4*v.xyzwuv[3]);
	_Temp.xyzwuv[2] = (m.z1*v.xyzwuv[0]) + (m.z2*v.xyzwuv[1]) + (m.z3*v.xyzwuv[2]) + (m.z4*v.xyzwuv[3]);
	_Temp.xyzwuv[3] = (m.w1*v.xyzwuv[0]) + (m.w2*v.xyzwuv[1]) + (m.w3*v.xyzwuv[2]) + (m.w4*v.xyzwuv[3]);
	
	return _Temp;
}

Matrix4x4 Matrix4x4_Scalar_Multiply(Matrix4x4 m, float s)
{

	m.x1 *= s;
	m.y1 *= s;
	m.z1 *= s;
	m.w1 *= s;

	m.x2 *= s;
	m.y2 *= s;
	m.z2 *= s;
	m.w2 *= s;

	m.x3 *= s;
	m.y3 *= s;
	m.z3 *= s;
	m.w3 *= s;

	m.x4 *= s;
	m.y4 *= s;
	m.z4 *= s;
	m.w4 *= s;
	return m;
}

Matrix4x4 Matrix4x4_Multiply(Matrix4x4 m, Matrix4x4 n)
{
	Matrix4x4 _Temp;
	_Temp.x1 = (m.x1*n.x1) + (m.y1*n.x2) + (m.z1*n.x3) + (m.w1*n.x4);
	_Temp.y1 = (m.x1*n.y1) + (m.y1*n.y2) + (m.z1*n.y3) + (m.w1*n.y4);
	_Temp.z1 = (m.x1*n.z1) + (m.y1*n.z2) + (m.z1*n.z3) + (m.w1*n.z4);
	_Temp.w1 = (m.x1*n.w1) + (m.y1*n.w2) + (m.z1*n.w3) + (m.w1*n.w4);

	_Temp.x2 = (m.x2*n.x1) + (m.y2*n.x2) + (m.z2*n.x3) + (m.w2*n.x4);
	_Temp.y2 = (m.x2*n.y1) + (m.y2*n.y2) + (m.z2*n.y3) + (m.w2*n.y4);
	_Temp.z2 = (m.x2*n.z1) + (m.y2*n.z2) + (m.z2*n.z3) + (m.w2*n.z4);
	_Temp.w2 = (m.x2*n.w1) + (m.y2*n.w2) + (m.z2*n.w3) + (m.w2*n.w4);

	_Temp.x3 = (m.x3*n.x1) + (m.y3*n.x2) + (m.z3*n.x3) + (m.w3*n.x4);
	_Temp.y3 = (m.x3*n.y1) + (m.y3*n.y2) + (m.z3*n.y3) + (m.w3*n.y4);
	_Temp.z3 = (m.x3*n.z1) + (m.y3*n.z2) + (m.z3*n.z3) + (m.w3*n.z4);
	_Temp.w3 = (m.x3*n.w1) + (m.y3*n.w2) + (m.z3*n.w3) + (m.w3*n.w4);

	_Temp.x4 = (m.x4*n.x1) + (m.y4*n.x2) + (m.z4*n.x3) + (m.w4*n.x4);
	_Temp.y4 = (m.x4*n.y1) + (m.y4*n.y2) + (m.z4*n.y3) + (m.w4*n.y4);
	_Temp.z4 = (m.x4*n.z1) + (m.y4*n.z2) + (m.z4*n.z3) + (m.w4*n.z4);
	_Temp.w4 = (m.x4*n.w1) + (m.y4*n.w2) + (m.z4*n.w3) + (m.w4*n.w4);

	return _Temp;
}

Matrix4x4 Matrix4x4_Transpose(Matrix4x4 m)
{

	Matrix4x4 _Temp;
	_Temp.x1 = m.x1;
	_Temp.x2 = m.y1;
	_Temp.x3 = m.z1;
	_Temp.x4 = m.w1;

	_Temp.y1 = m.x2;
	_Temp.y2 = m.y2;
	_Temp.y3 = m.z2;
	_Temp.y4 = m.w2;

	_Temp.z1 = m.x3;
	_Temp.z2 = m.y3;
	_Temp.z3 = m.z3;
	_Temp.z4 = m.w3;

	_Temp.w1 = m.x4;
	_Temp.w2 = m.y4;
	_Temp.w3 = m.z4;
	_Temp.w4 = m.w4;

	m = _Temp;
	return m;
}

float Matrix4x4_Determinant(Matrix4x4 m)
{

	return m.x1*(Matrix3x3_Determinant(m.y2, m.z2, m.w2, m.y3, m.z3, m.w3, m.y4, m.z4, m.w4)) -
		m.y1*(Matrix3x3_Determinant(m.x2, m.z2, m.w2, m.x3, m.z3, m.w3, m.x4, m.z4, m.w4)) +
		m.z1*(Matrix3x3_Determinant(m.x2, m.y2, m.w2, m.x3, m.y3, m.w3, m.x4, m.y4, m.w4)) -
		m.w1*(Matrix3x3_Determinant(m.x2, m.y2, m.z2, m.x3, m.y3, m.z3, m.x4, m.y4, m.z4));
}

Matrix4x4 Matrix4x4_Inverse(Matrix4x4 m)
{

	float Det = Matrix4x4_Determinant(m);
	if (Det == 0)
		return m;

	Det = 1.0F / Det;
	Matrix4x4 _Temp;
	_Temp.x1 = 1.0F *  (Matrix3x3_Determinant(m.y2, m.z2, m.w2, m.y3, m.z3, m.w3, m.y4, m.z4, m.w4));
	_Temp.y1 = -1.0F * (Matrix3x3_Determinant(m.x2, m.z2, m.w2, m.x3, m.z3, m.w3, m.x4, m.z4, m.w4));
	_Temp.z1 = 1.0F *  (Matrix3x3_Determinant(m.x2, m.y2, m.w2, m.x3, m.y3, m.w3, m.x4, m.y4, m.w4));
	_Temp.w1 = -1.0F * (Matrix3x3_Determinant(m.x2, m.y2, m.z2, m.x3, m.y3, m.z3, m.x4, m.y4, m.z4));

	_Temp.x2 = -1.0F * (Matrix3x3_Determinant(m.y1, m.z1, m.w1, m.y3, m.z3, m.w3, m.y4, m.z4, m.w4));
	_Temp.y2 = 1.0F *  (Matrix3x3_Determinant(m.x1, m.z1, m.w1, m.x3, m.z3, m.w3, m.x4, m.z4, m.w4));
	_Temp.z2 = -1.0F * (Matrix3x3_Determinant(m.x1, m.y1, m.w1, m.x3, m.y3, m.w3, m.x4, m.y4, m.w4));
	_Temp.w2 = 1.0F *  (Matrix3x3_Determinant(m.x1, m.y1, m.z1, m.x3, m.y3, m.z3, m.x4, m.y4, m.z4));

	_Temp.x3 = 1.0F *  (Matrix3x3_Determinant(m.y1, m.z1, m.w1, m.y2, m.z2, m.w2, m.y4, m.z4, m.w4));
	_Temp.y3 = -1.0F * (Matrix3x3_Determinant(m.x1, m.z1, m.w1, m.x2, m.z2, m.w2, m.x4, m.z4, m.w4));
	_Temp.z3 = 1.0F *  (Matrix3x3_Determinant(m.x1, m.y1, m.w1, m.x2, m.y2, m.w2, m.x4, m.y4, m.w4));
	_Temp.w3 = -1.0F * (Matrix3x3_Determinant(m.x1, m.y1, m.z1, m.x2, m.y2, m.z2, m.x4, m.y4, m.z4));

	_Temp.x4 = -1.0F * (Matrix3x3_Determinant(m.y1, m.z1, m.w1, m.y2, m.z2, m.w2, m.y3, m.z3, m.w3));
	_Temp.y4 = 1.0F *  (Matrix3x3_Determinant(m.x1, m.z1, m.w1, m.x2, m.z2, m.w2, m.x3, m.z3, m.w3));
	_Temp.z4 = -1.0F * (Matrix3x3_Determinant(m.x1, m.y1, m.w1, m.x2, m.y2, m.w2, m.x3, m.y3, m.w3));
	_Temp.w4 = 1.0F *  (Matrix3x3_Determinant(m.x1, m.y1, m.z1, m.x2, m.y2, m.z2, m.x3, m.y3, m.z3));

	_Temp = Matrix4x4_Transpose(_Temp);

	_Temp = Matrix4x4_Scalar_Multiply(_Temp, Det);

	return _Temp;
}

Matrix4x4 Matrix4x4_Identity(void)
{
	Matrix4x4 m = { 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };
	return m;
}

Matrix4x4 Matrix4x4_Translation(float x, float y, float z)
{
	Matrix4x4 m = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x,y, z, 1 };
	return m;
}

Matrix4x4 Matrix4x4_Create_Scale(float x, float y, float z)
{

	Matrix4x4 m = { x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1 };
	return m;
}

Matrix4x4 Matrix4x4_CreateProjection(FLOAT ZNear, FLOAT ZFar, FLOAT FOV, FLOAT AspectRatio)
{
	FLOAT Yscale = FLOAT(1/tan(FOV * 0.5f));
	FLOAT Xscale = Yscale * AspectRatio;

	Matrix4x4 PerspectiveProjection = {
		Xscale, 0, 0, 0,
		0, Yscale, 0, 0,
		0, 0, (ZFar / (ZFar - ZNear)), 1,
		0, 0, (-(ZFar * ZNear) / (ZFar - ZNear)), 0,
	};

	return PerspectiveProjection;

}
#endif