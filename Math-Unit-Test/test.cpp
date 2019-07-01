#include "pch.h"
#include"../SoftRasterizer/Math.h"

TEST(Point, PointPlusPoint) {
	Point2 point2 = Point2{1.0f, 2.0f} +Point2{3.0f, 4.0f};
	EXPECT_EQ(point2.x, 4.0f);
	EXPECT_EQ(point2.y, 6.0f);
	Point3 point3 = Point3{1.0F, 2.0F, 3.0F} +Point3{4.0F, 5.0F, 6.0F};
	EXPECT_EQ(point3.x, 5.0f);
	EXPECT_EQ(point3.y, 7.0f);
	EXPECT_EQ(point3.z, 9.0f);
	Point4 point4 = Point4{1.0F, 2.0F, 3.0F, 4.0F} +Point4{5.0F, 6.0F, 7.0F, 8.0F};
	EXPECT_EQ(point4.x, 6.0f);
	EXPECT_EQ(point4.y, 8.0f);
	EXPECT_EQ(point4.z, 10.0f);
	EXPECT_EQ(point4.w, 12.0f);
}

TEST(Point, PointMultiplyFloat) {
	Point2 point2 = Point2{1.0f, 2.0f} *2.0f;
	EXPECT_EQ(point2.x, 2.0f);
	EXPECT_EQ(point2.y, 4.0f);
	Point3 point3 = Point3{1.0F, 2.0F, 3.0F} *3.0f;
	EXPECT_EQ(point3.x, 3.0f);
	EXPECT_EQ(point3.y, 6.0f);
	EXPECT_EQ(point3.z, 9.0f);
	Point4 point4 = Point4{1.0F, 2.0F, 3.0F, 4.0F} *4.0f;
	EXPECT_EQ(point4.x, 4.0f);
	EXPECT_EQ(point4.y, 8.0f);
	EXPECT_EQ(point4.z, 12.0f);
	EXPECT_EQ(point4.w, 16.0f);
}

TEST(Point, PointPlusVector) {
	Point2 point2 = Point2{1.0f, 2.0f} +Vector2{3.0f, 4.0f};
	EXPECT_EQ(point2.x, 4.0f);
	EXPECT_EQ(point2.y, 6.0f);
	Point3 point3 = Point3{1.0F, 2.0F, 3.0F} +Vector3{4.0F, 5.0F, 6.0F};
	EXPECT_EQ(point3.x, 5.0f);
	EXPECT_EQ(point3.y, 7.0f);
	EXPECT_EQ(point3.z, 9.0f);
	Point4 point4 = Point4{1.0F, 2.0F, 3.0F, 4.0F} +Vector4{5.0F, 6.0F, 7.0F, 8.0F};
	EXPECT_EQ(point4.x, 6.0f);
	EXPECT_EQ(point4.y, 8.0f);
	EXPECT_EQ(point4.z, 10.0f);
	EXPECT_EQ(point4.w, 12.0f);
}

TEST(Point, PointMinusPoint) {
	Vector2 vector2 = Point2{1.0f, 2.0f} -Point2{3.0f, 4.0f};
	EXPECT_EQ(vector2.x, -2.0f);
	EXPECT_EQ(vector2.y, -2.0f);
	Vector3 vector3 = Point3{1.0F, 2.0F, 3.0F} -Point3{4.0F, 5.0F, 6.0F};
	EXPECT_EQ(vector3.x, -3.0f);
	EXPECT_EQ(vector3.y, -3.0f);
	EXPECT_EQ(vector3.z, -3.0f);
	Vector4 vector4 = Point4{1.0F, 2.0F, 3.0F, 4.0F} -Point4{5.0F, 6.0F, 7.0F, 8.0F};
	EXPECT_EQ(vector4.x, -4.0f);
	EXPECT_EQ(vector4.y, -4.0f);
	EXPECT_EQ(vector4.z, -4.0f);
	EXPECT_EQ(vector4.w, -4.0f);
}

