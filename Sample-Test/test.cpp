#include "pch.h"
#include "..\SoftRasterizer\Renderer.h"
TEST(LittleFunction, EdgeTest) {
	EXPECT_FALSE(InScreenZ(-1.0F));
	EXPECT_TRUE(InScreenZ(0.0F));
	EXPECT_TRUE(InScreenZ(1.0F));
	EXPECT_FALSE(InScreenZ(2.0f));

	EXPECT_FALSE(InScreenXY({-2.0f, -2.0f}));
	EXPECT_TRUE(InScreenXY({-1.0f, -1.0f}));
	EXPECT_TRUE(InScreenXY({0.0f, 0.0f}));
	EXPECT_TRUE(InScreenXY({1.0f, 1.0f}));
	EXPECT_FALSE(InScreenXY({2.0f, 2.0f}));

	EXPECT_FALSE(InPixelXY(-1, -1, 800, 600));
	EXPECT_TRUE(InPixelXY(0, 0, 800, 600));
	EXPECT_TRUE(InPixelXY(799, 599, 800, 600));
	EXPECT_FALSE(InPixelXY(800, 600, 800, 600));


	EXPECT_TRUE(PixelToScreen(-1, 800) < -1.0f);
	EXPECT_TRUE(-1.0f < PixelToScreen(0, 800));
	EXPECT_TRUE(PixelToScreen(799, 800) < 1.0f);
	EXPECT_TRUE(1.0f < PixelToScreen(800, 800));

	EXPECT_TRUE(ScreenToPixel(-1.0000001f, 800) < 0);
	EXPECT_TRUE(ScreenToPixel(-1.0f, 800) == 0);
	EXPECT_TRUE(ScreenToPixel(1.0f, 800) == 799);
	EXPECT_TRUE(ScreenToPixel(1.0000001f, 800) > 799);

	Color c{-1.0f, 0.0f, 2.0f};
	RGBColor rgb = ColorToRGBColor(c);
	EXPECT_EQ(rgb.r, 0);
	EXPECT_EQ(rgb.g, 0);
	EXPECT_EQ(rgb.b, 255);

	EXPECT_EQ(PixelToIndex(20, 20, 100), 2020);
	EXPECT_EQ(ReversePixelToIndex(20, 20, 100, 50), 2920);
}

TEST(BackCulling, Test1) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto points = Array<Point3, 3>{
		{0.0f, 1.0f, -2.0f}, {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 6.0f}
	}.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	//逆时针消除
	EXPECT_TRUE(BackCulling(points));
}

TEST(BackCulling, Test2) {
	//这里说明了不能直接对透视的顶点进行背面消除
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto points = Array<Point3, 3>{
		{-1.0, -2.0f, -1.0f}, {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 2.0f}
	}.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	EXPECT_FALSE(BackCulling(points));
}

TEST(BackCulling, Test3) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto points = Array<Point3, 3>{
		{-1.0, 1.0f, -1.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.5f}
	}.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	EXPECT_TRUE(BackCulling(points));
}

TEST(ScreenLine, EqualsTest) {
	Array<Point2, 2> a{{0.0f, 1.0f}, {1.0f, 0.0f}};
	Array<Point2, 2> b{{1.0f, 0.0f}, {0.0f, 1.0f}};
	Array<Point2, 2> c = a;
	Array<Point2, 2> d{{0.0f, 0.0f}, {0.0f, 0.0f}};
	EXPECT_TRUE(ScreenLineEqual(a, b));
	EXPECT_TRUE(ScreenLineEqual(a, c));

	EXPECT_FALSE(ScreenLineEqual(a, d));
	EXPECT_FALSE(ScreenLineEqual(b, d));
}

TEST(Line2DClip, Test) {
	/*
		A B C
		D E F
		G H I
		E是需要绘制的部分
	*/

	Array<Point2, 2> BH{{0.0f, 2.0f}, {0.0f, -2.0F}};
	EXPECT_TRUE(ScreenLineClip(BH));
	EXPECT_TRUE(InScreenXY(BH[0]));
	EXPECT_TRUE(InScreenXY(BH[1]));
	Array<Point2, 2> DF{{-2.0f, 0.0f}, {2.0f, 0.0F}};
	EXPECT_TRUE(ScreenLineClip(DF));
	EXPECT_TRUE(InScreenXY(DF[0]));
	EXPECT_TRUE(InScreenXY(DF[1]));
	Array<Point2, 2> CGcrossE{{2.0f, 2.0f}, {-2.0f, -2.0F}};
	EXPECT_TRUE(ScreenLineClip(CGcrossE));
	EXPECT_TRUE(InScreenXY(CGcrossE[0]));
	EXPECT_TRUE(InScreenXY(CGcrossE[1]));

	Array<Point2, 2> AG{{-2.0f, 1.0f}, {-2.0f, -1.0f}};
	EXPECT_FALSE(ScreenLineClip(AG));

	Array<Point2, 2> BDcrossA{{0.0f, 3.0f}, {-3.0f, 0.0f}};
	EXPECT_FALSE(ScreenLineClip(BDcrossA));

	Array<Point2, 2> BDcrossE{{0.0f, 1.5f}, {-1.5f, 0.0f}};
	EXPECT_TRUE(ScreenLineClip(BDcrossE));
	EXPECT_TRUE(InScreenXY(BDcrossE[0]));
	EXPECT_TRUE(InScreenXY(BDcrossE[1]));
}

