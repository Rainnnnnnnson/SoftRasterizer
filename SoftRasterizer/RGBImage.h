#pragma once
#include "Texture2D.h"
#include "Math.h"

class RGBColor {
public:
	RGBColor();
	RGBColor(unsigned char r, unsigned char g, unsigned char b);
	RGBColor(Color color);
	Color ToColor() const;
public:
	unsigned char r, g, b;
};

using RGBImage = Texture2D<RGBColor>;

Color PointSample(const RGBImage& image, Point2 point);
Color BilinearFilter(const RGBImage& image, Point2 point);