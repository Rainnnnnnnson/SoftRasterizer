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
	EXPECT_TRUE(rgb.r == 0);
	EXPECT_TRUE(rgb.g == 0);
	EXPECT_TRUE(rgb.b == 255);
}

TEST(BackCullingCase, Test) {
	//锐角
	Point3 up = {0.0f, 1.0f, 0.0f};
	Point3 leftDown = {-1.0f, -1.0f, 0.0f};
	Point3 rightDown = {1.0f, -1.0f, 0.0f};
	//逆时针消除
	EXPECT_TRUE(BackCulling(array<Point3, 3>{
		up, leftDown, rightDown
	}));
	//顺时针不消除
	EXPECT_FALSE(BackCulling(array<Point3, 3>{
		rightDown, leftDown, up
	}));
	//钝角
	Point3 leftUp = {-1.0, 1.0f, 0.0f};
	Point3 middle = {0.0f, 0.0f, 0.0f};
	Point3 right = {1.0f, 0.0f, 0.0f};
	//逆时针消除
	EXPECT_TRUE(BackCulling(array<Point3, 3>{
		leftUp, middle, right
	}));
	//顺时针不消除
	EXPECT_FALSE(BackCulling(array<Point3, 3>{
		right, middle, leftUp
	}));
}

TEST(Line2DCase, Test) {
	Line2D a{{0.0f, 1.0f}, {1.0f, 0.0f}};
	Line2D b{{1.0f, 0.0f}, {0.0f, 1.0f}};
	Line2D c = a;
	Line2D d{{0.0f, 0.0f}, {0.0f, 0.0f}};
	EXPECT_TRUE(a == b);
	EXPECT_TRUE(a == c);

	EXPECT_FALSE(a == d);
	EXPECT_FALSE(b == d);

}

TEST(Line2DClipCase, Test) {
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