TEST(ComputePlanePoint, NearPlane) {
	Vector4 near{0.0f, 0.0f, -1.0f, 0.0f};
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	Point3 A{-2.0f, 0.0f, 0.0f};
	Point3 B{2.0f, 0.0f, 2.0f};
	Point3 C{0.0f, 0.0f, 1.0f};
	Point4 A4 = per * A.ToPoint4();
	Point4 B4 = per * B.ToPoint4();
	Point4 C4 = per * C.ToPoint4();
	Point3 D1 = C4.ToPoint3();
	Point3 D2 = ComputePlanePoint(near, {A4, B4}).ToPoint3();
	EXPECT_EQ(D1.x, D2.x);
	EXPECT_EQ(D1.y, D2.y);
	EXPECT_EQ(D1.z, D2.z);
}

TEST(ComputePlanePoint, FarPlane) {
	Vector4 far{0.0f, 0.0f, 1.0f, -1.0f};
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	Point3 A{0.0f, 1.0f, -1.0f};
	Point3 B{0.0f, 1.0f, 5.0f};
	Point3 C{0.0f, 1.0f, 2.0f};
	Point4 A4 = per * A.ToPoint4();
	Point4 B4 = per * B.ToPoint4();
	Point4 C4 = per * C.ToPoint4();
	Point3 D1 = C4.ToPoint3();
	Point3 D2 = ComputePlanePoint(far, {A4, B4}).ToPoint3();
	EXPECT_EQ(D1.x, D2.x);
	EXPECT_EQ(D1.y, D2.y);
	EXPECT_EQ(D1.z, D2.z);
}


TEST(TriangleClip, NotClipTest) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 0.0f, 2.0f},
		Point3{-1.0f, 0.0f, 1.0f},
		Point3{1.0f, 0.0f, 1.0f}
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	Vector4 near{0.0f, 0.0f, -1.0f, 0.0f};
	auto clipTriangles = TriangleClip(near, point4s);
	EXPECT_EQ(clipTriangles.Size(), 1);
	auto it = clipTriangles.begin();
	EXPECT_EQ((*it)[0], point4s[0]);
	EXPECT_EQ((*it)[1], point4s[1]);
	EXPECT_EQ((*it)[2], point4s[2]);
}

TEST(TriangleClip, OnePointOut) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 0.0f, 4.0f},
		Point3{1.0f, 0.0f, 2.0f},
		Point3{0.0f, 0.0f, 0.0f}
	};
	auto nearLeft = per * Point3{0.0f, 0.0f, 1.0f}.ToPoint4();
	auto nearRight = per * Point3{0.5f, 0.0f, 1.0f}.ToPoint4();
	auto point4s = point3s.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	Vector4 near{0.0f, 0.0f, -1.0f, 0.0f};
	auto clipTriangles = TriangleClip(near, point4s);
	EXPECT_EQ(clipTriangles.Size(), 2);
	auto it = clipTriangles.begin();
	EXPECT_EQ((*it)[0], point4s[0]);
	EXPECT_EQ((*it)[1], nearLeft);
	EXPECT_EQ((*it)[2], nearRight);
	it += 1;
	EXPECT_EQ((*it)[0], point4s[0]);
	EXPECT_EQ((*it)[1], nearRight);
	EXPECT_EQ((*it)[2], point4s[1]);
}

TEST(TriangleClip, TwoPointOut) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	Array<Point3, 3> point3s{
		Point3{0.0f, 0.0f, 2.0f},
		Point3{1.0f, 0.0f, 0.0f},
		Point3{0.0f, 0.0f, -2.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	auto nearLeft = per * Point3{0.0f, 0.0f, 1.0f}.ToPoint4();
	auto nearRight = per * Point3{0.5f, 0.0f, 1.0f}.ToPoint4();
	Vector4 near{0.0f, 0.0f, -1.0f, 0.0f};
	auto clipTriangles = TriangleClip(near, point4s);
	auto it = clipTriangles.begin();
	EXPECT_EQ((*it)[0], point4s[0]);
	EXPECT_EQ((*it)[1], nearLeft);
	EXPECT_EQ((*it)[2], nearRight);
}

TEST(GetNotRepeatingLine2Ds, 4Triangle9Line) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, 3.0f},
		Point3{1.5f, 2.0f, 1.5f},
		Point3{0.0f, 3.0f, 0.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 4);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 9);
}

TEST(GetNotRepeatingLine2Ds, 3Triangle7Line) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, 3.0f},
		Point3{3.0f, 2.0f, 3.0f},
		Point3{0.0f, 3.0f, 0.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 3);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 7);
}

TEST(GetNotRepeatingLine2Ds, 2Triangle5Line) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, 2.0f},
		Point3{1.0f, 2.0f, 2.0f},
		Point3{0.0f, 3.0f, 0.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 2);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 5);
}

