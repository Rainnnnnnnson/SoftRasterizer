#include "Geometry.h"
#include "Assertion.h"
#include <cmath>
GeometryDate GenerateCube() {
	vector<Point3> points;
	points.reserve(8);
	points.push_back({-1.0f, -1.0f, -1.0f});
	points.push_back({-1.0f, 1.0f, -1.0f});
	points.push_back({1.0f, 1.0f, -1.0f});
	points.push_back({1.0f, -1.0f, -1.0f});
	points.push_back({-1.0f, -1.0f, 1.0f});
	points.push_back({-1.0f, 1.0f, 1.0f});
	points.push_back({1.0f, 1.0f, 1.0f});
	points.push_back({1.0f, -1.0f, 1.0f});
	vector<Vector3> normals;

	normals.reserve(8);
	for (auto point : points) {
		normals.push_back(point.GetVector3().Normalize());
	}

	vector<GeometryIndex> indexs;
	//正面
	indexs.push_back({{0, 1, 2}, {0, 1, 2}});
	indexs.push_back({{0, 2, 3}, {0, 2, 3}});
	//左面
	indexs.push_back({{4, 5, 1}, {4, 5, 1}});
	indexs.push_back({{4, 1, 0}, {4, 1, 0}});
	//上面
	indexs.push_back({{1, 5, 6}, {1, 5, 6}});
	indexs.push_back({{1, 6, 2}, {1, 6, 2}});
	//右面
	indexs.push_back({{3, 2, 6}, {3, 2, 6}});
	indexs.push_back({{3, 6, 7}, {3, 6, 7}});
	//下面
	indexs.push_back({{4, 0, 3}, {4, 0, 3}});
	indexs.push_back({{4, 3, 7}, {4, 3, 7}});
	//后面
	indexs.push_back({{7, 6, 5}, {7, 6, 5}});
	indexs.push_back({{7, 5, 4}, {7, 5, 4}});
	return{std::move(points), std::move(normals), std::move(indexs)};
}

GeometryDate GenerateSphere(unsigned longitudePointCount, unsigned latitudePointCount) {
	assertion(longitudePointCount >= 3);
	assertion(latitudePointCount >= 1);

	/*
		球体上下有两个独立的顶点
	*/
	Point3 bottom{0.0f, -1.0f, 0.0f};
	Point3 top{0.0f, 1.0f, 0.0f};
	unsigned pointCount = 2 + longitudePointCount * latitudePointCount;

	vector<Point3> points;
	points.reserve(pointCount);
	vector<Vector3> normals;
	normals.reserve(pointCount);

	points.push_back(bottom);
	normals.push_back(bottom.GetVector3());

	//纬度从 球的底部 开始计算 (排除底部和顶部两个点)
	float latitudeStart = -PI / 2.0f;
	float latitudeDelta = PI / static_cast<float>(latitudePointCount + 1);
	//经度从 球的右边 开始计算
	float longitudeStart = 0.0f;
	float longitudeDelta = (2.0f * PI) / static_cast<float>(longitudePointCount);

	for (unsigned yIndex = 0; yIndex < latitudePointCount; yIndex++) {
		float yRadian = latitudeStart + static_cast<float>(yIndex + 1) * latitudeDelta;
		float y = sin(yRadian);
		float radius = cos(yRadian);
		for (unsigned xzIndex = 0; xzIndex < longitudePointCount; xzIndex++) {
			float xzRadian = longitudeStart + static_cast<float>(xzIndex) * longitudeDelta;
			float x = radius * cos(xzRadian);
			float z = radius * sin(xzRadian);
			points.push_back({x, y, z});
			normals.push_back({x, y, z});
		}
	}

	points.push_back(top);
	normals.push_back(top.GetVector3());

	vector<GeometryIndex> indexs;
	array<unsigned, 3> arr;
	//计算底部一圈的索引
	unsigned firstPoint = 0;
	unsigned bottomIndex = 1;
	while (bottomIndex < longitudePointCount) {
		arr = {bottomIndex + 1, firstPoint, bottomIndex};
		indexs.push_back({arr, arr});
		bottomIndex++;
	}
	arr = {1, 0, longitudePointCount};
	indexs.push_back({arr, arr});

	//计算中间的条纹状的索引
	for (unsigned yIndex = 0; yIndex < latitudePointCount - 1; yIndex++) {
		unsigned middleIndex = 1;
		while (middleIndex < longitudePointCount) {
			arr = {yIndex * longitudePointCount + middleIndex,
				(yIndex + 1) * longitudePointCount + middleIndex,
				(yIndex + 1) * longitudePointCount + middleIndex + 1
			};
			indexs.push_back({arr, arr});
			arr = {yIndex * longitudePointCount + middleIndex,
				(yIndex + 1) * longitudePointCount + middleIndex + 1,
				yIndex * longitudePointCount + middleIndex + 1
			};
			indexs.push_back({arr, arr});
			middleIndex++;
		}
		arr = {yIndex * longitudePointCount + middleIndex,
			(yIndex + 1) * longitudePointCount + middleIndex,
			yIndex * longitudePointCount + middleIndex + 1
		};
		indexs.push_back({arr, arr});
		arr = {yIndex * longitudePointCount + middleIndex,
			yIndex * longitudePointCount + middleIndex + 1,
			(yIndex-1) * longitudePointCount + middleIndex + 1
		};
		indexs.push_back({arr, arr});
	}

	//计算顶部一圈的索引
	unsigned lastPoint = longitudePointCount * latitudePointCount + 1;
	unsigned topIndex = longitudePointCount * (latitudePointCount - 1) + 1;
	while (topIndex < lastPoint - 1) {
		arr = {topIndex, lastPoint, topIndex + 1};
		indexs.push_back({arr, arr});
		topIndex++;
	}
	arr = {topIndex, lastPoint, topIndex + 1 - longitudePointCount};
	indexs.push_back({arr, arr});
	return{std::move(points), std::move(normals), std::move(indexs)};
}
