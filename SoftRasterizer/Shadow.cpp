#include "Shadow.h"
#include "RasterizationAlgorithm.h"

//��Rasterizer ����Copy��������һ���
vector<PointTriangle> NearPlanClipAndBackCull(const PointTriangle& points);

ShadowBuilder::ShadowBuilder(unsigned width, unsigned height) : depths(width, height) {
	Clear();
}

float ShadowBuilder::GetAspectRatio() const {
	return static_cast<float>(depths.GetWidth()) / static_cast<float>(depths.GetHeight());
}

void ShadowBuilder::Clear() {
	for (unsigned y = 0; y < depths.GetHeight(); y++) {
		for (unsigned x = 0; x < depths.GetWidth(); x++) {
			depths.SetImagePoint(x, y, 1.0f);
		}
	}
}

ShadowTexture ShadowBuilder::Generate() const {
	ShadowTexture result(depths.GetWidth(), depths.GetHeight());
	for (unsigned y = 0; y < depths.GetHeight(); y++) {
		for (unsigned x = 0; x < depths.GetWidth(); x++) {
			float depth = depths.GetImagePoint(x, y);
			result.SetImagePoint(x, y, depth);
		}
	}
	return result;
}

void ShadowBuilder::DrawDepth(const vector<Point3>& points,
							  const vector<array<unsigned, 3>> & indexs,
							  function<Point4(Point4)> transfrom) {
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
			TriangleRasterization(depths.GetWidth(), depths.GetHeight(), clipTrianglePoint,
								  [&](unsigned x, unsigned y, float depth, const GravityCoefficient&) {
				if (depth < depths.GetScreenPoint(x, y)) {
					depths.SetScreenPoint(x, y, depth);
				}
			});
		}
	}
}

vector<PointTriangle> NearPlanClipAndBackCull(const PointTriangle& points) {
	constexpr auto nearPlane = Vector4{0.0f, 0.0f, -1.0f, 0.0f};

	array<std::pair<Point4, float>, 3> pointDistances;
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

bool Illuminated(const ShadowTexture& shadow, Point4 point4) {
	Point3 point = point4.ToPoint3();
	//���ڼ��ÿռ���ֱ�Ӳ��ɼ�
	if (point.x < -1.0f || point.x > 1.0f ||
		point.y < -1.0f || point.y > 1.0f ||
		point.z < 0.0f || point.z > 1.0f) {
		return false;
	}
	unsigned x = static_cast<unsigned>(ScreenCoordinateToPixelPoint(point.x, shadow.GetWidth()));
	unsigned y = static_cast<unsigned>(ScreenCoordinateToPixelPoint(point.y, shadow.GetHeight()));
	if (point.z <= shadow.GetScreenPoint(x, y)) {
		return true;
	}
	return false;
}
