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

//像素坐标系从左下角 一行一行往上计算
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

//将[0,1]映射至[0,255] 
//小于0 取 0
//大于1 取 255
RGBColor ColorToRGBColor(Color c);

//A B C顺时针不消除 逆时针消除
//消除 返回true
bool BackCulling(array<Point3, 3> points);

//判断剪裁后是否有直线在框中
//返回true时 line为剪裁后直线
bool Line2DClip(Line2D& line);

//计算超平面上的点
//近平面 -z + nw = 0 (n = 0 所以 C = -1 ,D =  0)
//远平面  z - fw = 0 (f = 1 所以 C =  1 ,D = -1)
Point4 ComputePlanePoint(float C, float D, Point4 point0, Point4 point1);

/*
	可以选择是否计算纹理 
	不计算纹理传入nullptr(断言) compute = false
	线框模式 和 纹理模式 可以同时使用这个方法
*/
void TriangleNearAndFarClip(const array<Point4, 3>* points, vector<array<Point4, 3>>* ps,
							const array<Point2, 3>* textureCoodinates = nullptr, vector<array<Point2, 3>>* ts = nullptr, 
							bool compute = false);

/*
	超平面剪裁 Ax + By + Cz + Dw = 0 
	近平面 C = -1     D =  0
	远平面 C =  1     D = -1
	传入 三角形的数据进入 points1
	返回 0 不需要绘制三角形
	返回 1 得到 一个三角形 (points1)
	返回 2 得到 两个三角形 (points1 和 points2)
*/

int TriangleClip(float C, float D, array<Point4, 3>& points1, array<Point4, 3> & points2);

/*
	将三角形变成不重复的Line2D
*/
vector<Line2D> GetNotRepeatingLine2D(const vector<array<Point4, 3>> & triangles);

//计算重心系数
array<float, 3> ComputeCenterCoefficient(Point2 point, array<Point2, 3> points);

//重心计算顶点
Point4 ComputeCenterPoint(array<float, 3> coefficients, array<Point4, 3> points);

//重心计算纹理坐标
Point2 ComputeCenterTextureCoordinate(array<float, 3> coefficients,
									  array<Point2, 3> textureCoordinates,
									  array<float, 3> pointW);

/*

//远近平面 Cz + Dw = 0
//从 triangle1 传入一个三角形
//返回 0 不需要绘制
//返回 1 剪裁后得到一个三角形 或者 不需要剪裁得到一个三角形 triangle1
//返回 2 剪裁后得到两个三角形 triangle1 triangle2
//未说明三角形不可用
//根据梯度方向来排序三角形的顶点
		pointCount == 1
			   | Point0
			   |\   newPoint2
	___________|_\↙______________
	newPoint1↗|  \
			   |  /Point1
			   | /
			   |/
			  Point2
	=========================================================
		pointCount == 2
			   | Point0
			   |\
			   | \
			   |  \ Point1
	___________|__/_______________  
			 ↗| /↖
	newPoint1  |/   newPoint2
			   Point2
	=========================================================
	Point0 Point1 Point2 为排序后的点
	newPoint1 newPoint2 为剪裁后得到的点
	两个三角形顶点分别为 
	triangle1 : Point0 newPoint1 newPoint2
	triangle2 : Point0 newPoint2 point1;
int WireframeTriangleClip(float C, float D, WireframeTriangle& triangle1, WireframeTriangle& triangle2);

//线框模式的近远平面剪裁 分别为 z = 0 和 z = 1;
//返回剪裁后的三角形
vector<WireframeTriangle> WireframeTriangleNearAndFarClip(WireframeTriangle triangle);

//先计算出2D点
//根据三角形顺序返回2D直线
//point0 point1
//point1 point2
//point2 point0
array<Line2D, 3> WireframeTriangleToLine2D(WireframeTriangle triangle);

vector<Line2D> GetNotRepeatingLine2D(const vector<WireframeTriangle>& wireframeTriangles);

//纹理三角形远近平面剪裁
//所得三角形纹理坐标 需要矫正
vector<TextureTriangle> TextureTriangleNearAndFarClip(TextureTriangle triangle);
*/

