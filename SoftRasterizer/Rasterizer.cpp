#include "Rasterizer.h"

/*
	ʹ��pair array tuple ���´����Ѷ�
	����ʹ��using������
	.cpp�ļ�����Ҫ����һ��
*/
using VertexData = tuple<Point2, Vector3, Color>;
using Vertex = pair<Point4, VertexData>;
using VertexTriangle = array<Vertex, 3>;
using PointTriangle = array<Point4, 3>;
using GravityCoefficient = array<float, 3>;
using ScreenTriangle = array<Point2, 3>;
using ScreenLine = array<Point2, 2>;

Rasterizer::Rasterizer(unsigned width, unsigned height)
	: zBuffer(width,height) {
	Clear();
}

float Rasterizer::GetAspectRatio() const {
	return static_cast<float>(zBuffer.GetWidth()) / static_cast<float>(zBuffer.GetHeight());
}

void Rasterizer::Clear() {
	/*
		�����״̬��ȴ���Ϊ2.0f
		�����ĻʱΪ��ɫ
	*/
	constexpr float clearDepth = 2.0f;
	constexpr Color black = {0.0f, 0.0f, 0.0f};
	for (unsigned y = 0; y < zBuffer.GetHeight(); y++) {
		for (unsigned x = 0; x < zBuffer.GetWidth(); x++) {
			zBuffer.SetImagePoint(x, y, {clearDepth, black});
		}
	}
}

RGBImage Rasterizer::GenerateRGBImage() const {
	unsigned width = zBuffer.GetWidth();
	unsigned height = zBuffer.GetHeight();
	RGBImage image(width, height);
	for (unsigned y = 0; y < height; y++) {
		for (unsigned x = 0; x < width; x++) {
			Color color = zBuffer.GetImagePoint(x, y).second;
			image.SetImagePoint(x, y, color);
		}
	}
	return image;
}

void Rasterizer::DrawZBuffer(unsigned x, unsigned y, float depth, Color color) {
	assertion(x < zBuffer.GetWidth() && y < zBuffer.GetHeight());
	assertion(depth >= 0.0f && depth <= 1.0f);
	float zBufferDepth = zBuffer.GetScreenPoint(x, y).first;
	if (depth < zBufferDepth) {
		zBuffer.SetScreenPoint(x, y, {depth, color});
	}
}

void Rasterizer::DrawWritePixel(unsigned x, unsigned y) {
	constexpr float depth = -1.0f;
	constexpr Color write = {1.0f, 1.0f, 1.0f};
	zBuffer.SetScreenPoint(x, y, {depth, write});
}

Point4 Rasterizer::CalculatePlanePoint(Vector4 plane, Point4 p0, Point4 p1) const {
	float t = CalculatePlaneInterpolationCoefficient(plane, p0, p1);
	return p0 * (1.0f - t) + p1 * t;
}

Vertex Rasterizer::CalculateInterpolationVertex(float t, const Vertex& p0, const Vertex& p1) const {
	Point4 A = p0.first;
	Point4 B = p1.first;
	VertexData dataA = p0.second;
	VertexData dataB = p1.second;
	Point4 point = A * (1.0f - t) + (B * t);
	Point2 coordinate = get<0>(dataA) * (1.0f - t) + get<0>(dataB) * t;
	Vector3 normal = get<1>(dataA) * (1.0f - t) + get<1>(dataB) * t;
	Color color = get<2>(dataA) * (1.0f - t) + get<2>(dataB) * t;
	return{point, {coordinate, normal, color}};
}

