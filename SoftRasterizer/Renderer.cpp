#include "Renderer.h"

RGBImage::RGBImage(int width, int height)
	: width(width), height(height),
	rgbs(static_cast<size_t>(width)* height, RGBColor{0, 0, 0}) {}

int RGBImage::GetWidth() const {
	return width;
}

int RGBImage::GetHeight() const {
	return height;
}

RGBColor RGBImage::GetPixel(int x, int y) const {
	assert(InPixelXY(x, y, width, height));
	return rgbs[static_cast<size_t>(width) * y + x];
}

void RGBImage::SetPixel(int x, int y, RGBColor rgb) {
	assert(InPixelXY(x, y, width, height));
	rgbs[static_cast<size_t>(width) * y + x] = rgb;
}

//================================================================================================================

bool Line2D::operator==(const Line2D& l) const {
	//线段两个点不一定是对应的
	if (((pointA == l.pointA) && (pointB == l.pointB)) ||
		((pointA == l.pointB) && (pointB == l.pointA))) {
		return true;
	}
	return false;
}

//================================================================================================================

//在清空状态深度储存为2.0f
//需要写入其他像素时候可以直接写入
constexpr float clearDepth = 2.0f;
//清空屏幕时为黑色
constexpr RGBColor black = {0, 0, 0};

Renderer::Renderer(int width, int height)
	: width(width), height(height),
	zBuffer(static_cast<size_t>(width)* height, {clearDepth, black}) {}

void Renderer::Clear() {
	for (auto& z : zBuffer) {
		z.first = clearDepth;
		z.second = black;
	}
}

RGBImage Renderer::GenerateImage() const {
	RGBImage image(width, height);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int index = j * width + i;
			auto zBufferColor = zBuffer[index];
			image.SetPixel(i, j, zBufferColor.second);
		}
	}
	return image;
}

void Renderer::DrawZBuffer(int x, int y, float z, RGBColor color) {
	assert(InPixelXY(x, y, width, height));
	assert(InScreenZ(z));
	//坐标系由下至上
	int index = (height + 1 - y) * width + x;
	zBuffer[index] = {z, color};
}

void Renderer::DrawWritePixel(int x, int y) {
	assert(InPixelXY(x, y, width, height));
	//绘制直线所在的深度
	//保证其他像素写入的时候不会覆盖掉直线
	constexpr float lineDepth = -1.0f;
	//白色
	constexpr RGBColor write = {255, 255, 255};
	//坐标系由下至上
	int index = (height + 1 - y) * width + x;
	zBuffer[index] = {lineDepth, write};
}

void DrawLineByMiddlePoint(Line2D line, int width, int height, function<void(int, int)> func) {
	if (line.pointB.x < line.pointA.x) {
		std::swap(line.pointB, line.pointA);
	}
	//线框模式不需要太精细
	//直接取像素中心来偷懒
	int xMin = ScreenToPixel(line.pointA.x, width);
	int xMax = ScreenToPixel(line.pointB.x, width);
	Point2 pointA{PixelToScreen(xMin, width), PixelToScreen(ScreenToPixel(line.pointA.y, height), height)};
	Point2 pointB{PixelToScreen(xMax, height), PixelToScreen(ScreenToPixel(line.pointB.y, height), height)};
	//新K值
	float newk = (pointB.y - pointA.y) / (pointB.x - pointA.x);
	//增量区分[-1,0)和 [0,1) 这样就不用写多几个分支
	float addtion = newk > 0.0f ? 1.0f : -1.0f;
	//获取中点的Y值
	float middleY = pointA.y + addtion * 0.5f * GetPixelDelta(height);
	//遍历
	for (int pixelX = xMin; pixelX <= xMax; pixelX++) {
		float x = PixelToScreen(pixelX, width);
		Point2 middlePoint = Point2{x, middleY};
		//取[0,1)作为例子 表示在直线是否在上方
		bool up = addtion * ComputeLineEquation(middlePoint, pointA, pointB) > 0.0f;
		float y;
		if (up) {
			y = middleY - addtion * 0.5f * GetPixelDelta(height);
		} else {
			y = middleY + addtion * 0.5f * GetPixelDelta(height);
		}
		int pixelY = ScreenToPixel(y, height);
		func(pixelX, pixelY);
		//取[0,1)作为例子 中点在直线下方 需要提高中点一个单位
		if (!up) {
			middleY += addtion * GetPixelDelta(height);
		}
	}
}

