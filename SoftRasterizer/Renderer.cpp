#include "Renderer.h"

RGBImage::RGBImage(int width, int height) : width(width), height(height),
rgbs(static_cast<size_t>(width)* height, RGBColor{0, 0, 0}) {
	assert(width > 0);
	assert(height > 0);
}

int RGBImage::GetWidth() const {
	return width;
}

int RGBImage::GetHeight() const {
	return height;
}

RGBColor RGBImage::ReverseGetPixel(int x, int y) const {
	assert(InPixelXY(x, y, width, height));
	return rgbs[ReversePixelToIndex(x, y, width, height)];
}

void RGBImage::ReverseSetPixel(int x, int y, RGBColor rgb) {
	assert(InPixelXY(x, y, width, height));
	rgbs[ReversePixelToIndex(x, y, width, height)] = rgb;
}

RGBColor RGBImage::GetPixel(int x, int y) const {
	assert(InPixelXY(x, y, width, height));
	return rgbs[PixelToIndex(x, y, width)];
}

void RGBImage::SetPixel(int x, int y, RGBColor rgb) {
	assert(InPixelXY(x, y, width, height));
	rgbs[PixelToIndex(x, y, width)] = rgb;
}

Color RGBImage::BilinearFiltering(Point2 p) const {
	assert(p.x >= 0.0f);
	assert(p.y >= 0.0f);
	assert(p.x <= 1.0f);
	assert(p.y <= 1.0f);
	float u = p.x * static_cast<float>(width) - 0.5f;
	float v = p.y * static_cast<float>(height) - 0.5f;
	//��������
	float u0 = floor(u);
	float u1 = u0 + 1.0f;
	float v0 = floor(v);
	float v1 = v0 + 1.0f;
	//����ϵ��
	float uRight = u1 - u;
	float uLeft = u - u0;
	float vUp = v1 - v;
	float vDown = v - v0;
	//�������ȡ����
	//�����߽�ȡ�߽�
	int u0i = std::clamp(static_cast<int>(u0), 0, width - 1);
	int u1i = std::clamp(static_cast<int>(u1), 0, width - 1);
	int v0i = std::clamp(static_cast<int>(v0), 0, height - 1);
	int v1i = std::clamp(static_cast<int>(v1), 0, height - 1);
	// A B
	// C D
	//�ĸ�����
	Color A = RGBColorToColor(ReverseGetPixel(u0i, v0i)) * uLeft * vUp;
	Color B = RGBColorToColor(ReverseGetPixel(u1i, v0i)) * uRight * vUp;
	Color C = RGBColorToColor(ReverseGetPixel(u0i, v1i)) * uLeft * vDown;
	Color D = RGBColorToColor(ReverseGetPixel(u1i, v1i)) * uRight * vDown;
	return A + B + C + D;
}

//================================================================================================================

//�����״̬��ȴ���Ϊ2.0f
//��Ҫд����������ʱ�����ֱ��д��
constexpr float clearDepth = 2.0f;
//�����ĻʱΪ��ɫ
constexpr RGBColor black = {0, 0, 0};

Renderer::Renderer(int width, int height)
	: width(width), height(height),
	zBuffer(static_cast<size_t>(width)* height, {clearDepth, black}) {
	assert(width > 0);
	assert(height > 0);
}

void Renderer::Clear() {
	for (auto& z : zBuffer) {
		z.first = clearDepth;
		z.second = black;
	}
}

RGBImage Renderer::GenerateImage() const {
	RGBImage image(width, height);
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int index = PixelToIndex(i, j, width);
			auto zBufferColor = zBuffer[index];
			image.SetPixel(i, j, zBufferColor.second);
		}
	}
	return image;
}

