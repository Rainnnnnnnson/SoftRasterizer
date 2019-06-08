#pragma once
#pragma once
#include<vector>
#include<array>
#include<functional>
#include<algorithm>
#include"ArrayStream.h"
#include"Assertion.h"
#include"Math.h"
using std::array;
using std::vector;
using std::function;

struct RGBColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct IndexData {
	unsigned int textureIndex;
	array<unsigned int, 3> pointIndex;
	array<unsigned int, 3> textureCoordinateIndex;
};

//��������ϵ�����½� һ��һ�����ϼ���
class RGBImage {
public:
	RGBImage(int width, int height);
	int GetWidth() const;
	int GetHeight() const;
	RGBColor GetPixel(int x, int y) const;
	void SetPixel(int x, int y, RGBColor rgb);
private:
	int width;
	int height;
	vector<RGBColor> rgbs;
};

struct Line2D {
	Point2 pointA, pointB;
	bool operator==(const Line2D& l) const;
};

struct WireframeTriangle {
	array<Point4, 3> points;
};

struct TextureTriangle {
	array<Point4, 3> points;
	array<Point2, 3> textureCoordinate;
};

//��[0,1]ӳ����[0,255] 
//С��0 ȡ 0
//����1 ȡ 255
RGBColor ColorToRGBColor(Color c);

//A B C˳ʱ��ɼ� ��ʱ�벻�ɼ�(����)
//�������� ��������������������
//�����ָ��Z�Ḻ���������� ���� true
bool BackCulling(array<Point3, 3> points);

//�жϼ��ú��Ƿ���ֱ���ڿ���
//����trueʱ lineΪ���ú�ֱ��
bool Line2DClip(Line2D& line);

//���㳬ƽ���ϵĵ�
Point4 ComputePlanePoint(float C, float D, Point4 point0, Point4 point1);

//�߿�ģʽ�Ľ�Զƽ����� �ֱ�Ϊ z = 0 �� z = 1;
//�������е�ֱ��
vector<Line2D> WireframeTriangleNearAndFarClip(WireframeTriangle triangle);

//����������Զ��ƽ�����
//������������������ ��Ҫ����
vector<TextureTriangle> TextureTriangleNearAndFarClip(TextureTriangle triangle);

//��������ϵ��
array<float, 3> ComputeCenterCoefficient(Point2 point, array<Point2, 3> points);

//���ļ��㶥��
Point4 ComputeCenterPoint(array<float, 3> coefficients, array<Point4, 3> points);

//���ļ�����������
Point2 ComputeCenterTextureCoordinate(array<float, 3> coefficients, array<Point2, 3> textureCoordinates,
									  array<float, 3> pointW);

//��ȡһ�����صĿ��
float GetPixelDelta(int pixelCount);

//��������ת������Ļ����(���ĵ�)
//[0,pixelCount - 1] ת���� [-1,1]
//pixelCount ���ظ���
//���ж�Խ��
float PixelToScreen(int pixel, int pixelCount);

//��Ļ����ת������������
//[-1,1] ת���� [0, pixelCount - 1]
//pixelCount ���ظ���
//���ж�Խ��
int ScreenToPixel(float screen, int pixelCount);

//�ж�XY�Ƿ���[-1,1]
bool InScreenXY(Point2 point);

//�ж�Z�Ƿ���[0,1]
bool InScreenZ(float z);

//�ж�x y �Ƿ������ؿռ���
bool InPixel(int x, int y, int width, int height);

//���ӿռ� x * y * z == [-1,1) * [-1.1) * [0,1]
//��������ϵ
class Renderer {
public:
	Renderer(int width, int height);
	void Clear();
	RGBImage GenerateImage() const;

	//����ɫ�߿� 
	//����������ͼ�����ǰ��
	//������ɫ�����ܶԶ�����иı�
	//������ɫ��û��Ӱ�� ����û�������ɫ��
	//�������߿��Ǽ����Ժ��
	template<typename Texture>
	void DrawTriangleByWireframe(const vector<Point3>& points,
								 const vector<Point2>& textureCoordinates,
								 const vector<Texture>& textures,
								 const vector<IndexData>& indexDatas,
								 function<Point4(Point3, Point2, const Texture&)> vertexShader);

