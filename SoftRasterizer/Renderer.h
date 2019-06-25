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
							 const function<Point4(Point3)>& vertexShader);

	/*
		画白色线框 
		覆盖在所有图像的最前面
		画出的线框是剪裁以后的
	*/
	void DrawTriangleByWireframe(const vector<Point3>& points,
								 const vector<WireframeIndexData>& indexDatas,
								 const function<Point4(Point3)>& vertexShader);

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
							   const function<Point4(Point3, Point2, const Texture&)>& vertexShader,
							   const function<Color(Point4, Point2, const Texture&)>& pixelShader);
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
void HandleLine(int width, int height, const Array<Point2, 2>& line,
				const function<void(int, int)>& func);
//画三角形像素
void HandleTriangle(int width, int height, const Array<Point4, 3>& points,
					const function<void(int, int, Array<float, 3>)>& howToUseCoefficient);


/*
	顺时针不消除 逆时针消除
	消除返回true
*/
bool BackCulling(const Array<Point4, 3>& points);

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
bool ScreenLineEqual(const Array<Point2, 2>& pointsA,const Array<Point2, 2>& pointsB);

/*
	计算超平面上的点
*/
Point4 ComputePlanePoint(Vector4 vector,const Array<Point4, 2>& points);

/*
	计算重心系数
*/
Array<float, 3> ComputeCenterCoefficient(Point2 point,const Array<Point2, 3>& points);

/*
	重心计算颜色 这个是根据w算的 不是直接重心计算
*/
Color ComputerCenterColor(const Array<float, 3>& coefficients, 
						  const Array<Color, 3>& colors, 
						  const Array<float,3>& pointW);

/*
	重心计算顶点
*/
Point4 ComputeCenterPoint(const Array<float, 3>& coefficients, 
						  const Array<Point4, 3>& points);

/*
	重心计算纹理坐标
*/
Point2 ComputeCenterTextureCoordinate(const Array<float, 3>& coefficients,
									  const Array<Point2, 3>& textureCoordinates,
									  const Array<float, 3>& pointW);

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
	三角形近平面剪裁和背面消除 
	Color 和 纹理坐标可以用一样的逻辑 这里设置成模板
*/
template<typename TriangleData>
MaxCapacityArray<std::pair<Array<Point4, 3>, Array<TriangleData, 3>>, 2> TriangleNearClipAndBackCulling(
	const Array<Point4, 3>& points, const Array<TriangleData, 3>& triangleDatas) {
	//近平面
	constexpr Vector4 nearPlane{0.0f, 0.0f, -1.0f, 0.0f};
	auto pointColors = ArrayIndex<3>().Stream([&](int i) {
		return std::pair{std::pair{points[i], triangleDatas[i]}, 0.0f};
	});
	//获得和近平面的距离
	for (auto& pointColor : pointColors) {
		pointColor.second = pointColor.first.first.GetVector4().Dot(nearPlane);
	}
	/*
		因为顶点需要排序 得到对应顺序后剪裁
		每次进行一次交换 代表三角形顶点顺时针逆时针将会转变
		一个三角形最多需要交换三次顶点
		可以根据距离排好序
		根据4种情况推导出来 !(BackCulling(triangle) ^ reverse) 可以确定是否需要绘制
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
	//可视空间方向的点
	int pointCount = static_cast<int>(std::count_if(pointColors.begin(), pointColors.end(), [](auto& pointBool) {
		return pointBool.second <= 0.0f;
	}));
	//需要返回的数据
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
		  _____________|_\↙______________
			newPoint1↗|  \
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
		//替换成剪裁后的点 不改变顺序
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
					 ↗| /↖
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
		//替换成剪裁后的点 不改变顺序
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
		//没有三角形要添加
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
		//近平面剪裁并且背面消除 由于顶点顺序原因需要放在一起
		auto trianglePoints = TriangleNearClipAndBackCulling<Point2>(mainPoints, mainTextureCoodinates);
		//得到纹理坐标
		for (auto& trianglePoint : trianglePoints) {
			auto triangleP = trianglePoint.first;
			auto triangleT = trianglePoint.second;
			auto triangleW = triangleP.Stream([](Point4 p) {
				return p.w;
			});
			//光栅化阶段
			HandleTriangle(width, height, triangleP, [&](int x, int y, Array<float, 3> coefficent) {
				auto point = ComputeCenterPoint(coefficent, triangleP);
				//判断深度
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