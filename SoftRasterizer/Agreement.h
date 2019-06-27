#pragma once
/*
	程序中有很多有歧义的坐标和点
	统一使用此规定
	增强代码可读性
*/
#include<utility>
#include"Math.h"

struct PixelPoint {
	size_t x, y;
};

struct PixelPointRange {
	size_t width, height;
	size_t GetSize() const;
	float GetAspectRatio() const;
};

/*
	图片像素坐标
	图片ImagePixelPoint的顶点表示

			 (0, 0)↘____↙(width-1, 0)
					|   |
					|___|
	(0, height - 1)↗    ↖(width-1, height - 1)
*/
using ImagePixelPoint = PixelPoint;
/*
	图片和纹理使用此坐标系
	图片坐标系ImageCoordinate的坐标表示
			 (0, 0)↘____↙(1, 0)
					|   |
					|___|
	         (1, 0)↗    ↖(1, 1)
*/
using ImageCoordinate = Point2;

/*
	屏幕像素坐标
	屏幕ScreenPixelPoint的顶点表示

	(0, height - 1)↘____↙(width-1, height - 1)
					|   |
					|___|
			 (0, 0)↗    ↖(width-1, 0)
*/
using ScreenPixelPoint = PixelPoint;

/*
	屏幕使用此坐标系
	屏幕坐标系ScreenCoordinate的坐标表示
			(-1, 1)↘____↙(1, 1)
					|   |
					|___|
		   (-1, -1)↗    ↖(1, -1)
*/
using ScreenCoordinate = Point2;

/*
	判断各种范围
	在断言中使用
*/

bool PixelPointInRange(PixelPoint point, PixelPointRange range);
bool ImageCoordinateInRangle(ImageCoordinate coordinate);
bool ScreenCoordinateInRangle(ScreenCoordinate coordinate);
bool DepthInViewVolumn(float depth);

/*
	常见的转化 转化后可能在范围外面
	x和y的逻辑是一样的 使用同一个函数表达
	坐标系的边刚好经过像素中心点
*/
/*
	[0, pixelCount - 1] => [0, 1]
	[0, 1] => [0, pixelCount - 1]
*/
float ImagePixelPointToCoordinate(size_t pixel, size_t pixelCount);
int ImageCoordinateToPixelPoint(float coordinate, size_t pixelCount);
/*
	[0, pixelCount - 1] => [-1, 1]
	[-1, 1] => [0, pixelCount - 1]
*/
float ScreenPixelPointToCoordinate(size_t pixel, size_t pixelCount);
int ScreenCoordinateToPixelPoint(float coordinate, size_t pixelCount);

/*
	存储索引表达
	统一采用Image的存储方式
	但是在使用的时候并不一样
	ScreenPixelPointToIndex 需要倒转存储
*/
size_t ImagePixelPointToIndex(ImagePixelPoint point, PixelPointRange range);
size_t ScreenPixelPointToIndex(ScreenPixelPoint point, PixelPointRange range);