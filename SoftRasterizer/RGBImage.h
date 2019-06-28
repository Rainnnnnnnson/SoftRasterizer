#pragma once
#include<vector>
#include"Agreement.h"
using std::vector;

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

	Color PointSample(ImageCoordinate coordinate) const;
	Color BilinearFilter(ImageCoordinate coordinate) const;
private:
	PixelPointRange range;
	vector<RGBColor> rgbs;
};