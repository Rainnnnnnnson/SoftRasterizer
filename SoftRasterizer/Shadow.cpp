#include "Shadow.h"
#include "RasterizationAlgorithm.h"
using std::pair;

//从Rasterizer 里面Copy过来改了一点点
vector<PointTriangle> NearPlanClipAndBackCull(const PointTriangle& points);

Texture2D<float> GenerateShadowMap(unsigned width, unsigned height, 
								   const vector<Point3>& points, 
								   const vector<array<unsigned, 3>> & indexs,
								   function<Point4(Point4)> transfrom) {
	auto result = Texture2D<float>{width, height};
	for (const auto& index : indexs) {
		assertion(std::all_of(index.begin(), index.end(), [&](unsigned i) {
			return i < points.size();
		}));
		array<Point4, 3> trianglePoints;
		for (int i = 0; i < 3; i++) {
			trianglePoints[i] = transfrom(points[index[i]].ToPoint4());
		}
		if (ViewVolumnCull(trianglePoints)) {
			continue;
		}
		auto clipTrianglePoints = NearPlanClipAndBackCull(trianglePoints);
		for (auto clipTrianglePoint : clipTrianglePoints) {
			TriangleRasterization(width, height, clipTrianglePoint,
								  [&](unsigned x, unsigned y, float depth, const GravityCoefficient&) {
				if (depth < result.GetScreenPoint(x, y)) {
					result.SetScreenPoint(x, y, depth);
				}
			});
		}
	}
	return result;
}

vector<PointTriangle> NearPlanClipAndBackCull(const PointTriangle& points) {
	constexpr auto nearPlane = Vector4{0.0f, 0.0f, -1.0f, 0.0f};

	array<pair<Point4, float>, 3> pointDistances;
	for (int i = 0; i < 3; i++) {
		pointDistances[i].first = points[i];
		pointDistances[i].second = CalculatePlaneDistance(nearPlane, points[i]);
	}
	/*
		因为顶点需要排序 得到对应顺序后剪裁
		每次进行一次交换 代表三角形顶点顺时针逆时针将会转变
		一个三角形最多需要交换三次顶点
		根据4种情况的列举可得 !(BackCull(triangle) ^ reverse) 可以确定是否需要绘制
		使用新顶点构造三角形来确定是否绘制
		保持顺时针传入 再根据reverse综合判断
	*/
	bool reverse = false;
	if (pointDistances[2].second < pointDistances[1].second) {
		std::swap(pointDistances[2], pointDistances[1]);
		reverse = !reverse;
	}
	if (pointDistances[1].second < pointDistances[0].second) {
		std::swap(pointDistances[1], pointDistances[0]);
		reverse = !reverse;
	}
	if (pointDistances[2].second < pointDistances[1].second) {
		std::swap(pointDistances[2], pointDistances[1]);
		reverse = !reverse;
	}
	//计算保留的点
	auto keepPointCount = std::count_if(pointDistances.begin(), pointDistances.end(), [](const auto& data) {
		return data.second <= 0.0f;
	});
	//需要使用顶点来计算剪裁后的插值
	Point4 A = pointDistances[0].first;
	Point4 B = pointDistances[1].first;
	Point4 C = pointDistances[2].first;
	vector<PointTriangle> result;
	if (keepPointCount == 1) {
		/*
					   *A
					   |\   F
		  _____________|_\↙______________近平面
					E↗|  \
					   |  /↖B
					   | /
					   |/
					   *C
		*/
		float tAEC = CalculatePlaneInterpolationCoefficient(nearPlane, A, C);
		float tAFB = CalculatePlaneInterpolationCoefficient(nearPlane, A, B);
		Point4 E = A * (1.0f - tAEC) + C * (tAEC);
		Point4 F = A * (1.0f - tAFB) + B * (tAFB);
		PointTriangle testTriangle{A, F, E};
		if (!(BackCull(testTriangle) ^ reverse)) {
			result.reserve(1);
			result.push_back({A, F, E});
		}
	} else if (keepPointCount == 2) {
		//这里的逻辑和上面相似不再重复注释
		/*
					   * A
					   |\
					   | \
					   |  \ B
			___________|__/_______________
					 ↗| /↖
					E  |/   F
					   *C
		*/
		float tAEC = CalculatePlaneInterpolationCoefficient(nearPlane, A, C);
		float tBFC = CalculatePlaneInterpolationCoefficient(nearPlane, B, C);
		Point4 E = A * (1.0f - tAEC) + C * (tAEC);
		Point4 F = B * (1.0f - tBFC) + C * (tBFC);
		PointTriangle testTriangle{A, B, F};
		if (!(BackCull(testTriangle) ^ reverse)) {
			result.reserve(2);
			result.push_back({A, B, F});
			result.push_back({A, F, E});
		}
	} else if (keepPointCount == 3) {
		PointTriangle testTriangle{A, B, C};
		if (!(BackCull(testTriangle) ^ reverse)) {
			result.reserve(1);
			result.push_back(testTriangle);
		}
	} else {
		//keepPointCount == 0 不需要绘制
	}
	return result;
}