void Renderer::DrawTriangleByColor(const vector<Point3>& points,
								   const vector<Color>& colors,
								   const vector<ColorIndexData> indexDatas,
								   function<Point4(Point3)> vertexShader) {
	for (auto& data : indexDatas) {
		assert(data.pointIndex[0] < points.size());
		assert(data.pointIndex[1] < points.size());
		assert(data.pointIndex[2] < points.size());
		assert(data.colorIndex[0] < colors.size());
		assert(data.colorIndex[1] < colors.size());
		assert(data.colorIndex[2] < colors.size());
		//ִ�ж�����ɫ����õ���
		auto mainPoint4s = Array<int, 3>{0, 1, 2}.Stream([&](std::size_t i) {
			return vertexShader(points[data.pointIndex[i]]);
		});
		auto mainColors = Array<int, 3>{0, 1, 2}.Stream([&](std::size_t i) {
			return colors[data.colorIndex[i]];
		});

		//��������(��ʱ������) ������ֱ�ӽ�����һ��ѭ��
		auto point2s = mainPoint4s.Stream([](const Point4& point4) {
			return point4.GetPoint2();
		});
		if (BackCulling(point2s)) {
			continue;
		}
		//Զ��ƽ����� �����ó�4��������
		MaxCapacityArray<Array<Point4, 3>, 4> trianglePoints = TriangleNearAndFarClip(mainPoint4s);
		for (auto& points : trianglePoints) {
			HandleTriangle(points, [&](int x, int y, Array<float, 3> coefficients) {
				Point4 point = ComputeCenterPoint(coefficients, mainPoint4s);
				Color color = ComputerCenterColor(coefficients, mainColors);
				//���Ƹ������������� ����z ��[0,1]
				float z = std::clamp(point.z / point.w, 0.0f, 1.0f);
				DrawZBuffer(x, y, z, ColorToRGBColor(color));
			});
		}

	}
}

void Renderer::DrawZBuffer(int x, int y, float z, RGBColor color) {
	assert(InPixelXY(x, y, width, height));
	assert(InScreenZ(z));
	int index = ReversePixelToIndex(x, y, width, height);
	zBuffer[index] = {z, color};
}

void Renderer::DrawWritePixel(int x, int y) {
	assert(InPixelXY(x, y, width, height));
	//����ֱ�����ڵ����
	//��֤��������д���ʱ�򲻻Ḳ�ǵ�ֱ��
	constexpr float lineDepth = -1.0f;
	//��ɫ
	constexpr RGBColor write = {255, 255, 255};
	int index = ReversePixelToIndex(x, y, width, height);
	zBuffer[index] = {lineDepth, write};
}

void DrawLineByMiddlePoint(Array<Point2, 2> points, int width, int height, function<void(int, int)> func) {
	if (points[1].x < points[0].x) {
		std::swap(points[1], points[0]);
	}
	//�߿�ģʽ����Ҫ̫��ϸ
	//ֱ��ȡ����������͵��
	int xMin = ScreenToPixel(points[0].x, width);
	int xMax = ScreenToPixel(points[1].x, width);
	//�õ��µĵ�
	Point2 pointA{PixelToScreen(xMin, width), PixelToScreen(ScreenToPixel(points[0].y, height), height)};
	Point2 pointB{PixelToScreen(xMax, width), PixelToScreen(ScreenToPixel(points[1].y, height), height)};
	//��Kֵ
	float newk = (pointB.y - pointA.y) / (pointB.x - pointA.x);
	//��������[-1,0)�� [0,1) �����Ͳ���д�༸����֧
	float addtion = newk > 0.0f ? 1.0f : -1.0f;
	//������ظ߶�
	const float halfPixelHeight = 0.5f * GetPixelDelta(height);
	//��ȡ�е��Yֵ
	float middleY = pointA.y + addtion * halfPixelHeight;
	//����
	for (int pixelX = xMin; pixelX <= xMax; pixelX++) {
		float x = PixelToScreen(pixelX, width);
		Point2 middlePoint = Point2{x, middleY};
		//ȡ[0,1)��Ϊ���� ��ʾ��ֱ���Ƿ����Ϸ�
		bool pointUpLine = (addtion * ComputeLineEquation(middlePoint, pointA, pointB)) >= 0.0f;
		float y = pointUpLine ? (middleY - addtion * halfPixelHeight) : (middleY + addtion * halfPixelHeight);
		int pixelY = ScreenToPixel(y, height);
		func(pixelX, pixelY);
		//ȡ[0,1)��Ϊ���� �е���ֱ���·� ��Ҫ����е�һ����λ
		if (!pointUpLine) {
			middleY += addtion * GetPixelDelta(height);
		} 
	}
}

