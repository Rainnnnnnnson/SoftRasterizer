#include "Math.h"
#include <cmath>
float ComputeLine(float k, float x, float b) {
	return k * x + b;
}

float ComputeLineEquation(Point2 p, Point2 p0, Point2 p1) {
	return (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y + p0.x * p1.y - p1.x * p0.y;
}

Vector3 operator*(const float& f, const Vector3& v) {
	return Vector3{f * v.x, f * v.y, f * v.z};
}

Matrix4X4 Perspective(float n, float f) {
	return Matrix4X4{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, f / (f - n), -(n * f) / (f - n),
		0.0f, 0.0f, 1.0f, 0.0f
	};
}

bool Point2::operator==(const Point2& p) const {
	return x == p.x && y == p.y;
}

Vector3 Point3::operator-(const Point3& p) const {
	return Vector3{x - p.x, y - p.y, z - p.z};
}

Point3 Point3::operator+(const Vector3& v) const {
	return Point3{x + v.x, y + v.y, z + v.z};
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

Point3 Point4::ToPoint3() const {
	return Point3{x / w, y / w, z / w};
}

float Vector3::Dot(const Vector3& v) const {
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::Cross(const Vector3& v) const {
	return Vector3{y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
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

Matrix4X4 Matrix4X4::Inverse() const {
	Matrix4X4 matrix;
	//��������
	for (int line = 0; line < 4; line++) {
		for (int column = 0; column < 4; column++) {
			//�Ӿ���
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
	//������ʽ
	float matrixDeterminant = Determinant();
	//��������������ʽ
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
		//��һ��
		f[0][0] * m.f[0][0] + f[0][1] * m.f[1][0] + f[0][2] * m.f[2][0] + f[0][3] * m.f[3][0],
		f[0][0] * m.f[0][1] + f[0][1] * m.f[1][1] + f[0][2] * m.f[2][1] + f[0][3] * m.f[3][1],
		f[0][0] * m.f[0][2] + f[0][1] * m.f[1][2] + f[0][2] * m.f[2][2] + f[0][3] * m.f[3][2],
		f[0][0] * m.f[0][3] + f[0][1] * m.f[1][3] + f[0][2] * m.f[2][3] + f[0][3] * m.f[3][3],
		//�ڶ���
		f[1][0] * m.f[0][0] + f[1][1] * m.f[1][0] + f[1][2] * m.f[2][0] + f[1][3] * m.f[3][0],
		f[1][0] * m.f[0][1] + f[1][1] * m.f[1][1] + f[1][2] * m.f[2][1] + f[1][3] * m.f[3][1],
		f[1][0] * m.f[0][2] + f[1][1] * m.f[1][2] + f[1][2] * m.f[2][2] + f[1][3] * m.f[3][2],
		f[1][0] * m.f[0][3] + f[1][1] * m.f[1][3] + f[1][2] * m.f[2][3] + f[1][3] * m.f[3][3],
		//������
		f[2][0] * m.f[0][0] + f[2][1] * m.f[1][0] + f[2][2] * m.f[2][0] + f[2][3] * m.f[3][0],
		f[2][0] * m.f[0][1] + f[2][1] * m.f[1][1] + f[2][2] * m.f[2][1] + f[2][3] * m.f[3][1],
		f[2][0] * m.f[0][2] + f[2][1] * m.f[1][2] + f[2][2] * m.f[2][2] + f[2][3] * m.f[3][2],
		f[2][0] * m.f[0][3] + f[2][1] * m.f[1][3] + f[2][2] * m.f[2][3] + f[2][3] * m.f[3][3],
		//������
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

