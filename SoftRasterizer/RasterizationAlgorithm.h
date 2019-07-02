#pragma once
/*
	在Rasterization和ShadowMap中出现大量相同代码
	提取出来归结在这里
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

//坐标转化
float ScreenPixelPointToCoordinate(unsigned pixel, unsigned pixelCount);
int ScreenCoordinateToPixelPoint(float coordinate, unsigned pixelCount);
/*
	将齐次顶点转化成屏幕上的点
	point(x/w, y/w)
*/
Point2 ConvertToScreenPoint(Point4 p);

/*
	p0 p1 为超平面两侧的点
	返回平面上的线性插值系数t
	满足式子 p = (1 - t) * p0 + t * p1;

	向量plane(A,B,C,D)代表平面
	表达式  Ax + By + Cz + Dw = 0

	左平面  -x           + lw = 0     plane(-1,  0,  0, -1)
	右平面   x           - rw = 0     plane( 1,  0,  0, -1)
	下平面       -y      + bw = 0     plane( 0, -1,  0, -1)
	上平面        y      - tw = 0     plane( 0,  1,  0, -1)
	近平面            -z + nw = 0     plane( 0,  0, -1,  0)
	远平面             z - fw = 0     plane( 0,  0,  1, -1)
*/
float CalculatePlaneInterpolationCoefficient(Vector4 plane, Point4 p0, Point4 p1);

/*
	带入上面6个平面的向量
	计算顶点到平面上的距离
	这里的距离仅仅判断在平面的哪个方位
	distance > 0   point在剪裁空间外
	distance = 0   point在超平面上
	distance < 0   point在剪裁空间方向
*/
float CalculatePlaneDistance(Vector4 plane, Point4 point);

/*
	根据梯度方向返回在直线上方还是下方
	带入直线方程式得到的数值
	p0 p1 为直线端点
	p为需要判断的顶点
*/
float CalculateLineEquation(Point2 p, Point2 p0, Point2 p1);

/*
	所有顶点都在剪裁空间外面的三角形将直接被剔除
	返回true则代表消除
*/
bool ViewVolumnCull(const PointTriangle& points);

/*
	逆时针消除 根据传入顶点的顺序
	返回true则代表消除
*/
bool BackCull(const PointTriangle& points);

/*
	使用重心系数计算顶点
	这里直接使用线性来计算
*/
Point4 CalculatePointByCoefficient(const GravityCoefficient& coefficients,
								   const PointTriangle& points);

/*
	三角形光栅化步骤如下
	1 计算各像素位置
	2 根据位置生成 重心系数
	3 若该位置在深度内和三角形内调用function
*/
void TriangleRasterization(unsigned width,unsigned height,
						   const PointTriangle& points,
						   const UseScreenPoint& useCoefficient);