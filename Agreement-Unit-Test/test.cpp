#include "pch.h"
#include "../SoftRasterizer/Agreement.h"

TEST(PixelPointRange, Test) {
	PixelPointRange range{1200, 600};
	EXPECT_EQ(range.GetSize(),720000);
	EXPECT_EQ(range.GetAspectRatio(), 2.0f);
}

TEST(PixelPointInRange, EdgeTest) {
	EXPECT_TRUE(PixelPointInRange({0, 0}, {800, 600}));
	EXPECT_TRUE(PixelPointInRange({799, 599}, {800, 600}));

	EXPECT_FALSE(PixelPointInRange({800, 0}, {800, 600}));
	EXPECT_FALSE(PixelPointInRange({0, 600}, {800, 600}));
}

TEST(ImageCoordinateInRangle, EdgeTest) {
	EXPECT_TRUE(ImageCoordinateInRangle({0.0f, 0.0f}));
	EXPECT_TRUE(ImageCoordinateInRangle({0.0f, 1.0f}));
	EXPECT_TRUE(ImageCoordinateInRangle({1.0f, 0.0f}));
	EXPECT_TRUE(ImageCoordinateInRangle({1.0f, 1.0f}));

	EXPECT_FALSE(ImageCoordinateInRangle({-0.001f, 0.0f}));
	EXPECT_FALSE(ImageCoordinateInRangle({1.001f, 0.0f}));
	EXPECT_FALSE(ImageCoordinateInRangle({0.0f, -0.001f}));
	EXPECT_FALSE(ImageCoordinateInRangle({0.0f, 1.001f}));
}

TEST(ScreenCoordinateInRangle, EdgeTest) {
	EXPECT_TRUE(ScreenCoordinateInRangle({-1.0f, -1.0f}));
	EXPECT_TRUE(ScreenCoordinateInRangle({-1.0f, 1.0f}));
	EXPECT_TRUE(ScreenCoordinateInRangle({1.0f, -1.0f}));
	EXPECT_TRUE(ScreenCoordinateInRangle({1.0f, 1.0f}));

	EXPECT_FALSE(ScreenCoordinateInRangle({-1.001f, 0.0f}));
	EXPECT_FALSE(ScreenCoordinateInRangle({1.001f, 0.0f}));
	EXPECT_FALSE(ScreenCoordinateInRangle({0.0f, -1.001f}));
	EXPECT_FALSE(ScreenCoordinateInRangle({0.0f, 1.001f}));
}

TEST(DepthInViewVolumn, EdgeTest) {
	EXPECT_TRUE(DepthInViewVolumn(0.0f));
	EXPECT_TRUE(DepthInViewVolumn(1.0f));

	EXPECT_FALSE(DepthInViewVolumn(-0.001f));
	EXPECT_FALSE(DepthInViewVolumn(1.001f));
}

TEST(ImagePixelPointToCoordinate, EdgeTest) {
	EXPECT_EQ(ImagePixelPointToCoordinate(0, 800), 0.0f);
	EXPECT_EQ(ImagePixelPointToCoordinate(799, 800), 1.0f);
}

TEST(ImageCoordinateToPixelPoint, EdgeTest) {
	EXPECT_EQ(ImageCoordinateToPixelPoint(0.0f, 800), 0);
	EXPECT_EQ(ImageCoordinateToPixelPoint(1.0f, 800), 799);
}

TEST(ScreenPixelPointToCoordinate, EdgeTest) {
	EXPECT_EQ(ScreenPixelPointToCoordinate(0, 800), -1.0f);
	EXPECT_EQ(ScreenPixelPointToCoordinate(799, 800), 1.0f);
}

TEST(ScreenCoordinateToPixelPoint, EdgeTest) {
	EXPECT_EQ(ScreenCoordinateToPixelPoint(-1.0f, 800), 0);
	EXPECT_EQ(ScreenCoordinateToPixelPoint(1.0f, 800), 799);
}

TEST(ImagePixelPointToIndex, EdgeTest) {
	EXPECT_EQ(ImagePixelPointToIndex({0, 0}, {20, 30}), 0);
	EXPECT_EQ(ImagePixelPointToIndex({19, 29}, {20, 30}), 599);
}

TEST(ScreenPixelPointToIndex, EdgeTest) {
	EXPECT_EQ(ScreenPixelPointToIndex({0, 29}, {20, 30}), 0);
	EXPECT_EQ(ScreenPixelPointToIndex({19, 0}, {20, 30}), 599);
}