void Renderer::DrawLine2D(Line2D line) {
	assert(InScreenXY(line.pointA));
	assert(InScreenXY(line.pointB));
	//斜率
	float k = (line.pointB.y - line.pointA.y) / (line.pointB.x - line.pointA.x);
	if (k >= -1.0f && k < 1.0f) {
		DrawLineByMiddlePoint(line, width, height, [&](int x, int y) {
			DrawWritePixel(x, y);
		});
	} else if (k < -1.0f || k >= 1.0f) {
		//用x = ky + b当作直线
		//关于y = x 对称
		//反转 x 和 y的属性
		Line2D reverseLine = {line.pointB, line.pointA};
		int reverseWidth = height;
		int reverseHeight = width;
		DrawLineByMiddlePoint(reverseLine, reverseWidth, reverseHeight, [&](int x, int y) {
			//注意这里也要反转
			DrawWritePixel(y, x);
		});
	} else {
		//k = NaN
		//两点重合不绘制
	}
}

unsigned char ColorFloatToByte(float f) {
	float test = f * 255.0f;
	if (test <= 0) {
		return 0;
	} else if (test >= 255.0f) {
		return static_cast<unsigned char>(255);
	} else {
		return static_cast<unsigned char>(test);
	}
}

RGBColor ColorToRGBColor(Color c) {
	return RGBColor{
		ColorFloatToByte(c.r),
		ColorFloatToByte(c.g),
		ColorFloatToByte(c.b),
	};
}

bool BackCulling(Array<Point3, 3> points) {
	Vector3 AB = points[1] - points[0];
	Vector3 BC = points[2] - points[1];
	//Z轴正方向
	constexpr Vector3 zAxis{0.0f, 0.0f, 1.0f};
	Vector3 direction = AB.Cross(BC);
	float cosX = direction.Dot(zAxis);
	if (cosX >= 0.0f) {
		return true;
	}
	return false;
}

bool Line2DClip(Line2D& line) {
	//边界
	constexpr float xMin = -1.0f;
	constexpr float xMax = 1.0f;
	constexpr float yMin = -1.0f;
	constexpr float yMax = 1.0F;
	//使用liang-barsky算法
	float x0 = line.pointA.x;
	float y0 = line.pointA.y;
	float x1 = line.pointB.x;
	float y1 = line.pointB.y;

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

	line.pointA = Point2{newX0, newY0};
	line.pointB = Point2{newX1, newY1};
	return true;
}

//返回与两点直线在平面上的交点
Point4 ComputePlanePoint(float C, float D, Point4 point0, Point4 point1) {
	//计算两点与平面的系数t
	//这里扩展到四维 方法和三维一样
	//N[P0 + t(P1 - P0)] = 0
	// t = - (N * P0) / (N * (P1 - P0))
	//但是平面 A B 都等于0 只需要计算 z 和 w
	array<float, 4> vector4{
		point1.x - point0.x, point1.y - point0.y, point1.z - point0.z, point1.w - point0.w
	};
	float nP0 = -(C * point0.z + D * point0.w);
	float nP0_P1 = (C * vector4[2] + D * vector4[3]);
	float t = nP0 / nP0_P1;
	return Point4{
		point0.x + t * vector4[0],
		point0.y + t * vector4[1],
		point0.z + t * vector4[2],
		point0.w + t * vector4[3],
	};
}

MaxCapacityArray<Array<Point4, 3>, 4> TriangleNearAndFarClip(const Array<Point4, 3> & points) {
	MaxCapacityArray<Array<Point4, 3>, 4> triangleArray;
	MaxCapacityArray<Array<Point4, 3>, 2> nearClipTriangles = TriangleClip(-1.0f, 0.0f, points);
	for (auto& nearClipTriangle : nearClipTriangles) {
		auto farClipTriangles = TriangleClip(1.0f, -1.0f, nearClipTriangle);
		for (auto& farClipTriangle : farClipTriangles) {
			triangleArray.Push(farClipTriangle);
		}
	}
	return triangleArray;
}

