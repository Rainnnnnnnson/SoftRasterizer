#pragma once
#include<vector>
#include<functional>
#include<algorithm>
#include"Array.h"
#include"Assertion.h"
#include"Math.h"
#include"Rasterizer.h"
using std::vector;
using std::function;

struct WireframeIndexData;
struct ColorIndexData;
struct TextureIndexData;
class Renderer;

struct WireframeIndexData {
	Array<unsigned, 3> pointIndex;
};


struct ColorIndexData {
	Array<unsigned, 3> pointIndex;
	Array<unsigned, 3> colorIndex;
};

struct TextureIndexData {
	Array<unsigned, 3> pointIndex;
	Array<unsigned, 3> textureCoordinateIndex;
	unsigned textureIndex;
};

/*
	Զ��ƽ�����μ��ú����õ�4��������
*/
MaxCapacityArray<Array<Point4, 3>, 4> TriangleNearAndFarClip(const Array<Point4, 3> & points);

/*
	�������α�ɲ��ظ���Line2D
	��Ϊ�������4�����и�������� 
	�����������9����ͬ���߶�
*/
MaxCapacityArray<Array<Point2, 2>, 9> GetNotRepeatingScreenLines(const MaxCapacityArray<Array<Point4, 3>, 4> & triangles);

/*
	���ӿռ� x * y * z == [-1,1) * [-1.1) * [0,1]
	��������ϵ
*/
class Renderer {
public:
	Renderer(int width, int height);

	/*
		���zbuffer ��Ϊ��ɫ ������
	*/
	void Clear();

	/*
		���ݵ�ǰzbuffer״̬����ͼ��
	*/
	RGBImage GenerateImage() const;

	/*
		����ɫ������
	*/
	void DrawTriangleByColor(const vector<Point3>& points,
							 const vector<Color>& colors,
							 const vector<ColorIndexData>& indexs,
							 const function<Point4(Point3)>& vertexShader);

	/*
		����ɫ�߿� 
		����������ͼ�����ǰ��
		�������߿��Ǽ����Ժ��
	*/
	void DrawTriangleByWireframe(const vector<Point3>& points,
								 const vector<WireframeIndexData>& indexDatas,
								 const function<Point4(Point3)>& vertexShader);

	/*
		������������
		Texture �Զ����������� ��ʽ�Զ���
		���㷨�� ��������ʵ��
		��������ɫ����ʵ�ָ���Ч��
		
		vertexShader ������ɫ��
		���ز��� Point4 ��������ת������������ ע�Ᵽ�� w ����   
		������� Point3 ԭʼ�������� 
		        Point2 ԭʼ�������� 
		        const Texture& �Զ������������
		
		pixelShader  ������ɫ��
		���ز��� Color �����ɫ[0,1]=>[0,255] С��0ȡ0 ����1ȡ255 
		������� Point4 �������������μ��������� ����ͨ������󷵻�ԭʼ����
		        Point2 ��������������͸�ӽ��������������
		        const Texture& �Զ������������
	*/
	template<typename Texture>
	void DrawTriangleByTexture(const vector<Point3>& points,
							   const vector<Point2>& textureCoordinates,
							   const vector<Texture>& textures,
							   const vector<TextureIndexData>& indexs,
							   const function<Point4(Point3, Point2, const Texture&)>& vertexShader,
							   const function<Color(Point4, Point2, const Texture&)>& pixelShader);
private:
	//������ڻ��������������������ص�
	void DrawZBuffer(int x, int y, float z, RGBColor color);
	//������ڻ��ư�ɫֱ�߸�������ǰ��ĵ�
	void DrawWritePixel(int x, int y);
private:
	int width;
	int height;
	/*
		�������
		���״̬����zbufferʱ  2.0f
		���ڻ������δ洢��     [0,1]
		���ư�ɫֱ�ߵĴ洢��   -1.0f
		�洢���� index = y * width + x
	*/
	vector<std::pair<float, RGBColor>> zBuffer;
};

//����ɫ�߶� ������ͼ����ǰ��
void HandleLine(int width, int height, const Array<Point2, 2>& line,
				const function<void(int, int)>& func);
//������������
void HandleTriangle(int width, int height, const Array<Point4, 3>& points,
					const function<void(int, int, Array<float, 3>)>& howToUseCoefficient);


/*
	˳ʱ�벻���� ��ʱ������
	��������true
*/
bool BackCulling(const Array<Point4, 3>& points);

/*
	��ƽ����� Ax + By + Cz + Dw = 0
	vetctor(A,B,C,D)
	��ƽ�� -z + nw = 0
	Զƽ��  z - fw = 0
	����������Ļ���������Ƶķ������� ���ﲻ��
	һ��ƽ��������õ�����������
*/
MaxCapacityArray<Array<Point4, 3>, 2> TriangleClip(Vector4 vector, const Array<Point4, 3> & points);

