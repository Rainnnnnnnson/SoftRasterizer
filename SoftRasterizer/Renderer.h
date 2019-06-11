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
	内存是图片顺序存储的 index = y * width + x
	但是 GetPixel SetPixel 是上下倒转的
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
	将[0,1]映射至[0,255] 
	小于0 取 0
	大于1 取 255
*/
RGBColor ColorToRGBColor(Color c);

/*
    这里传入的是Point4 中的 x y 生成的point2 
	在透视矩阵处理后 x y 正负不改变 
    顺时针不消除 逆时针消除
	消除返回true
*/
bool BackCulling(Array<Point2, 3> points);

/*
	判断剪裁后是否有直线在框中
	返回true时 line为剪裁后直线
*/
bool ScreenLineClip(Array<Point2, 2>& points);

/*
	判断两直线是否相等
	点不一定对应
*/
bool ScreenLineEqual(Array<Point2, 2> pointsA, Array<Point2, 2> pointsB);

/*
    计算超平面上的点
*/
Point4 ComputePlanePoint(float C, float D, Point4 point0, Point4 point1);

/*
	线框模式 和 纹理模式 可以同时使用这个方法
	远近平面两次剪裁后最多得到4个三角形
*/
MaxCapacityArray<Array<Point4, 3>, 4> TriangleNearAndFarClip(const Array<Point4, 3> & points);

/*
	超平面剪裁 Ax + By + Cz + Dw = 0 
	近平面 C = -1     D =  0
	远平面 C =  1     D = -1
	一个平面剪裁最多得到两个三角形
*/
MaxCapacityArray<Array<Point4, 3>, 2> TriangleClip(float C, float D, const Array<Point4, 3>& points);

/*
	将三角形变成不重复的Line2D
	因为最多生成4个被切割的三角形 
	所以最多生成9条不同的线段
*/
MaxCapacityArray<Array<Point2, 2>, 9> GetNotRepeatingScreenLines(const MaxCapacityArray<Array<Point4, 3>, 4> & triangles);

/*
	计算重心系数
*/
Array<float, 3> ComputeCenterCoefficient(Point2 point, Array<Point2, 3> points);

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
		画白色线框 
		覆盖在所有图像的最前面
		顶点着色器可能对顶点进行改变
		像素着色器没有影响 所以没有这个着色器
		画出的线框是剪裁以后的
	*/
	template<typename Texture>
	void DrawTriangleByWireframe(const vector<Point3>& points,
								 const vector<Point2>& textureCoordinates,
								 const vector<Texture>& textures,
								 const vector<IndexData>& indexDatas,
								 function<Point4(Point3, Point2, const Texture&)> vertexShader);

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
							   const vector<IndexData>& indexs,
							   function<Point4(Point3, Point2, const Texture&)> vertexShader,
							   function<Color(Point4, Point2, const Texture&)> pixelShader);
private:
	//画白色线段 覆盖在图像最前面
	void DrawScreenLine(Array<Point2, 2> line);
	//画三角形
	void DrawTriangle(const Array<Point4, 3> & points,
					  const Array<Point2, 3> & coordinate,
					  const Array<Point4, 3> & needComputePoint,
					  function<Color(Point4, Point2)> pixelShader);

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
		//执行顶点着色器后得到点
		auto point4 = Array<int, 3>{0, 1, 2}.Stream([&](int i) {
			return vertexShader(points[data.pointIndex[i]],
								textureCoordinates[data.textureCoordinateIndex[i]], 
								textures[data.textureIndex]);
		});
		//线框模式下不进行背面消除 显示全部线段
		//剪裁后最多得到4个三角形
		MaxCapacityArray<Array<Point4, 3>, 4> triangles = TriangleNearAndFarClip(point4);
		//获取不重复线段
		auto screenLines = GetNotRepeatingScreenLines(triangles);
		//绘制线段
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
		//执行顶点着色器后得到点
		auto mainPoints = Array<int, 3>{0, 1, 2}.Stream([&](std::size_t i) {
			return vertexShader(points[data.pointIndex[i]],
								textureCoordinates[data.textureCoordinateIndex[i]],
								textures[data.textureIndex]);
		});
		//背面消除(逆时针消除) 若消除直接进入下一个循环
		auto point2s = mainPoints.Stream([](const Point4& p) {
			return Point2{p.x, p.y};
		});
		if (BackCulling(point2s)) {
			continue;
		}
		//远近平面剪裁 最多剪裁出4个三角形
		MaxCapacityArray<Array<Point4, 3>, 4> trianglePoints = TriangleNearAndFarClip(mainPoints);
		//得到纹理坐标
		auto mainTextureCoordinate = Array<int, 3>{0, 1, 2}.Stream([&](std::size_t i) {
			return textureCoordinates[data.textureCoordinateIndex[i]];
		});
		for (auto& trianglePoint : trianglePoints) {
			//光栅化阶段
			DrawTriangle(mainPoints, mainTextureCoordinate, trianglePoint, [&](Point4 p, Point2 t) {
				//会调用像素着色器
				return pixelShader(p, t, textures[data.textureIndex]);
			});
		}
	}
}