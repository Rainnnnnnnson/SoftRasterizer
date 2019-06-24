#include "Rasterizer.h"
//�����״̬��ȴ���Ϊ2.0f
constexpr float clearDepth = 2.0f;
//�����ĻʱΪ��ɫ
constexpr Color black = {0.0f, 0.0f, 0.0f};

Rasterizer::Rasterizer(int width, int height)
	: width(width), height(height), 
	zBuffer(static_cast<size_t>(width)* height, {clearDepth, black}) {
	assert(width > 0);
	assert(height > 0);
}

void Rasterizer::Clear() {
	for (auto& buffer : zBuffer) {
		buffer.first = clearDepth;
		buffer.second = black;
	}
}

RGBImage Rasterizer::GenerateRGBImage() const {
	RGBImage image(width, height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = XYInPixel(x, y);
			auto zBufferColor = zBuffer[index];
			image.SetPixel(x, y, zBufferColor.second);
		}
	}
	return image;
}

float Rasterizer::XPixelToScreen(int x) const {
	float delta = 1.0f / width;
	float start = -1.0f + 0.5f * delta;
	float addtion = static_cast<float>(x) * delta;
	return start + addtion;
}

float Rasterizer::YPixelToScreen(int y) const {
	float delta = 1.0f / height;
	float start = -1.0f + 0.5f * delta;
	float addtion = static_cast<float>(y) * delta;
	return start + addtion;
}

int Rasterizer::XScreenToPixel(float x) const {
	//��1.0f��ʱ��Ӧ�ñ�ӳ��������,�����������Խ��
	if (x == 1.0f) {
		return width - 1;
	}
	return static_cast<int>(floor(((x + 1.0f) / 2.0f) * static_cast<float>(width)));
}

int Rasterizer::YScreenToPixel(float y) const {
	//��1.0f��ʱ��Ӧ�ñ�ӳ��������,�����������Խ��
	if (y == 1.0f) {
		return height - 1;
	}
	return static_cast<int>(floor(((y + 1.0f) / 2.0f) * static_cast<float>(height)));
}

bool Rasterizer::XYInPixel(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}

bool Rasterizer::XYInScreen(float x, float y) const {
	return x >= -1.0f && x <= 1.0f && y >= -1.0f && y <= 1.0f;
}

bool Rasterizer::ZInViewVolumn(float z) const {
	return z >= 0.0f && z <= 1.0f;
}
