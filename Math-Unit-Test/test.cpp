#include "pch.h"
#include"../SoftRasterizer/Math.h"

TEST(Point, PointPlusPoint) {
	float t1 = 2.0f;
	float t2 = 3.0f;
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