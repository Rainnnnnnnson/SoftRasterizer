#pragma once
#pragma once
#include<vector>
#include<array>
#include<functional>
#include<algorithm>
#include"Array.h"
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
	unsigned textureIndex;
	Array<unsigned, 3> pointIndex;
	Array<unsigned, 3> textureCoordinateIndex;
};

/*
	�ڴ���ͼƬ˳��洢�� index = y * width + x
	���� GetPixel SetPixel �����µ�ת��
*/
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

/*
	��[0,1]ӳ����[0,255] 
	С��0 ȡ 0
	����1 ȡ 255
*/
RGBColor ColorToRGBColor(Color c);

/*
    ���ﴫ�����Point4 �е� x y ���ɵ�point2 
	��͸�Ӿ������ x y �������ı� 
    ˳ʱ�벻���� ��ʱ������
	��������true
*/
bool BackCulling(Array<Point2, 3> points);

/*
	�жϼ��ú��Ƿ���ֱ���ڿ���
	����trueʱ lineΪ���ú�ֱ��
*/
bool ScreenLineClip(Array<Point2, 2>& points);

/*
	�ж���ֱ���Ƿ����
	�㲻һ����Ӧ
*/
bool ScreenLineEqual(Array<Point2, 2> pointsA, Array<Point2, 2> pointsB);

/*
    ���㳬ƽ���ϵĵ�
*/
Point4 ComputePlanePoint(float C, float D, Point4 point0, Point4 point1);

/*
	�߿�ģʽ �� ����ģʽ ����ͬʱʹ���������
	Զ��ƽ�����μ��ú����õ�4��������
*/
MaxCapacityArray<Array<Point4, 3>, 4> TriangleNearAndFarClip(const Array<Point4, 3> & points);

/*
	��ƽ����� Ax + By + Cz + Dw = 0 
	��ƽ�� C = -1     D =  0
	Զƽ�� C =  1     D = -1
	һ��ƽ��������õ�����������
*/
MaxCapacityArray<Array<Point4, 3>, 2> TriangleClip(float C, float D, const Array<Point4, 3>& points);

/*
	�������α�ɲ��ظ���Line2D
	��Ϊ�������4�����и�������� 
	�����������9����ͬ���߶�
*/
MaxCapacityArray<Array<Point2, 2>, 9> GetNotRepeatingScreenLines(const MaxCapacityArray<Array<Point4, 3>, 4> & triangles);

/*
	��������ϵ��
*/
Array<float, 3> ComputeCenterCoefficient(Point2 point, Array<Point2, 3> points);

/*
	���ļ��㶥��
*/
Point4 ComputeCenterPoint(Array<float, 3> coefficients, Array<Point4, 3> points);
/*
	���ļ�����������
*/
Point2 ComputeCenterTextureCoordinate(Array<float, 3> coefficients,
									  Array<Point2, 3> textureCoordinates,
									  Array<float, 3> pointW);
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
		����ɫ�߿� 
		����������ͼ�����ǰ��
		������ɫ�����ܶԶ�����иı�
		������ɫ��û��Ӱ�� ����û�������ɫ��
		�������߿��Ǽ����Ժ��
	*/
	template<typename Texture>
	void DrawTriangleByWireframe(const vector<Point3>& points,
								 const vector<Point2>& textureCoordinates,
								 const vector<Texture>& textures,
								 const vector<IndexData>& indexDatas,
								 function<Point4(Point3, Point2, const Texture&)> vertexShader);

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
							   const vector<IndexData>& indexs,
							   function<Point4(Point3, Point2, const Texture&)> vertexShader,
							   function<Color(Point4, Point2, const Texture&)> pixelShader);
private:
	//����ɫ�߶� ������ͼ����ǰ��
	void DrawScreenLine(Array<Point2, 2> line);
	//��������
	void DrawTriangle(const Array<Point4, 3> & points,
					  const Array<Point2, 3> & coordinate,
					  const Array<Point4, 3> & needComputePoint,
					  function<Color(Point4, Point2)> pixelShader);

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
		auto point4 = Array<int, 3>{0, 1, 2}.Stream([&](int i) {
			return vertexShader(points[data.pointIndex[i]],
								textureCoordinates[data.textureCoordinateIndex[i]], 
								textures[data.textureIndex]);
		});
		//�߿�ģʽ�²����б������� ��ʾȫ���߶�
		//���ú����õ�4��������
		MaxCapacityArray<Array<Point4, 3>, 4> triangles = TriangleNearAndFarClip(point4);
		//��ȡ���ظ��߶�
		auto screenLines = GetNotRepeatingScreenLines(triangles);
		//�����߶�
		for (auto& screenLine : screenLines) {
			if (ScreenLineClip(screenLine)) {
				DrawScreenLine(screenLine);
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
		auto mainPoints = Array<int, 3>{0, 1, 2}.Stream([&](std::size_t i) {
			return vertexShader(points[data.pointIndex[i]],
								textureCoordinates[data.textureCoordinateIndex[i]],
								textures[data.textureIndex]);
		});
		//��������(��ʱ������) ������ֱ�ӽ�����һ��ѭ��
		auto point2s = mainPoints.Stream([](const Point4& p) {
			return Point2{p.x, p.y};
		});
		if (BackCulling(point2s)) {
			continue;
		}
		//Զ��ƽ����� �����ó�4��������
		MaxCapacityArray<Array<Point4, 3>, 4> trianglePoints = TriangleNearAndFarClip(mainPoints);
		//�õ���������
		auto mainTextureCoordinate = Array<int, 3>{0, 1, 2}.Stream([&](std::size_t i) {
			return textureCoordinates[data.textureCoordinateIndex[i]];
		});
		for (auto& trianglePoint : trianglePoints) {
			//��դ���׶�
			DrawTriangle(mainPoints, mainTextureCoordinate, trianglePoint, [&](Point4 p, Point2 t) {
				//�����������ɫ��
				return pixelShader(p, t, textures[data.textureIndex]);
			});
		}
	}
}