	//������������
	//Texture �Զ����������� ��ʽ�Զ���
	//���㷨�� ��������ʵ��
	//��������ɫ����ʵ�ָ���Ч��
	//
	//vertexShader ������ɫ��
	//���ز��� Point4 ��������ת������������ ע�Ᵽ�� w ����   
	//������� Point3 ԭʼ�������� 
	//        Point2 ԭʼ�������� 
	//        const Texture& �Զ������������
	//
	//pixelShader  ������ɫ��
	//���ز��� Color �����ɫ[0,1]=>[0,255] С��0ȡ0 ����1ȡ255 
	//������� Point4 �������������μ��������� ����ͨ������󷵻�ԭʼ����
	//        Point2 ��������������͸�ӽ��������������
	//        const Texture& �Զ������������
	template<typename Texture>
	void DrawTriangleByTexture(const vector<Point3>& points,
							   const vector<Point2>& textureCoordinates,
							   const vector<Texture>& textures,
							   const vector<IndexData>& indexs,
							   function<Point4(Point3, Point2, const Texture&)> vertexShader,
							   function<Color(Point4, Point2, const Texture&)> pixelShader);
private:
	template<typename Texture>
	void HandlePixel(TextureTriangle triangle, const Texture& texture,
					 function<Color(Point4, Point2, const Texture&)> pixelShader);
	//����ɫ�߶� ������ͼ����ǰ��
	void DrawLine2D(Line2D line);
	//������ڻ��������������������ص�
	void DrawZBuffer(int x, int y, float z, RGBColor color);
	//������ڻ��ư�ɫֱ�߸�������ǰ��ĵ�
	void DrawWritePixel(int x, int y);
private:
	int width;
	int height;
	//�������
	//���״̬����zbufferʱ  2.0f
	//���ڻ������δ洢��     [0,1]
	//���ư�ɫֱ�ߵĴ洢��   -1.0f
	//��������ϵ�����½� һ��һ�����ϼ���
	vector<std::pair<float, RGBColor>> zBuffer;
};

template<typename Texture>
inline void Renderer::DrawTriangleByWireframe(const vector<Point3>& points,
											  const vector<Point2>& textureCoordinates,
											  const vector<Texture>& textures,
											  const vector<IndexData>& indexDatas,
											  function<Point4(Point3, Point2, const Texture&)> vertexShader) {
	assert(!points.empty());
	assert(!textureCoordinates.empty());
	assert(!textures.empty());
	for (auto& data : indexDatas) {
		assert(data.pointIndex[0] <= points.size());
		assert(data.pointIndex[1] <= points.size());
		assert(data.pointIndex[2] <= points.size());
		assert(data.textureCoordinateIndex[0] <= textureCoordinates.size());
		assert(data.textureCoordinateIndex[1] <= textureCoordinates.size());
		assert(data.textureCoordinateIndex[2] <= textureCoordinates.size());
		assert(data.textureIndex <= textures.size());
		//ִ�ж�����ɫ����õ���
		auto p = Stream(array<int, 3>{0, 1, 2}, [&](int i) {
			return vertexShader(points[data.pointIndex[i]],
								textureCoordinates[data.textureCoordinateIndex[i]], textures[data.textureIndex]);
		});
		//�߿�ģʽ�²����б������� ��ʾȫ���߶�
		//Զ��ƽ����ú�ת��Ϊ2D�߶�
		auto clippedLines = WireframeTriangleNearAndFarClip(WireframeTriangle{p});
		for (auto& clippedLine : clippedLines) {
			//����
			if (Line2DClip(clippedLine)) {
				//����2D�߶�
				DrawLine2D(clippedLine);
			}
		}
	}
}

