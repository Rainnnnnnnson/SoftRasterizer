#include "Rasterizer.h"
//�����״̬��ȴ���Ϊ2.0f
constexpr float clearDepth = 2.0f;
//�����ĻʱΪ��ɫ
constexpr Color black = {0.0f, 0.0f, 0.0f};

Rasterizer::Rasterizer(int width, int height)
	: width(width), height(height), 
	zBuffer(static_cast<size_t>(width)* height, {clearDepth, black}) {
	assert(width > 0);
	assert(height > 0);
}

void Rasterizer::Clear() {
	for (auto& buffer : zBuffer) {
		buffer.first = clearDepth;
		buffer.second = black;
	}
}

RGBImage Rasterizer::GenerateRGBImage() const {
	RGBImage image(width, height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = XYInPixel(x, y);
			auto zBufferColor = zBuffer[index];
			image.SetPixel(x, y, zBufferColor.second);
		}
	}
	return image;
}

int Rasterizer::PixelToIndex(int x, int y) {
	return y * width + x;
}

int Rasterizer::ReversePixelToIndex(int x, int y) {
	int reverseY = height - y - 1;
	return reverseY * width + x;
}

float Rasterizer::XPixelToScreen(int x) const {
	float delta = 1.0f / width;
	float start = -1.0f + 0.5f * delta;
	float addtion = static_cast<float>(x) * delta;
	return start + addtion;
}

float Rasterizer::YPixelToScreen(int y) const {
	float delta = 1.0f / height;
	float start = -1.0f + 0.5f * delta;
	float addtion = static_cast<float>(y) * delta;
	return start + addtion;
}

int Rasterizer::XScreenToPixel(float x) const {
	//��1.0f��ʱ��Ӧ�ñ�ӳ��������,�����������Խ��
	if (x == 1.0f) {
		return width - 1;
	}
	return static_cast<int>(floor(((x + 1.0f) / 2.0f) * static_cast<float>(width)));
}

int Rasterizer::YScreenToPixel(float y) const {
	//��1.0f��ʱ��Ӧ�ñ�ӳ��������,�����������Խ��
	if (y == 1.0f) {
		return height - 1;
	}
	return static_cast<int>(floor(((y + 1.0f) / 2.0f) * static_cast<float>(height)));
}

bool Rasterizer::XYInPixel(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}

bool Rasterizer::XYInScreen(float x, float y) const {
	return x >= -1.0f && x <= 1.0f && y >= -1.0f && y <= 1.0f;
}

bool Rasterizer::ZInViewVolumn(float z) const {
	return z >= 0.0f && z <= 1.0f;
}