VertexData Rasterizer::CalculateVertexData(const GravityCoefficient& coefficients,
										   const VertexTriangle& vertexs) const {
	array<Point4, 3> points;
	array<Point2, 3> coodinates;
	array<Vector3, 3> normals;
	array<Color, 3> colors;
	for (int i = 0; i < 3; i++) {
		points[i] = vertexs[i].first;
		coodinates[i] = get<0>(vertexs[i].second);
		normals[i] = get<1>(vertexs[i].second);
		colors[i] = get<2>(vertexs[i].second);
	}
	float screenOne = coefficients[0] / points[0].w 
		+ coefficients[1] / points[1].w 
		+ coefficients[2] / points[2].w;
	//����
	float screenU = coefficients[0] * (coodinates[0].x / points[0].w)
		+ coefficients[1] * (coodinates[1].x / points[1].w)
		+ coefficients[2] * (coodinates[2].x / points[2].w);
	float screenV = coefficients[0] * (coodinates[0].y / points[0].w)
		+ coefficients[1] * (coodinates[1].y / points[1].w)
		+ coefficients[2] * (coodinates[2].y / points[2].w);
	//����
	float screenX = coefficients[0] * (normals[0].x / points[0].w)
		+ coefficients[1] * (normals[1].x / points[1].w)
		+ coefficients[2] * (normals[2].x / points[2].w);
	float screenY = coefficients[0] * (normals[0].y / points[0].w)
		+ coefficients[1] * (normals[1].y / points[1].w)
		+ coefficients[2] * (normals[2].y / points[2].w);
	float screenZ = coefficients[0] * (normals[0].z / points[0].w)
		+ coefficients[1] * (normals[1].z / points[1].w)
		+ coefficients[2] * (normals[2].z / points[2].w);
	//��ɫ
	float screenR = coefficients[0] * (colors[0].r / points[0].w)
		+ coefficients[1] * (colors[1].r / points[1].w)
		+ coefficients[2] * (colors[2].r / points[2].w);
	float screenG = coefficients[0] * (colors[0].g / points[0].w)
		+ coefficients[1] * (colors[1].g / points[1].w)
		+ coefficients[2] * (colors[2].g / points[2].w);
	float screenB = coefficients[0] * (colors[0].b / points[0].w)
		+ coefficients[1] * (colors[1].b / points[1].w)
		+ coefficients[2] * (colors[2].b / points[2].w);
	Point2 coodinate{screenU / screenOne, screenV / screenOne};
	Vector3 normal{screenX / screenOne, screenY / screenOne, screenZ / screenOne};
	Color color{screenR / screenOne, screenG / screenOne, screenB / screenOne};
	return {coodinate, normal, color};
}

vector<VertexTriangle> Rasterizer::TriangleNearPlaneClipAndBackCull(const VertexTriangle& vertexs) const {
	constexpr auto nearPlane = Vector4{0.0f, 0.0f, -1.0f, 0.0f};

	array<pair<Vertex, float>, 3> vertexDistances;
	for (int i = 0; i < 3; i++) {
		vertexDistances[i].first = vertexs[i];
		vertexDistances[i].second = CalculatePlaneDistance(nearPlane, vertexs[i].first);
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
	if (vertexDistances[2].second < vertexDistances[1].second) {
		std::swap(vertexDistances[2], vertexDistances[1]);
		reverse = !reverse;
	}
	if (vertexDistances[1].second < vertexDistances[0].second) {
		std::swap(vertexDistances[1], vertexDistances[0]);
		reverse = !reverse;
	}
	if (vertexDistances[2].second < vertexDistances[1].second) {
		std::swap(vertexDistances[2], vertexDistances[1]);
		reverse = !reverse;
	}
	//���㱣���ĵ�
	auto keepPointCount = std::count_if(vertexDistances.begin(), vertexDistances.end(), [](const auto& data) {
		return data.second <= 0.0f;
	});
	//��Ҫʹ�ö�����������ú�Ĳ�ֵ
	Point4 A = vertexDistances[0].first.first;
	Point4 B = vertexDistances[1].first.first;
	Point4 C = vertexDistances[2].first.first;
	const Vertex& vertexA = vertexDistances[0].first;
	const Vertex& vertexB = vertexDistances[1].first;
	const Vertex& vertexC = vertexDistances[2].first;
	vector<VertexTriangle> result;
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
			auto vertexE = CalculateInterpolationVertex(tAEC, vertexA, vertexC);
			auto vertexF = CalculateInterpolationVertex(tAFB, vertexA, vertexB);
			result.reserve(1);
			result.push_back({vertexA, vertexF, vertexE});
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
			auto vertexE = CalculateInterpolationVertex(tAEC, vertexA, vertexC);
			auto vertexF = CalculateInterpolationVertex(tBFC, vertexB, vertexC);
			result.reserve(2);
			result.push_back({vertexA, vertexB, vertexF});
			result.push_back({vertexA, vertexF, vertexE});
		}
	} else if (keepPointCount == 3) {
		PointTriangle testTriangle{A, B, C};
		if (!(BackCull(testTriangle) ^ reverse)) {
			result.reserve(1);
			result.push_back({vertexA, vertexB, vertexC});
		}
	} else {
		//keepPointCount == 0 ����Ҫ����
	}
	return result;
}

