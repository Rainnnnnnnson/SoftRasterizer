#pragma once

struct Point2;
struct Point3;
struct Point4;
struct Vector3;
struct Matrix;
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
	//(x, y)
	Point2 GetPoint2() const;
	//(x, y, z, 1)
	Point4 ToPoint4() const;
};

struct Point4 {
	float x, y, z, w;
	//(x/w, y/w, z/w)
	Point3 ToPoint3() const;
};

struct Vector3 {
	float x, y, z;
	float Dot(const Vector3& v) const;
	Vector3 Cross(const Vector3& v) const;
};

struct Matrix {
	float m[4][4];
};

struct Color {
	float r, g, b;
};