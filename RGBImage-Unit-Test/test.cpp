#include "pch.h"
#include "../SoftRasterizer/RGBImage.h"

TEST(RGBColor, EdgeTest) {
	RGBColor rgb(Color{-1.0f, -1.0f, -1.0f});
	EXPECT_EQ(rgb.r, 0);
	EXPECT_EQ(rgb.g, 0);
	EXPECT_EQ(rgb.b, 0);
	Color c = rgb.ToColor();
	EXPECT_EQ(c.r, 0.0f);
	EXPECT_EQ(c.g, 0.0f);
	EXPECT_EQ(c.b, 0.0f);

	rgb = RGBColor(Color{2.0f, 2.0f, 2.0f});
	EXPECT_EQ(rgb.r, 255);
	EXPECT_EQ(rgb.g, 255);
	EXPECT_EQ(rgb.b, 255);
	c = rgb.ToColor();
	EXPECT_EQ(c.r, 1.0f);
	EXPECT_EQ(c.g, 1.0f);
	EXPECT_EQ(c.b, 1.0f);
}

TEST(RGBImage, PointSampleTest) {
	RGBImage image({800, 600});
	image.SetImagePixel({0, 0}, {255, 255, 255});
	image.SetImagePixel({799, 0}, {0, 255, 255});
	image.SetImagePixel({0, 599}, {255, 0, 255});
	image.SetImagePixel({799, 599}, {255, 255, 0});
	Color c = image.PointSample({0.0f, 0.0f});
	EXPECT_EQ(c.r, 1.0f);
	EXPECT_EQ(c.g, 1.0f);
	EXPECT_EQ(c.b, 1.0f);
	c = image.PointSample({1.0f, 0.0f});
	EXPECT_EQ(c.r, 0.0f);
	EXPECT_EQ(c.g, 1.0f);
	EXPECT_EQ(c.b, 1.0f);
	c = image.PointSample({0.0f, 1.0f});
	EXPECT_EQ(c.r, 1.0f);
	EXPECT_EQ(c.g, 0.0f);
	EXPECT_EQ(c.b, 1.0f);
	c = image.PointSample({1.0f, 1.0f});
	EXPECT_EQ(c.r, 1.0f);
	EXPECT_EQ(c.g, 1.0f);
	EXPECT_EQ(c.b, 0.0f);
}

TEST(RGBImage, BilinearFilterTest) {
	RGBImage image({2, 2});
	image.SetImagePixel({0, 0}, {255, 255, 255});
	image.SetImagePixel({1, 0}, {0, 255, 255});
	image.SetImagePixel({0, 1}, {255, 0, 255});
	image.SetImagePixel({1, 1}, {255, 255, 0});
	Color c = image.BilinearFilter({0.0f, 0.0f});
	EXPECT_EQ(c.r, 1.0f);
	EXPECT_EQ(c.g, 1.0f);
	EXPECT_EQ(c.b, 1.0f);
	c = image.BilinearFilter({1.0f, 0.0f});
	EXPECT_EQ(c.r, 0.0f);
	EXPECT_EQ(c.g, 1.0f);
	EXPECT_EQ(c.b, 1.0f);
	c = image.BilinearFilter({0.0f, 1.0f});
	EXPECT_EQ(c.r, 1.0f);
	EXPECT_EQ(c.g, 0.0f);
	EXPECT_EQ(c.b, 1.0f);
	c = image.BilinearFilter({1.0f, 1.0f});
	EXPECT_EQ(c.r, 1.0f);
	EXPECT_EQ(c.g, 1.0f);
	EXPECT_EQ(c.b, 0.0f);
	c = image.BilinearFilter({0.5f, 0.5f});
	EXPECT_EQ(c.r, 0.75f);
	EXPECT_EQ(c.g, 0.75f);
	EXPECT_EQ(c.b, 0.75f);
}