#pragma once

struct Point2;
struct Point3;
struct Point4;
struct Vector3;
struct Vector4;
struct Matrix3X3;
struct Matrix4X4;
struct Color;

float ComputeLineEquation(Point2 p, Point2 p0, Point2 p1);

struct Point2 {
	float x, y;
	bool operator== (Point2 p) const;
	Point2 operator*(float f) const;
	Point2 operator+(Point2 p) const;
};

struct Point3 {
	float x, y, z;
	Vector3 operator-(Point3 p) const;
	Point3 operator+(Vector3 v) const;
	Vector3 GetVector3() const;
	Point2 GetPoint2() const;
	//(x, y, z, 1)
	Point4 ToPoint4() const;
};

struct Point4 {
	float x, y, z, w;
	bool operator==(Point4 p) const;
	//主要用于重心计算
	Point4 operator+(Point4 p) const;
	Point4 operator*(float f) const;
	Vector4 operator-(Point4 p) const;
	Point4 operator+(Vector4 v) const;
	Vector4 GetVector4() const;
	//(x/w, y/w)
	Point2 ToPoint2() const;
	//(x/w, y/w, z/w)
	Point3 ToPoint3() const;
};

struct Vector3 {
	float x, y, z;
	Vector3 operator*(float f) const;
	Vector3 operator+(Vector3 v) const;
	Point3 operator-(Vector3 v) const;
	float Dot(Vector3 v) const;
	Vector3 Cross(Vector3 v) const;
	float Length() const;
	Vector3 Normalize() const;
};

struct Vector4 {
	float x, y, z, w;
	float Dot(Vector4 v) const;
	Vector4 operator*(float f) const;
};

struct Matrix3X3 {
	float f[3][3];
	Matrix4X4 ToMatrix4X4() const;
	Matrix3X3 Inverse() const;
	Matrix3X3 Transpose() const;
	float Determinant() const;
	Matrix3X3 operator*(const Matrix3X3& m) const;
	Point3 operator*(Point3 p) const;
	Vector3 operator*(Vector3 v) const;
};

struct Matrix4X4 {
	float f[4][4];
	Matrix3X3 GetMatrix3X3() const;
	Matrix4X4 Inverse() const;
	Matrix4X4 Transpose() const;
	float Determinant() const;
	Matrix4X4 operator*(const Matrix4X4& m) const;
	Point4 operator*(Point4 p) const;
};

Matrix4X4 CameraLookTo(Point3 eye, Vector3 direction, Vector3 up);
Matrix4X4 Scale(float x, float y, float z);
Matrix4X4 Move(Vector3 direction);
Matrix4X4 PerspectiveByAspect(float n, float f, float aspect);
Matrix4X4 Perspective(float n, float f, float l, float r, float b, float t);
Matrix4X4 RotateX(float radian);
Matrix4X4 RotateY(float radian);
Matrix4X4 RotateZ(float radian);

struct Color {
	float r, g, b;
	Color operator+(Color c) const;
	Color operator-(Color c) const;
	Color operator*(float f) const;
	Color operator*(Color c) const;
	Color operator/(Color c) const;
};