vector<array<pair<Point4, tuple<Point2, Vector3, Color>>, 3>> Rasterizer::TriangleNearPlaneClipAndBackCull(
	const array<pair<Point4, tuple<Point2, Vector3, Color>>, 3> & triangleData) {
	//��ƽ�� ����������ƽ�� 
	constexpr auto nearPlane = Vector4{0.0f, 0.0f, -1.0f, 0.0f};
	array<pair<pair<Point4, tuple<Point2, Vector3, Color>>, float>, 3> vertexs;
	for (int i = 0; i < 3; i++) {
		float distance = triangleData[i].first.GetVector4().Dot(nearPlane);
		vertexs[i] = {triangleData[i], distance};
	}
	/*
		��Ϊ������Ҫ���� �õ���Ӧ˳������
		ÿ�ν���һ�ν��� ���������ζ���˳ʱ����ʱ�뽫��ת��
		һ�������������Ҫ�������ζ���
		����4��������оٿɵ� !(BackCull(triangle) ^ reverse) ����ȷ���Ƿ���Ҫ����
	*/
	bool reverse = false;
	if (vertexs[2].second < vertexs[1].second) {
		std::swap(vertexs[2], vertexs[1]);
		reverse = !reverse;
	}
	if (vertexs[1].second < vertexs[0].second) {
		std::swap(vertexs[1], vertexs[0]);
		reverse = !reverse;
	}
	if (vertexs[2].second < vertexs[1].second) {
		std::swap(vertexs[2], vertexs[1]);
		reverse = !reverse;
	}
	//��ƽ���ݶ�ָ��Z�Ḻ���� �������Եõ���Ҫ������ĸ���
	auto vertexCount = std::count_if(vertexs.begin(), vertexs.end(), [](const auto& data) {
		return data.second <= 0.0f;
	});
	//��Ҫʹ�ö�����������ú�Ĳ�ֵ
	Point4 point0 = vertexs[0].first.first;
	Point4 point1 = vertexs[1].first.first;
	Point4 point2 = vertexs[2].first.first;
	vector<array<pair<Point4, tuple<Point2, Vector3, Color>>, 3>> result;
	/*
		�������������
		����������Ҫ����
		���ֲ���Ҫ����
	*/
	if (vertexCount == 1) {
		/*
					   *Point0
					   |\   newPoint2
		  _____________|_\�L______________
			newPoint1�J|  \
					   |  /Point1
					   | /
					   |/
					   *Point2
		*/
		Point4 newPoint1 = CalculatePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = CalculatePlanePoint(nearPlane, {point0, point1});
		/*
			ʹ���¶��㹹����������ȷ���Ƿ����
			����˳ʱ�봫�� �ٸ���reverse�ۺ��ж�
		*/
		array<Point2, 3> testTriangle{point0.ToPoint2(), newPoint2.ToPoint2(), newPoint1.ToPoint2()};
		if (!(BackCull(testTriangle) ^ reverse)) {
		    /*
				����������������Բ�ֵ
				��Ϊ�ǽ�ƽ����ñض�����ƽ�� ����Ҫȷ��ʹ��w��ֵ ����z��ֵ
				͸��ͶӰ����� ʹ��w��ֵ w = ����ԭʼz
				����ͶӰ����� ʹ��z��ֵ w = 1
				ʹ�� ƽ������Ķ��� w == 1 ͬʱ���� �ж��Ƿ�Ϊ����ͶӰ
		    */
			float t1;
			float t2;
			bool orthographic = (point0.w == 1.0f && point2.w == 1.0f);
			if (orthographic) {
				t1 = (newPoint1.z - point0.z) / (point2.z - point0.z);
				t2 = (newPoint2.z - point1.z) / (point2.z - point1.z);
			} else {
				t1 = (newPoint1.w - point0.w) / (point2.w - point0.w);
				t2 = (newPoint2.w - point1.w) / (point2.w - point1.w);
			}
			//���㶥�����ݲ�ֵ
			const auto& data0 = vertexs[0].first.second;
			const auto& data1 = vertexs[1].first.second;
			const auto& data2 = vertexs[2].first.second;
			Point2 newCoordinate1 = get<0>(data0) * (1.0f - t1) + get<0>(data2) * t1;
			Point2 newCoordinate2 = get<0>(data0) * (1.0f - t2) + get<0>(data1) * t2;
			Vector3 newNormal1 = get<1>(data0) * (1.0f - t1) + get<1>(data2) * t1;
			Vector3 newNormal2 = get<1>(data0) * (1.0f - t2) + get<1>(data1) * t2;
			Color newColor1 = get<2>(data0) * (1.0f - t1) + get<2>(data2) * t1;
			Color newColor2 = get<2>(data0) * (1.0f - t2) + get<2>(data1) * t2;
		    //ͨ������������������ǰ���˳ʱ�����
			const auto& vertex0 = vertexs[0].first;
			auto newVertex1 = 
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint1, {newCoordinate1, newNormal1, newColor1}};
			auto newVertex2 = 
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint2, {newCoordinate2, newNormal2, newColor2}};
			result.reserve(1);
			result.push_back({vertex0, newVertex2, newVertex1});
		}
	} else if (vertexCount == 2) {
		//������߼����������Ʋ����ظ�ע��
		/*
					   * Point0
					   |\
					   | \
					   |  \ Point1
			___________|__/_______________
					 �J| /�I
			newPoint1  |/   newPoint2
					   *Point2
		*/
		Point4 newPoint1 = CalculatePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = CalculatePlanePoint(nearPlane, {point1, point2});
		array<Point2, 3> testTriangle{point0.ToPoint2(), newPoint2.ToPoint2(), newPoint1.ToPoint2()};
		if (!(BackCull(testTriangle) ^ reverse)) {
			float t1;
			float t2;
			bool orthographic = (point0.w == 1.0f && point2.w == 1.0f);
			if (orthographic) {
				t1 = (newPoint1.z - point0.z) / (point2.z - point0.z);
				t2 = (newPoint2.z - point1.z) / (point2.z - point1.z);
			} else {
				t1 = (newPoint1.w - point0.w) / (point2.w - point0.w);
				t2 = (newPoint2.w - point1.w) / (point2.w - point1.w);
			}
			const auto& data0 = vertexs[0].first.second;
			const auto& data1 = vertexs[1].first.second;
			const auto& data2 = vertexs[2].first.second;
			Point2 newCoordinate1 = get<0>(data0) * (1.0f - t1) + get<0>(data2) * t1;
			Point2 newCoordinate2 = get<0>(data1) * (1.0f - t2) + get<0>(data2) * t2;
			Vector3 newNormal1 = get<1>(data0) * (1.0f - t1) + get<1>(data2) * t1;
			Vector3 newNormal2 = get<1>(data1) * (1.0f - t2) + get<1>(data2) * t2;
			Color newColor1 = get<2>(data0) * (1.0f - t1) + get<2>(data2) * t1;
			Color newColor2 = get<2>(data1) * (1.0f - t2) + get<2>(data2) * t2;
			const auto& vertex0 = vertexs[0].first;
			const auto& vertex1 = vertexs[1].first;
			auto newVertex1 =
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint1, {newCoordinate1, newNormal1, newColor1}};
			auto newVertex2 =
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint2, {newCoordinate2, newNormal2, newColor2}};
			result.reserve(2);
			result.push_back({vertex0, vertex1, newVertex2});
			result.push_back({vertex0, newVertex2, newVertex1});
		}
	} else if (vertexCount == 3) {
		//����Ҫ����
		array<Point2, 3> testTriangle{point0.ToPoint2(), point1.ToPoint2(), point2.ToPoint2()};
		if (!(BackCull(testTriangle) ^ reverse)) {
			result.reserve(1);
			result.push_back({vertexs[0].first, vertexs[1].first, vertexs[2].first});
		}
	} else {
		//pointCount == 0 ����Ҫ����
	}
	return result;
}

