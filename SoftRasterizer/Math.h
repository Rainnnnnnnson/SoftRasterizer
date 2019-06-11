#pragma once

struct Point2;
struct Point3;
struct Point4;
struct Vector3;
struct Matrix3X3;
struct Matrix4X4;
struct Color;

// y = kx + b
float ComputeLine(float k, float x, float b);

float ComputeLineEquation(Point2 p, Point2 p0, Point2 p1);

struct Point2 {
	float x, y;
	bool operator== (const Point2& p) const;
};

struct Point3 {
	float x, y, z;
	Vector3 operator-(const Point3& p) const;
	Point3 operator+(const Vector3& v) const;
	//(x, y)
	Point2 GetPoint2() const;
	//(x, y, z, 1)
	Point4 ToPoint4() const;
};

struct Point4 {
	float x, y, z, w;
	//(x/w, y/w, z/w)
	bool operator==(const Point4& p) const;
	Point3 ToPoint3() const;
};

struct Vector3 {
	float x, y, z;
	friend Vector3 operator*(const float& f, const Vector3& v);
	Vector3 operator+(const Vector3& v) const;
	Vector3 operator-(const Vector3& v) const;
	float Dot(const Vector3& v) const;
	Vector3 Cross(const Vector3& v) const;
	Vector3 Normalize() const;
	Vector3 Negative() const;
};

struct Matrix3X3 {
	float f[3][3];
	Matrix4X4 ToMatrix4X4() const;
	Matrix3X3 Inverse() const;
	Matrix3X3 Transpose() const;
	float Determinant() const;
	Matrix3X3 operator*(const Matrix3X3& m) const;
	Point3 operator*(const Point3& p) const;
	Vector3 operator*(const Vector3& v) const;
};

struct Matrix4X4 {
	float f[4][4];
	Matrix3X3 GetMatrix3X3() const;
	Matrix4X4 Inverse() const;
	Matrix4X4 Transpose() const;
	float Determinant() const;
	Matrix4X4 operator*(const Matrix4X4& m) const;
	Point4 operator*(const Point4& p) const;
};

Matrix4X4 Scale(float x, float y, float z);
Matrix4X4 Move(Vector3 direction);
Matrix4X4 Perspective(float n, float f, float l, float r, float b, float t);
Matrix4X4 RotateX(float radian);
Matrix4X4 RotateY(float radian);
Matrix4X4 RotateZ(float radian);

struct Color {
	float r, g, b;
	Color operator+(const Color& c) const;
	Color operator-(const Color& c) const;
	Color operator*(const Color& c) const;
	Color operator/(const Color& c) const;
};