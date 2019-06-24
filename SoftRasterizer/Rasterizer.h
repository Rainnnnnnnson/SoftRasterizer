#pragma once
#include<vector>
#include<array>
#include<functional>
#include"Assertion.h"
#include"RGBImage.h"
using std::function;
using std::vector;
using std::array;
using std::pair;
using std::tuple;

struct IndexData {
	array<int, 3> vertexIndex;
	array<int, 3> coodinateIndex;
	array<int, 3> normalIndex;
	array<int, 3> colorIndex;
	int textureIndex;
};


class Rasterizer {
public:
	Rasterizer(int width, int height);
	void Clear();
	RGBImage GenerateRGBImage() const;
	/*
		vertexs      顶点数组
		coodinates   纹理坐标数组
		normals      法线数组
		colors       颜色数组
		textures     纹理数组
		indexs       索引数组

		vertexShader 顶点着色器
		返回值 Point4 经过转化后的顶点坐标
		参数 Point3  传入的顶点
		参数 Point2  传入的纹理坐标
		参数 Vector3 传入的法线
		参数 Color   传入的颜色
		参数 const Texture& 对应绑定的纹理

		pixelShader 像素着色器
		返回值 Color 经过计算后像素上的颜色(在这里做gamma矫正)
		参数 Point4  像素位置对应的坐标(可以通过逆矩阵返回相机空间)
		参数 Point2  经过插值计算的纹理坐标(双线性滤波在这里实现)
		参数 Vector3 经过插值计算的法线
		参数 Color   经过插值计算的颜色
		参数 const Texture& 对应绑定的纹理

		若不使用的元素也需要传入一个长度为1的vector
		index中填写为0
	*/
	template<typename Texture>
	void DrawTriangle(const vector<Point3>& vertexs,
					  const vector<Point2>& coordinates,
					  const vector<Vector3>& normals,
					  const vector<Color>& colors,
					  const vector<Texture>& textures,
					  const vector<IndexData>& indexs,
					  const function<Point4(Point3, Point2, Vector3, Color, const Texture&)>& vertexShader,
					  const function<Color(Point4, Point2, Vector3, Color, const Texture&)>& pixelShader);

	/*
		接口和DrawTriangle一样只需要修改函数名称
		顶点着色器运行结果与DrawTriangle保持一致
		像素着色器不会使用
	*/
	template<typename Texture>
	void DrawWireframe(const vector<Point3>& vertexs,
					   const vector<Point2>& coordinates,
					   const vector<Vector3>& normals,
					   const vector<Color>& colors,
					   const vector<Texture>& textures,
					   const vector<IndexData>& indexs,
					   const function<Point4(Point3, Point2, Vector3, Color, const Texture&)>& vertexShader,
					   const function<Color(Point4, Point2, Vector3, Color, const Texture&)>& pixelShader);
private:
	//单元测试接口

	// x 从像素空间转化至屏幕空间 [0, width - 1] => [-1, 1]
	float XPixelToScreen(int x) const;
	// y 从像素空间转化至屏幕空间 [0, height - 1] => [-1, 1]
	float YPixelToScreen(int y) const;
	// x 从屏幕空间转化至像素空间 [-1, 1] => [0, width - 1]
	int XScreenToPixel(float x) const;
	// y 从屏幕空间转化至像素空间 [-1, 1] => [0, height - 1]
	int YScreenToPixel(float y) const;
	//判断 x 在[0, width - 1]中  y 在[0, height - 1]中
	bool XYInPixel(int x, int y) const;
	//判断 x 在[-1, 1]中 y 在[-1, 1]中
	bool XYInScreen(float x, float y) const;
	//判断 z 在 [0,1]中
	bool ZInViewVolumn(float z) const;




	//剪裁近平面 因为顶点顺序问题 需要同时进行背面剪裁
	vector<array<pair<Point4, tuple<Point2, Color, Vector3>>, 3>> TriangleNearPlaneClipAndBackCull(
		const array<pair<Point4, tuple<Point2, Color, Vector3>>, 3> & triangleData);

	//背面消除
	bool BackCulling(const array<Point2, 3> & vertexs);

	/*
		三角形光栅化 
		会生成像素坐标 和 重心系数
		调用function来使用 这样写主要是为了增强主体函数可读性
	*/
	void TriangleRasterization(const array<Point2, 3> & vertexs,
							   const function<void(int, int, const array<float, 3>&)>& useCoefficient);

	//通过屏幕的重心坐标计算插值 需要进行插值矫正
	pair<Point4, tuple<Point2, Color, Vector3>> CaculateCoefficientData(
		const array<pair<Point4, tuple<Point2, Color, Vector3>>, 3> & triangleData,
		const array<float, 3> & coefficients);

	//颜色写入ZBuffer
	void DrawZBuffer(int x, int y, float z, Color color);





	//近平面剪裁 顶点结果与非线框模式保持一致 
	vector<array<Point4, 3>> WireframeNearPlaneClip(const array<Point4, 3> & vertexs);

	/*
		远平面剪裁 这里并不是剪裁成三角形
		非线框模式下根据深度进行消除或保留
		这里只在远平面 留一条线 并且将不重复线段 加入至其中
	*/
	vector<array<Point4, 2>> WireframeFarPlaneClipAndGetNotRepeatingLines();

	/*
		Liang-Barsky直线段裁剪
		获得线段在[-1,1] * [-1,1]中
		返回false 则线完全在屏幕外面 不需要绘制该直线
	*/
	bool LineClip(array<Point2, 2> & line);

	//mid-Point算法画直线
	void DrawLine(const array<Point2, 2> & line);

	//线框模式下绘制白色顶点 会覆盖在ZBuffer最前面 无法被其他元素覆盖
	void DrawWritePixel(int x, int y);
private:
	int width;
	int height;
	vector<pair<float, Color>> zBuffer;
};

template<typename Texture>
inline void Rasterizer::DrawTriangle(const vector<Point3>& vertexs, 
									 const vector<Point2>& coordinates, 
									 const vector<Vector3>& normals, 
									 const vector<Color>& colors, 
									 const vector<Texture>& textures, 
									 const vector<IndexData>& indexs, 
									 const function<Point4(Point3, Point2, Vector3, Color, const Texture&)>& vertexShader, 
									 const function<Color(Point4, Point2, Vector3, Color, const Texture&)>& pixelShader) {

}

template<typename Texture>
inline void Rasterizer::DrawWireframe(const vector<Point3>& vertexs, 
									  const vector<Point2>& coordinates, 
									  const vector<Vector3>& normals, 
									  const vector<Color>& colors, 
									  const vector<Texture>& textures, 
									  const vector<IndexData>& indexs, 
									  const function<Point4(Point3, Point2, Vector3, Color, const Texture&)>& vertexShader, 
									  const function<Color(Point4, Point2, Vector3, Color, const Texture&)>& pixelShader) {

}