/*
	�жϼ��ú��Ƿ���ֱ���ڿ���
	����trueʱ lineΪ���ú�ֱ��
*/
bool ScreenLineClip(Array<Point2, 2> & points);

/*
	�ж���ֱ���Ƿ����
	�㲻һ����Ӧ
*/
bool ScreenLineEqual(const Array<Point2, 2>& pointsA,const Array<Point2, 2>& pointsB);

/*
	���㳬ƽ���ϵĵ�
*/
Point4 ComputePlanePoint(Vector4 vector,const Array<Point4, 2>& points);

/*
	��������ϵ��
*/
Array<float, 3> ComputeCenterCoefficient(Point2 point,const Array<Point2, 3>& points);

/*
	���ļ�����ɫ ����Ǹ���w��� ����ֱ�����ļ���
*/
Color ComputerCenterColor(const Array<float, 3>& coefficients, 
						  const Array<Color, 3>& colors, 
						  const Array<float,3>& pointW);

/*
	���ļ��㶥��
*/
Point4 ComputeCenterPoint(const Array<float, 3>& coefficients, 
						  const Array<Point4, 3>& points);

/*
	���ļ�����������
*/
Point2 ComputeCenterTextureCoordinate(const Array<float, 3>& coefficients,
									  const Array<Point2, 3>& textureCoordinates,
									  const Array<float, 3>& pointW);

/*
	��ȡһ�����صĿ��
*/
float GetPixelDelta(int pixelCount);

/*
	��������ת������Ļ����(���ĵ�)
	[0,pixelCount - 1] ת���� [-1,1]
	pixelCount ���ظ���
	���ж�Խ��
*/
float PixelToScreen(int pixel, int pixelCount);

/*
	��Ļ����ת������������
	[-1,1] ת���� [0, pixelCount - 1]
	pixelCount ���ظ���
	���ж�Խ��
*/
int ScreenToPixel(float screen, int pixelCount);

/*
	�ж�XY�Ƿ���[-1,1]
*/
bool InScreenXY(Point2 point);

/*
	�ж�Z�Ƿ���[0,1]
*/
bool InScreenZ(float z);

/*
	�ж�x y �Ƿ������ؿռ���
*/
bool InPixelXY(int x, int y, int width, int height);

/*
	������������һά��������
*/
int PixelToIndex(int x, int y, int width);

/*
	ͼƬ���صĴ�� �� ����ϵ �ܲ�һ��
	����� x �� y �Ǵ����½Ǽ����
	����ֵ �Ǵ����ϽǼ����
*/
int ReversePixelToIndex(int x, int y, int width, int height);

