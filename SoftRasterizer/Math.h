#pragma once
/*
	这里的符号有物理意义
	总体规则如下

	Point = Point * f + Point * f
	Point = Point + Vector
	Vector = Point - Point

	Vector = Vector * f
	Vector = Vector + Vector
	Vector = -Vector

	Point = Matrix * Point
	Matrix = Matrix * Matrix

	Color = Color * f + Color * f
	Color = Color * Color

	有歧义使用函数运算

	ToXXX表示  齐次与非齐次转化
	GetXX表示  内存一样转化类型
*/

struct Point2;
struct Point3;
struct Point4;
struct Vector2;
struct Vector3;
struct Vector4;
struct Matrix3X3;
struct Matrix4X4;
struct Color;

constexpr float PI = 3.14159265359f;

struct Point2 {
	float x, y;
	Point2 operator*(float f) const;
	Point2 operator+(Point2 p) const;
	Point2 operator+(Vector2 v) const;
	Vector2 operator-(Point2 p) const;

	Vector2 GetVector2() const;
};

struct Point3 {
	float x, y, z;
	Point3 operator*(float f) const;
	Point3 operator+(Point3 p) const;
	Point3 operator+(Vector3 v) const;
	Vector3 operator-(Point3 p) const;

	Vector3 GetVector3() const;
	Point4 ToPoint4() const;
};

struct Point4 {
	float x, y, z, w;
	Point4 operator*(float f) const;
	Point4 operator+(Point4 p) const;
	Point4 operator+(Vector4 v) const;
	Vector4 operator-(Point4 p) const;

	Vector4 GetVector4() const;
	Point3 ToPoint3() const;
};

struct Vector2 {
	float x, y;
	Vector2 operator*(float f) const;
	Vector2 operator+(Vector2 v) const;
	Vector2 operator-() const;

	float Dot(Vector2 v) const;
	float Cross(Vector2 v) const;
	float Length() const;
	Vector2 Normalize() const;

	Point2 GetPoint2() const;
};

struct Vector3 {
	float x, y, z;
	Vector3 operator*(float f) const;
	Vector3 operator+(Vector3 v) const;
	Vector3 operator-() const;

	float Dot(Vector3 v) const;
	Vector3 Cross(Vector3 v) const;
	float Length() const;
	Vector3 Normalize() const;

	Point3 GetPoint3() const;
};

struct Vector4 {
	float x, y, z, w;
	Vector4 operator*(float f) const;
	Vector4 operator+(Vector4 v) const;
	Vector4 operator-() const;

	float Dot(Vector4 v) const;

	Point4 GetPoint4() const;
};

struct Matrix3X3 {
	float f[3][3];
	Point3 operator*(Point3 p) const;
	Matrix3X3 operator*(Matrix3X3 m) const;

	Matrix3X3 Inverse() const;
	Matrix3X3 Transpose() const;
	float Determinant() const;
	Matrix4X4 ToMatrix4X4() const;

	/*
		这里做与Matrix * Point一样的运算
		这里只是提供接口
		需要自己做矩阵调整
	*/
	Vector3 TransformVector(Vector3 v) const;
};

struct Matrix4X4 {
	float f[4][4];
	Point4 operator*(Point4 p) const;
	Matrix4X4 operator*(Matrix4X4 m) const;

	Matrix4X4 Inverse() const;
	Matrix4X4 Transpose() const;
	float Determinant() const;
	Matrix3X3 ToMatrix3X3() const;
};


struct Color {
	float r, g, b;
	Color operator*(float f) const;
	Color operator+(Color c) const;
	Color operator*(Color c) const;
};

/*
	需要传入单位向量
*/
Matrix4X4 CameraLookTo(Point3 eye, Vector3 direction, Vector3 up);
Matrix4X4 CameraLookAt(Point3 eye, Point3 target, Vector3 up);
Matrix4X4 Scale(float x, float y, float z);
Matrix4X4 Move(Vector3 direction);
Matrix4X4 Perspective(float n, float f, float l, float r, float b, float t);
Matrix4X4 PerspectiveByAspect(float n, float f, float aspect);

/*
	朝着轴的方向看去 顺时针转动rad弧度
*/
Matrix4X4 RotateX(float radian);
Matrix4X4 RotateY(float radian);
Matrix4X4 RotateZ(float radian);