vector<ScreenLine> Rasterizer::WireframeNearFarPlaneClipAndGetLines(const PointTriangle & points) const {
	constexpr auto nearPlane = Vector4{0.0f, 0.0f, -1.0f, 0.0f};
	constexpr auto farPlane = Vector4{0.0f, 0.0f, 1.0f, -1.0f};
	array<pair<Point4, float>, 3> pointDistances;
	for (int i = 0; i < 3; i++) {
		pointDistances[i].first = points[i];
		pointDistances[i].second = CalculatePlaneDistance(nearPlane, points[i]);
	}
	//���ﲻ��Ҫ�����޳� ֱ������
	std::sort(pointDistances.begin(), pointDistances.end(), [](auto pointDistancesA, auto pointDistancesB) {
		return pointDistancesA.second < pointDistancesB.second;
	});
	auto keepPointCount = std::count_if(pointDistances.begin(), pointDistances.end(), [](const auto& data) {
		return data.second <= 0.0f;
	});
	Point4 A = pointDistances[0].first;
	Point4 B = pointDistances[1].first;
	Point4 C = pointDistances[2].first;
	vector<ScreenLine> result;
	if (keepPointCount == 1) {
		/*
		 _________________________________Զƽ��(����Ҫ����)
					   *A
					   |\   H
		  _____________|_\�L______________Զƽ��(��Ҫ����)
					G�J|  \   F
		 ______________|___\�L______________��ƽ��
					E�J|    \
					   |    /�IB
					   |   /
					   |  /
					   | /
					   |/
					   *C
		*/
		Point4 E = CalculatePlanePoint(nearPlane, A, C);
		Point4 F = CalculatePlanePoint(nearPlane, A, B);
		bool needClip = CalculatePlaneDistance(farPlane, A) > 0.0f;
		if (needClip) {
			Point4 G = CalculatePlanePoint(farPlane, A, C);
			Point4 H = CalculatePlanePoint(farPlane, A, B);
			Point2 screenG = ConvertToScreenPoint(G);
			Point2 screenH = ConvertToScreenPoint(H);
			Point2 screenF = ConvertToScreenPoint(F);
			Point2 screenE = ConvertToScreenPoint(E);
			result.reserve(4);
			result.push_back({screenG, screenH});
			result.push_back({screenH, screenF});
			result.push_back({screenF, screenE});
			result.push_back({screenE, screenG});
		} else {
			Point2 screenA = ConvertToScreenPoint(A);
			Point2 screenE = ConvertToScreenPoint(E);
			Point2 screenF = ConvertToScreenPoint(F);
			result.reserve(3);
			result.push_back({screenA, screenF});
			result.push_back({screenF, screenE});
			result.push_back({screenE, screenA});
		}
	} else if (keepPointCount == 2) {
		/*
			____________________________________Զƽ��(���1) (����AB�պ�ͬʱ��ƽ������)

		               *A
		               |\
                    G  | \   H
            _________�K|__\�L___________________Զƽ��(���2)
					   | �� \
			           | I  \   B
		            G  |  I  \�L
		    _________�K|__��__/__________________Զƽ��(���3)
		               |    / �IH
			___________|___/_____________________��ƽ��
			       	 �J|  /�I
			        E  | /   F
					   |/
					   *C
			��ƽ����ú��������������� ��Ҫ���������ι�ͬ�ı�
			ֱ��AF��ͼ�һ������� ��Զƽ�潻��I
		*/
		Point4 E = CalculatePlanePoint(nearPlane, A, C);
		Point4 F = CalculatePlanePoint(nearPlane, B, C);
		bool case1 = CalculatePlaneDistance(farPlane, A) <= 0.0f;
		bool case2 = CalculatePlaneDistance(farPlane, B) < 0.0f;
		if (case1) {
			Point2 screenA = ConvertToScreenPoint(A);
			Point2 screenB = ConvertToScreenPoint(B);
			Point2 screenF = ConvertToScreenPoint(F);
			Point2 screenE = ConvertToScreenPoint(E);
			result.reserve(5);
			result.push_back({screenA, screenB});
			result.push_back({screenB, screenF});
			result.push_back({screenF, screenE});
			result.push_back({screenE, screenA});

			result.push_back({screenF, screenA});
		} else if(case2) {
			Point4 G = CalculatePlanePoint(farPlane, A, C);
			Point4 H = CalculatePlanePoint(farPlane, A, B);
			Point2 screenG = ConvertToScreenPoint(G);
			Point2 screenH = ConvertToScreenPoint(H);
			Point2 screenB = ConvertToScreenPoint(B);
			Point2 screenF = ConvertToScreenPoint(F);
			Point2 screenE = ConvertToScreenPoint(E);
			result.reserve(6);
			result.push_back({screenG, screenH});
			result.push_back({screenH, screenB});
			result.push_back({screenB, screenF});
			result.push_back({screenF, screenE});
			result.push_back({screenE, screenG});
			Point4 I = CalculatePlanePoint(farPlane, A, F);
			Point2 screenI = ConvertToScreenPoint(I);
			result.push_back({screenI, screenF});
		} else {
			Point4 G = CalculatePlanePoint(farPlane, A, C);
			Point4 H = CalculatePlanePoint(farPlane, B, C);
			Point2 screenG = ConvertToScreenPoint(G);
			Point2 screenH = ConvertToScreenPoint(H);
			Point2 screenF = ConvertToScreenPoint(F);
			Point2 screenE = ConvertToScreenPoint(E);
			result.reserve(5);
			result.push_back({screenG, screenH});
			result.push_back({screenH, screenF});
			result.push_back({screenF, screenE});
			result.push_back({screenE, screenG});
			Point4 I = CalculatePlanePoint(farPlane, A, F);
			Point2 screenI = ConvertToScreenPoint(I);
			result.push_back({screenI, screenF});
		}
	} else if (keepPointCount == 3) {

		/* 
		    ________________________________ Զƽ��(���1) (���� AB ��ƽ���� �� ABC ��ƽ���ϵ����)
		               *A
			        E  |\   F
		    _________�K|_\�L_________________ Զƽ��(���2)
		               |  \
			           |   \B
			___________|  /_________________ Զƽ��(���3)
			         �J| /�I
					E  |/  F
					   *C
            ________________________________ Զƽ��(���4) (���� BC ��ƽ���ϵ����)

			________________________________ ��ƽ��
		*/
		bool case1 = CalculatePlaneDistance(farPlane, A) <= 0.0f;
		bool case2 = CalculatePlaneDistance(farPlane, B) < 0.0f;
		bool case3 = CalculatePlaneDistance(farPlane, C) < 0.0f;
		if (case1) {
			Point2 screenA = ConvertToScreenPoint(A);
			Point2 screenB = ConvertToScreenPoint(B);
			Point2 screenC = ConvertToScreenPoint(C);
			result.reserve(3);
			result.push_back({screenA, screenB});
			result.push_back({screenB, screenC});
			result.push_back({screenC, screenA});
		} else if(case2) {
			Point4 E = CalculatePlanePoint(farPlane, A, C);
			Point4 F = CalculatePlanePoint(farPlane, A, B);
			Point2 screenE = ConvertToScreenPoint(E);
			Point2 screenF = ConvertToScreenPoint(F);
			Point2 screenB = ConvertToScreenPoint(B);
			Point2 screenC = ConvertToScreenPoint(C);
			result.reserve(4);
			result.push_back({screenE, screenF});
			result.push_back({screenF, screenB});
			result.push_back({screenB, screenC});
			result.push_back({screenC, screenE});
		} else if(case3) {
			Point4 E = CalculatePlanePoint(farPlane, A, C);
			Point4 F = CalculatePlanePoint(farPlane, B, C);
			Point2 screenE = ConvertToScreenPoint(E);
			Point2 screenF = ConvertToScreenPoint(F);
			Point2 screenC = ConvertToScreenPoint(C);
			result.reserve(3);
			result.push_back({screenE, screenF});
			result.push_back({screenF, screenC});
			result.push_back({screenC, screenE});
		} else {
			//������
		}
	} else {
		//pointCount == 0 ����Ҫ����
	}
	return result;
}

