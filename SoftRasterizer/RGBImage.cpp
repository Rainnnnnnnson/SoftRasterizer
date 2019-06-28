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

RGBImage::RGBImage(PixelPointRange range) : range(range), rgbs(range.GetSize(), RGBColor{0, 0, 0}) {}

size_t RGBImage::GetWidth() const {
	return range.width;
}

size_t RGBImage::GetHeight() const {
	return range.height;
}

RGBColor RGBImage::GetImagePixel(ImagePixelPoint point) const {
	assertion(PixelPointInRange(point, range));
	return rgbs[ImagePixelPointToIndex(point, range)];
}

void RGBImage::SetImagePixel(ImagePixelPoint point, RGBColor rgb) {
	assertion(PixelPointInRange(point, range));
	rgbs[ImagePixelPointToIndex(point, range)] = rgb;
}

RGBColor RGBImage::GetScreenPixel(ScreenPixelPoint point) const {
	assertion(PixelPointInRange(point, range));
	return rgbs[ScreenPixelPointToIndex(point, range)];
}

void RGBImage::SetScreenPixel(ScreenPixelPoint point, RGBColor rgb) {
	assertion(PixelPointInRange(point, range));
	rgbs[ScreenPixelPointToIndex(point, range)] = rgb;
}

Color RGBImage::BilinearFilter(ImageCoordinate coordinate) const {
	assertion(ImageCoordinateInRangle(coordinate));
	int width = static_cast<int>(range.width);
	int height = static_cast<int>(range.height);
	float u = coordinate.x * static_cast<float>(width) - 0.5f;
	float v = coordinate.y * static_cast<float>(height) - 0.5f;
	//��������
	float u0 = floor(u);
	float u1 = u0 + 1.0f;
	float v0 = ceil(v);
	float v1 = v0 - 1.0f;
	//����ϵ��
	float uLeft = u - u0;
	float uRight = u1 - u;
	float vUp = v0 - v;
	float vDown = v - v1;
	//�������ȡ����
	//�����߽�ȡ�߽�
	size_t u0i = static_cast<size_t>(std::clamp(static_cast<int>(u0), 0, width - 1));
	size_t u1i = static_cast<size_t>(std::clamp(static_cast<int>(u1), 0, width - 1));
	size_t v0i = static_cast<size_t>(std::clamp(static_cast<int>(v0), 0, height - 1));
	size_t v1i = static_cast<size_t>(std::clamp(static_cast<int>(v1), 0, height - 1));
	// A B
	// C D
	//�ĸ�����
	Color A = GetImagePixel({u0i, v0i}).ToColor() * uLeft * vUp;
	Color B = GetImagePixel({u1i, v0i}).ToColor() * uRight * vUp;
	Color C = GetImagePixel({u0i, v1i}).ToColor()* uLeft* vDown;
	Color D = GetImagePixel({u1i, v1i}).ToColor() * uRight * vDown;
	return A + B + C + D;
}