TEST(Point, GetVector) {
	Vector2 vector2 = Point2{1.0f, 2.0f}.GetVector2();
	EXPECT_EQ(vector2.x, 1.0f);
	EXPECT_EQ(vector2.y, 2.0f);
	Vector3 vector3 = Point3{1.0F, 2.0F, 3.0F}.GetVector3();
	EXPECT_EQ(vector3.x, 1.0f);
	EXPECT_EQ(vector3.y, 2.0f);
	EXPECT_EQ(vector3.z, 3.0f);
	Vector4 vector4 = Point4{1.0F, 2.0F, 3.0F, 4.0F}.GetVector4();
	EXPECT_EQ(vector4.x, 1.0f);
	EXPECT_EQ(vector4.y, 2.0f);
	EXPECT_EQ(vector4.z, 3.0f);
	EXPECT_EQ(vector4.w, 4.0f);
}

TEST(Point, HomogeneousTransform) {
	Point4 point4 = Point3{1.0f, 2.0f, 3.0f}.ToPoint4();
	EXPECT_EQ(point4.x, 1.0f);
	EXPECT_EQ(point4.y, 2.0f);
	EXPECT_EQ(point4.z, 3.0f);
	EXPECT_EQ(point4.w, 1.0f);
	Point3 point3 = Point4{1.0, 2.0f, 3.0f, 2.0f}.ToPoint3();
	EXPECT_EQ(point3.x, 0.5f);
	EXPECT_EQ(point3.y, 1.0f);
	EXPECT_EQ(point3.z, 1.5f);
}

TEST(Vector, VectorMultiplyFloat) {
	Vector2 vector2 = Vector2{1.0f, 2.0f} *2.0f;
	EXPECT_EQ(vector2.x, 2.0f);
	EXPECT_EQ(vector2.y, 4.0f);
	Vector3 vector3 = Vector3{1.0F, 2.0F, 3.0F} *3.0f;
	EXPECT_EQ(vector3.x, 3.0f);
	EXPECT_EQ(vector3.y, 6.0f);
	EXPECT_EQ(vector3.z, 9.0f);
	Vector4 vector4 = Vector4{1.0F, 2.0F, 3.0F, 4.0F} *4.0f;
	EXPECT_EQ(vector4.x, 4.0f);
	EXPECT_EQ(vector4.y, 8.0f);
	EXPECT_EQ(vector4.z, 12.0f);
	EXPECT_EQ(vector4.w, 16.0f);
}

TEST(Vector, VectorPlusVector) {
	Vector2 vector2 = Vector2{1.0f, 2.0f} +Vector2{3.0f, 4.0f};
	EXPECT_EQ(vector2.x, 4.0f);
	EXPECT_EQ(vector2.y, 6.0f);
	Vector3 vector3 = Vector3{1.0F, 2.0F, 3.0F} +Vector3{4.0F, 5.0F, 6.0F};
	EXPECT_EQ(vector3.x, 5.0f);
	EXPECT_EQ(vector3.y, 7.0f);
	EXPECT_EQ(vector3.z, 9.0f);
	Vector4 vector4 = Vector4{1.0F, 2.0F, 3.0F, 4.0F} +Vector4{5.0F, 6.0F, 7.0F, 8.0F};
	EXPECT_EQ(vector4.x, 6.0f);
	EXPECT_EQ(vector4.y, 8.0f);
	EXPECT_EQ(vector4.z, 10.0f);
	EXPECT_EQ(vector4.w, 12.0f);
}

TEST(Vector, MinusVector) {
	Vector2 vector2 = -Vector2{1.0f, 2.0f};
	EXPECT_EQ(vector2.x, -1.0f);
	EXPECT_EQ(vector2.y, -2.0f);
	Vector3 vector3 = -Vector3{1.0F, 2.0F, 3.0F};
	EXPECT_EQ(vector3.x, -1.0f);
	EXPECT_EQ(vector3.y, -2.0f);
	EXPECT_EQ(vector3.z, -3.0f);
	Vector4 vector4 = -Vector4{1.0F, 2.0F, 3.0F, 4.0F};
	EXPECT_EQ(vector4.x, -1.0f);
	EXPECT_EQ(vector4.y, -2.0f);
	EXPECT_EQ(vector4.z, -3.0f);
	EXPECT_EQ(vector4.w, -4.0f);
}