void Renderer::HandleLine(Array<Point2, 2> points) {
	assert(InScreenXY(points[0]));
	assert(InScreenXY(points[1]));
	//�������ͼƬ������Ҫ����ΪͼƬ�����ᵼ��k > 1 ���� k < -1����� ���߲���ȷ
	//�е��㷨һ��ֻ�����������½�һ������ ��K > 1ʱ ֻ��ȡK == 1 (K<-1 ͬ��)
	float y = (points[1].y - points[0].y) * static_cast<float>(height);
	float x = (points[1].x - points[0].x) * static_cast<float>(width);
	//б��
	float k = y / x;
	if (k >= -1.0f && k < 1.0f) {
		DrawLineByMiddlePoint(points, width, height, [&](int x, int y) {
			DrawWritePixel(x, y);
		});
	} else if (k < -1.0f || k >= 1.0f) {
		//��x = ky + b����ֱ��
		//����y = x �Գ�
		//��ת x �� y������
		auto reversePoints = points.Stream([](Point2 p) {
			return Point2{p.y, p.x};
		});
		int reverseWidth = height;
		int reverseHeight = width;
		DrawLineByMiddlePoint(reversePoints, reverseWidth, reverseHeight, [&](int x, int y) {
			//����ҲҪ��ת
			DrawWritePixel(y, x);
		});
	} else {
		//k = NaN
		//�����غϲ�����
	}
}

