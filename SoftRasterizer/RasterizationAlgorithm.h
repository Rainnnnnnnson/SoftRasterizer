#pragma once
/*
	��Rasterization��ShadowMap�г��ִ�����ͬ����
	��ȡ�������������
*/
#include "Math.h"
#include <array>
#include <functional>
using std::function;
using std::array;
using std::tuple;
using PointTriangle = array<Point4, 3>;
using GravityCoefficient = array<float, 3>;
using ScreenTriangle = array<Point2, 3>;
using UseScreenPoint = function<void(unsigned, unsigned, float, const GravityCoefficient&)>;

//����ת��
float ScreenPixelPointToCoordinate(unsigned pixel, unsigned pixelCount);
int ScreenCoordinateToPixelPoint(float coordinate, unsigned pixelCount);
/*
	����ζ���ת������Ļ�ϵĵ�
	point(x/w, y/w)
*/
Point2 ConvertToScreenPoint(Point4 p);

/*
	p0 p1 Ϊ��ƽ������ĵ�
	����ƽ���ϵ����Բ�ֵϵ��t
	����ʽ�� p = (1 - t) * p0 + t * p1;

	����plane(A,B,C,D)����ƽ��
	���ʽ  Ax + By + Cz + Dw = 0

	��ƽ��  -x           + lw = 0     plane(-1,  0,  0, -1)
	��ƽ��   x           - rw = 0     plane( 1,  0,  0, -1)
	��ƽ��       -y      + bw = 0     plane( 0, -1,  0, -1)
	��ƽ��        y      - tw = 0     plane( 0,  1,  0, -1)
	��ƽ��            -z + nw = 0     plane( 0,  0, -1,  0)
	Զƽ��             z - fw = 0     plane( 0,  0,  1, -1)
*/
float CalculatePlaneInterpolationCoefficient(Vector4 plane, Point4 p0, Point4 p1);

/*
	��������6��ƽ�������
	���㶥�㵽ƽ���ϵľ���
	����ľ�������ж���ƽ����ĸ���λ
	distance > 0   point�ڼ��ÿռ���
	distance = 0   point�ڳ�ƽ����
	distance < 0   point�ڼ��ÿռ䷽��
*/
float CalculatePlaneDistance(Vector4 plane, Point4 point);

/*
	�����ݶȷ��򷵻���ֱ���Ϸ������·�
	����ֱ�߷���ʽ�õ�����ֵ
	p0 p1 Ϊֱ�߶˵�
	pΪ��Ҫ�жϵĶ���
*/
float CalculateLineEquation(Point2 p, Point2 p0, Point2 p1);

/*
	���ж��㶼�ڼ��ÿռ�����������ν�ֱ�ӱ��޳�
	����true���������
*/
bool ViewVolumnCull(const PointTriangle& points);

/*
	��ʱ������ ���ݴ��붥���˳��
	����true���������
*/
bool BackCull(const PointTriangle& points);

/*
	ʹ������ϵ�����㶥��
	����ֱ��ʹ������������
*/
Point4 CalculatePointByCoefficient(const GravityCoefficient& coefficients,
								   const PointTriangle& points);

/*
	�����ι�դ����������
	1 ���������λ��
	2 ����λ������ ����ϵ��
	3 ����λ��������ں��������ڵ���function
*/
void TriangleRasterization(unsigned width,unsigned height,
						   const PointTriangle& points,
						   const UseScreenPoint& useCoefficient);