TEST(Vector, GetPoint) {
	auto v2 = Vector2{1.0f, 2.0f}.GetPoint2();
	EXPECT_EQ(v2.x, 1.0f);
	EXPECT_EQ(v2.y, 2.0f);
	auto v3 = Vector3{1.0F, 2.0F, 3.0F}.GetPoint3();
	EXPECT_EQ(v3.x, 1.0f);
	EXPECT_EQ(v3.y, 2.0f);
	EXPECT_EQ(v3.z, 3.0f);
	auto v4 = Vector4{1.0F, 2.0F, 3.0F, 4.0F}.GetPoint4();
	EXPECT_EQ(v4.x, 1.0f);
	EXPECT_EQ(v4.y, 2.0f);
	EXPECT_EQ(v4.z, 3.0f);
	EXPECT_EQ(v4.w, 4.0f);
}

TEST(Vector, Dot) {
	auto v2 = Vector2{1.0f, 2.0f}.Dot({2.0f, 3.0f});
	EXPECT_EQ(v2, 8.0f);
	auto v3 = Vector3{1.0F, 2.0F, 3.0F}.Dot({2.0f, 3.0f, 4.0f});
	EXPECT_EQ(v3, 20.0f);
	auto v4 = Vector4{1.0F, 2.0F, 3.0F, 4.0F}.Dot({2.0f, 3.0f, 4.0f, 5.0f});
	EXPECT_EQ(v4, 40.0f);
}

TEST(Vector, Vector2Cross) {
	auto v21 = Vector2{1.0f, 1.0f}.Cross({2.0f, 3.0f});
	EXPECT_EQ(v21, 1.0f);
	auto v22 = Vector2{2.0f, 3.0f}.Cross({1.0f, 1.0f});
	EXPECT_EQ(v22, -1.0f);
}

TEST(Vector, Vector3Cross) {
	auto v31 = Vector3{1.0F, 2.0F, 3.0F}.Cross({2.0f, 3.0f, 4.0f});
	EXPECT_EQ(v31.x,-1.0f);
	EXPECT_EQ(v31.y, 2.0f);
	EXPECT_EQ(v31.z, -1.0f);
	auto v32 = Vector3{2.0F, 3.0F, 4.0F}.Cross({1.0f, 2.0f, 3.0f});
	EXPECT_EQ(v32.x, 1.0f);
	EXPECT_EQ(v32.y, -2.0f);
	EXPECT_EQ(v32.z, 1.0f);
}

TEST(Vector, Length) {
	auto v2 = Vector2{3.0f, 4.0f}.Length();
	EXPECT_EQ(v2, 5.0f);
	auto v3 = Vector3{3.0F, 4.0F, 12.0F}.Length();
	EXPECT_EQ(v3, 13.0f);
}

TEST(Vector, Normalize) {
	auto v2 = Vector2{3.0f, 4.0f}.Normalize();
	EXPECT_EQ(v2.x, 0.6f);
	EXPECT_EQ(v2.y, 0.8f);
	auto v3 = Vector3{3.0F, 4.0F, 12.0F}.Normalize();
	EXPECT_EQ(v3.x, 3.0f / 13.0f);
	EXPECT_EQ(v3.y, 4.0f / 13.0f);
	EXPECT_EQ(v3.z, 12.0f / 13.0f);
}

