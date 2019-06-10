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

TEST(Line2D, EqualsTest) {
	Line2D a{{0.0f, 1.0f}, {1.0f, 0.0f}};
	Line2D b{{1.0f, 0.0f}, {0.0f, 1.0f}};
	Line2D c = a;
	Line2D d{{0.0f, 0.0f}, {0.0f, 0.0f}};
	EXPECT_TRUE(a == b);
	EXPECT_TRUE(a == c);

	EXPECT_FALSE(a == d);
	EXPECT_FALSE(b == d);

}

TEST(Line2DClip, Test) {
	// A B C
	// D E F
	// G H I
	Line2D BH{{0.0f, 2.0f}, {0.0f, -2.0F}};
	EXPECT_TRUE(Line2DClip(BH));
	EXPECT_TRUE(InScreenXY(BH.pointA));
	EXPECT_TRUE(InScreenXY(BH.pointB));
	Line2D DF{{-2.0f, 0.0f}, {2.0f, 0.0F}};
	EXPECT_TRUE(Line2DClip(DF));
	EXPECT_TRUE(InScreenXY(DF.pointA));
	EXPECT_TRUE(InScreenXY(DF.pointB));
	Line2D CG{{2.0f, 2.0f}, {-2.0f, -2.0F}};
	EXPECT_TRUE(Line2DClip(CG));
	EXPECT_TRUE(InScreenXY(CG.pointA));
	EXPECT_TRUE(InScreenXY(CG.pointB));
	Line2D AG{{-2.0f, 1.0f}};
	EXPECT_TRUE(Line2DClip(AG));
}

TEST(ComputePlanePoint, Test) {
	Matrix4X4 Per = Perspective(1.0f, 2.0f);
	//近平面
	Point3 A{-2.0f, 0.0f, 0.0f};
	Point3 B{2.0f, 0.0f, 2.0f};
	Point3 C{0.0f, 0.0f, 1.0f};
	Point4 A4 = Per * A.ToPoint4();
	Point4 B4 = Per * B.ToPoint4();
	Point4 C4 = Per * C.ToPoint4();
	Point3 D1 = C4.ToPoint3();
	Point3 D2 = ComputePlanePoint(-1.0f, 0.0f, A4, B4).ToPoint3();
	EXPECT_TRUE(abs(D1.x - D2.x) < 0.000001f);
	EXPECT_TRUE(abs(D1.y - D2.y) < 0.000001f);
	EXPECT_TRUE(abs(D1.z - D2.z) < 0.000001f);
	//远平面
	Point3 E{0.0f, 1.0f, -1.0f};
	Point3 F{0.0f, 1.0f, 5.0f};
	Point3 G{0.0f, 1.0f, 2.0f};
	Point4 E4 = Per * E.ToPoint4();
	Point4 F4 = Per * F.ToPoint4();
	Point4 G4 = Per * G.ToPoint4();
	Point3 H1 = G4.ToPoint3();
	Point3 H2 = ComputePlanePoint(1.0f, -1.0f, E4, F4).ToPoint3();
	EXPECT_TRUE(abs(H1.x - H2.x) < 0.000001f);
	EXPECT_TRUE(abs(H1.y - H2.y) < 0.000001f);
	EXPECT_TRUE(abs(H1.z - H2.z) < 0.000001f);
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
	auto nearClipTriangles = TriangleClip(-1.0f, 0.0f, point4s);
	EXPECT_EQ(nearClipTriangles.Size(), 1);
	EXPECT_EQ((*nearClipTriangles.begin())[0], point4s[0]);
	EXPECT_EQ((*nearClipTriangles.begin())[1], point4s[1]);
	EXPECT_EQ((*nearClipTriangles.begin())[2], point4s[2]);
}

/*
TEST(WireframeTriangleClipCase, NotClipTest) {
	Matrix4X4 Per = Perspective(1.0f, 2.0f);
	auto point3s = array<Point3, 3>{
		Point3{0.0f, 0.0f, 2.0f},
		Point3{-1.0f, 0.0f, 1.0f},
		Point3{1.0f, 0.0f, 1.0f}
	};
	auto point4s = Stream(point3s, [&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	WireframeTriangle triangle1;
	WireframeTriangle triangle2;
	int count;
	//近平面
	triangle1 = WireframeTriangle{point4s};
	count = WireframeTriangleClip(-1.0f, 0.0f, triangle1, triangle2);
	EXPECT_EQ(triangle1.points[0], point4s[0]);
	EXPECT_EQ(triangle1.points[1], point4s[1]);
	EXPECT_EQ(triangle1.points[2], point4s[2]);
	EXPECT_EQ(count, 1);
	//远平面
	triangle1 = WireframeTriangle{point4s};
	count = WireframeTriangleClip(1.0f, -1.0f, triangle1, triangle2);
	EXPECT_EQ(triangle1.points[0], point4s[0]);
	EXPECT_EQ(triangle1.points[1], point4s[1]);
	EXPECT_EQ(triangle1.points[2], point4s[2]);
	EXPECT_EQ(count, 1);
}

TEST(WireframeTriangleClipCase, OnePointOutTest) {
	Matrix4X4 Per = Perspective(1.0f, 2.0f);
	auto nearPoint3s = array<Point3, 3>{
		Point3{0.0f, 0.0f, 4.0f},
		Point3{1.0f, 0.0f, 2.0f},
		Point3{0.0f, 0.0f, 0.0f}
	};
	auto point4s = Stream(nearPoint3s, [&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto nearLeft = Per * Point3{0.0f, 0.0f, 1.0f}.ToPoint4();
	auto nearRight = Per * Point3{0.5f, 0.0f, 1.0f}.ToPoint4();
	WireframeTriangle triangle1 = WireframeTriangle{point4s};
	WireframeTriangle triangle2;
	int count = WireframeTriangleClip(-1.0f, 0.0f, triangle1, triangle2);
	EXPECT_EQ(count, 2);
	EXPECT_EQ(triangle1.points[0], point4s[0]);
	EXPECT_EQ(triangle1.points[1], nearLeft);
	EXPECT_EQ(triangle1.points[2], nearRight);
	EXPECT_EQ(triangle2.points[0], point4s[0]);
	EXPECT_EQ(triangle2.points[1], nearRight);
	EXPECT_EQ(triangle2.points[2], point4s[1]);
}
TEST(WireframeTriangleClipCase, TwoPointOutTest) {
	Matrix4X4 Per = Perspective(1.0f, 2.0f);
	array<Point3, 3> nearPoint3s{
		Point3{0.0f, 0.0f, 2.0f},
		Point3{1.0f, 0.0f, 0.0f},
		Point3{0.0f, 0.0f, -2.0f},
	};
	auto point4s = Stream(nearPoint3s, [&](const Point3& p) {
		return Per * p.ToPoint4();
	});
	auto nearLeft = Per * Point3{0.0f, 0.0f, 1.0f}.ToPoint4();
	auto nearRight = Per * Point3{0.5f, 0.0f, 1.0f}.ToPoint4();
	WireframeTriangle triangle1 = WireframeTriangle{point4s};
	WireframeTriangle triangle2;
	int count = WireframeTriangleClip(-1.0f, 0.0f, triangle1, triangle2);
	EXPECT_EQ(count, 1);
	EXPECT_EQ(triangle1.points[0], point4s[0]);
	EXPECT_EQ(triangle1.points[1], nearLeft);
	EXPECT_EQ(triangle1.points[2], nearRight);
}

TEST(WireframeTriangleToLine2DCase,Test) {

}
*/