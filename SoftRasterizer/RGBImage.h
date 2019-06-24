#pragma once
#include<vector>
using std::vector;
#include"Math.h"
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
	RGBImage(int width, int height);
	int GetWidth() const;
	int GetHeight() const;

	//index = index = y * width + x
	RGBColor GetPixel(int x, int y) const;
	void SetPixel(int x, int y, RGBColor rgb);

	//���·�ת��Ҫ�������� index = index = (height - 1 - y) * width + x
	RGBColor ReverseGetPixel(int x, int y) const;
	void ReverseSetPixel(int x, int y, RGBColor rgb);

	//��������ʹ��ReverseGetPixel ȡֵ[0,1] ����ȡ�߽�
	Color BilinearFilter(Point2 p) const;
private:
	bool XYInPixel(int x, int y) const;
	int PixelToIndex(int x, int y) const;
	int ReversePixelToIndex(int x, int y) const;
private:
	int width;
	int height;
	//ͼƬ�洢˳�� index = y * width + x
	vector<RGBColor> rgbs;
};