TEST(Matrix, MatrixMultiplyPoint) {
	auto m3 = Matrix3X3{
		1.0f, 2.0f, 3.0f,
		4.0f, 5.0f, 6.0f,
		7.0f, 8.0f, 9.0f
	} *Point3{3.0f, 2.0f, 1.0f};
	EXPECT_EQ(m3.x, 10.0f);
	EXPECT_EQ(m3.y, 28.0f);
	EXPECT_EQ(m3.z, 46.0f);
	auto m4 = Matrix4X4{
		1.0f, 2.0f, 3.0f, 10.0f,
		4.0f, 5.0f, 6.0f, 11.0f,
		7.0f, 8.0f, 9.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f
	} *Point4{4.0f, 3.0f, 2.0f, 1.0f};
	EXPECT_EQ(m4.x, 26.0f);
	EXPECT_EQ(m4.y, 54.0f);
	EXPECT_EQ(m4.z, 82.0f);
	EXPECT_EQ(m4.w, 140.0f);
}
TEST(Matrix, Matrix3X3MultiplyMatrix3X3) {
	auto m3 = Matrix3X3{
		1.0f, 2.0f, 3.0f,
		4.0f, 5.0f, 6.0f,
		7.0f, 8.0f, 9.0f
	} *Matrix3X3{
		11.0f, 12.0f, 13.0f,
		14.0f, 15.0f, 16.0f,
		17.0f, 18.0f, 19.0f
	};
	Matrix3X3 m3e{
		90.0f, 96.0f, 102.0f,
		216.0f, 231.0f, 246.0f,
		342.0f, 366.0f, 390.0f
	};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			EXPECT_EQ(m3.f[i][j], m3e.f[i][j]);
		}
	}
}

TEST(Matrix, Matrix4X4MultiplyMatrix4X4) {
	auto m4 = Matrix4X4{
		1.0f, 2.0f, 3.0f, 10.0f,
		4.0f, 5.0f, 6.0f, 11.0f,
		7.0f, 8.0f, 9.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f
	} *Matrix4X4{
		11.0f, 12.0f, 13.0f, 10.0f,
		14.0f, 15.0f, 16.0f, 11.0f,
		17.0f, 18.0f, 19.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f
	};
	Matrix4X4 m4e{
		220.0f, 236.0f, 252.0f, 228.0f,
		359.0f, 385.0f, 411.0f, 343.0f,
		498.0f, 534.0f, 570.0f, 458.0f,
		802.0f, 860.0f, 918.0f, 720.0f

	};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			EXPECT_EQ(m4.f[i][j], m4e.f[i][j]);
		}
	}
}

TEST(Matrix, Determinant) {
	auto m3 = Matrix3X3{
		1.0f, 2.0f, 3.0f,
		5.0f, 6.0f, 7.0f,
		9.0f, 8.0f, 8.0f
	}.Determinant();
	EXPECT_EQ(m3, -4.0f);
	auto m4 = Matrix4X4{
		1.0f, 2.0f, 3.0f, 10.0f,
		5.0f, 6.0f, 7.0f, 11.0f,
		9.0f, 8.0f, 8.0f, 12.0f,
		13.0f, 12.0f, 11.0f, 10.0f
	}.Determinant();
	EXPECT_EQ(m4, -66.0f);
}

TEST(Matrix, Matrix3Inverse) {
	auto m3 = Matrix3X3{
		1.0f, 2.0f, 3.0f,
		5.0f, 6.0f, 7.0f,
		9.0f, 8.0f, 8.0f
	}.Inverse();
	Matrix3X3 m3e{
		2.0f, -2.0f, 1.0f,
		-5.75f, 4.75f, -2.0f,
		3.5f, -2.5f, 1.0f
	};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			EXPECT_EQ(m3.f[i][j], m3e.f[i][j]);
		}
	}
}

TEST(Matrix, Matrix4Inverse) {
	auto m4 = Matrix4X4{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,2.0f,-2.0f,
		0.0f,0.0f,1.0f,0.0f
	}.Inverse();
	Matrix4X4 m4e{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, -0.5f, 1.0f
	};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			EXPECT_EQ(m4.f[i][j], m4e.f[i][j]);
		}
	}
}