MaxCapacityArray<Array<Point4, 3>, 2> TriangleClip(float C, float D, const Array<Point4, 3> & points) {
	//判断点在平面的哪一侧
	auto pointBools = points.Stream([](const Point4& p) {
		return std::pair<Point4, float>{p, 0.0f};
	});
	//带入超平面得到梯度方向的距离
	for (auto& pointBool : pointBools) {
		pointBool.second = pointBool.first.z * C + pointBool.first.w * D;
	}
	//根据距离排序
	std::sort(pointBools.begin(), pointBools.end(), [](auto& pointBool1, auto& pointBool2) {
		return pointBool1.second < pointBool2.second;
	});
	//在平面梯度负方向的点的个数
	int pointCount = static_cast<int>(std::count_if(pointBools.begin(), pointBools.end(), [](auto& pointBool) {
		return pointBool.second <= 0.0f;
	}));
	//需要返回的三角形
	MaxCapacityArray<Array<Point4, 3>, 2> triangleArray;
	if (pointCount == 1) {
		/*
					   *Point0
					   |\   newPoint[1]
		  _____________|_\↙______________
		  newPoint[0]↗|  \
					   |  /Point1
					   | /
					   |/
					   *Point2
		*/
		Point4 newPoint1 = ComputePlanePoint(C, D, pointBools[0].first, pointBools[2].first);
		Point4 newPoint2 = ComputePlanePoint(C, D, pointBools[0].first, pointBools[1].first);
		triangleArray.Push({pointBools[0].first, newPoint1, newPoint2});
	} else if (pointCount == 2) {
		/*
					   * Point0
					   |\
					   | \
					   |  \ Point1
			___________|__/_______________
					 ↗| /↖
			newPoint[0]|/   newPoint[1]
	    		       *Point2
        */
		Point4 newPoint1 = ComputePlanePoint(C, D, pointBools[0].first, pointBools[2].first);
		Point4 newPoint2 = ComputePlanePoint(C, D, pointBools[1].first, pointBools[2].first);
		triangleArray.Push({pointBools[0].first, newPoint1, newPoint2});
		triangleArray.Push({pointBools[0].first, newPoint2, pointBools[1].first});
	} else if (pointCount == 3) {
		triangleArray.Push(points);
	} else {
		//pointCount == 0
		//没有三角形要添加
	}
	return triangleArray;
}

MaxCapacityArray<Line2D, 9> GetNotRepeatingLine2Ds(const MaxCapacityArray<Array<Point4, 3>, 4> & triangles) {
	MaxCapacityArray<Line2D, 9> line2DArray;
	for (auto& triangle : triangles) {
		auto point2s = triangle.Stream([](const Point4& p) {
			return p.ToPoint3().GetPoint2();
		});
		//线框三角形变成2D线段
		array<Line2D, 3> line2Ds{
			Line2D{point2s[0], point2s[1]},
			Line2D{point2s[1], point2s[2]},
			Line2D{point2s[2], point2s[0]},
		};
		//不重复线段加入容器中
		//因为最多9条线段 就不考虑复杂度了 
		for (auto& line2D : line2Ds) {
			if (std::find_if(line2DArray.begin(), line2DArray.end(),
				[&](auto& line) { return line2D == line; }) == line2DArray.end()) {
				line2DArray.Push(line2D);
			}
		}
	}
	return line2DArray;
}

