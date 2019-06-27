#pragma once
/*
	�������кܶ������������͵�
	ͳһʹ�ô˹涨
	��ǿ����ɶ���
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
	ͼƬ��������
	ͼƬImagePixelPoint�Ķ����ʾ

			 (0, 0)�K____�L(width-1, 0)
					|   |
					|___|
	(0, height - 1)�J    �I(width-1, height - 1)
*/
using ImagePixelPoint = PixelPoint;
/*
	ͼƬ������ʹ�ô�����ϵ
	ͼƬ����ϵImageCoordinate�������ʾ
			 (0, 0)�K____�L(1, 0)
					|   |
					|___|
	         (1, 0)�J    �I(1, 1)
*/
using ImageCoordinate = Point2;

/*
	��Ļ��������
	��ĻScreenPixelPoint�Ķ����ʾ

	(0, height - 1)�K____�L(width-1, height - 1)
					|   |
					|___|
			 (0, 0)�J    �I(width-1, 0)
*/
using ScreenPixelPoint = PixelPoint;

/*
	��Ļʹ�ô�����ϵ
	��Ļ����ϵScreenCoordinate�������ʾ
			(-1, 1)�K____�L(1, 1)
					|   |
					|___|
		   (-1, -1)�J    �I(1, -1)
*/
using ScreenCoordinate = Point2;

/*
	�жϸ��ַ�Χ
	�ڶ�����ʹ��
*/

bool PixelPointInRange(PixelPoint point, PixelPointRange range);
bool ImageCoordinateInRangle(ImageCoordinate coordinate);
bool ScreenCoordinateInRangle(ScreenCoordinate coordinate);
bool DepthInViewVolumn(float depth);

/*
	������ת�� ת��������ڷ�Χ����
	x��y���߼���һ���� ʹ��ͬһ���������
	����ϵ�ı߸պþ����������ĵ�
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
	�洢�������
	ͳһ����Image�Ĵ洢��ʽ
	������ʹ�õ�ʱ�򲢲�һ��
	ScreenPixelPointToIndex ��Ҫ��ת�洢
*/
size_t ImagePixelPointToIndex(ImagePixelPoint point, PixelPointRange range);
size_t ScreenPixelPointToIndex(ScreenPixelPoint point, PixelPointRange range);