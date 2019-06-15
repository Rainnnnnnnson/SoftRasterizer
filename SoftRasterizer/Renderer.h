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
	将[0,1]映射至[0,255] 
	小于0 取 0
	大于1 取 255
*/
RGBColor ColorToRGBColor(Color c);

/*
	将[0,255]映射至[0,1] 
*/
Color RGBColorToColor(RGBColor c);

/*
    这里传入的是Point4 中的 x y 生成的point2 
	在透视矩阵处理后 x y 正负不改变 
    顺时针不消除 逆时针消除
	消除返回true
*/
bool BackCulling(Array<Point2, 3> points);

/*
	远近平面两次剪裁后最多得到4个三角形
*/
MaxCapacityArray<Array<Point4, 3>, 4> TriangleNearAndFarClip(const Array<Point4, 3> & points);

/*
	将三角形变成不重复的Line2D
	因为最多生成4个被切割的三角形 
	所以最多生成9条不同的线段
*/
MaxCapacityArray<Array<Point2, 2>, 9> GetNotRepeatingScreenLines(const MaxCapacityArray<Array<Point4, 3>, 4> & triangles);

/*
	int   获取坐标使用[0, width -1] * [0, height - 1]
	float 获取坐标使用[0,1] * [0,1]
*/
class RGBImage {
public:
	RGBImage(int width, int height);
	int GetWidth() const;
	int GetHeight() const;

	//==========================================

	//左上角为[0,0]
	RGBColor GetPixel(int x, int y) const;
	void SetPixel(int x, int y, RGBColor rgb);

	//=========================================

	//左下角为[0,0]
	RGBColor ReverseGetPixel(int x, int y) const;
	void ReverseSetPixel(int x, int y, RGBColor rgb);
	//取值[0,1] 超过取边界
	Color BilinearFiltering(Point2 p) const;
private:
	int width;
	int height;
	//以左上角为[0,0]计算
	//内存是图片顺序存储的 index = y * width + x
	vector<RGBColor> rgbs;
};

/*
	可视空间 x * y * z == [-1,1) * [-1.1) * [0,1]
	左手坐标系
*/
class Renderer {
public:
	Renderer(int width, int height);

	/*
		清空zbuffer 变为黑色 深度清空
	*/
	void Clear();

	/*
		根据当前zbuffer状态生成图像
	*/
	RGBImage GenerateImage() const;

	/*
		画彩色三角形
	*/
	void DrawTriangleByColor(const vector<Point3>& points,
							 const vector<Color>& colors,
							 const vector<ColorIndexData>& indexs,
							 function<Point4(Point3)> vertexShader);

	/*
		画白色线框 
		覆盖在所有图像的最前面
		画出的线框是剪裁以后的
	*/
	void DrawTriangleByWireframe(const vector<Point3>& points,
								 const vector<WireframeIndexData>& indexDatas,
								 function<Point4(Point3)> vertexShader);

	/*
		画纹理三角形
		Texture 自定义纹理类型 格式自定义
		顶点法线 在纹理中实现
		在两个着色器中实现各种效果
		
		vertexShader 顶点着色器
		返回参数 Point4 经过矩阵转化后的其次坐标 注意保留 w 分量   
		输入参数 Point3 原始世界坐标 
		        Point2 原始纹理坐标 
		        const Texture& 自定义纹理的引用
		
		pixelShader  像素着色器
		返回参数 Color 输出颜色[0,1]=>[0,255] 小于0取0 大于1取255 
		输入参数 Point4 经过重心三角形计算后的坐标 可以通过逆矩阵返回原始坐标
		        Point2 经过重心三角形透视矫正后的纹理坐标
		        const Texture& 自定义纹理的引用
	*/
	template<typename Texture>
	void DrawTriangleByTexture(const vector<Point3>& points,
							   const vector<Point2>& textureCoordinates,
							   const vector<Texture>& textures,
							   const vector<TextureIndexData>& indexs,
							   function<Point4(Point3, Point2, const Texture&)> vertexShader,
							   function<Color(Point4, Point2, const Texture&)> pixelShader);
private:
	//这个用于绘制纹理三角形来画像素的
	void DrawZBuffer(int x, int y, float z, RGBColor color);
	//这个用于绘制白色直线覆盖在最前面的点
	void DrawWritePixel(int x, int y);
private:
	int width;
	int height;
	/*
		储存深度
		清空状态所有zbuffer时  2.0f
		存在画三角形存储在     [0,1]
		绘制白色直线的存储在   -1.0f
		存储索引 index = y * width + x
	*/
	vector<std::pair<float, RGBColor>> zBuffer;
};