Point4 Rasterizer::CalculatePlanePoint(Vector4 N, const array<Point4, 2> & points) {
	/*
		��w��û�й�һ����ʱ��
		͸��ͶӰ������ͶӰ�������Ե�
		����������ƽ���ϵ��t
		������չ����ά ��������άһ��
		N[A + t(B - A)] = 0
		t = - (N * A) / (N * (B - A))
	*/
	Point4 A = points[0];
	Point4 B = points[1];
	Vector4 BA = B - A;
	float t = -(N.Dot(A.GetVector4())) / (N.Dot(BA));
	Point4 p = A + (BA * t);
	return p;
}

bool Rasterizer::BackCull(const array<Point2, 3> & points) {
	Point3 A{points[0].x, points[0].y, 0.0f};
	Point3 B{points[1].x, points[1].y, 0.0f};
	Point3 C{points[2].x, points[2].y, 0.0f};
	Vector3 BA = B - A;
	Vector3 CB = C - B;
	Vector3 cross = BA.Cross(CB);
	Vector3 z{0.0f, 0.0f, 1.0f};
	if ((cross.Dot(z)) < 0.0f) {
		return false;
	}
	return true;
}

void Rasterizer::TriangleRasterization(const array<Point2, 3> & points, 
									   const function<void(int, int, const array<float, 3>&)>& useCoefficient) {
	//��ȡ�������ж��������С��x yֵ
	//���ڼ�����Ҫ���Ƶı߿�
	array<float, 3> xValue;
	array<float, 3> yValue;
	for (int i = 0; i < 3; i++) {
		xValue[i] = points[i].x;
		yValue[i] = points[i].y;
	}
	std::sort(xValue.begin(), xValue.end(), std::less<float>());
	std::sort(yValue.begin(), yValue.end(), std::less<float>());
	//ȷ����Ҫ���Ƶı߽�
	int xMax = std::min(XScreenToPixel(xValue[2]), width - 1);
	int xMin = std::max(XScreenToPixel(xValue[0]), 0);
	int yMax = std::min(YScreenToPixel(yValue[2]), height - 1);
	int yMin = std::max(YScreenToPixel(yValue[0]), 0);
	Point2 A = points[0];
	Point2 B = points[1];
	Point2 C = points[2];
	float fa = CalculateLineEquation(A, B, C);
	float fb = CalculateLineEquation(B, C, A);
	float fc = CalculateLineEquation(C, A, B);
	//ѭ���޶����� [xMin,xMax] * [yMin,yMax]
	for (int yIndex = yMin; yIndex <= yMax; yIndex++) {
		for (int xIndex = xMin; xIndex <= xMax; xIndex++) {
			//ÿһ����Ҫ���Ƶ���Ļ�ϵĵ� ����������������
			Point2 point{XPixelToScreen(xIndex), YPixelToScreen(yIndex)};
			float alpha = CalculateLineEquation(point, B, C) / fa;
			float beta = CalculateLineEquation(point, C, A) / fb;
			float gamma = CalculateLineEquation(point, A, B) / fc;
			array<float, 3> coefficients{alpha, beta, gamma};
			//�ж��Ƿ����������ڲ�
			bool inTriangle = std::all_of(coefficients.begin(), coefficients.end(), [](float f) {
				return f > 0.0f;
			});
			if (inTriangle) {
				useCoefficient(xIndex, yIndex, coefficients);
			}
		}
	}
}

