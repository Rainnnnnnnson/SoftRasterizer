#include "Shadow.h"
#include "RasterizationAlgorithm.h"
using std::pair;

//��Rasterizer ����Copy��������һ���
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
		��Ϊ������Ҫ���� �õ���Ӧ˳������
		ÿ�ν���һ�ν��� ���������ζ���˳ʱ����ʱ�뽫��ת��
		һ�������������Ҫ�������ζ���
		����4��������оٿɵ� !(BackCull(triangle) ^ reverse) ����ȷ���Ƿ���Ҫ����
		ʹ���¶��㹹����������ȷ���Ƿ����
		����˳ʱ�봫�� �ٸ���reverse�ۺ��ж�
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
	//���㱣���ĵ�
	auto keepPointCount = std::count_if(pointDistances.begin(), pointDistances.end(), [](const auto& data) {
		return data.second <= 0.0f;
	});
	//��Ҫʹ�ö�����������ú�Ĳ�ֵ
	Point4 A = pointDistances[0].first;
	Point4 B = pointDistances[1].first;
	Point4 C = pointDistances[2].first;
	vector<PointTriangle> result;
	if (keepPointCount == 1) {
		/*
					   *A
					   |\   F
		  _____________|_\�L______________��ƽ��
					E�J|  \
					   |  /�IB
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
		//������߼����������Ʋ����ظ�ע��
		/*
					   * A
					   |\
					   | \
					   |  \ B
			___________|__/_______________
					 �J| /�I
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
		//keepPointCount == 0 ����Ҫ����
	}
	return result;
}
