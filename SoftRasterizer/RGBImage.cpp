#include "RGBImage.h"
#include "Assertion.h"
#include <algorithm>
#include <cmath>
unsigned char ColorFloatToByte(float f) {
	assert(!isnan(f));
	float test = f * 255.0f;
	if (test <= 0) {
		return 0;
	} else if (test >= 255.0f) {
		return static_cast<unsigned char>(255);
	} else {
		return static_cast<unsigned char>(test);
	}
}

RGBColor::RGBColor(unsigned char r, unsigned char g, unsigned char b)
	: r(r), g(g), b(b) {}

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

RGBImage::RGBImage(int width, int height) : width(width), height(height),
rgbs(static_cast<size_t>(width)* height, RGBColor{0, 0, 0}) {
	assert(width > 0);
	assert(height > 0);
}

int RGBImage::GetWidth() const {
	return width;
}

int RGBImage::GetHeight() const {
	return height;
}

RGBColor RGBImage::ReverseGetPixel(int x, int y) const {
	assert(XYInPixel(x, y));
	return rgbs[ReversePixelToIndex(x, y)];
}

void RGBImage::ReverseSetPixel(int x, int y, RGBColor rgb) {
	assert(XYInPixel(x, y));
	rgbs[ReversePixelToIndex(x, y)] = rgb;
}

RGBColor RGBImage::GetPixel(int x, int y) const {
	assert(XYInPixel(x, y));
	return rgbs[PixelToIndex(x, y)];
}

void RGBImage::SetPixel(int x, int y, RGBColor rgb) {
	assert(XYInPixel(x, y));
	rgbs[PixelToIndex(x, y)] = rgb;
}

Color RGBImage::BilinearFilter(Point2 p) const {
	assert(p.x >= 0.0f);
	assert(p.y >= 0.0f);
	assert(p.x <= 1.0f);
	assert(p.y <= 1.0f);
	float u = p.x * static_cast<float>(width) - 0.5f;
	float v = p.y * static_cast<float>(height) - 0.5f;
	//像素坐标
	float u0 = floor(u);
	float u1 = u0 + 1.0f;
	float v0 = floor(v);
	float v1 = v0 + 1.0f;
	//坐标系数
	float uRight = u1 - u;
	float uLeft = u - u0;
	float vUp = v1 - v;
	float vDown = v - v0;
	//变成整数取数组
	//超过边界取边界
	int u0i = std::clamp(static_cast<int>(u0), 0, width - 1);
	int u1i = std::clamp(static_cast<int>(u1), 0, width - 1);
	int v0i = std::clamp(static_cast<int>(v0), 0, height - 1);
	int v1i = std::clamp(static_cast<int>(v1), 0, height - 1);
	// A B
	// C D
	//四个像素
	Color A = ReverseGetPixel(u0i, v0i).ToColor() * uLeft * vUp;
	Color B = ReverseGetPixel(u1i, v0i).ToColor() * uRight * vUp;
	Color C = ReverseGetPixel(u0i, v1i).ToColor() * uLeft * vDown;
	Color D = ReverseGetPixel(u1i, v1i).ToColor() * uRight * vDown;
	return A + B + C + D;
}

bool RGBImage::XYInPixel(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}

int RGBImage::PixelToIndex(int x, int y) const {
	return y * width + x;
}

int RGBImage::ReversePixelToIndex(int x, int y) const {
	int reverseY = height - y - 1;
	return reverseY * width + x;
}
