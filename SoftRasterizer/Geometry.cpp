#include "Geometry.h"

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

GeometryDate GenerateSphere(int longitudePointCount, int latitudePointCount) {
	return GeometryDate();
}