TEST(Matrix, Matrix4HomogeneousTransform) {
	auto m4 = Matrix3X3{
		1.0f, 2.0f, 3.0f,
		5.0f, 6.0f, 7.0f,
		9.0f, 8.0f, 8.0f
	}.ToMatrix4X4();
	Matrix4X4 m4e{
		1.0f, 2.0f, 3.0f, 0.0f,
		5.0f, 6.0f, 7.0f, 0.0f,
		9.0f, 8.0f, 8.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			EXPECT_EQ(m4.f[i][j], m4e.f[i][j]);
		}
	}
}

TEST(Matrix, Matrix3HomogeneousTransform) {
	auto m3 = Matrix4X4{
		1.0f, 2.0f, 3.0f, 2.0f,
		5.0f, 6.0f, 7.0f, 3.0f,
		9.0f, 8.0f, 8.0f, 4.0f,
		2.0f, 3.0f, 4.0f, 1.0f
	}.ToMatrix3X3();
	auto m3e = Matrix3X3{
		1.0f, 2.0f, 3.0f,
		5.0f, 6.0f, 7.0f,
		9.0f, 8.0f, 8.0f
	};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			EXPECT_EQ(m3.f[i][j], m3e.f[i][j]);
		}
	}
}

TEST(Color, ColorMultiplyFloat) {
	auto c = Color{1.0f, 2.0f, 3.0f} *2.0f;
	EXPECT_EQ(c.r, 2.0f);
	EXPECT_EQ(c.g, 4.0f);
	EXPECT_EQ(c.b, 6.0f);
}

TEST(Color, ColorPlusColor) {
	auto c = Color{1.0f, 2.0f, 3.0f}+Color{3.0f, 2.0f, 1.0f};
	EXPECT_EQ(c.r, 4.0f);
	EXPECT_EQ(c.g, 4.0f);
	EXPECT_EQ(c.b, 4.0f);
}

TEST(Color, ColorMultiplyColor) {
	auto c = Color{1.0f, 2.0f, 3.0f}*Color{3.0f, 2.0f, 1.0f};
	EXPECT_EQ(c.r, 3.0f);
	EXPECT_EQ(c.g, 4.0f);
	EXPECT_EQ(c.b, 3.0f);
}

TEST(CameraLookTo, Test) {
	Point4 point4 = CameraLookTo({0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}) * Point4 {
		1.0f, 2.0f, 2.0f, 1.0f
	};
	EXPECT_EQ(point4.x, -1.0f);
	EXPECT_EQ(point4.y, 2.0f);
	EXPECT_EQ(point4.z, 1.0f);
	EXPECT_EQ(point4.w, 1.0f);
}

