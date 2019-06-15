#pragma once
#include<vector>
#include<functional>
#include<algorithm>
#include"Array.h"
#include"Assertion.h"
#include"Math.h"
using std::vector;
using std::function;

struct RGBColor;
struct WireframeIndexData;
struct ColorIndexData;
struct TextureIndexData;
class RGBImage;
class Renderer;

struct RGBColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

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
	��[0,1]ӳ����[0,255] 
	С��0 ȡ 0
	����1 ȡ 255
*/
RGBColor ColorToRGBColor(Color c);

/*
	��[0,255]ӳ����[0,1] 
*/
Color RGBColorToColor(RGBColor c);

/*
    ���ﴫ�����Point4 �е� x y ���ɵ�point2 
	��͸�Ӿ������ x y �������ı� 
    ˳ʱ�벻���� ��ʱ������
	��������true
*/
bool BackCulling(Array<Point2, 3> points);

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
	int   ��ȡ����ʹ��[0, width -1] * [0, height - 1]
	float ��ȡ����ʹ��[0,1] * [0,1]
*/
class RGBImage {
public:
	RGBImage(int width, int height);
	int GetWidth() const;
	int GetHeight() const;

	//==========================================

	//���Ͻ�Ϊ[0,0]
	RGBColor GetPixel(int x, int y) const;
	void SetPixel(int x, int y, RGBColor rgb);

	//=========================================

	//���½�Ϊ[0,0]
	RGBColor ReverseGetPixel(int x, int y) const;
	void ReverseSetPixel(int x, int y, RGBColor rgb);
	//ȡֵ[0,1] ����ȡ�߽�
	Color BilinearFiltering(Point2 p) const;
private:
	int width;
	int height;
	//�����Ͻ�Ϊ[0,0]����
	//�ڴ���ͼƬ˳��洢�� index = y * width + x
	vector<RGBColor> rgbs;
};

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
							 function<Point4(Point3)> vertexShader);

	/*
		����ɫ�߿� 
		����������ͼ�����ǰ��
		�������߿��Ǽ����Ժ��
	*/
	void DrawTriangleByWireframe(const vector<Point3>& points,
								 const vector<WireframeIndexData>& indexDatas,
								 function<Point4(Point3)> vertexShader);

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
							   function<Point4(Point3, Point2, const Texture&)> vertexShader,
							   function<Color(Point4, Point2, const Texture&)> pixelShader);
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
void HandleLine(int width, int height, Array<Point2, 2> line,
				function<void(int, int)> func);
//������������
void HandleTriangle(int width, int height, Array<Point4, 3> mainPoints, Array<Point4, 3> points,
					function<void(int, int, Array<float, 3>)> howToUseCoefficient);

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
bool ScreenLineEqual(Array<Point2, 2> pointsA, Array<Point2, 2> pointsB);

/*
	���㳬ƽ���ϵĵ�
*/
Point4 ComputePlanePoint(Vector4 vector, Array<Point4, 2> points);

/*
	��������ϵ��
*/
Array<float, 3> ComputeCenterCoefficient(Point2 point, Array<Point2, 3> points);

/*
	���ļ�����ɫ
*/
Color ComputerCenterColor(Array<float, 3> coefficients, Array<Color, 3> colors);

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

template<typename Texture>
inline void Renderer::DrawTriangleByTexture(const vector<Point3>& points,
											const vector<Point2>& textureCoordinates,
											const vector<Texture>& textures,
											const vector<TextureIndexData>& indexDatas,
											function<Point4(Point3, Point2, const Texture&)> vertexShader,
											function<Color(Point4, Point2, const Texture&)> pixelShader) {
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
		//��������(��ʱ������) ������ֱ�ӽ�����һ��ѭ��
		auto point2s = mainPoints.Stream([](const Point4& p) {
			return p.ToPoint3().GetPoint2();
		});
		if (BackCulling(point2s)) {
			continue;
		}
		auto mainPointsW = mainPoints.Stream([](const Point4& p) {
			return p.w;
		});
		//Զ��ƽ����� �����ó�4��������
		auto trianglePoints = TriangleNearAndFarClip(mainPoints);
		//�õ���������
		for (auto& trianglePoint : trianglePoints) {
			//��դ���׶�
			HandleTriangle(width, height, mainPoints, trianglePoint, [&](int x, int y, Array<float, 3> coefficent) {
				auto point = ComputeCenterPoint(coefficent, mainPoints);
				//�ж����
				float depth = point.ToPoint3().z;
				if (InScreenZ(depth)) {
					Point2 textureCoodinate = ComputeCenterTextureCoordinate(coefficent, mainTextureCoodinates, mainPointsW);
					Color color = pixelShader(point, textureCoodinate, texture);
					DrawZBuffer(x, y, depth, ColorToRGBColor(color));
				}
			});
		}
	}
}