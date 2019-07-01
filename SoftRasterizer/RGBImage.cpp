#include "RGBImage.h"
#include "Assertion.h"
#include <algorithm>
#include <cmath>
unsigned char ColorFloatToByte(float f) {
	assertion(!isnan(f));
	float test = f * 255.0f;
	if (test <= 0) {
		return 0;
	} else if (test >= 255.0f) {
		return static_cast<unsigned char>(255);
	} else {
		return static_cast<unsigned char>(test);
	}
}

RGBColor::RGBColor() : r(0), g(0), b(0) {}

RGBColor::RGBColor(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}

RGBColor::RGBColor(Color color) {
	r = ColorFloatToByte(color.r);
	g = ColorFloatToByte(color.g);
	b = ColorFloatToByte(color.b);
}

float ByteToColorFloat(unsigned char b) {
	return static_cast<float>(b) / 255.0f;
}

Color RGBColor::ToColor() const {
	return Color{
		ByteToColorFloat(r),
		ByteToColorFloat(g),
		ByteToColorFloat(b)
	};
}

Color PointSample(const RGBImage& image, Point2 point) {
	assertion(point.x >= 0.0f && point.x <= 1.0f);
	assertion(point.y >= 0.0f && point.y <= 1.0f);
	unsigned x = static_cast<unsigned>(round(point.x * static_cast<float>(image.GetWidth() - 1)));
	unsigned y = static_cast<unsigned>(round(point.y * static_cast<float>(image.GetHeight() - 1)));
	return image.GetImagePoint(x, y).ToColor();
}

Color BilinearFilter(const RGBImage& image, Point2 point) {
	assertion(point.x >= 0.0f && point.x <= 1.0f);
	assertion(point.y >= 0.0f && point.y <= 1.0f);
	assertion(image.GetWidth() >= 2 && image.GetHeight() >= 2);
	//经过计算刚好在4倍分辨率的时候完美采样
	float u = point.x * static_cast<float>(image.GetWidth() - 2) + 0.5f;
	float v = point.y * static_cast<float>(image.GetHeight() - 2) + 0.5f;
	float u0 = floor(u);
	float v0 = floor(v);
	float u1 = u0 + 1.0f;
	float v1 = v0 + 1.0f;
	//坐标系数
	float uLeft = u - u0;
	float uRight = u1 - u;
	float vUp = v - v0;
	float vDown = v1 - v;
	//变成整数
	unsigned u0i = static_cast<unsigned>(u0);
	unsigned u1i = static_cast<unsigned>(u1);
	unsigned v0i = static_cast<unsigned>(v0);
	unsigned v1i = static_cast<unsigned>(v1);
	// A B
	// C D
	//四个像素
	Color A = image.GetImagePoint(u0i, v0i).ToColor() * uLeft * vUp;
	Color B = image.GetImagePoint(u1i, v0i).ToColor() * uRight * vUp;
	Color C = image.GetImagePoint(u0i, v1i).ToColor() * uLeft * vDown;
	Color D = image.GetImagePoint(u1i, v1i).ToColor() * uRight * vDown;
	return A + B + C + D;
}