void Renderer::HandleTriangle(Array<Point4, 3> points, 
							  function<void(int x, int y, Array<float, 3>coefficent)> howToUseCoefficient) {
	assert(points[0].w != 0.0f);
	assert(points[1].w != 0.0f);
	assert(points[2].w != 0.0f);
	//��ȡ�������ж��������С��x yֵ
	//���ڼ�����Ҫ���Ƶı߿�
	auto xValue = points.Stream([](const Point4& p) {
		return p.x;
	});
	std::sort(xValue.begin(), xValue.end(), std::less<float>());
	auto yValue = points.Stream([](const Point4& p) {
		return p.y;
	});
	std::sort(yValue.begin(), yValue.end(), std::less<float>());
	//ȷ����Ҫ���Ƶı߽�
	int xMax = std::min(ScreenToPixel(xValue[2], width), width - 1);
	int xMin = std::max(ScreenToPixel(xValue[0], width), 0);
	int yMax = std::min(ScreenToPixel(yValue[2], height), height - 1);
	int yMin = std::max(ScreenToPixel(yValue[0], height), 0);
	//��Ҫ���Ƶ�������ӳ������Ļ
	auto mainPoints = points.Stream([](const Point4& p) {
		return p.ToPoint3().GetPoint2();
	});
	//ѭ���޶����� [xMin,xMax] * [yMin,yMax]
	for (int yIndex = yMin; yIndex <= yMax; yIndex++) {
		for (int xIndex = xMin; xIndex <= xMax; xIndex++) {
			//ÿһ����Ҫ���Ƶ���Ļ�ϵĵ�
			Point2 screenPoint{
				PixelToScreen(xIndex, width), PixelToScreen(yIndex, height)
			};
			//��������ϵ��
			Array<float, 3> coefficient = ComputeCenterCoefficient(screenPoint, mainPoints);
			//���������ڲ�
			bool inTriangle = std::all_of(coefficient.begin(), coefficient.end(), [](float f) {
				return f > 0.0f;
			});
			if (inTriangle) {
				howToUseCoefficient(xIndex, yIndex, coefficient);
			}
		}
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

float ByteToColorFloat(unsigned char b) {
	return static_cast<float>(b) / 255.0f;
}

Color RGBColorToColor(RGBColor c) {
	return Color{
		ByteToColorFloat(c.r),
		ByteToColorFloat(c.g),
		ByteToColorFloat(c.b),
	};
}

bool BackCulling(Array<Point2, 3> points) {
	/*
		��������
		g��������ʽ
		i  j  k
		x1 y1 0
		x2 y2 0
	*/
	float x1 = points[1].x - points[0].x;
	float y1 = points[1].y - points[0].y;
	float x2 = points[2].x - points[1].x;
	float y2 = points[2].y - points[1].y;
	float g = x1 * y2 - y1 * x2;
	if (g >= 0.0f) {
		return true;
	}
	return false;
}

bool ScreenLineClip(Array<Point2, 2>& points) {
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

	points[0] = Point2{newX0, newY0};
	points[1] = Point2{newX1, newY1};
	return true;
}

bool ScreenLineEqual(Array<Point2, 2> pointsA, Array<Point2, 2> pointsB) {
	//����Ӧ
	if (((pointsA[0] == pointsB[0]) && (pointsA[1] == pointsB[1])) ||
		((pointsA[0] == pointsB[1]) && (pointsA[1] == pointsB[0]))) {
		return true;
	}
	return false;
}

//����������ֱ����ƽ���ϵĽ���
Point4 ComputePlanePoint(float C, float D, Point4 point0, Point4 point1) {
	//����������ƽ���ϵ��t
	//������չ����ά ��������άһ��
	//N[P0 + t(P1 - P0)] = 0
	// t = - (N * P0) / (N * (P1 - P0))
	//����ƽ�� A B ������0 ֻ��Ҫ���� z �� w
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
	//�жϵ���ƽ�����һ��
	auto pointBools = points.Stream([](const Point4& p) {
		return std::pair<Point4, float>{p, 0.0f};
	});
	//���볬ƽ��õ��ݶȷ���ľ���
	for (auto& pointBool : pointBools) {
		pointBool.second = pointBool.first.z * C + pointBool.first.w * D;
	}
	//���ݾ�������
	std::sort(pointBools.begin(), pointBools.end(), [](auto& pointBool1, auto& pointBool2) {
		return pointBool1.second < pointBool2.second;
	});
	//��ƽ���ݶȸ�����ĵ�ĸ���
	int pointCount = static_cast<int>(std::count_if(pointBools.begin(), pointBools.end(), [](auto& pointBool) {
		return pointBool.second <= 0.0f;
	}));
	//��Ҫ���ص�������
	MaxCapacityArray<Array<Point4, 3>, 2> triangleArray;
	if (pointCount == 1) {
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
					 �J| /�I
			newPoint1  |/   newPoint2
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
		//û��������Ҫ���
	}
	return triangleArray;
}

MaxCapacityArray<Array<Point2, 2>, 9> GetNotRepeatingScreenLines(const MaxCapacityArray<Array<Point4, 3>, 4> & triangles) {
	MaxCapacityArray<Array<Point2, 2>, 9> returnLines;
	for (auto& triangle : triangles) {
		auto point2s = triangle.Stream([](const Point4& p) {
			return p.ToPoint3().GetPoint2();
		});
		//�߿������α��2D�߶�
		array<Array<Point2, 2>, 3> lines{
			Array<Point2, 2>{point2s[0], point2s[1]},
			Array<Point2, 2>{point2s[1], point2s[2]},
			Array<Point2, 2>{point2s[2], point2s[0]},
		};
		//���ظ��߶μ���������
		for (auto& line : lines) {
			//�Ҳ�����ͬ��ֱ��
			bool notRepecting = std::find_if(returnLines.begin(), returnLines.end(), [&](Array<Point2, 2> returnLine) {
				return ScreenLineEqual(line, returnLine);
			}) == returnLines.end();
			if (notRepecting) {
				returnLines.Push(line);
			}
		}
	}
	return returnLines;
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
Color ComputerCenterColor(Array<float, 3> coefficients, Array<Color, 3> colors) {
	return ArrayIndex<3>().Stream([&](int i) {
		return colors[i] * coefficients[i];
	}).Sum();
}
Point4 ComputeCenterPoint(Array<float, 3> coefficients, Array<Point4, 3> points) {
	return ArrayIndex<3>().Stream([&](int i) {
		return points[i] * coefficients[i];
	}).Sum();
}
Point2 ComputeCenterTextureCoordinate(Array<float, 3> coefficients,
									  Array<Point2, 3> textureCoordinates,
									  Array<float, 3> pointW) {
	float screenU = ArrayIndex<3>().Stream([&](int i) {
		return coefficients[i] * (textureCoordinates[i].x / pointW[i]);
	}).Sum();
	float screenV = ArrayIndex<3>().Stream([&](int i) {
		return coefficients[i] * (textureCoordinates[i].y / pointW[i]);
	}).Sum();
	float screenOne = ArrayIndex<3>().Stream([&](int i) {
		constexpr float one = 1.0f;
		return coefficients[i] * (one / pointW[i]);
	}).Sum();
	return Point2{screenU / screenOne, screenV / screenOne};
}

float GetPixelDelta(int pixelCount) {
	constexpr float screenLength = 1.0f - (-1.0f);
	return screenLength / static_cast<float>(pixelCount);
}

float PixelToScreen(int pixel, int pixelCount) {
	float delta = GetPixelDelta(pixelCount);
	float start = -1.0f + 0.5f * delta;
	float addtion = static_cast<float>(pixel) * delta;
	return start + addtion;
}

int ScreenToPixel(float screen, int pixelCount) {
	//��1.0f��ʱ��Ӧ�ñ�ӳ��������,�����������Խ��
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

int PixelToIndex(int x, int y, int width) {
	return y * width + x;
}

int ReversePixelToIndex(int x, int y, int width, int height) {
	int reverseY = height - y - 1;
	return reverseY * width + x;
}