void HandlePixel(int width, int height, 
				 const Array<Point4, 3> & points, 
				 const Array<Point2, 3> & coordinate, 
				 const Array<Point4, 3> & needComputePoint, 
				 function<Color(Point4, Point2)> ComputeColor, 
				 function<void(int, int, float, Color)> drawZBuffer) {
	//获取三角形中顶点最大最小的x y值
	//用于计算需要绘制的边框
	auto xValue = points.Stream([](const Point4& p) {
		return p.x;
	});
	std::sort(xValue.begin(), xValue.end(), std::less<float>());
	auto yValue = points.Stream([](const Point4& p) {
		return p.y;
	});
	std::sort(yValue.begin(), yValue.end(), std::less<float>());
	//确定需要绘制的边界
	int xMax = std::min(ScreenToPixel(xValue[2], width), width - 1);
	int xMin = std::max(ScreenToPixel(xValue[0], width), 0);
	int yMax = std::min(ScreenToPixel(yValue[2], height), height - 1);
	int yMin = std::max(ScreenToPixel(yValue[0], height), 0);
	//需要绘制的三角形映射至屏幕
	auto mainPoints = points.Stream([](const Point4& p) {
		return p.ToPoint3().GetPoint2();
	});
	//循环限定矩形 [xMin,xMax] * [yMin,yMax]
	for (int yIndex = yMin; yIndex <= yMax; yIndex++) {
		for (int xIndex = xMin; xIndex <= xMax; xIndex++) {
			//每一个需要绘制的屏幕上的点
			Point2 screenPoint{
				PixelToScreen(xIndex, width), PixelToScreen(yIndex, height)
			};
			//计算重心系数
			Array<float, 3> coefficient = ComputeCenterCoefficient(screenPoint, mainPoints);
			//在三角形内部
			bool inTriangle = std::all_of(coefficient.begin(), coefficient.end(), [](float f) {
				return f > 0.0f;
			});
			if (inTriangle) {
				//使用重心系数计算出像素位置对应的齐次坐标点
				Point4 pixelPoint = ComputeCenterPoint(coefficient, points);
				//所有点的w坐标
				auto pointsW = points.Stream([](const Point4& p) {
					return p.w;
				});
				//矫正后的纹理坐标
				Point2 textureCoordinate = ComputeCenterTextureCoordinate(
					coefficient, coordinate, pointsW
				);
				//写入zBuffer
				float depth = pixelPoint.z / pixelPoint.w;
				//浮点数精度问题 需要限制到[0,1]
				std::clamp(depth, 0.0f, 1.0f);
				//执行像素着色器
				Color color = ComputeColor(pixelPoint, textureCoordinate);
				//绘制进入图像
				drawZBuffer(xIndex, yIndex, depth, color);
			}
		}
	}


}
Array<float, 3> ComputeCenterCoefficient(Point2 point, Array<Point2, 3> points) {
	float fa = ComputeLineEquation(points[0], points[1], points[2]);
	float fb = ComputeLineEquation(points[1], points[2], points[0]);
	float fc = ComputeLineEquation(points[2], points[0], points[1]);
	float alpha = ComputeLineEquation(point, points[1], points[2]) / fa;
	float beta = ComputeLineEquation(point, points[2], points[0]) / fb;
	float gamma = ComputeLineEquation(point, points[0], points[1]) / fc;
	return Array<float, 3>{alpha, beta, gamma};
}
Point4 ComputeCenterPoint(Array<float, 3> coefficients, Array<Point4, 3> points) {
	return Point4{
		coefficients[0] * points[0].x + coefficients[1] * points[1].x + coefficients[2] * points[2].x,
		coefficients[0] * points[0].y + coefficients[1] * points[1].y + coefficients[2] * points[2].y,
		coefficients[0] * points[0].z + coefficients[1] * points[1].z + coefficients[2] * points[2].z,
		coefficients[0] * points[0].w + coefficients[1] * points[1].w + coefficients[2] * points[2].w
	};
}
Point2 ComputeCenterTextureCoordinate(Array<float, 3> coefficients, 
									  Array<Point2, 3> textureCoordinates, 
									  Array<float, 3> pointW) {
	float screenU = (coefficients[0] * (textureCoordinates[0].x / pointW[0])) +
		(coefficients[1] * (textureCoordinates[1].x / pointW[1])) +
		(coefficients[2] * (textureCoordinates[2].x / pointW[2]));
	float screenV = (coefficients[0] * (textureCoordinates[0].y / pointW[0])) +
		(coefficients[1] * (textureCoordinates[1].y / pointW[1])) +
		(coefficients[2] * (textureCoordinates[2].y / pointW[2]));
	constexpr float one = 1.0f;
	float screenOne = (1.0f / pointW[0]) + (1.0f / pointW[1]) + (1.0f / pointW[2]);
	return Point2{screenU / screenOne, screenV / screenOne};
}

float GetPixelDelta(int pixelCount) {
	return 2.0f / static_cast<float>(pixelCount);
}

float PixelToScreen(int pixel, int pixelCount) {
	float delta = GetPixelDelta(pixelCount);
	return static_cast<float>(pixel) * delta + (-1.0f + 0.5f * delta);
}

int ScreenToPixel(float screen, int pixelCount) {
	//当1.0f的时候应该被映射至其中,但是算出来会越界
	if (screen == 1.0f) {
		return pixelCount - 1;
	}
	return static_cast<int>(floor(((screen + 1.0f) / 2.0f) * static_cast<float>(pixelCount)));
}

bool InScreenXY(Point2 point) {
	return point.x >= -1.0f && point.x <= 1.0f && point.y >= -1.0f && point.y <= 1.0f;
}

bool InScreenZ(float z) {
	return z >= 0.0f && z <= 1.0f;
}

bool InPixelXY(int x, int y, int width, int height) {
	return x >= 0 && x < width && y >= 0 && y < height;
}
