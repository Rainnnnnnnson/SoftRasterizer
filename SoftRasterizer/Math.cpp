#include "Math.h"
#include <cmath>

Point2 Point2::operator*(float f) const {
	return {x * f, y * f};
}

Point2 Point2::operator+(Point2 p) const {
	return {x + p.x, y + p.y};
}

Point2 Point2::operator+(Vector2 v) const {
	return {x + v.x, y + v.y};
}

Vector2 Point2::operator-(Point2 p) const {
	return {x - p.x, y - p.y};
}

Vector2 Point2::GetVector2() const {
	return {x, y};
}

Point3 Point3::operator*(float f) const {
	return {x * f, y * f, z * f};
}

Point3 Point3::operator+(Point3 p) const {
	return {x + p.x, y + p.y, z + p.z};
}

Point3 Point3::operator+(Vector3 v) const {
	return {x + v.x, y + v.y, z + v.z};
}

Vector3 Point3::operator-(Point3 p) const {
	return {x - p.x, y - p.y, z - p.z};
}

Vector3 Point3::GetVector3() const {
	return {x, y, z};
}

Point4 Point3::ToPoint4() const {
	return {x, y, z, 1.0f};
}

Point4 Point4::operator*(float f) const {
	return {x * f, y * f, z * f, w * f};
}

Point4 Point4::operator+(Point4 p) const {
	return {x + p.x, y + p.y, z + p.z, w + p.w};
}

Point4 Point4::operator+(Vector4 v) const {
	return {x + v.x, y + v.y, z + v.z, w + v.w};
}

Vector4 Point4::operator-(Point4 p) const {
	return {x - p.x, y - p.y, z - p.z, w - p.w};
}

Vector4 Point4::GetVector4() const {
	return {x, y, z, w};
}

Point3 Point4::ToPoint3() const {
	return Point3{x / w, y / w, z / w};
}

Vector2 Vector2::operator*(float f) const {
	return {x * f, y * f};
}

Vector2 Vector2::operator+(Vector2 v) const {
	return {x + v.x, y + v.y};
}

Vector2 Vector2::operator-() const {
	return {-x, -y};
}

float Vector2::Dot(Vector2 v) const {
	return x * v.x + y * v.y;
}

float Vector2::Cross(Vector2 v) const {
	return x * v.y - y * v.x;
}

float Vector2::Length() const {
	return sqrt(x * x + y * y);
}

Vector2 Vector2::Normalize() const {
	float length = Length();
	return {x / length, y / length};
}

Point2 Vector2::GetPoint2() const {
	return {x, y};
}

Vector3 Vector3::operator*(float f) const {
	return {x * f, y * f, z * f};
}

Vector3 Vector3::operator+(Vector3 v) const {
	return {x + v.x, y + v.y, z + v.z};
}

Vector3 Vector3::operator-() const {
	return {-x, -y, -z};
}

