#pragma once
#include "Math.h"
#include <vector>
#include <array>
using std::vector;
using std::array;

struct GeometryIndex {
	array<size_t, 3> point;
	array<size_t, 3> normal;
};

struct GeometryDate {
	vector<Point3> points;
	vector<Vector3> normals;
	vector<GeometryIndex> index;
};

/*
	生成中心在原点的正方体
	正方体每一个面轴对齐
	正方体边长为2
	法线由中心指向顶点

	假设一个面四个顶点为ABCD
	B C
	A D
	该面由两个三角形索引为(A B C)(A C D)
	以 正 左 上 右 下 后 的顺序绘制
*/
GeometryDate GenerateCube();

/*
	根据纵向和横向顶点数量建立球 
	若顶点数量较少,可能看不出是球体
	球心在原点 边长为1
	法向由圆心指向顶点

	longitudePointCount 球经度上的点的个数 (必须大于3)
	latitudePointCount  球纬度上的点的个数 (必须大于1)
*/
GeometryDate GenerateSphere(int longitudePointCount, int latitudePointCount);