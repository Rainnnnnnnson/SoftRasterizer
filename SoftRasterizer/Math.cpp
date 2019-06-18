#include "Math.h"
#include <cmath>

float ComputeLineEquation(Point2 p, Point2 p0, Point2 p1) {
	return (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y + p0.x * p1.y - p1.x * p0.y;
}

Vector3 operator*(const float& f, const Vector3& v) {
	return Vector3{f * v.x, f * v.y, f * v.z};
}

Matrix4X4 CameraLookTo(Point3 eye, Vector3 direction, Vector3 up) {
	Vector3 right = up.Cross(direction).Normalize();
	return Matrix4X4{
		right.x, up.x, direction.x, eye.x,
		right.y, up.y, direction.y, eye.y,
		right.z, up.z, direction.z, eye.z,
		0.0f, 0.0f, 0.0f, 1.0f
	}.Inverse();
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

Matrix4X4 PerspectiveByAspect(float n, float f, float aspect) {
	return Perspective(n, f, -aspect, aspect, -1.0f, 1.0f);
}

Matrix4X4 Perspective(float n, float f, float l, float r, float b, float t) {
	return Matrix4X4{
		(2.0f * n) / (r - l), 0.0f, -(r + l) / (r - l), 0.0f,
		0.0f, (2.0f * n) / (t - b), -(t + b) / (t - b), 0.0f,
		0.0f, 0.0f, f / (f - n), -(n * f) / (f - n),
		0.0f, 0.0f, 1.0f, 0.0f
	};
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

bool Point2::operator==(const Point2& p) const {
	return x == p.x && y == p.y;
}

Point2 Point2::operator*(float f) const {
	return {x * f, y * f};
}

Point2 Point2::operator+(const Point2& p) const {
	return {x + p.x, y + p.y};
}

Vector3 Point3::operator-(const Point3& p) const {
	return Vector3{x - p.x, y - p.y, z - p.z};
}

Point3 Point3::operator+(const Vector3& v) const {
	return Point3{x + v.x, y + v.y, z + v.z};
}

Vector3 Point3::GetVector3FormOrigin() const {
	return {x, y, z};
}

Point2 Point3::GetPoint2() const {
	return Point2{x, y};
}

Point4 Point3::ToPoint4() const {
	return Point4{x, y, z, 1};
}

bool Point4::operator==(const Point4& p) const {
	return (x == p.x) && (y == p.y) && (z == p.z) && (w == p.w);
}

Point4 Point4::operator+(const Point4& p) const {
	return Point4{x + p.x, y + p.y, z + p.z, w + p.w};
}

Point4 Point4::operator*(float f) const {
	return Point4{x * f, y * f, z * f, w * f};
}

Vector4 Point4::GetVector4FormOrigin() const {
	return {x, y, z, w};
}

Vector4 Point4::operator-(const Point4& p) const {
	return Vector4{x - p.x, y - p.y, z - p.z, w - p.w};
}

Point4 Point4::operator+(const Vector4& v) const {
	return Point4{x + v.x, y + v.y, z + v.z, w + v.w};
}

Point2 Point4::GetPoint2() const {
	return Point2{x, y};
}

Point3 Point4::ToPoint3() const {
	return Point3{x / w, y / w, z / w};
}

Vector3 Vector3::operator+(const Vector3& v) const {
	return {x + v.x, y + v.y, z + v.z};
}

Point3 Vector3::operator-(const Vector3& v) const {
	return {x - v.x, y - v.y, z - v.z};
}

float Vector3::Dot(const Vector3& v) const {
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::Cross(const Vector3& v) const {
	return Vector3{y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
}

float Vector3::Length() const {
	return sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::Normalize() const {
	return (1.0f / Length()) * (*this);
}

Matrix3X3 Matrix3X3::Inverse() const {
	return Matrix3X3();
}

Matrix3X3 Matrix3X3::Transpose() const {
	Matrix3X3 m;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			m.f[i][j] = f[j][i];
		}
	}
	return m;
}

float Matrix3X3::Determinant() const {
	return f[0][0] * f[1][1] * f[2][2] - f[0][0] * f[1][2] * f[2][1]
		+ f[0][1] * f[1][2] * f[2][0] - f[0][1] * f[1][0] * f[2][2]
		+ f[0][2] * f[1][0] * f[2][1] - f[0][2] * f[1][1] * f[2][0];
}

Vector3 Matrix3X3::operator*(const Vector3& v) const {
	return Vector3{
		f[0][0] * v.x + f[0][1] * v.y + f[0][2] * v.z,
		f[1][0] * v.x + f[1][1] * v.y + f[1][2] * v.z,
		f[2][0] * v.x + f[2][1] * v.y + f[2][2] * v.z,
	};
}

Matrix3X3 Matrix4X4::GetMatrix3X3() const {
	return Matrix3X3{
		f[0][0], f[0][1], f[0][2],
		f[1][0], f[1][1], f[1][2],
		f[2][0], f[2][1], f[2][2]
	};
}

Matrix4X4 Matrix4X4::Inverse() const {
	Matrix4X4 matrix;
	//求伴随矩阵
	for (int line = 0; line < 4; line++) {
		for (int column = 0; column < 4; column++) {
			//子矩阵
			Matrix3X3 matrix3X3;
			float sign = ((line + column) % 2 == 0) ? 1.0f : -1.0f;
			int sonLine = 0;
			for (int parentsLine = 0; parentsLine < 4; parentsLine++) {
				int sonColumn = 0;
				if (parentsLine == line) {
					continue;
				}
				for (int parentsColumn = 0; parentsColumn < 4; parentsColumn++) {
					if (parentsColumn == column) {
						continue;
					}
					matrix3X3.f[sonLine][sonColumn] = f[parentsLine][parentsColumn];
					sonColumn++;
				}
				sonLine++;
			}
			float determinant = matrix3X3.Determinant();
			matrix.f[column][line] = sign * determinant;
		}
	}
	//求行列式
	float matrixDeterminant = Determinant();
	//伴随矩阵除以行列式
	Matrix4X4 returnMatrix;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			returnMatrix.f[j][i] = matrix.f[j][i] / matrixDeterminant;
		}
	}
	return returnMatrix;
}

Matrix4X4 Matrix4X4::Transpose() const {
	Matrix4X4 matrix;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.f[j][i] = f[i][j];
		}
	}
	return matrix;
}

