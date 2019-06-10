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

TEST(BackCulling, Test) {
	//锐角
	Point3 up = {0.0f, 1.0f, 0.0f};
	Point3 leftDown = {-1.0f, -1.0f, 0.0f};
	Point3 rightDown = {1.0f, -1.0f, 0.0f};
	//逆时针消除
	EXPECT_TRUE(BackCulling(Array<Point3, 3>{
		up, leftDown, rightDown
	}));
	//顺时针不消除
	EXPECT_FALSE(BackCulling(Array<Point3, 3>{
		rightDown, leftDown, up
	}));
	//钝角
	Point3 leftUp = {-1.0, 1.0f, 0.0f};
	Point3 middle = {0.0f, 0.0f, 0.0f};
	Point3 right = {1.0f, 0.0f, 0.0f};
	//逆时针消除
	EXPECT_TRUE(BackCulling(Array<Point3, 3>{
		leftUp, middle, right
	}));
	//顺时针不消除
	EXPECT_FALSE(BackCulling(Array<Point3, 3>{
		right, middle, leftUp
	}));
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
	Matrix4X4 Per = Perspective(1.0f, 2.0f);
	Point3 A{-2.0f, 0.0f, 0.0f};
	Point3 B{2.0f, 0.0f, 2.0f};
	Point3 C{0.0f, 0.0f, 1.0f};
	Point4 A4 = Per * A.ToPoint4();
	Point4 B4 = Per * B.ToPoint4();
	Point4 C4 = Per * C.ToPoint4();
	Point3 D1 = C4.ToPoint3();
	Point3 D2 = ComputePlanePoint(-1.0f, 0.0f, A4, B4).ToPoint3();
	EXPECT_EQ(D1.x, D2.x);
	EXPECT_EQ(D1.y, D2.y);
	EXPECT_EQ(D1.z, D2.z);
}

TEST(ComputePlanePoint, FarPlane) {
	Matrix4X4 Per = Perspective(1.0f, 2.0f);
	Point3 A{0.0f, 1.0f, -1.0f};
	Point3 B{0.0f, 1.0f, 5.0f};
	Point3 C{0.0f, 1.0f, 2.0f};
	Point4 A4 = Per * A.ToPoint4();
	Point4 B4 = Per * B.ToPoint4();
	Point4 C4 = Per * C.ToPoint4();
	Point3 D1 = C4.ToPoint3();
	Point3 D2 = ComputePlanePoint(1.0f, -1.0f, A4, B4).ToPoint3();
	EXPECT_EQ(D1.x, D2.x);
	EXPECT_EQ(D1.y, D2.y);
	EXPECT_EQ(D1.z, D2.z);
}


TEST(TriangleClip, NotClipTest) {
	Matrix4X4 Per = Perspective(1.0f, 2.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 0.0f, 2.0f},
		Point3{-1.0f, 0.0f, 1.0f},
		Point3{1.0f, 0.0f, 1.0f}
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto clipTriangles = TriangleClip(-1.0f, 0.0f, point4s);
	EXPECT_EQ(clipTriangles.Size(), 1);
	auto it = clipTriangles.begin();
	EXPECT_EQ((*it)[0], point4s[0]);
	EXPECT_EQ((*it)[1], point4s[1]);
	EXPECT_EQ((*it)[2], point4s[2]);
}

TEST(TriangleClip, OnePointOut) {
	Matrix4X4 Per = Perspective(1.0f, 2.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 0.0f, 4.0f},
		Point3{1.0f, 0.0f, 2.0f},
		Point3{0.0f, 0.0f, 0.0f}
	};
	auto nearLeft = Per * Point3{0.0f, 0.0f, 1.0f}.ToPoint4();
	auto nearRight = Per * Point3{0.5f, 0.0f, 1.0f}.ToPoint4();
	auto point4s = point3s.Stream([&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto clipTriangles = TriangleClip(-1.0f, 0.0f, point4s);
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
	Matrix4X4 Per = Perspective(1.0f, 2.0f);
	Array<Point3, 3> point3s{
		Point3{0.0f, 0.0f, 2.0f},
		Point3{1.0f, 0.0f, 0.0f},
		Point3{0.0f, 0.0f, -2.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto nearLeft = Per * Point3{0.0f, 0.0f, 1.0f}.ToPoint4();
	auto nearRight = Per * Point3{0.5f, 0.0f, 1.0f}.ToPoint4();
	auto clipTriangles = TriangleClip(-1.0f, 0.0f, point4s);
	auto it = clipTriangles.begin();
	EXPECT_EQ((*it)[0], point4s[0]);
	EXPECT_EQ((*it)[1], nearLeft);
	EXPECT_EQ((*it)[2], nearRight);
}

TEST(GetNotRepeatingLine2Ds, 4Triangle9Line) {
	auto Per = Perspective(1.0f, 2.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, 3.0f},
		Point3{1.5f, 2.0f, 1.5f},
		Point3{0.0f, 3.0f, 0.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 4);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 9);
}

TEST(GetNotRepeatingLine2Ds, 3Triangle7Line) {
	auto Per = Perspective(1.0f, 2.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, 3.0f},
		Point3{3.0f, 2.0f, 3.0f},
		Point3{0.0f, 3.0f, 0.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 3);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 7);
}

TEST(GetNotRepeatingLine2Ds, 2Triangle5Line) {
	auto Per = Perspective(1.0f, 2.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, 2.0f},
		Point3{1.0f, 2.0f, 2.0f},
		Point3{0.0f, 3.0f, 0.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 2);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 5);
}

TEST(GetNotRepeatingLine2Ds, 1Triangle3Line) {
	auto Per = Perspective(1.0f, 2.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, 2.0f},
		Point3{1.0f, 2.0f, 2.0f},
		Point3{0.0f, 3.0f, 1.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 1);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 3);
}

TEST(GetNotRepeatingLine2Ds, 0Triangle0Line) {
	auto Per = Perspective(1.0f, 2.0f);
	auto point3s = Array<Point3, 3>{
		Point3{0.0f, 1.0f, -1.0f},
		Point3{1.0f, 2.0f, 0.0f},
		Point3{0.0f, 3.0f, 0.0f},
	};
	auto point4s = point3s.Stream([&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto clipTriangles = TriangleNearAndFarClip(point4s);
	EXPECT_EQ(clipTriangles.Size(), 0);
	EXPECT_EQ(GetNotRepeatingScreenLines(clipTriangles).Size(), 0);
}