bool Rasterizer::LineClip(ScreenLine& points) const {
	//�߽�
	constexpr float xMin = -1.0f;
	constexpr float xMax = 1.0f;
	constexpr float yMin = -1.0f;
	constexpr float yMax = 1.0F;
	//ʹ��liang-barsky�㷨
	float x0 = points[0].x;
	float y0 = points[0].y;
	float x1 = points[1].x;
	float y1 = points[1].y;

	float deltaX = x1 - x0;
	float deltaY = y1 - y0;

	float p1 = -deltaX;
	float p2 = deltaX;
	float p3 = -deltaY;
	float p4 = deltaY;

	float q1 = x0 - xMin;
	float q2 = xMax - x0;
	float q3 = y0 - yMin;
	float q4 = yMax - y0;

	//ƽ�в����ڿ�����
	if ((deltaX == 0.0f && (q1 < 0.0f || q2 < 0.0f)) ||
		(deltaY == 0.0f && (q3 < 0.0f || q4 < 0.0f))) {
		return false;
	}

	//������������t (x0 + t��x)
	//����ȡ��ԭ����λ�û��߽߱�
	array<float, 3> t0Array{0.0f, 0.0f, 0.0f};//ȡ���
	array<float, 3> t1Array{1.0f, 1.0f, 1.0f};//ȡ��С

	if (deltaX != 0.0f) {
		float u1 = q1 / p1;
		float u2 = q2 / p2;
		if (deltaX > 0.0f) {
			t0Array[1] = u1;
			t1Array[1] = u2;
		} else {
			t0Array[1] = u2;
			t1Array[1] = u1;
		}
	}

	if (deltaY != 0.0f) {
		float u3 = q3 / p3;
		float u4 = q4 / p4;
		if (deltaY > 0.0f) {
			t0Array[2] = u3;
			t1Array[2] = u4;
		} else {
			t0Array[2] = u4;
			t1Array[2] = u3;
		}
	}

	float t0 = *std::max_element(t0Array.begin(), t0Array.end());
	float t1 = *std::min_element(t1Array.begin(), t1Array.end());

	//������˵���ڿ���
	if (t0 >= t1) {
		return false;
	}

	float newX0 = x0 + t0 * deltaX;
	float newY0 = y0 + t0 * deltaY;
	float newX1 = x0 + t1 * deltaX;
	float newY1 = y0 + t1 * deltaY;

	points[0] = {newX0, newY0};
	points[1] = {newX1, newY1};
	return true;
}

