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
		Point4 newPoint1 = ComputePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = ComputePlanePoint(nearPlane, {point0, point1});
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
			const auto& vectex0 = vertexs[0];
			auto newVectex1 = 
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint1, {newCoordinate1, newNormal1, newColor1}};
			auto newVectex2 = 
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint2, {newCoordinate2, newNormal2, newColor2}};
			result.reserve(1);
			result.emplace_back(vectex0, newVectex2, newVectex1);
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
		Point4 newPoint1 = ComputePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = ComputePlanePoint(nearPlane, {point1, point2});
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
			const auto& vectex0 = vertexs[0];
			const auto& vectex1 = vertexs[1];
			auto newVectex1 =
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint1, {newCoordinate1, newNormal1, newColor1}};
			auto newVectex2 =
				pair<Point4, tuple<Point2, Vector3, Color>>{newPoint2, {newCoordinate2, newNormal2, newColor2}};
			result.reserve(2);
			result.emplace_back(vectex0, vectex1, newVectex2);
			result.emplace_back(vectex0, newVectex2, newVectex1);
		}
	} else if (vertexCount == 3) {
		//不需要剪裁
		array<Point2, 3> testTriangle{point0.ToPoint2(), point1.ToPoint2(), point2.ToPoint2()};
		if (!(BackCull(testTriangle) ^ reverse)) {
			result.reserve(1);
			result.emplace_back(vertexs[0], vertexs[1], vertexs[2]);
		}
	} else {
		//pointCount == 0 不需要绘制
	}
	return result;
}