float Vector3::Dot(Vector3 v) const {
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::Cross(Vector3 v) const {
	return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
}

float Vector3::Length() const {
	return sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::Normalize() const {
	float length = Length();
	return {x / length, y / length, z / length};
}

Point3 Vector3::GetPoint3() const {
	return {x, y, z};
}

Vector4 Vector4::operator*(float f) const {
	return {x * f, y * f, z * f, w * f};
}

Vector4 Vector4::operator+(Vector4 v) const {
	return {x + v.x, y + v.y, z + v.z, w + v.w};
}

Vector4 Vector4::operator-() const {
	return {-x, -y, -z, -w};
}

float Vector4::Dot(Vector4 v) const {
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

Point4 Vector4::GetPoint4() const {
	return {x, y, z, w};
}

Point3 Matrix3X3::operator*(Point3 p) const {
	return {
		f[0][0] * p.x + f[0][1] * p.y + f[0][2] * p.z,
		f[1][0] * p.x + f[1][1] * p.y + f[1][2] * p.z,
		f[2][0] * p.x + f[2][1] * p.y + f[2][2] * p.z,
	};
}

Matrix3X3 Matrix3X3::operator*(Matrix3X3 m) const {
	return {
		//第一行
		f[0][0] * m.f[0][0] + f[0][1] * m.f[1][0] + f[0][2] * m.f[2][0],
		f[0][0] * m.f[0][1] + f[0][1] * m.f[1][1] + f[0][2] * m.f[2][1],
		f[0][0] * m.f[0][2] + f[0][1] * m.f[1][2] + f[0][2] * m.f[2][2],
		//第二行
		f[1][0] * m.f[0][0] + f[1][1] * m.f[1][0] + f[1][2] * m.f[2][0],
		f[1][0] * m.f[0][1] + f[1][1] * m.f[1][1] + f[1][2] * m.f[2][1],
		f[1][0] * m.f[0][2] + f[1][1] * m.f[1][2] + f[1][2] * m.f[2][2],
		//第三行
		f[2][0] * m.f[0][0] + f[2][1] * m.f[1][0] + f[2][2] * m.f[2][0],
		f[2][0] * m.f[0][1] + f[2][1] * m.f[1][1] + f[2][2] * m.f[2][1],
		f[2][0] * m.f[0][2] + f[2][1] * m.f[1][2] + f[2][2] * m.f[2][2]
	};
}

Matrix3X3 Matrix3X3::Inverse() const {
	float a00 = +(f[1][1] * f[2][2] - f[1][2] * f[2][1]);
	float a01 = -(f[1][0] * f[2][2] - f[1][2] * f[2][0]);
	float a02 = +(f[1][0] * f[2][1] - f[1][1] * f[2][0]);

	float a10 = -(f[0][1] * f[2][2] - f[0][2] * f[2][1]);
	float a11 = +(f[0][0] * f[2][2] - f[0][2] * f[2][0]);
	float a12 = -(f[0][0] * f[2][1] - f[0][1] * f[2][0]);

	float a20 = +(f[0][1] * f[1][2] - f[0][2] * f[1][1]);
	float a21 = -(f[0][0] * f[1][2] - f[0][2] * f[1][0]);
	float a22 = +(f[0][0] * f[1][1] - f[0][1] * f[1][0]);

	float d = Determinant();

	return {
		a00 / d, a10 / d, a20 / d,
		a01 / d, a11 / d, a21 / d,
		a02 / d, a12 / d, a22 / d
	};
}

Matrix3X3 Matrix3X3::Transpose() const {
	return {
		f[0][0], f[1][0], f[2][0],
		f[0][1], f[1][1], f[2][1],
		f[0][2], f[1][2], f[2][2],
	};
}

float Matrix3X3::Determinant() const {
	float f0 = f[0][0] * f[1][1] * f[2][2] - f[0][0] * f[1][2] * f[2][1];
	float f1 = f[0][1] * f[1][2] * f[2][0] - f[0][1] * f[1][0] * f[2][2];
	float f2 = f[0][2] * f[1][0] * f[2][1] - f[0][2] * f[1][1] * f[2][0];
	return f0 + f1 + f2;
}

Matrix4X4 Matrix3X3::ToMatrix4X4() const {
	return {
		f[0][0], f[0][1], f[0][2], 0.0f,
		f[1][0], f[1][1], f[1][2], 0.0f,
		f[2][0], f[2][1], f[2][2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Vector3 Matrix3X3::TransformVector(Vector3 v) const {
	return {
		f[0][0] * v.x + f[0][1] * v.y + f[0][2] * v.z,
		f[1][0] * v.x + f[1][1] * v.y + f[1][2] * v.z,
		f[2][0] * v.x + f[2][1] * v.y + f[2][2] * v.z,
	};
}

Point4 Matrix4X4::operator*(Point4 p) const {
	float x = f[0][0] * p.x + f[0][1] * p.y + f[0][2] * p.z + f[0][3] * p.w;
	float y = f[1][0] * p.x + f[1][1] * p.y + f[1][2] * p.z + f[1][3] * p.w;
	float z = f[2][0] * p.x + f[2][1] * p.y + f[2][2] * p.z + f[2][3] * p.w;
	float w = f[3][0] * p.x + f[3][1] * p.y + f[3][2] * p.z + f[3][3] * p.w;
	return {x, y, z, w};
}

Matrix4X4 Matrix4X4::operator*(Matrix4X4 m) const {
	return Matrix4X4{
		//第一行
		f[0][0] * m.f[0][0] + f[0][1] * m.f[1][0] + f[0][2] * m.f[2][0] + f[0][3] * m.f[3][0],
		f[0][0] * m.f[0][1] + f[0][1] * m.f[1][1] + f[0][2] * m.f[2][1] + f[0][3] * m.f[3][1],
		f[0][0] * m.f[0][2] + f[0][1] * m.f[1][2] + f[0][2] * m.f[2][2] + f[0][3] * m.f[3][2],
		f[0][0] * m.f[0][3] + f[0][1] * m.f[1][3] + f[0][2] * m.f[2][3] + f[0][3] * m.f[3][3],
		//第二行
		f[1][0] * m.f[0][0] + f[1][1] * m.f[1][0] + f[1][2] * m.f[2][0] + f[1][3] * m.f[3][0],
		f[1][0] * m.f[0][1] + f[1][1] * m.f[1][1] + f[1][2] * m.f[2][1] + f[1][3] * m.f[3][1],
		f[1][0] * m.f[0][2] + f[1][1] * m.f[1][2] + f[1][2] * m.f[2][2] + f[1][3] * m.f[3][2],
		f[1][0] * m.f[0][3] + f[1][1] * m.f[1][3] + f[1][2] * m.f[2][3] + f[1][3] * m.f[3][3],
		//第三行
		f[2][0] * m.f[0][0] + f[2][1] * m.f[1][0] + f[2][2] * m.f[2][0] + f[2][3] * m.f[3][0],
		f[2][0] * m.f[0][1] + f[2][1] * m.f[1][1] + f[2][2] * m.f[2][1] + f[2][3] * m.f[3][1],
		f[2][0] * m.f[0][2] + f[2][1] * m.f[1][2] + f[2][2] * m.f[2][2] + f[2][3] * m.f[3][2],
		f[2][0] * m.f[0][3] + f[2][1] * m.f[1][3] + f[2][2] * m.f[2][3] + f[2][3] * m.f[3][3],
		//第四行
		f[3][0] * m.f[0][0] + f[3][1] * m.f[1][0] + f[3][2] * m.f[2][0] + f[3][3] * m.f[3][0],
		f[3][0] * m.f[0][1] + f[3][1] * m.f[1][1] + f[3][2] * m.f[2][1] + f[3][3] * m.f[3][1],
		f[3][0] * m.f[0][2] + f[3][1] * m.f[1][2] + f[3][2] * m.f[2][2] + f[3][3] * m.f[3][2],
		f[3][0] * m.f[0][3] + f[3][1] * m.f[1][3] + f[3][2] * m.f[2][3] + f[3][3] * m.f[3][3]
	};
}

Matrix4X4 Matrix4X4::Inverse() const {
	float a00 = +Matrix3X3{
		f[1][1], f[1][2], f[1][3],
		f[2][1], f[2][2], f[2][3],
		f[3][1], f[3][2], f[3][3]
	}.Determinant();
	float a01 = -Matrix3X3{
		f[1][0], f[1][2], f[1][3],
		f[2][0], f[2][2], f[2][3],
		f[3][0], f[3][2], f[3][3]
	}.Determinant();
	float a02 = +Matrix3X3{
		f[1][0], f[1][1], f[1][3],
		f[2][0], f[2][1], f[2][3],
		f[3][0], f[3][1], f[3][3]
	}.Determinant();
	float a03 = -Matrix3X3{
		f[1][0], f[1][1], f[1][2],
		f[2][0], f[2][1], f[2][2],
		f[3][0], f[3][1], f[3][2]
	}.Determinant();

	float a10 = -Matrix3X3{
		f[0][1], f[0][2], f[0][3],
		f[2][1], f[2][2], f[2][3],
		f[3][1], f[3][2], f[3][3]
	}.Determinant();
	float a11 = +Matrix3X3{
		f[0][0], f[0][2], f[0][3],
		f[2][0], f[2][2], f[2][3],
		f[3][0], f[3][2], f[3][3]
	}.Determinant();
	float a12 = -Matrix3X3{
		f[0][0], f[0][1], f[0][3],
		f[2][0], f[2][1], f[2][3],
		f[3][0], f[3][1], f[3][3]
	}.Determinant();
	float a13 = +Matrix3X3{
		f[0][0], f[0][1], f[0][2],
		f[2][0], f[2][1], f[2][2],
		f[3][0], f[3][1], f[3][2]
	}.Determinant();

	float a20 = +Matrix3X3{
		f[0][1], f[0][2], f[0][3],
		f[1][1], f[1][2], f[1][3],
		f[3][1], f[3][2], f[3][3]
	}.Determinant();
	float a21 = -Matrix3X3{
		f[0][0], f[0][2], f[0][3],
		f[1][0], f[1][2], f[1][3],
		f[3][0], f[3][2], f[3][3]
	}.Determinant();
	float a22 = +Matrix3X3{
		f[0][0], f[0][1], f[0][3],
		f[1][0], f[1][1], f[1][3],
		f[3][0], f[3][1], f[3][3]
	}.Determinant();
	float a23 = -Matrix3X3{
		f[0][0], f[0][1], f[0][2],
		f[1][0], f[1][1], f[1][2],
		f[3][0], f[3][1], f[3][2]
	}.Determinant();

	float a30 = -Matrix3X3{
		f[0][1], f[0][2], f[0][3],
		f[1][1], f[1][2], f[1][3],
		f[2][1], f[2][2], f[2][3]
	}.Determinant();
	float a31 = +Matrix3X3{
		f[0][0], f[0][2], f[0][3],
		f[1][0], f[1][2], f[1][3],
		f[2][0], f[2][2], f[2][3]
	}.Determinant();
	float a32 = -Matrix3X3{
		f[0][0], f[0][1], f[0][3],
		f[1][0], f[1][1], f[1][3],
		f[2][0], f[2][1], f[2][3]
	}.Determinant();
	float a33 = +Matrix3X3{
		f[0][0], f[0][1], f[0][2],
		f[1][0], f[1][1], f[1][2],
		f[2][0], f[2][1], f[2][2]
	}.Determinant();

	float d = Determinant();

	return{
		a00 / d, a10 / d, a20 / d, a30 / d,
		a01 / d, a11 / d, a21 / d, a31 / d,
		a02 / d, a12 / d, a22 / d, a32 / d,
		a03 / d, a13 / d, a23 / d, a33 / d
	};
}

Matrix4X4 Matrix4X4::Transpose() const {
	return {
		f[0][0], f[1][0], f[2][0], f[3][0],
		f[0][1], f[1][1], f[2][1], f[3][1],
		f[0][2], f[1][2], f[2][2], f[3][2],
		f[0][3], f[1][3], f[2][3], f[3][3],
	};
}

float Matrix4X4::Determinant() const {
	float a00 = +f[0][0] * Matrix3X3{
		f[1][1], f[1][2], f[1][3],
		f[2][1], f[2][2], f[2][3],
		f[3][1], f[3][2], f[3][3]
	}.Determinant();
	float a01 = -f[0][1] * Matrix3X3{
		f[1][0], f[1][2], f[1][3],
		f[2][0], f[2][2], f[2][3],
		f[3][0], f[3][2], f[3][3]
	}.Determinant();
	float a02 = +f[0][2] * Matrix3X3{
		f[1][0], f[1][1], f[1][3],
		f[2][0], f[2][1], f[2][3],
		f[3][0], f[3][1], f[3][3]
	}.Determinant();
	float a03 = -f[0][3] * Matrix3X3{
		f[1][0], f[1][1], f[1][2],
		f[2][0], f[2][1], f[2][2],
		f[3][0], f[3][1], f[3][2]
	}.Determinant();
	return a00 + a01 + a02 + a03;
}

Matrix3X3 Matrix4X4::ToMatrix3X3() const {
	return {
		f[0][0], f[0][1], f[0][2],
		f[1][0], f[1][1], f[1][2],
		f[2][0], f[2][1], f[2][2]
	};
}

Color Color::operator*(float f) const {
	return {r * f, g * f, b * f};
}

Color Color::operator+(Color c) const {
	return {r + c.r, g + c.g, b + c.b};
}

Color Color::operator*(Color c) const {
	return {r * c.r, g * c.g, b * c.b};
}

Matrix4X4 CameraLookTo(Point3 eye, Vector3 direction, Vector3 up) {
	auto right = up.Cross(direction).Normalize();
	Matrix4X4 coordinate{
		right.x, right.y, right.z, 0.0f,
		up.x, up.y, up.z, 0.0f,
		direction.x, direction.y, direction.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	Matrix4X4 move{
		1.0f, 0.0f, 0.0f, -eye.x,
		0.0f, 1.0f, 0.0f, -eye.y,
		0.0f, 0.0f, 1.0f, -eye.z,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return coordinate * move;
}

Matrix4X4 CameraLookAt(Point3 eye, Point3 target, Vector3 up) {
	Vector3 direction = target - eye;
	return CameraLookTo(eye, direction, up);
}

Matrix4X4 Scale(float x, float y, float z) {
	return {
		x, 0.0f, 0.0f, 0.0f,
		0.0f, y, 0.0f, 0.0f,
		0.0f, 0.0f, z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4X4 Move(Vector3 direction) {
	return {
		1.0f, 0.0f, 0.0f, direction.x,
		0.0f, 1.0f, 0.0f, direction.y,
		0.0f, 0.0f, 1.0f, direction.z,
		0.0f, 0.0f, 0.0f, 1.0,
	};
}

Matrix4X4 Orthogonal(float n, float f, float l, float r, float b, float t) {
	return {
		(2.0f * n) / (r - l), 0.0f, 0.0f, -(r + l) / (r - l),
		0.0f, (2.0f * n) / (t - b), 0.0f, -(t + b) / (t - b),
		0.0f, 0.0f, 1.0f / (f - n), -n / (f - n),
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4X4 OrthogonalByAspect(float n, float f, float aspect) {
	return Orthogonal(n, f, -aspect, aspect, -1.0f, 1.0f);
}


Matrix4X4 Perspective(float n, float f, float l, float r, float b, float t) {
	return {
		(2.0f * n) / (r - l), 0.0f, -(r + l) / (r - l), 0.0f,
		0.0f, (2.0f * n) / (t - b), -(t + b) / (t - b), 0.0f,
		0.0f, 0.0f, f / (f - n), -(n * f) / (f - n),
		0.0f, 0.0f, 1.0f, 0.0f
	};
}

Matrix4X4 PerspectiveByAspect(float n, float f, float aspect) {
	return Perspective(n, f, -aspect, aspect, -1.0f, 1.0f);
}

Matrix4X4 RotateX(float r) {
	float sinX = sin(r);
	float cosX = cos(r);
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosX, -sinX, 0.0f,
		0.0f, sinX, cosX, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4X4 RotateY(float r) {
	float sinX = sin(r);
	float cosX = cos(r);
	return {
		cosX, 0.0f, sinX, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinX, 0.0f, cosX, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4X4 RotateZ(float r) {
	float sinX = sin(r);
	float cosX = cos(r);
	return {
		cosX, -sinX, 0.0f, 0.0f, 
		sinX, cosX, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}