/*
	�����ν�ƽ����úͱ������� 
	Color �� �������������һ�����߼� �������ó�ģ��
*/
template<typename TriangleData>
MaxCapacityArray<std::pair<Array<Point4, 3>, Array<TriangleData, 3>>, 2> TriangleNearClipAndBackCulling(
	const Array<Point4, 3>& points, const Array<TriangleData, 3>& triangleDatas) {
	//��ƽ��
	constexpr Vector4 nearPlane{0.0f, 0.0f, -1.0f, 0.0f};
	auto pointColors = ArrayIndex<3>().Stream([&](int i) {
		return std::pair{std::pair{points[i], triangleDatas[i]}, 0.0f};
	});
	//��úͽ�ƽ��ľ���
	for (auto& pointColor : pointColors) {
		pointColor.second = pointColor.first.first.GetVector4().Dot(nearPlane);
	}
	/*
		��Ϊ������Ҫ���� �õ���Ӧ˳������
		ÿ�ν���һ�ν��� ���������ζ���˳ʱ����ʱ�뽫��ת��
		һ�������������Ҫ�������ζ���
		���Ը��ݾ����ź���
		����4������Ƶ����� !(BackCulling(triangle) ^ reverse) ����ȷ���Ƿ���Ҫ����
	*/
	bool reverse = false;
	if (pointColors[2].second < pointColors[1].second) {
		std::swap(pointColors[2], pointColors[1]);
		reverse = !reverse;
	}
	if (pointColors[1].second < pointColors[0].second) {
		std::swap(pointColors[1], pointColors[0]);
		reverse = !reverse;
	}
	if (pointColors[2].second < pointColors[1].second) {
		std::swap(pointColors[2], pointColors[1]);
		reverse = !reverse;
	}
	//���ӿռ䷽��ĵ�
	int pointCount = static_cast<int>(std::count_if(pointColors.begin(), pointColors.end(), [](auto& pointBool) {
		return pointBool.second <= 0.0f;
	}));
	//��Ҫ���ص�����
	MaxCapacityArray<std::pair<Array<Point4, 3>, Array<TriangleData, 3>>, 2> returnArray;
	Point4 point0 = pointColors[0].first.first;
	Point4 point1 = pointColors[1].first.first;
	Point4 point2 = pointColors[2].first.first;
	TriangleData color0 = pointColors[0].first.second;
	TriangleData color1 = pointColors[1].first.second;
	TriangleData color2 = pointColors[2].first.second;
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
		Point4 newPoint1 = ComputePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = ComputePlanePoint(nearPlane, {point0, point1});
		float t1 = (newPoint1.w - point0.w) / (point2.w - point0.w);
		float t2 = (newPoint2.w - point0.w) / (point1.w - point0.w);
		TriangleData newColor1 = color0 * (1.0f - t1) + color2 * t1;
		TriangleData newColor2 = color0 * (1.0f - t2) + color1 * t2;
		Array<Point4, 3> trianglePoint{point0, newPoint2, newPoint1};
		Array<TriangleData, 3> triangleColor{color0, newColor2, newColor1};
		//�滻�ɼ��ú�ĵ� ���ı�˳��
		Array<Point4, 3> testBackCull{point0, newPoint2, newPoint1};
		if (!(BackCulling(testBackCull) ^ reverse)) {
			returnArray.Push({trianglePoint, triangleColor});
		}
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
		Point4 newPoint1 = ComputePlanePoint(nearPlane, {point0, point2});
		Point4 newPoint2 = ComputePlanePoint(nearPlane, {point1, point2});
		float t1 = (newPoint1.w - point0.w) / (point2.w - point0.w);
		float t2 = (newPoint2.w - point1.w) / (point2.w - point1.w);
		TriangleData newColor1 = color0 * (1.0f - t1) + color2 * t1;
		TriangleData newColor2 = color1 * (1.0f - t2) + color2 * t2;
		Array<Point4, 3> trianglePointA{point0, point1, newPoint2};
		Array<TriangleData, 3> triangleColorA{color0, color1, newColor2};
		Array<Point4, 3> trianglePointB{point0, newPoint2, newPoint1};
		Array<TriangleData, 3> triangleColorB{color0, newColor2, newColor1};
		//�滻�ɼ��ú�ĵ� ���ı�˳��
		Array<Point4, 3> testBackCull{point0, point1, newPoint2};
		if (!(BackCulling(testBackCull) ^ reverse)) {
			returnArray.Push({trianglePointA, triangleColorA});
			returnArray.Push({trianglePointB, triangleColorB});
		}
	} else if (pointCount == 3) {
		Array<Point4, 3> trianglePoint{point0, point1, point2};
		Array<TriangleData, 3> triangleColor{color0, color1, color2};
		if (!(BackCulling(trianglePoint) ^ reverse)) {
			returnArray.Push({trianglePoint, triangleColor});
		}
	} else {
		//pointCount == 0
		//û��������Ҫ���
	}
	return returnArray;
}

template<typename Texture>
inline void Renderer::DrawTriangleByTexture(const vector<Point3>& points,
											const vector<Point2>& textureCoordinates,
											const vector<Texture>& textures,
											const vector<TextureIndexData>& indexDatas,
											const function<Point4(Point3, Point2, const Texture&)>& vertexShader,
											const function<Color(Point4, Point2, const Texture&)>& pixelShader) {
	for (auto& data : indexDatas) {
		assert(std::all_of(data.pointIndex.begin(), data.pointIndex.end(), [&](unsigned i) {
			return i < points.size();
		}));
		assert(std::all_of(data.textureCoordinateIndex.begin(), data.textureCoordinateIndex.end(), [&](unsigned i) {
			return i < points.size();
		}));
		assert(data.textureIndex < textures.size());
		//ִ�ж�����ɫ����õ���
		const auto& texture = textures[data.textureIndex];
		auto mainPoints = ArrayIndex<3>().Stream([&](int i) {
			return vertexShader(points[data.pointIndex[i]],
								textureCoordinates[data.textureCoordinateIndex[i]],
								texture);
		});
		auto mainTextureCoodinates = ArrayIndex<3>().Stream([&](int i) {
			return textureCoordinates[data.textureCoordinateIndex[i]];
		});
		//��ƽ����ò��ұ������� ���ڶ���˳��ԭ����Ҫ����һ��
		auto trianglePoints = TriangleNearClipAndBackCulling<Point2>(mainPoints, mainTextureCoodinates);
		//�õ���������
		for (auto& trianglePoint : trianglePoints) {
			auto triangleP = trianglePoint.first;
			auto triangleT = trianglePoint.second;
			auto triangleW = triangleP.Stream([](Point4 p) {
				return p.w;
			});
			//��դ���׶�
			HandleTriangle(width, height, triangleP, [&](int x, int y, Array<float, 3> coefficent) {
				auto point = ComputeCenterPoint(coefficent, triangleP);
				//�ж����
				float depth = point.ToPoint3().z;
				if (InScreenZ(depth)) {
					Point2 textureCoodinate = ComputeCenterTextureCoordinate(coefficent, triangleT, triangleW);
					Color color = pixelShader(point, textureCoodinate, texture);
					DrawZBuffer(x, y, depth, ColorToRGBColor(color));
				}
			});
		}
	}
}