//画白色线段 覆盖在图像最前面
void HandleLine(int width, int height, Array<Point2, 2> line,
				function<void(int, int)> func);
//画三角形像素
void HandleTriangle(int width, int height, Array<Point4, 3> mainPoints, Array<Point4, 3> points,
					function<void(int, int, Array<float, 3>)> howToUseCoefficient);

/*
	超平面剪裁 Ax + By + Cz + Dw = 0
	vetctor(A,B,C,D)
	近平面 -z + nw = 0
	远平面  z - fw = 0
	左右上下屏幕可以用类似的方法剪裁 这里不用
	一个平面剪裁最多得到两个三角形
*/
MaxCapacityArray<Array<Point4, 3>, 2> TriangleClip(Vector4 vector, const Array<Point4, 3> & points);

/*
	判断剪裁后是否有直线在框中
	返回true时 line为剪裁后直线
*/
bool ScreenLineClip(Array<Point2, 2> & points);

/*
	判断两直线是否相等
	点不一定对应
*/
bool ScreenLineEqual(Array<Point2, 2> pointsA, Array<Point2, 2> pointsB);

/*
	计算超平面上的点
*/
Point4 ComputePlanePoint(Vector4 vector, Array<Point4, 2> points);

/*
	计算重心系数
*/
Array<float, 3> ComputeCenterCoefficient(Point2 point, Array<Point2, 3> points);

/*
	重心计算颜色
*/
Color ComputerCenterColor(Array<float, 3> coefficients, Array<Color, 3> colors);

/*
	重心计算顶点
*/
Point4 ComputeCenterPoint(Array<float, 3> coefficients, Array<Point4, 3> points);

/*
	重心计算纹理坐标
*/
Point2 ComputeCenterTextureCoordinate(Array<float, 3> coefficients,
									  Array<Point2, 3> textureCoordinates,
									  Array<float, 3> pointW);
/*
	获取一个像素的宽度
*/
float GetPixelDelta(int pixelCount);

/*
	像素坐标转换至屏幕坐标(中心点)
	[0,pixelCount - 1] 转换至 [-1,1]
	pixelCount 像素个数
	不判断越界
*/
float PixelToScreen(int pixel, int pixelCount);

/*
	屏幕坐标转化至像素坐标
	[-1,1] 转换至 [0, pixelCount - 1]
	pixelCount 像素个数
	不判断越界
*/
int ScreenToPixel(float screen, int pixelCount);

/*
	判断XY是否在[-1,1]
*/
bool InScreenXY(Point2 point);

/*
	判断Z是否在[0,1]
*/
bool InScreenZ(float z);

/*
	判断x y 是否在像素空间中
*/
bool InPixelXY(int x, int y, int width, int height);

/*
	像素坐标生成一维数组索引
*/
int PixelToIndex(int x, int y, int width);

/*
	图片像素的存放 和 坐标系 很不一样
	传入的 x 和 y 是从左下角计算的
	返回值 是从左上角计算的
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
		//执行顶点着色器后得到点
		const auto& texture = textures[data.textureIndex];
		auto mainPoints = ArrayIndex<3>().Stream([&](int i) {
			return vertexShader(points[data.pointIndex[i]],
								textureCoordinates[data.textureCoordinateIndex[i]],
								texture);
		});
		auto mainTextureCoodinates = ArrayIndex<3>().Stream([&](int i) {
			return textureCoordinates[data.textureCoordinateIndex[i]];
		});
		//背面消除(逆时针消除) 若消除直接进入下一个循环
		auto point2s = mainPoints.Stream([](const Point4& p) {
			return p.ToPoint3().GetPoint2();
		});
		if (BackCulling(point2s)) {
			continue;
		}
		auto mainPointsW = mainPoints.Stream([](const Point4& p) {
			return p.w;
		});
		//远近平面剪裁 最多剪裁出4个三角形
		auto trianglePoints = TriangleNearAndFarClip(mainPoints);
		//得到纹理坐标
		for (auto& trianglePoint : trianglePoints) {
			//光栅化阶段
			HandleTriangle(width, height, mainPoints, trianglePoint, [&](int x, int y, Array<float, 3> coefficent) {
				auto point = ComputeCenterPoint(coefficent, mainPoints);
				//判断深度
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