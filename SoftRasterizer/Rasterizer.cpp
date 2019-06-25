#include "Rasterizer.h"
//在清空状态深度储存为2.0f
constexpr float clearDepth = 2.0f;
//清空屏幕时为黑色
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
	//当1.0f的时候应该被映射至其中,但是算出来会越界
	if (x == 1.0f) {
		return width - 1;
	}
	return static_cast<int>(floor(((x + 1.0f) / 2.0f) * static_cast<float>(width)));
}

int Rasterizer::YScreenToPixel(float y) const {
	//当1.0f的时候应该被映射至其中,但是算出来会越界
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
	//近平面 向量来代表平面 
	constexpr auto nearPlane = Vector4{0.0f, 0.0f, -1.0f, 0.0f};
	array<pair<pair<Point4, tuple<Point2, Vector3, Color>>, float>, 3> vertexs;
	for (int i = 0; i < 3; i++) {
		float distance = triangleData[i].first.GetVector4().Dot(nearPlane);
		vertexs[i] = {triangleData[i], distance};
	}
	/*
		因为顶点需要排序 得到对应顺序后剪裁
		每次进行一次交换 代表三角形顶点顺时针逆时针将会转变
		一个三角形最多需要交换三次顶点
		根据4种情况的列举可得 !(BackCull(triangle) ^ reverse) 可以确定是否需要绘制
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
	//近平面梯度指向Z轴负方向 这样可以得到需要保留点的个数
	auto vertexCount = std::count_if(vertexs.begin(), vertexs.end(), [](const auto& data) {
		return data.second <= 0.0f;
	});
	//需要使用顶点来计算剪裁后的插值
	Point4 point0 = vertexs[0].first.first;
	Point4 point1 = vertexs[1].first.first;
	Point4 point2 = vertexs[2].first.first;
	vector<array<pair<Point4, tuple<Point2, Vector3, Color>>, 3>> result;
	/*
		分四种情况讨论
		其中两种需要剪裁
		两种不需要剪裁
	*/
	if (vertexCount == 1) {
		/*
					   *Point0
					   |\   newPoint2
		  _____________|_\↙______________
			newPoint1↗|  \
					   |  /Point1
					   | /
					   |/
					   *Point2
		*/
		Point4 newPoint1 = CalculatePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = CalculatePlanePoint(nearPlane, {point0, point1});
		/*
			使用新顶点构造三角形来确定是否绘制
			保持顺时针传入 再根据reverse综合判断
		*/
		array<Point2, 3> testTriangle{point0.ToPoint2(), newPoint2.ToPoint2(), newPoint1.ToPoint2()};
		if (!(BackCull(testTriangle) ^ reverse)) {
		    /*
				计算两个顶点的线性插值
				因为是近平面剪裁必定过近平面 但需要确定使用w插值 还是z插值
				透视投影情况下 使用w插值 w = 顶点原始z
				正交投影情况下 使用z插值 w = 1
				使用 平面两侧的顶点 w == 1 同时成立 判断是否为正交投影
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
			//计算顶点数据插值
			const auto& data0 = vertexs[0].first.second;
			const auto& data1 = vertexs[1].first.second;
			const auto& data2 = vertexs[2].first.second;
			Point2 newCoordinate1 = get<0>(data0) * (1.0f - t1) + get<0>(data2) * t1;
			Point2 newCoordinate2 = get<0>(data0) * (1.0f - t2) + get<0>(data1) * t2;
			Vector3 newNormal1 = get<1>(data0) * (1.0f - t1) + get<1>(data2) * t1;
			Vector3 newNormal2 = get<1>(data0) * (1.0f - t2) + get<1>(data1) * t2;
			Color newColor1 = get<2>(data0) * (1.0f - t1) + get<2>(data2) * t1;
			Color newColor2 = get<2>(data0) * (1.0f - t2) + get<2>(data1) * t2;
		    //通过顶点加入至容器还是按照顺时针加入
			const auto& vertex0 = vertexs[0].first;
			auto newVertex1 = 
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint1, {newCoordinate1, newNormal1, newColor1}};
			auto newVertex2 = 
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint2, {newCoordinate2, newNormal2, newColor2}};
			result.reserve(1);
			result.push_back({vertex0, newVertex2, newVertex1});
		}
	} else if (vertexCount == 2) {
		//这里的逻辑和上面相似不再重复注释
		/*
					   * Point0
					   |\
					   | \
					   |  \ Point1
			___________|__/_______________
					 ↗| /↖
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
		//不需要剪裁
		array<Point2, 3> testTriangle{point0.ToPoint2(), point1.ToPoint2(), point2.ToPoint2()};
		if (!(BackCull(testTriangle) ^ reverse)) {
			result.reserve(1);
			result.push_back({vertexs[0].first, vertexs[1].first, vertexs[2].first});
		}
	} else {
		//pointCount == 0 不需要绘制
	}
	return result;
}

Point4 Rasterizer::CalculatePlanePoint(Vector4 N, const array<Point4, 2> & points) {
	/*
		在w还没有归一化的时候
		透视投影和正交投影都是线性的
		计算两点与平面的系数t
		这里扩展到四维 方法和三维一样
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
	//获取三角形中顶点最大最小的x y值
	//用于计算需要绘制的边框
	array<float, 3> xValue;
	array<float, 3> yValue;
	for (int i = 0; i < 3; i++) {
		xValue[i] = points[i].x;
		yValue[i] = points[i].y;
	}
	std::sort(xValue.begin(), xValue.end(), std::less<float>());
	std::sort(yValue.begin(), yValue.end(), std::less<float>());
	//确定需要绘制的边界
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
	//循环限定矩形 [xMin,xMax] * [yMin,yMax]
	for (int yIndex = yMin; yIndex <= yMax; yIndex++) {
		for (int xIndex = xMin; xIndex <= xMax; xIndex++) {
			//每一个需要绘制的屏幕上的点 根据这个点计算重心
			Point2 point{XPixelToScreen(xIndex), YPixelToScreen(yIndex)};
			float alpha = CalculateLineEquation(point, B, C) / fa;
			float beta = CalculateLineEquation(point, C, A) / fb;
			float gamma = CalculateLineEquation(point, A, B) / fc;
			array<float, 3> coefficients{alpha, beta, gamma};
			//判断是否在三角形内部
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
	//纹理
	float screenU = coefficients[0] * (coodinates[0].x / points[0].w)
		+ coefficients[1] * (coodinates[1].x / points[1].w)
		+ coefficients[2] * (coodinates[2].x / points[2].w);
	float screenV = coefficients[0] * (coodinates[0].y / points[0].w)
		+ coefficients[1] * (coodinates[1].y / points[1].w)
		+ coefficients[2] * (coodinates[2].y / points[2].w);
	//法线
	float screenX = coefficients[0] * (normals[0].x / points[0].w)
		+ coefficients[1] * (normals[1].x / points[1].w)
		+ coefficients[2] * (normals[2].x / points[2].w);
	float screenY = coefficients[0] * (normals[0].y / points[0].w)
		+ coefficients[1] * (normals[1].y / points[1].w)
		+ coefficients[2] * (normals[2].y / points[2].w);
	float screenZ = coefficients[0] * (normals[0].z / points[0].w)
		+ coefficients[1] * (normals[1].z / points[1].w)
		+ coefficients[2] * (normals[2].z / points[2].w);
	//颜色
	float screenR = coefficients[0] * (colors[0].r / points[0].w)
		+ coefficients[1] * (colors[1].r / points[1].w)
		+ coefficients[2] * (colors[2].r / points[2].w);
	float screenG = coefficients[0] * (colors[0].g / points[0].w)
		+ coefficients[1] * (colors[1].g / points[1].w)
		+ coefficients[2] * (colors[2].g / points[2].w);
	float screenB = coefficients[0] * (colors[0].b / points[0].w)
		+ coefficients[1] * (colors[1].b / points[1].w)
		+ coefficients[2] * (colors[2].b / points[2].w);
	//顶点
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
	//近平面和远平面 向量表示平面 
	constexpr auto nearPlane = Vector4{0.0f, 0.0f, -1.0f, 0.0f};
	constexpr auto farPlane = Vector4{0.0f, 0.0f, 1.0f, -1.0f};
	array<pair<Point4, float>, 3> vertexs;
	for (int i = 0; i < 3; i++) {
		float distance = points[i].GetVector4().Dot(nearPlane);
		vertexs[i] = {points[i], distance};
	}
	//这里不需要背面剔除 直接排序
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
		  ___________↘|_\↙_________________第二次剪裁(如果需要剪裁)
		  _____________|__\_________________ 第一次剪裁
			newPoint1↗|   \↖newPoint2
					   |  /Point1
					   | /
					   |/
					   *Point2
		*/
		Point4 newPoint1 = CalculatePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = CalculatePlanePoint(nearPlane, {point0, point1});
		//判断Point0是否需要剪裁
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
            _________↘|_*_\↙____________________第二次剪裁(情况2)
					   |    \
			           |     \   Point1
		    newPoint4  |      \↙
		    _________↘|__*___/__________________第二次剪裁(情况3)
		               |    / ↖newPoint3
			___________|___/____________________第一次剪裁
			       	 ↗|  /↖
			newPoint1  | /   newPoint2
					   |/
					   *Point2

		*/
		Point4 newPoint1 = CalculatePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = CalculatePlanePoint(nearPlane, {point1, point2});
		/*
			远平面有三个位置可选 
			1.point0 上面 不需要继续剪裁(包含了Point0 point1平行 且刚好在平面上的情况)
			2.point1 上面
			3.point1 下面
			注意point0 和 newPoint2 中间 有一根线 我想尽办法也画不出来了 用*号表示需要得到的点 newPoint5
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
		  ___________↘|_\↙_________________ 情况2
		               |  \
			           |   \Point1
			___________|  /_________________ 情况3
			newPoint1↗| /↖newPoint2
					   |/
					   *Point2
		*/
		/*
			远平面有四个位置可选 
			1.point0 上面(包含了Point0 point1平行 且刚好在平面上的情况)
			2.point1 上面
			3.point2 上面
			4.point2 下面(包含了Point2 point1平行 且刚好在平面上的情况)
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
			//不需要绘制
		}
	} else {
		//pointCount == 0 不需要绘制
	}
	return result;
}

bool Rasterizer::LineClip(array<Point2, 2> & points) {
	//边界
	constexpr float xMin = -1.0f;
	constexpr float xMax = 1.0f;
	constexpr float yMin = -1.0f;
	constexpr float yMax = 1.0F;
	//使用liang-barsky算法
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

	//平行并且在框外面
	if ((deltaX == 0.0f && (q1 <= 0.0f || q2 <= 0.0f)) ||
		(deltaY == 0.0f && (q3 <= 0.0f || q4 <= 0.0f))) {
		return false;
	}

	//储存点所代表的t (x0 + tΔx)
	//代表取点原本的位置或者边界
	array<float, 3> t0Array{0.0f, 0.0f, 0.0f};//取最大
	array<float, 3> t1Array{1.0f, 1.0f, 1.0f};//取最小

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

	//反过来说明在框外
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
		这里乘以图片比例主要是因为图片比例会导致k > 1 或者 k < -1的情况
		中点算法一次只能上升或者下降一格像素 当K > 1时 只能取K == 1 (K<-1 同理)
	*/
	float y = (points[1].y - points[0].y) * static_cast<float>(height);
	float x = (points[1].x - points[0].x) * static_cast<float>(width);
	float k = y / x;
	/*
		k < -1 || k >= 1 的情况 可以看作 y = x 轴对称
		所以只需要反转顶点和图像来绘制即可
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
		//两点重合 k = NaN 不画线
		return;
	}
	/*
		这里开始中点算法 只需要讨论 0 < k < 1 的情况即可
		-1 < k < 0 使用正负变量来区分
	*/
	if (B.x < A.x) {
		std::swap(B, A);
	}
	/*
		这里直接将非像素中心点 转化成像素中心点
		可以省很多情况 增加代码可读性
		而且两种情况画出的线条最多差距1像素
	*/
	int xMin = XScreenToPixel(A.x);
	int xMax = XScreenToPixel(B.x);
	Point2 pointA{XPixelToScreen(xMin), YPixelToScreen(YScreenToPixel(A.y))};
	Point2 pointB{XPixelToScreen(xMax), YPixelToScreen(YScreenToPixel(B.y))};

	const float pixelHeight = 1.0f / static_cast<float>(drawHeight);
	const float halfPixelHeight = 0.5f / pixelHeight;
	/*
		使用新K值来画线
		addtion 区分 线往上走还是往下走 归一成一种情况
	*/
	float newk = (pointB.y - pointA.y) / (pointB.x - pointA.x);
	float addtion = newk > 0.0f ? 1.0f : -1.0f;
	float middleY = pointA.y + addtion * halfPixelHeight;

	for (int pixelX = xMin; pixelX <= xMax; pixelX++) {
		float x = XPixelToScreen(pixelX);
		Point2 middlePoint = Point2{x, middleY};
		//取[0,1)作为例子 表示在直线是否在上方
		bool pointUpLine = (addtion * ComputeLineEquation(middlePoint, pointA, pointB)) >= 0.0f;
		float y = pointUpLine ? (middleY - addtion * halfPixelHeight) : (middleY + addtion * halfPixelHeight);
		int pixelY = YScreenToPixel(y);
		//若跑出去了则代表需要提前停止
		if (!XYInPixel(pixelX, pixelY)) {
			return;
		}
		//这里也需要反转绘制
		if (reverse) {
			DrawWritePixel(pixelY, pixelX);
		} else {
			DrawWritePixel(pixelX, pixelY);
		}
		//取[0,1)作为例子 中点在直线下方 需要提高中点一个单位
		if (!pointUpLine) {
			middleY += addtion * pixelHeight;
		}
	}
}

void Rasterizer::DrawWritePixel(int x, int y) {
	assert(XYInPixel(x, y));
	//绘制直线所在的深度
	//保证其他像素写入的时候不会覆盖掉直线
	constexpr float lineDepth = -1.0f;
	const Color write = {1.0f, 1.0f, 1.0f};
	int index = ReversePixelToIndex(x, y);
	zBuffer[index] = {lineDepth, write};
}