float Matrix4X4::Determinant() const {
	float a00 = f[0][0] * Matrix3X3{
		f[1][1], f[1][2], f[1][3],
		f[2][1], f[2][2], f[2][3],
		f[3][1], f[3][2], f[3][3]
	}.Determinant();
	float a01 = -f[0][1] * Matrix3X3{
		f[1][0], f[1][2], f[1][3],
		f[2][0], f[2][2], f[2][3],
		f[3][0], f[3][2], f[3][3]
	}.Determinant();
	float a02 = f[0][2] * Matrix3X3{
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

Matrix4X4 Matrix4X4::operator*(const Matrix4X4& m) const {
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

Point4 Matrix4X4::operator*(const Point4& p) const {
	float x = f[0][0] * p.x + f[0][1] * p.y + f[0][2] * p.z + f[0][3] * p.w;
	float y = f[1][0] * p.x + f[1][1] * p.y + f[1][2] * p.z + f[1][3] * p.w;
	float z = f[2][0] * p.x + f[2][1] * p.y + f[2][2] * p.z + f[2][3] * p.w;
	float w = f[3][0] * p.x + f[3][1] * p.y + f[3][2] * p.z + f[3][3] * p.w;
	return Point4{x, y, z, w};
}

Color Color::operator+(const Color& c) const {
	return Color{r + c.r, g + c.g, b + c.b};
}

Color Color::operator*(float f) const {
	return Color{r * f, g * f, b * f};
}

float Vector4::Dot(const Vector4& v) const {
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

Vector4 Vector4::operator*(float f) const {
	return {x * f, y * f, z * f, w * f};
}