TEST(CameraLookAt, Test) {
	Point4 point4 = CameraLookAt({0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}) * Point4 {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	EXPECT_EQ(point4.x, 0.0f);
	EXPECT_EQ(point4.y, 1.0f);
	EXPECT_EQ(point4.z, 1.0f);
	EXPECT_EQ(point4.w, 1.0f);
}

TEST(Scale, Test) {
	Point4 point4 = Scale(2.0f, 3.0f, 4.0f) * Point4 {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	EXPECT_EQ(point4.x, 2.0f);
	EXPECT_EQ(point4.y, 3.0f);
	EXPECT_EQ(point4.z, 4.0f);
	EXPECT_EQ(point4.w, 1.0f);
}

TEST(Move, Test) {
	Point4 point4 = Move({2.0f, 3.0f, 4.0f}) * Point4 {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	EXPECT_EQ(point4.x, 3.0f);
	EXPECT_EQ(point4.y, 4.0f);
	EXPECT_EQ(point4.z, 5.0f);
	EXPECT_EQ(point4.w, 1.0f);
}

TEST(Orthogonal, Test) {
	auto matrix = Orthogonal(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto p0 = (matrix * Point4{-1.0f, -1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p0.x, -1.0f);
	EXPECT_EQ(p0.y, -1.0f);
	EXPECT_EQ(p0.z, 0.0f);
	auto p1 = (matrix * Point4{-1.0f, 1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p1.x, -1.0f);
	EXPECT_EQ(p1.y, 1.0f);
	EXPECT_EQ(p1.z, 0.0f);
	auto p2 = (matrix * Point4{1.0f, 1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p2.x, 1.0f);
	EXPECT_EQ(p2.y, 1.0f);
	EXPECT_EQ(p2.z, 0.0f);
	auto p3 = (matrix * Point4{1.0f, -1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p3.x, 1.0f);
	EXPECT_EQ(p3.y, -1.0f);
	EXPECT_EQ(p3.z, 0.0f);
	auto p4 = (matrix * Point4{-1.0f, -1.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p4.x, -1.0f);
	EXPECT_EQ(p4.y, -1.0f);
	EXPECT_EQ(p4.z, 1.0f);
	auto p5 = (matrix * Point4{-1.0f, 1.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p5.x, -1.0f);
	EXPECT_EQ(p5.y, 1.0f);
	EXPECT_EQ(p5.z, 1.0f);
	auto p6 = (matrix * Point4{1.0f, 1.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p6.x, 1.0f);
	EXPECT_EQ(p6.y, 1.0f);
	EXPECT_EQ(p6.z, 1.0f);
	auto p7 = (matrix * Point4{1.0f, -1.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p7.x, 1.0f);
	EXPECT_EQ(p7.y, -1.0f);
	EXPECT_EQ(p7.z, 1.0f);
}

TEST(OrthogonalByAspect, Test) {
	auto matrix = OrthogonalByAspect(1.0f, 2.0f, 2.0f);
	auto p0 = (matrix * Point4{-2.0f, -1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p0.x, -1.0f);
	EXPECT_EQ(p0.y, -1.0f);
	EXPECT_EQ(p0.z, 0.0f);
	auto p1 = (matrix * Point4{-2.0f, 1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p1.x, -1.0f);
	EXPECT_EQ(p1.y, 1.0f);
	EXPECT_EQ(p1.z, 0.0f);
	auto p2 = (matrix * Point4{2.0f, 1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p2.x, 1.0f);
	EXPECT_EQ(p2.y, 1.0f);
	EXPECT_EQ(p2.z, 0.0f);
	auto p3 = (matrix * Point4{2.0f, -1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p3.x, 1.0f);
	EXPECT_EQ(p3.y, -1.0f);
	EXPECT_EQ(p3.z, 0.0f);
	auto p4 = (matrix * Point4{-2.0f, -1.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p4.x, -1.0f);
	EXPECT_EQ(p4.y, -1.0f);
	EXPECT_EQ(p4.z, 1.0f);
	auto p5 = (matrix * Point4{-2.0f, 1.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p5.x, -1.0f);
	EXPECT_EQ(p5.y, 1.0f);
	EXPECT_EQ(p5.z, 1.0f);
	auto p6 = (matrix * Point4{2.0f, 1.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p6.x, 1.0f);
	EXPECT_EQ(p6.y, 1.0f);
	EXPECT_EQ(p6.z, 1.0f);
	auto p7 = (matrix * Point4{2.0f, -1.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p7.x, 1.0f);
	EXPECT_EQ(p7.y, -1.0f);
	EXPECT_EQ(p7.z, 1.0f);
}

TEST(Perspective, Test) {
	auto matrix = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto p0 = (matrix * Point4{-1.0f, -1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p0.x, -1.0f);
	EXPECT_EQ(p0.y, -1.0f);
	EXPECT_EQ(p0.z, 0.0f);
	auto p1 = (matrix * Point4{-1.0f, 1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p1.x, -1.0f);
	EXPECT_EQ(p1.y, 1.0f);
	EXPECT_EQ(p1.z, 0.0f);
	auto p2 = (matrix * Point4{1.0f, 1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p2.x, 1.0f);
	EXPECT_EQ(p2.y, 1.0f);
	EXPECT_EQ(p2.z, 0.0f);
	auto p3 = (matrix * Point4{1.0f, -1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p3.x, 1.0f);
	EXPECT_EQ(p3.y, -1.0f);
	EXPECT_EQ(p3.z, 0.0f);
	auto p4 = (matrix * Point4{-2.0f, -2.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p4.x, -1.0f);
	EXPECT_EQ(p4.y, -1.0f);
	EXPECT_EQ(p4.z, 1.0f);
	auto p5 = (matrix * Point4{-2.0f, 2.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p5.x, -1.0f);
	EXPECT_EQ(p5.y, 1.0f);
	EXPECT_EQ(p5.z, 1.0f);
	auto p6 = (matrix * Point4{2.0f, 2.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p6.x, 1.0f);
	EXPECT_EQ(p6.y, 1.0f);
	EXPECT_EQ(p6.z, 1.0f);
	auto p7 = (matrix * Point4{2.0f, -2.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p7.x, 1.0f);
	EXPECT_EQ(p7.y, -1.0f);
	EXPECT_EQ(p7.z, 1.0f);
}

TEST(PerspectiveByAspect, Test) {
	auto matrix = PerspectiveByAspect(1.0f, 2.0f, 2.0f);
	auto p0 = (matrix * Point4{-2.0f, -1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p0.x, -1.0f);
	EXPECT_EQ(p0.y, -1.0f);
	EXPECT_EQ(p0.z, 0.0f);
	auto p1 = (matrix * Point4{-2.0f, 1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p1.x, -1.0f);
	EXPECT_EQ(p1.y, 1.0f);
	EXPECT_EQ(p1.z, 0.0f);
	auto p2 = (matrix * Point4{2.0f, 1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p2.x, 1.0f);
	EXPECT_EQ(p2.y, 1.0f);
	EXPECT_EQ(p2.z, 0.0f);
	auto p3 = (matrix * Point4{2.0f, -1.0f, 1.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p3.x, 1.0f);
	EXPECT_EQ(p3.y, -1.0f);
	EXPECT_EQ(p3.z, 0.0f);
	auto p4 = (matrix * Point4{-4.0f, -2.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p4.x, -1.0f);
	EXPECT_EQ(p4.y, -1.0f);
	EXPECT_EQ(p4.z, 1.0f);
	auto p5 = (matrix * Point4{-4.0f, 2.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p5.x, -1.0f);
	EXPECT_EQ(p5.y, 1.0f);
	EXPECT_EQ(p5.z, 1.0f);
	auto p6 = (matrix * Point4{4.0f, 2.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p6.x, 1.0f);
	EXPECT_EQ(p6.y, 1.0f);
	EXPECT_EQ(p6.z, 1.0f);
	auto p7 = (matrix * Point4{4.0f, -2.0f, 2.0f, 1.0f}).ToPoint3();
	EXPECT_EQ(p7.x, 1.0f);
	EXPECT_EQ(p7.y, -1.0f);
	EXPECT_EQ(p7.z, 1.0f);
}

TEST(RotateX, Test) {
	Point4 point4 = RotateX(PI / 2.0f) * Point4 {
		3.0f, 4.0f, 5.0f, 1.0f
	};
	EXPECT_EQ(point4.x, 3.0f);
	EXPECT_LE(abs(point4.y - (-5.0f)), 0.0001f);
	EXPECT_LE(abs(point4.z - (4.0f)), 0.0001f);
	EXPECT_EQ(point4.w, 1.0f);
}

TEST(RotateY, Test) {
	Point4 point4 = RotateY(PI / 2.0f) * Point4 {
		3.0f, 4.0f, 5.0f, 1.0f
	};
	EXPECT_LE(abs(point4.x - (5.0f)), 0.0001f);
	EXPECT_EQ(point4.y, 4.0f);
	EXPECT_LE(abs(point4.z - (-3.0f)), 0.0001f);
	EXPECT_EQ(point4.w, 1.0f);
}

TEST(RotateZ, Test) {
	Point4 point4 = RotateZ(PI / 2.0f) * Point4 {
		3.0f, 4.0f, 5.0f, 1.0f
	};
	EXPECT_LE(abs(point4.x - (-4.0f)), 0.0001f);
	EXPECT_LE(abs(point4.y - (3.0f)), 0.0001f);
	EXPECT_EQ(point4.z, 5.0f);
	EXPECT_EQ(point4.w, 1.0f);
}