template<typename Texture>
inline void Renderer::DrawTriangleByTexture(const vector<Point3>& points,
											const vector<Point2>& textureCoordinates,
											const vector<Texture>& textures,
											const vector<IndexData>& indexDatas,
											function<Point4(Point3, Point2, const Texture&)> vertexShader,
											function<Color(Point4, Point2, const Texture&)> pixelShader) {
	assert(!points.empty());
	assert(!textureCoordinates.empty());
	assert(!textures.empty());
	for (auto& data : indexDatas) {
		assert(data.pointIndex[0] <= points.size());
		assert(data.pointIndex[1] <= points.size());
		assert(data.pointIndex[2] <= points.size());
		assert(data.textureCoordinateIndex[0] <= textureCoordinates.size());
		assert(data.textureCoordinateIndex[1] <= textureCoordinates.size());
		assert(data.textureCoordinateIndex[2] <= textureCoordinates.size());
		assert(data.textureIndex <= textures.size());
		//ִ�ж�����ɫ����õ���
		auto textureCoordinate = Stream(data.textureCoordinateIndex, [&](unsigned int i) {
			return textureCoordinates[i];
		});
		auto point4s = Stream(array<int, 3>{0, 1, 2}, [&](int i) {
			return vertexShader(points[data.pointIndex[i]], textureCoordinate[i], textures[data.textureIndex]);
		});
		//��������(��ʱ������) ������ֱ�ӽ�����һ��ѭ��
		auto point3s = Stream(point4s, [](const Point4& p) {
			return p.ToPoint3();
		});
		if (BackCulling(point3s)) {
			continue;
		}
		//Զ��ƽ����� 
		//����������в�ֵ���� 
		auto clippedTriangles = TextureTriangleNearAndFarClip(TextureTriangle{point4s, textureCoordinate});
		//����ÿ�����ص�λ�� 
		//ִ��������ɫ��
		//���������ɫ
		for (auto& clippedTriangle : clippedTriangles) {
			HandlePixel(clippedTriangle, textures[data.textureIndex], pixelShader);
		}
	}

}

template<typename Texture>
inline void Renderer::HandlePixel(TextureTriangle triangle, const Texture& texture,
								  function<Color(Point4, Point2, const Texture&)> pixelShader) {
	//��ȡ�������ж��������С��x yֵ
	//���ڼ�����Ҫ���Ƶı߿�
	array<float, 3> xValue = Stream(triangle.points, [](const Point4& p) {
		return p.x;
	});
	std::sort(xValue.begin(), xValue.end(), std::less<float>());
	array<float, 3> yValue = Stream(triangle.points, [](const Point4& p) {
		return p.y;
	});
	std::sort(yValue.begin(), yValue.end(), std::less<float>());
	//ȷ����Ҫ���Ƶı߽�
	int xMax = std::min(ScreenToPixel(xValue[2], width), width - 1);
	int xMin = std::max(ScreenToPixel(xValue[0], width), 0);
	int yMax = std::min(ScreenToPixel(yValue[2], height), height - 1);
	int yMin = std::max(ScreenToPixel(yValue[0], height), 0);
	//������ӳ�����������
	array<Point3, 3> points = Stream(triangle.points, [](const Point4& p) {
		return p.ToPoint3();
	});
	//ѭ���޶����� [xMin,xMax] * [yMin,yMax]
	for (int yIndex = yMin; yIndex <= yMax; yIndex++) {
		for (int xIndex = xMin; xIndex <= xMax; xIndex++) {
			Point2 screenPoint{
				PixelToScreen(xIndex, width),
				PixelToScreen(yIndex, width)
			};
			//�õ���Ļ����
			auto point2s = Stream(points, [](const Point3& p) {
				return p.GetPoint2();
			});
			//��������ϵ��
			array<float, 3> coefficient = ComputeCenterCoefficient(screenPoint, point2s);
			bool inTriangle = std::all_of(coefficient.begin(), coefficient.end(), [](float f) {
				return f > 0.0f;
			});
			//���������ڲ�
			if (inTriangle) {
				//ʹ������������������λ�ö�Ӧ����������
				Point4 pixelPoint = ComputeCenterPoint(
					coefficient, triangle.points
				);
				//���е��w����
				auto pointsW = Stream(triangle.points, [](const Point4& p) {
					return p.w;
				});
				//���������������
				Point2 textureCoordinate = ComputeCenterTextureCoordinate(
					coefficient, triangle.textureCoordinate, pointsW
				);
				//ִ��������ɫ��
				Color color = pixelShader(pixelPoint, textureCoordinate, texture);
				//д��zBuffer
				float depth = pixelPoint.z / pixelPoint.w;
				//�������������� ��Ҫ���Ƶ�[0,1]
				std::clamp(depth, 0.0f, 1.0f);
				DrawZBuffer(xIndex, yIndex, depth, ColorToRGBColor(color));
			}
		}
	}
}