//获取一个像素的宽度
float GetPixelDelta(int pixelCount);

//像素坐标转换至屏幕坐标(中心点)
//[0,pixelCount - 1] 转换至 [-1,1]
//pixelCount 像素个数
//不判断越界
float PixelToScreen(int pixel, int pixelCount);

//屏幕坐标转化至像素坐标
//[-1,1] 转换至 [0, pixelCount - 1]
//pixelCount 像素个数
//不判断越界
int ScreenToPixel(float screen, int pixelCount);

//判断XY是否在[-1,1]
bool InScreenXY(Point2 point);

//判断Z是否在[0,1]
bool InScreenZ(float z);

//判断x y 是否在像素空间中
bool InPixelXY(int x, int y, int width, int height);

//可视空间 x * y * z == [-1,1) * [-1.1) * [0,1]
//左手坐标系
class Renderer {
public:
	Renderer(int width, int height);
	void Clear();
	RGBImage GenerateImage() const;

	//画白色线段 覆盖在图像最前面
	void DrawLine2D(Line2D line);

	//画白色线框 
	//覆盖在所有图像的最前面
	//顶点着色器可能对顶点进行改变
	//像素着色器没有影响 所以没有这个着色器
	//画出的线框是剪裁以后的
	template<typename Texture>
	void DrawTriangleByWireframe(const vector<Point3>& points,
								 const vector<Point2>& textureCoordinates,
								 const vector<Texture>& textures,
								 const vector<IndexData>& indexDatas,
								 function<Point4(Point3, Point2, const Texture&)> vertexShader);

	//画纹理三角形
	//Texture 自定义纹理类型 格式自定义
	//顶点法线 在纹理中实现
	//在两个着色器中实现各种效果
	//
	//vertexShader 顶点着色器
	//返回参数 Point4 经过矩阵转化后的其次坐标 注意保留 w 分量   
	//输入参数 Point3 原始世界坐标 
	//        Point2 原始纹理坐标 
	//        const Texture& 自定义纹理的引用
	//
	//pixelShader  像素着色器
	//返回参数 Color 输出颜色[0,1]=>[0,255] 小于0取0 大于1取255 
	//输入参数 Point4 经过重心三角形计算后的坐标 可以通过逆矩阵返回原始坐标
	//        Point2 经过重心三角形透视矫正后的纹理坐标
	//        const Texture& 自定义纹理的引用
	template<typename Texture>
	void DrawTriangleByTexture(const vector<Point3>& points,
							   const vector<Point2>& textureCoordinates,
							   const vector<Texture>& textures,
							   const vector<IndexData>& indexs,
							   function<Point4(Point3, Point2, const Texture&)> vertexShader,
							   function<Color(Point4, Point2, const Texture&)> pixelShader);
private:
	template<typename Texture>
	void HandlePixel(array<Point4, 3> points, array<Point2, 3> coordinates, const Texture& texture,
					 function<Color(Point4, Point2, const Texture&)> pixelShader);
	//这个用于绘制纹理三角形来画像素的
	void DrawZBuffer(int x, int y, float z, RGBColor color);
	//这个用于绘制白色直线覆盖在最前面的点
	void DrawWritePixel(int x, int y);
private:
	int width;
	int height;
	//储存深度
	//清空状态所有zbuffer时  2.0f
	//存在画三角形存储在     [0,1]
	//绘制白色直线的存储在   -1.0f
	//像素坐标系从左下角 一行一行往上计算
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
		auto p = Stream(array<int, 3>{0, 1, 2}, [&](int i) {
			return vertexShader(points[data.pointIndex[i]],
								textureCoordinates[data.textureCoordinateIndex[i]], textures[data.textureIndex]);
		});
		//线框模式下不进行背面消除 显示全部线段
		//得到剪裁后的三角形
		vector<array<Point4, 3>> triangles;
		TriangleNearAndFarClip(&p, &triangles);
		//获取不重复线段
		auto clippedLines = GetNotRepeatingLine2D(triangles);
		//绘制线段
		for (auto& clippedLine : clippedLines) {
			DrawLine2D(clippedLine);
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
		auto textureCoordinate = Stream(data.textureCoordinateIndex, [&](unsigned int i) {
			return textureCoordinates[i];
		});
		auto point4s = Stream(array<int, 3>{0, 1, 2}, [&](int i) {
			return vertexShader(points[data.pointIndex[i]], textureCoordinate[i], textures[data.textureIndex]);
		});
		//背面消除(逆时针消除) 若消除直接进入下一个循环
		auto point3s = Stream(point4s, [](const Point4& p) {
			return p.ToPoint3();
		});
		if (BackCulling(point3s)) {
			continue;
		}
		//远近平面剪裁 
		//纹理坐标进行插值矫正
		vector<array<Point4, 3>> trianglePoints;
		vector<array<Point2, 3>> triangleTextureCoodinates;
		TriangleNearAndFarClip(&point4s, &trianglePoints,
							   &textureCoordinate, &triangleTextureCoodinates, true);
		//计算每个像素的位置 
		//执行像素着色器
		//输出像素颜色
		for (int i = 0; i < trianglePoints.size(); i++) {
			HandlePixel(trianglePoints[i], triangleTextureCoodinates[i], textures[data.textureIndex], pixelShader);
		}
	}

}

