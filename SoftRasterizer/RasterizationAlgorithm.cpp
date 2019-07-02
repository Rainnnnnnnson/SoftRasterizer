#include "RasterizationAlgorithm.h"

using std::pair;

Point2 ConvertToScreenPoint(Point4 p) {
	return {p.x / p.w, p.y / p.w};
}

float ScreenPixelPointToCoordinate(unsigned pixel, unsigned pixelCount) {
	float coordinate = static_cast<float>(pixel) / static_cast<float>(pixelCount - 1);
	float normalizeCoordinate = coordinate * 2.0f - 1.0f;
	return normalizeCoordinate;
}

int ScreenCoordinateToPixelPoint(float coordinate, unsigned pixelCount) {
	float normalizeCoordinate = (coordinate + 1.0f) * 0.5f;
	return static_cast<int>(round(normalizeCoordinate * static_cast<float>(pixelCount - 1)));
}

float CalculatePlaneInterpolationCoefficient(Vector4 plane, Point4 p0, Point4 p1) {
	/*
		Ax + By + Cz + Dw = 0
		plane(A, B, C, D)
		P = (x, y, z, w) = A + t(B - A)
		plane[A + t(B - A)] = 0
	*/
	Point4 A = p0;
	Point4 B = p1;
	Vector4 AB = B - A;
	float t = -(plane.Dot(A.GetVector4())) / (plane.Dot(AB));
	return t;
}

Point4 CalculatePlanePoint(Vector4 plane, Point4 p0, Point4 p1) {
	float t = CalculatePlaneInterpolationCoefficient(plane, p0, p1);
	return p0 * (1.0f - t) + p1 * t;
}

float CalculatePlaneDistance(Vector4 plane, Point4 point) {
	return plane.Dot(point.GetVector4());
}

float CalculateLineEquation(Point2 p, Point2 p0, Point2 p1) {
	return (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y + p0.x * p1.y - p1.x * p0.y;
}

bool ViewVolumnCull(const PointTriangle& points) {
	constexpr auto left = Vector4{-1.0f, 0.0f, 0.0f, -1.0f};
	constexpr auto right = Vector4{1.0f, 0.0f, 0.0f, -1.0f};
	constexpr auto bottom = Vector4{0.0f, -1.0f, 0.0f, -1.0f};
	constexpr auto top = Vector4{0.0f, 1.0f, 0.0f, -1.0f};
	constexpr auto near = Vector4{0.0f, 0.0f, -1.0f, 0.0f};
	constexpr auto far = Vector4{0.0f, 0.0f, 1.0f, -1.0f};
	bool outLeft = std::all_of(points.begin(), points.end(), [&](auto point) {
		return CalculatePlaneDistance(left, point) > 0.0f;
	});
	bool outRight = std::all_of(points.begin(), points.end(), [&](auto point) {
		return CalculatePlaneDistance(right, point) > 0.0f;
	});
	bool outBottom = std::all_of(points.begin(), points.end(), [&](auto point) {
		return CalculatePlaneDistance(bottom, point) > 0.0f;
	});
	bool outTop = std::all_of(points.begin(), points.end(), [&](auto point) {
		return CalculatePlaneDistance(top, point) > 0.0f;
	});
	bool outNear = std::all_of(points.begin(), points.end(), [&](auto point) {
		return CalculatePlaneDistance(near, point) > 0.0f;
	});
	bool outFar = std::all_of(points.begin(), points.end(), [&](auto point) {
		return CalculatePlaneDistance(far, point) > 0.0f;
	});
	//满足任意一种情况可以直接消除
	if (outLeft || outRight || outBottom || outTop || outNear || outFar) {
		return true;
	}
	return false;
}

bool BackCull(const PointTriangle& points) {
	constexpr Vector3 z{0.0f, 0.0f, 1.0f};
	Point3 A = points[0].ToPoint3();
	Point3 B = points[1].ToPoint3();
	Point3 C = points[2].ToPoint3();
	Vector3 BA = B - A;
	Vector3 CB = C - B;
	Vector3 cross = BA.Cross(CB);
	bool cull = cross.Dot(z) >= 0.0f;
	return cull;
}

Point4 CalculatePointByCoefficient(const GravityCoefficient& coefficients, const PointTriangle& points) {
	return points[0] * coefficients[0] + points[1] * coefficients[1] + points[2] * coefficients[2];
}

void TriangleRasterization(unsigned w, unsigned h, 
						   const PointTriangle& points, 
						   const UseScreenPoint& useCoefficient) {
	const int width = static_cast<int>(w);
	const int height = static_cast<int>(h);
	//获取三角形中顶点最大最小的x y值
	//用于计算需要绘制的边框
	array<float, 3> xValue;
	array<float, 3> yValue;
	for (int i = 0; i < 3; i++) {
		xValue[i] = ConvertToScreenPoint(points[i]).x;
		yValue[i] = ConvertToScreenPoint(points[i]).y;
	}
	std::sort(xValue.begin(), xValue.end(), std::less<float>());
	std::sort(yValue.begin(), yValue.end(), std::less<float>());
	Point2 A = ConvertToScreenPoint(points[0]);
	Point2 B = ConvertToScreenPoint(points[1]);
	Point2 C = ConvertToScreenPoint(points[2]);
	float fa = CalculateLineEquation(A, B, C);
	float fb = CalculateLineEquation(B, C, A);
	float fc = CalculateLineEquation(C, A, B);
	//确定需要绘制的边界
	size_t xMax = std::min(ScreenCoordinateToPixelPoint(xValue[2], width), width - 1);
	size_t xMin = std::max(ScreenCoordinateToPixelPoint(xValue[0], width), 0);
	size_t yMax = std::min(ScreenCoordinateToPixelPoint(yValue[2], height), height - 1);
	size_t yMin = std::max(ScreenCoordinateToPixelPoint(yValue[0], height), 0);
	//循环限定矩形 [xMin,xMax] * [yMin,yMax]
	for (size_t yIndex = yMin; yIndex <= yMax; yIndex++) {
		for (size_t xIndex = xMin; xIndex <= xMax; xIndex++) {
			//每一个需要绘制的屏幕上的点 根据这个点计算重心
			auto screenPoint = Point2{
				ScreenPixelPointToCoordinate(xIndex, width),
				ScreenPixelPointToCoordinate(yIndex, height)
			};
			float alpha = CalculateLineEquation(screenPoint, B, C) / fa;
			float beta = CalculateLineEquation(screenPoint, C, A) / fb;
			float gamma = CalculateLineEquation(screenPoint, A, B) / fc;
			//判断是否未超出远平面
			auto coefficients = GravityCoefficient{alpha, beta, gamma};
			Point4 point = CalculatePointByCoefficient(coefficients, points);
			float depth = point.ToPoint3().z;
			if (depth >= 0.0f && depth <= 1.0f) {
				//判断是否在三角形内部
				bool inTriangle = std::all_of(coefficients.begin(), coefficients.end(), [](float f) {
					return f >= 0.0f;
				});
				if (inTriangle) {
					useCoefficient(xIndex, yIndex, depth, coefficients);
				}
			}
		}
	}
}