void Rasterizer::DrawLine(const ScreenLine& points) {
	Point2 A = points[0];
	Point2 B = points[1];
	size_t width = zBuffer.GetWidth();
	size_t height = zBuffer.GetHeight();
	/*
		�������ͼƬ������Ҫ����ΪͼƬ�����ᵼ��k > 1 ���� k < -1�����
		�е��㷨һ��ֻ�����������½�һ������ ��K > 1ʱ ֻ��ȡK == 1 (K<-1 ͬ��)
	*/
	float y = (points[1].y - points[0].y) * static_cast<float>(height);
	float x = (points[1].x - points[0].x) * static_cast<float>(width);
	float k = y / x;
	/*
		k < -1 || k >= 1 ����� ���Կ��� y = x ��Գ�
		����ֻ��Ҫ��ת�����ͼ�������Ƽ���
	*/
	size_t drawWidth = width;
	size_t drawHeight = height;
	bool reverse;
	if (k >= -1.0f && k < 1.0f) {
		reverse = false;
	} else if (k < -1.0f || k >= 1.0f) {
		std::swap(A.x, A.y);
		std::swap(B.x, B.y);
		std::swap(drawWidth, drawHeight);
		reverse = true;
	} else {
		//�����غ� k = NaN ������
		return;
	}
	/*
		���￪ʼ�е��㷨 ֻ��Ҫ���� 0 < k < 1 ���������
		-1 < k < 0 ʹ����������������
	*/
	if (B.x < A.x) {
		std::swap(B, A);
	}
	/*
		����ֱ�ӽ����������ĵ� ת�����������ĵ�
		����ʡ�ܶ���� ���Ӵ���ɶ���
		��������������������������1����
	*/
	size_t xMin = ScreenCoordinateToPixelPoint(A.x, drawWidth);
	size_t xMax = ScreenCoordinateToPixelPoint(B.x, drawWidth);
	size_t yMin = ScreenCoordinateToPixelPoint(A.y, drawHeight);
	size_t yMax = ScreenCoordinateToPixelPoint(B.y, drawHeight);
	Point2 pointA{ScreenPixelPointToCoordinate(xMin, drawWidth), ScreenPixelPointToCoordinate(yMin, drawHeight)};
	Point2 pointB{ScreenPixelPointToCoordinate(xMax, drawWidth), ScreenPixelPointToCoordinate(yMax, drawHeight)};

	/*
		ʹ����Kֵ������
		addtion ���� �������߻��������� ��һ��һ�����
	*/
	const float newk = (pointB.y - pointA.y) / (pointB.x - pointA.x);
	const float addtion = newk >= 0.0f ? 1.0f : -1.0f;
	/*
	    һ����������Ļ�ĸ߶� 
	*/
	const float screenPixelHeight = 2.0f / static_cast<float>(drawHeight - 1);
	const float halfScreenPixelHeight = 0.5f * screenPixelHeight;

	float middleY = pointA.y + addtion * halfScreenPixelHeight;

	for (size_t pixelX = xMin; pixelX <= xMax; pixelX++) {
		float x = ScreenPixelPointToCoordinate(pixelX, drawWidth);
		auto middlePoint = Point2{x, middleY};
		//ȡ[0,1)��Ϊ���� ��ʾ��ֱ���Ƿ����Ϸ�
		bool pointUpLine = (addtion * CalculateLineEquation(middlePoint, pointA, pointB)) >= 0.0f;
		float y;
		//ȡ[0,1)��Ϊ���� �е���ֱ���Ϸ������е��·����� ��֮�·�����
		if (pointUpLine) {
			y = middleY - addtion * halfScreenPixelHeight;
		} else {
			y = middleY + addtion * halfScreenPixelHeight;
		}
		size_t pixelY = ScreenCoordinateToPixelPoint(y, drawHeight);
		//����Ҳ��Ҫ��ת����
		if (reverse) {
			DrawWritePixel(pixelY, pixelX);
		} else {
			DrawWritePixel(pixelX, pixelY);
		}
		//ȡ[0,1)��Ϊ���� �е���ֱ���·� ��Ҫ����е�һ����λ
		if (!pointUpLine) {
			middleY += addtion * screenPixelHeight;
		}
	}
}