template<typename Texture>
inline void Renderer::HandlePixel(array<Point4, 3> points, array<Point2, 3> coordinate, const Texture& texture,
								  function<Color(Point4, Point2, const Texture&)> pixelShader) {
	//获取三角形中顶点最大最小的x y值
	//用于计算需要绘制的边框
	array<float, 3> xValue = Stream(points, [](const Point4& p) {
		return p.x;
	});
	std::sort(xValue.begin(), xValue.end(), std::less<float>());
	array<float, 3> yValue = Stream(points, [](const Point4& p) {
		return p.y;
	});
	std::sort(yValue.begin(), yValue.end(), std::less<float>());
	//确定需要绘制的边界
	int xMax = std::min(ScreenToPixel(xValue[2], width), width - 1);
	int xMin = std::max(ScreenToPixel(xValue[0], width), 0);
	int yMax = std::min(ScreenToPixel(yValue[2], height), height - 1);
	int yMin = std::max(ScreenToPixel(yValue[0], height), 0);
	//三个点映射至齐次坐标
	array<Point3, 3> point3s = Stream(points, [](const Point4& p) {
		return p.ToPoint3();
	});
	//循环限定矩形 [xMin,xMax] * [yMin,yMax]
	for (int yIndex = yMin; yIndex <= yMax; yIndex++) {
		for (int xIndex = xMin; xIndex <= xMax; xIndex++) {
			Point2 screenPoint{
				PixelToScreen(xIndex, width),
				PixelToScreen(yIndex, width)
			};
			//得到屏幕坐标
			auto point2s = Stream(point3s, [](const Point3& p) {
				return p.GetPoint2();
			});
			//计算重心系数
			array<float, 3> coefficient = ComputeCenterCoefficient(screenPoint, point2s);
			bool inTriangle = std::all_of(coefficient.begin(), coefficient.end(), [](float f) {
				return f > 0.0f;
			});
			//在三角形内部
			if (inTriangle) {
				//使用重心坐标计算出像素位置对应的齐次坐标点
				Point4 pixelPoint = ComputeCenterPoint(
					coefficient, points
				);
				//所有点的w坐标
				auto pointsW = Stream(points, [](const Point4& p) {
					return p.w;
				});
				//矫正后的纹理坐标
				Point2 textureCoordinate = ComputeCenterTextureCoordinate(
					coefficient, coordinate, pointsW
				);
				//执行像素着色器
				Color color = pixelShader(pixelPoint, textureCoordinate, texture);
				//写入zBuffer
				float depth = pixelPoint.z / pixelPoint.w;
				//浮点数精度问题 需要限制到[0,1]
				std::clamp(depth, 0.0f, 1.0f);
				DrawZBuffer(xIndex, yIndex, depth, ColorToRGBColor(color));
			}
		}
	}
}