TEST(GetNotRepeatingLine2Ds, 1Triangle3Line) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, 2.0f},
		Point3{1.0f, 2.0f, 2.0f},
		Point3{0.0f, 3.0f, 1.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 1);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 3);
}

TEST(GetNotRepeatingLine2Ds, 0Triangle0Line) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, -1.0f},
		Point3{1.0f, 2.0f, 0.0f},
		Point3{0.0f, 3.0f, 0.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 0);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 0);
}

TEST(ComputeCenterCoefficient, Test) {
	auto coefficent = ComputeCenterCoefficient(
		Point2{0.0f, 0.0f},
		{{-1.0f, 1.0f}, {-1.0f, -1.0f}, {1.0f, -1.0f}}
	);
	EXPECT_EQ(coefficent[0], 0.5f);
	EXPECT_EQ(coefficent[1], 0.0f);
	EXPECT_EQ(coefficent[2], 0.5f);
}

TEST(ComputeCenterPoint, Test) {
	auto point = ComputeCenterPoint(
		{0.5f, 0.25f, 0.25f},
		{{1.0f, 2.0f, 3.0f, 4.0f}, {2.0f, 2.0f, 2.0f, 2.0f}, {4.0f, 3.0f, 2.0f, 1.0f}}
	);
	EXPECT_EQ(point.x, 2.0f);
	EXPECT_EQ(point.y, 2.25f);
	EXPECT_EQ(point.z, 2.5f);
	EXPECT_EQ(point.w, 2.75f);
}

TEST(ComputeCenterTextureCoordinate, Test) {
	auto per = Perspective(1.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	auto pointsW = Array<Point3, 3>{
		{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 2.0f}, {1.0f, 0.0f, 1.0f}
	}.Stream([&](Point3 p) {
		return (per * p.ToPoint4()).w;
	});
	Point2 t = ComputeCenterTextureCoordinate(
		{0.5f, 0.5f, 0.0f}, {{0.0f, 0.5f}, {0.0f, 1.0f}, {0.5f, 0.5f}}, pointsW
	);
	EXPECT_EQ(t.x, 0.0f);
	float pointZ = (per * Point3{0.0f, 0.0f, 1.5f}.ToPoint4()).ToPoint3().z;
	EXPECT_EQ(t.y, pointZ);
}

TEST(Matrix3x3, Transpose) {
	Matrix3X3 m1{
		1.0f, 2.0f, 3.0f,
		4.0f, 5.0f, 6.0f,
		7.0f, 8.0f, 9.0f
	};
	m1 = m1.Transpose();
	Matrix3X3 m2{
		1.0f, 4.0f, 7.0f,
		2.0f, 5.0f, 8.0f,
		3.0f, 6.0f, 9.0f
	};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			EXPECT_EQ(m1.f[i][j], m2.f[i][j]);
		}
	}
}

TEST(Matrix3x3, Determinant) {
	Matrix3X3 m1{
		1.0f, 1.0f, 3.0f,
		0.0f, 5.0f, 6.0f,
		7.0f, 8.0f, 0.0f
	};
	EXPECT_EQ(m1.Determinant(), -111.0f);
}

TEST(Matrix4X4, Transpose) {
	Matrix4X4 m1{
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f
	};
	m1 = m1.Transpose();
	Matrix4X4 m2{
		1.0f, 5.0f, 9.0f, 13.0f,
		2.0f, 6.0f, 10.0f, 14.0f,
		3.0f, 7.0f, 11.0f, 15.0f,
		4.0f, 8.0f, 12.0f, 16.0f
	};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			EXPECT_EQ(m1.f[i][j], m2.f[i][j]);
		}
	}
}

TEST(Matrix4x4, Determinant) {
	Matrix4X4 m1{
		1.0f, 1.0f, 3.0f, 2.0f,
		0.0f, 5.0f, 6.0f, 3.0f,
		7.0f, 8.0f, 0.0f, 4.0f,
		9.0f, 8.0f, 0.0f, 5.0f
	};
	EXPECT_EQ(m1.Determinant(), 9.0f);
}

TEST(Matrix4x4, Inverse1) {
	Matrix4X4 m1{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	auto m2 = m1.Inverse();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			EXPECT_EQ(m1.f[i][j], m2.f[i][j]);
		}
	}
}

TEST(Matrix4x4, Inverse2) {
	Matrix4X4 m1{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f, -2.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};
	EXPECT_EQ(m1.Determinant(), 2.0f);
	m1 = m1.Inverse();
	Matrix4X4 m2{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, -0.5f, 1.0f
	};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			EXPECT_EQ(m1.f[i][j], m2.f[i][j]);
		}
	}
}

TEST(Camera, Test) {
	auto camera = CameraLookTo({0.0f, 0.0f, 2.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	Point4 p{-1.0f, 3.0f, 2.0f, 1.0f};
	Point3 point = (camera * p).ToPoint3();
	EXPECT_EQ(point.x, 0.0f);
	EXPECT_EQ(point.y, 3.0f);
	EXPECT_EQ(point.z, 1.0f);
}