float Rasterizer::CalculateLineEquation(Point2 p, Point2 p0, Point2 p1) {
	return (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y + p0.x * p1.y - p1.x * p0.y;
}

pair<Point4, tuple<Point2, Vector3, Color>> Rasterizer::CaculateCoefficientData(
	const array<pair<Point4, tuple<Point2, Vector3, Color>>, 3> & triangleData, 
	const array<float, 3> & coefficients) {
	array<Point4, 3> points;
	array<Point2, 3> coodinates;
	array<Vector3, 3> normals;
	array<Color, 3> colors;
	for (int i = 0; i < 3; i++) {
		points[i] = triangleData[i].first;
		coodinates[i] = get<0>(triangleData[i].second);
		normals[i] = get<1>(triangleData[i].second);
		colors[i] = get<2>(triangleData[i].second);
	}
	float screenOne = coefficients[0] / points[0].w + coefficients[1] / points[1].w + coefficients[2] / points[2].w;
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
	//����
	Point4 point = points[0] * coefficients[0] + points[1] * coefficients[1] + points[2] * coefficients[2];
	Point2 coodinate{screenU / screenOne, screenV / screenOne};
	Vector3 normal{screenX / screenOne, screenY / screenOne, screenZ / screenOne};
	Color color{screenR / screenOne, screenG / screenOne, screenB / screenOne};
	return {point, {coodinate, normal, color}};
}

void Rasterizer::DrawZBuffer(int x, int y, float z, Color color) {
	assert(XYInPixel(x, y));
	assert(ZInViewVolumn(z));
	int index = ReversePixelToIndex(x, y);
	if (z < zBuffer[index].first) {
		zBuffer[index] = {z, color};
	}
}

vector<array<Point2, 2>> Rasterizer::WireframeNearFarPlaneClipAndGetLines(const array<Point4, 3> & points) {
	//��ƽ���Զƽ�� ������ʾƽ�� 
	constexpr auto nearPlane = Vector4{0.0f, 0.0f, -1.0f, 0.0f};
	constexpr auto farPlane = Vector4{0.0f, 0.0f, 1.0f, -1.0f};
	array<pair<Point4, float>, 3> vertexs;
	for (int i = 0; i < 3; i++) {
		float distance = points[i].GetVector4().Dot(nearPlane);
		vertexs[i] = {points[i], distance};
	}
	//���ﲻ��Ҫ�����޳� ֱ������
	std::sort(vertexs.begin(), vertexs.end(), [](const auto& vertexA, const auto& vertexB) {
		return vertexA.second < vertexB.second;
	});
	auto vertexCount = std::count_if(vertexs.begin(), vertexs.end(), [](const auto& data) {
		return data.second <= 0.0f;
	});
	Point4 point0 = vertexs[0].first;
	Point4 point1 = vertexs[1].first;
	Point4 point2 = vertexs[2].first;
	vector<array<Point2, 2>> result;
	if (vertexCount == 1) {
		/*             *Point0
			newPoint4  |\  newPoint3
		  ___________�K|_\�L_________________�ڶ��μ���(�����Ҫ����)
		  _____________|__\_________________ ��һ�μ���
			newPoint1�J|   \�InewPoint2
					   |  /Point1
					   | /
					   |/
					   *Point2
		*/
		Point4 newPoint1 = CalculatePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = CalculatePlanePoint(nearPlane, {point0, point1});
		//�ж�Point0�Ƿ���Ҫ����
		bool needClip = point0.GetVector4().Dot(farPlane) > 0.0f;
		if (needClip) {
			Point4 newPoint3 = CalculatePlanePoint(nearPlane, {point0, newPoint2});
			Point4 newPoint4 = CalculatePlanePoint(nearPlane, {point0, newPoint1});
			Point2 newScreenPoint1 = newPoint1.ToPoint2();
			Point2 newScreenPoint2 = newPoint2.ToPoint2();
			Point2 newScreenPoint3 = newPoint3.ToPoint2();
			Point2 newScreenPoint4 = newPoint4.ToPoint2();
			result.reserve(4);
			result.push_back({newScreenPoint1, newScreenPoint2});
			result.push_back({newScreenPoint2, newScreenPoint3});
			result.push_back({newScreenPoint3, newScreenPoint4});
			result.push_back({newScreenPoint4, newScreenPoint1});
		} else {
			Point2 screenPoint0 = point0.ToPoint2();
			Point2 newScreenPoint1 = newPoint1.ToPoint2();
			Point2 newScreenPoint2 = newPoint2.ToPoint2();
			result.reserve(3);
			result.push_back({screenPoint0, newScreenPoint2});
			result.push_back({newScreenPoint2, newScreenPoint1});
			result.push_back({newScreenPoint1, screenPoint0});
		}
	} else if (vertexCount == 2) {
		/*             *Point0
		               | \
            newPoint4  |  \   newPoint3
            _________�K|_*_\�L____________________�ڶ��μ���(���2)
					   |    \
			           |     \   Point1
		    newPoint4  |      \�L
		    _________�K|__*___/__________________�ڶ��μ���(���3)
		               |    / �InewPoint3
			___________|___/____________________��һ�μ���
			       	 �J|  /�I
			newPoint1  | /   newPoint2
					   |/
					   *Point2

		*/
		Point4 newPoint1 = CalculatePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = CalculatePlanePoint(nearPlane, {point1, point2});
		/*
			Զƽ��������λ�ÿ�ѡ 
			1.point0 ���� ����Ҫ��������(������Point0 point1ƽ�� �Ҹպ���ƽ���ϵ����)
			2.point1 ����
			3.point1 ����
			ע��point0 �� newPoint2 �м� ��һ���� ���뾡�취Ҳ���������� ��*�ű�ʾ��Ҫ�õ��ĵ� newPoint5
		*/
		bool upPoint0 = point0.GetVector4().Dot(farPlane) >= 0.0f;
		bool upPoint1 = point1.GetVector4().Dot(farPlane) > 0.0f;
		if (upPoint0) {
			Point2 screenPoint0 = point0.ToPoint2();
			Point2 screenPoint1 = point1.ToPoint2();
			Point2 newScreenPoint1 = newPoint1.ToPoint2();
			Point2 newScreenPoint2 = newPoint2.ToPoint2();
			result.reserve(5);
			result.push_back({screenPoint0, screenPoint1});
			result.push_back({screenPoint1, newScreenPoint2});
			result.push_back({newScreenPoint2, screenPoint0});
			result.push_back({newScreenPoint2, newScreenPoint1});
			result.push_back({newScreenPoint1, screenPoint0});
		} else if(upPoint1) {
			Point4 newPoint3 = CalculatePlanePoint(nearPlane, {point0, point1});
			Point4 newPoint4 = CalculatePlanePoint(nearPlane, {point0, newPoint1});
			Point4 newPoint5 = CalculatePlanePoint(nearPlane, {point0, newPoint2});
			Point2 screenPoint1 = point1.ToPoint2();
			Point2 newScreenPoint1 = newPoint1.ToPoint2();
			Point2 newScreenPoint2 = newPoint2.ToPoint2();
			Point2 newScreenPoint3 = newPoint3.ToPoint2();
			Point2 newScreenPoint4 = newPoint4.ToPoint2();
			Point2 newScreenPoint5 = newPoint5.ToPoint2();
			result.reserve(7);
			result.push_back({newScreenPoint5,newScreenPoint3});
			result.push_back({newScreenPoint3, screenPoint1});
			result.push_back({screenPoint1, newScreenPoint2});
			result.push_back({newScreenPoint2, newScreenPoint5});
			result.push_back({newScreenPoint2, newScreenPoint1});
			result.push_back({newScreenPoint1, newScreenPoint4});
			result.push_back({newScreenPoint4, newScreenPoint5});
		} else {
			Point4 newPoint3 = CalculatePlanePoint(nearPlane, {point1, newPoint2});
			Point4 newPoint4 = CalculatePlanePoint(nearPlane, {point0, newPoint1});
			Point4 newPoint5 = CalculatePlanePoint(nearPlane, {point0, newPoint2});
			Point2 newScreenPoint1 = newPoint1.ToPoint2();
			Point2 newScreenPoint2 = newPoint2.ToPoint2();
			Point2 newScreenPoint3 = newPoint3.ToPoint2();
			Point2 newScreenPoint4 = newPoint4.ToPoint2();
			Point2 newScreenPoint5 = newPoint5.ToPoint2();
			result.reserve(6);
			result.push_back({newScreenPoint5, newScreenPoint3});
			result.push_back({newScreenPoint3, newScreenPoint2});
			result.push_back({newScreenPoint2, newScreenPoint5});
			result.push_back({newScreenPoint2, newScreenPoint1});
			result.push_back({newScreenPoint1, newScreenPoint4});
			result.push_back({newScreenPoint4, newScreenPoint5});
		}
	} else if (vertexCount == 3) {
		/*             *Point0
			newPoint1  |\  newPoint2
		  ___________�K|_\�L_________________ ���2
		               |  \
			           |   \Point1
			___________|  /_________________ ���3
			newPoint1�J| /�InewPoint2
					   |/
					   *Point2
		*/
		/*
			Զƽ�����ĸ�λ�ÿ�ѡ 
			1.point0 ����(������Point0 point1ƽ�� �Ҹպ���ƽ���ϵ����)
			2.point1 ����
			3.point2 ����
			4.point2 ����(������Point2 point1ƽ�� �Ҹպ���ƽ���ϵ����)
		*/
		bool upPoint0 = point0.GetVector4().Dot(farPlane) >= 0.0f;
		bool upPoint1 = point1.GetVector4().Dot(farPlane) > 0.0f;
		bool upPoint2 = point2.GetVector4().Dot(farPlane) > 0.0f;
		if (upPoint0) {
			Point2 screenPoint0 = point0.ToPoint2();
			Point2 screenPoint1 = point1.ToPoint2();
			Point2 screenPoint2 = point2.ToPoint2();
			result.reserve(3);
			result.push_back({screenPoint0, screenPoint1});
			result.push_back({screenPoint1, screenPoint2});
			result.push_back({screenPoint2, screenPoint0});
		} else if (upPoint1) {
			Point4 newPoint1 = CalculatePlanePoint(nearPlane, {point0, point2});
			Point4 newPoint2 = CalculatePlanePoint(nearPlane, {point0, point1});
			Point2 screenPoint1 = point1.ToPoint2();
			Point2 screenPoint2 = point2.ToPoint2();
			Point2 newScreenPoint1 = newPoint1.ToPoint2();
			Point2 newScreenPoint2 = newPoint2.ToPoint2();
			result.reserve(4);
			result.push_back({newScreenPoint1, newScreenPoint2});
			result.push_back({newScreenPoint2, screenPoint1});
			result.push_back({screenPoint1, screenPoint2});
			result.push_back({screenPoint2, newScreenPoint1});
		} else if (upPoint2) {
			Point4 newPoint1 = CalculatePlanePoint(nearPlane, {point0, point2});
			Point4 newPoint2 = CalculatePlanePoint(nearPlane, {point1, point2});
			Point2 screenPoint2 = point2.ToPoint2();
			Point2 newScreenPoint1 = newPoint1.ToPoint2();
			Point2 newScreenPoint2 = newPoint2.ToPoint2();
			result.reserve(3);
			result.push_back({newScreenPoint1, newScreenPoint2});
			result.push_back({newScreenPoint2, screenPoint2});
			result.push_back({screenPoint2, newScreenPoint1});
		} else {
			//����Ҫ����
		}
	} else {
		//pointCount == 0 ����Ҫ����
	}
	return result;
}

bool Rasterizer::LineClip(array<Point2, 2> & points) {
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
	if ((deltaX == 0.0f && (q1 <= 0.0f || q2 <= 0.0f)) ||
		(deltaY == 0.0f && (q3 <= 0.0f || q4 <= 0.0f))) {
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

void Rasterizer::DrawLine(const array<Point2, 2> & points) {
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
	Point2 A;
	Point2 B;
	int drawWidth;
	int drawHeight;
	bool reverse;
	if (k >= -1.0f && k < 1.0f) {
		A = points[0];
		B = points[1];
		drawWidth = width;
		drawHeight = height;
		reverse = false;
	} else if (k < -1.0f || k >= 1.0f) {
		A = Point2{points[0].y, points[0].x};
		B = Point2{points[1].y, points[1].x};
		drawWidth = height;
		drawHeight = width;
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
	int xMin = XScreenToPixel(A.x);
	int xMax = XScreenToPixel(B.x);
	Point2 pointA{XPixelToScreen(xMin), YPixelToScreen(YScreenToPixel(A.y))};
	Point2 pointB{XPixelToScreen(xMax), YPixelToScreen(YScreenToPixel(B.y))};

	const float pixelHeight = 1.0f / static_cast<float>(drawHeight);
	const float halfPixelHeight = 0.5f / pixelHeight;
	/*
		ʹ����Kֵ������
		addtion ���� �������߻��������� ��һ��һ�����
	*/
	float newk = (pointB.y - pointA.y) / (pointB.x - pointA.x);
	float addtion = newk > 0.0f ? 1.0f : -1.0f;
	float middleY = pointA.y + addtion * halfPixelHeight;

	for (int pixelX = xMin; pixelX <= xMax; pixelX++) {
		float x = XPixelToScreen(pixelX);
		Point2 middlePoint = Point2{x, middleY};
		//ȡ[0,1)��Ϊ���� ��ʾ��ֱ���Ƿ����Ϸ�
		bool pointUpLine = (addtion * ComputeLineEquation(middlePoint, pointA, pointB)) >= 0.0f;
		float y = pointUpLine ? (middleY - addtion * halfPixelHeight) : (middleY + addtion * halfPixelHeight);
		int pixelY = YScreenToPixel(y);
		//���ܳ�ȥ���������Ҫ��ǰֹͣ
		if (!XYInPixel(pixelX, pixelY)) {
			return;
		}
		//����Ҳ��Ҫ��ת����
		if (reverse) {
			DrawWritePixel(pixelY, pixelX);
		} else {
			DrawWritePixel(pixelX, pixelY);
		}
		//ȡ[0,1)��Ϊ���� �е���ֱ���·� ��Ҫ����е�һ����λ
		if (!pointUpLine) {
			middleY += addtion * pixelHeight;
		}
	}
}

void Rasterizer::DrawWritePixel(int x, int y) {
	assert(XYInPixel(x, y));
	//����ֱ�����ڵ����
	//��֤��������д���ʱ�򲻻Ḳ�ǵ�ֱ��
	constexpr float lineDepth = -1.0f;
	const Color write = {1.0f, 1.0f, 1.0f};
	int index = ReversePixelToIndex(x, y);
	zBuffer[index] = {lineDepth, write};
}
