#pragma once
#include<vector>
#include"Agreement.h"
using std::vector;
using std::pair;

class RGBColor {
public:
	RGBColor(unsigned char r, unsigned char g, unsigned char b);
	RGBColor(Color color);
	Color ToColor() const;
public:
	unsigned char r, g, b;
};

class RGBImage {
public:
	RGBImage(PixelPointRange range);
	size_t GetWidth() const;
	size_t GetHeight() const;

	RGBColor GetImagePixel(ImagePixelPoint point) const;
	void SetImagePixel(ImagePixelPoint point, RGBColor rgb);
	RGBColor GetScreenPixel(ScreenPixelPoint point) const;
	void SetScreenPixel(ScreenPixelPoint point, RGBColor rgb);

	/*
		双线性滤波
		纹理坐标[0,1] * [0,1] 超出取边界
	*/
	Color BilinearFilter(ImageCoordinate coordinate) const;
private:
	PixelPointRange range;
	vector<RGBColor> rgbs;
};