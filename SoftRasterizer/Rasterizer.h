#pragma once
#include <vector>
#include <array>
#include <functional>
#include "Assertion.h"
#include "RGBImage.h"
#include "RasterizationAlgorithm.h"
using std::function;
using std::vector;
using std::array;
using std::pair;
using std::tuple;
using std::get;

//默认的时候全部都是0
struct DataIndex {
	array<unsigned, 3> point{0, 0, 0};
	array<unsigned, 3> coordinate{0, 0, 0};
	array<unsigned, 3> normal{0, 0, 0};
	array<unsigned, 3> color{0, 0, 0};
	unsigned texture = 0;
};


class Rasterizer {
public:
	Rasterizer(unsigned width, unsigned height);
	float GetAspectRatio() const;
	void Clear();
	RGBImage GenerateRGBImage() const;

	/*
		返回值 Point4 转化后的顶点
		参数 Point3   传入的顶点
		参数 Point2&  传入的纹理坐标的备份 
		参数 Vector3& 传入的法线的备份 
		参数 Color&   传入的颜色的备份 (修改此颜色用于实现Gouraud Shading)
		参数 const Texture& 对应绑定的纹理 (也可以是其他数据)
	*/
	template<typename Texture>
	using VertexShader = function<Point4(Point4, Point2&, Vector3&, Color&, const Texture&)>;

	/*
		返回值 Color 经过计算后像素上的颜色 (在这里做gamma矫正)
		参数 Point4  像素位置对应的坐标 (可以通过逆矩阵返回相机空间)
		参数 Point2  经过插值计算的纹理坐标 (双线性滤波在这里实现)
		参数 Vector3 经过插值计算的法线
		参数 Color   经过插值计算的颜色
		参数 const Texture& 对应绑定的纹理
	*/
	template<typename Texture>
	using PixelShader = function<Color(Point4, Point2, Vector3, Color, const Texture&)>;

	/*
		若不使用的元素也需要传入一个长度为1的vector
		index中填写为0
	*/
	template<typename Texture>
	void DrawTriangle(const vector<Point3>& points,
					  const vector<Point2>& coordinates,
					  const vector<Vector3>& normals,
					  const vector<Color>& colors,
					  const vector<Texture>& textures,
					  const vector<DataIndex>& indexs,
					  const VertexShader<Texture>& vertexShader,
					  const PixelShader<Texture>& pixelShader);

	/*
		接口参数和DrawTriangle一样只需要修改函数名称
		顶点着色器运行结果与DrawTriangle保持一致
		像素着色器不会使用
		绘制出来的点深度为-1 保证不会被覆盖
		颜色必定是白色
	*/
	template<typename Texture>
	void DrawWireframe(const vector<Point3>& points,
					   const vector<Point2>& coordinates,
					   const vector<Vector3>& normals,
					   const vector<Color>& colors,
					   const vector<Texture>& textures,
					   const vector<DataIndex>& indexs,
					   const VertexShader<Texture>& vertexShader,
					   const PixelShader<Texture>& pixelShader);
private:
	/*
		使用pair array tuple 导致代码难读
		这里使用using来消除
		.cpp文件内需要复制一份
	*/
	using VertexData = tuple<Point2, Vector3, Color>;
	using Vertex = pair<Point4, VertexData>;
	using VertexTriangle = array<Vertex, 3>;
	using ScreenLine = array<Point2, 2>;

	/*
		分别对应两种模式的绘制
	*/
	void DrawZBuffer(unsigned x, unsigned y, float depth, Color color);
	void DrawWritePixel(unsigned x, unsigned y);

	/*
	通过两个点直接计算出平面上的点
	p0 和 p1 需要在平面两侧
*/
	Point4 CalculatePlanePoint(Vector4 plane, Point4 p0, Point4 p1) const;

	/*
		世界空间直接用线性插值计算
		计算所有的数据的到新顶点
		满足 p = (1 - t) * p0 + t * p1;
	*/
	Vertex CalculateInterpolationVertex(float t, const Vertex& p0, const Vertex& p1) const;

	/*
		使用重心系数计算顶点数据
		这里使用纹理矫正的方法计算
		需要使用对应三角形的w坐标
	*/
	VertexData CalculateVertexData(const GravityCoefficient& coefficients,
								   const VertexTriangle& vertexs) const;

	/*
		步骤如下
		1 近平面剪裁,得到顶点和插值系数
		2 背面消除
		3 计算线性插值
		4 加入容器返回
	*/
	vector<VertexTriangle> TriangleNearPlaneClipAndBackCull(const VertexTriangle& vertexs) const;

	/*
		步骤如下
		1 近平面剪裁得到新顶点
		2 远平面剪裁得到新顶点
		3 获得直线,近平面剪裁需要分解成三角形的边,远平面剪裁只需要得到远平面的那条直线
		4 放入容器返回
	*/
	vector<ScreenLine> WireframeNearFarPlaneClipAndGetLines(const PointTriangle& points) const;

	/*
		Liang-Barsky直线段裁剪
		保证端点在[-1, 1] * [-1, 1]
	*/
	bool LineClip(ScreenLine& points) const;

	/*
		middle-Point算法画直线
	*/
	void DrawLine(const ScreenLine& points);
private:
	Texture2D<pair<float, Color>> zBuffer;
};

template<typename Texture>
inline void Rasterizer::DrawTriangle(const vector<Point3>& points,
									 const vector<Point2>& coordinates,
									 const vector<Vector3>& normals,
									 const vector<Color>& colors,
									 const vector<Texture>& textures,
									 const vector<DataIndex>& indexs,
									 const VertexShader<Texture>& vertexShader,
									 const PixelShader<Texture>& pixelShader) {
	for (const auto& index : indexs) {
		assertion(std::all_of(index.point.begin(), index.point.end(), [&](unsigned i) {
			return i < points.size();
		}));
		assertion(std::all_of(index.coordinate.begin(), index.coordinate.end(), [&](unsigned i) {
			return i < coordinates.size();
		}));
		assertion(std::all_of(index.normal.begin(), index.normal.end(), [&](unsigned i) {
			return i < normals.size();
		}));
		assertion(std::all_of(index.color.begin(), index.color.end(), [&](unsigned i) {
			return i < colors.size();
		}));
		assertion(index.texture < textures.size());

		const Texture& texture = textures[index.texture];

		VertexTriangle triangleVertexs;
		for (int i = 0; i < 3; i++) {
			triangleVertexs[i].first = points[index.point[i]].ToPoint4();
			auto vertexData = VertexData{
				coordinates[index.coordinate[i]],
				normals[index.normal[i]],
				colors[index.color[i]]
			};
			triangleVertexs[i].second = vertexData;
		}

		PointTriangle trianglePoints;
		for (int i = 0; i < 3; i++) {
			trianglePoints[i] = vertexShader(
				triangleVertexs[i].first,
				get<0>(triangleVertexs[i].second),
				get<1>(triangleVertexs[i].second),
				get<2>(triangleVertexs[i].second),
				texture
			);
			triangleVertexs[i].first = trianglePoints[i];
		}

		if (ViewVolumnCull(trianglePoints)) {
			continue;
		}

		auto clipTriangleVertexs = TriangleNearPlaneClipAndBackCull(triangleVertexs);
		for (const auto& clipTriangleVertex : clipTriangleVertexs) {

			array<Point4, 3> clipTrianglePoints;
			for (int i = 0; i < 3; i++) {
				clipTrianglePoints[i] = clipTriangleVertex[i].first;
			}


			TriangleRasterization(zBuffer.GetWidth(), zBuffer.GetHeight(),
								  clipTrianglePoints, [&](unsigned x, unsigned y, float depth,
								  const GravityCoefficient& coefficient) {
				if (depth < zBuffer.GetScreenPoint(x, y).first) {
					auto point = CalculatePointByCoefficient(coefficient, clipTrianglePoints);
					auto triangleData = CalculateVertexData(coefficient, clipTriangleVertex);
					auto coordinate = get<0>(triangleData);
					auto normal = get<1>(triangleData);
					auto color = get<2>(triangleData);
					auto pixelColor = pixelShader(point, coordinate, normal, color, texture);
					DrawZBuffer(x, y, depth, pixelColor);
				}
			});
		}
	}
}

template<typename Texture>
inline void Rasterizer::DrawWireframe(const vector<Point3>& points,
									  const vector<Point2>& coordinates,
									  const vector<Vector3>& normals,
									  const vector<Color>& colors,
									  const vector<Texture>& textures,
									  const vector<DataIndex>& indexs,
									  const VertexShader<Texture>& vertexShader,
									  const PixelShader<Texture>& pixelShader) {
	for (const auto& index : indexs) {
		assertion(std::all_of(index.point.begin(), index.point.end(), [&](unsigned i) {
			return i < points.size();
		}));
		assertion(std::all_of(index.coordinate.begin(), index.coordinate.end(), [&](unsigned i) {
			return i < coordinates.size();
		}));
		assertion(std::all_of(index.normal.begin(), index.normal.end(), [&](unsigned i) {
			return i < normals.size();
		}));
		assertion(std::all_of(index.color.begin(), index.color.end(), [&](unsigned i) {
			return i < colors.size();
		}));
		assertion(index.texture < textures.size());

		VertexTriangle triangleVertexs;
		for (int i = 0; i < 3; i++) {
			triangleVertexs[i].first = points[index.point[i]].ToPoint4();
			auto vertexData = VertexData{
				coordinates[index.coordinate[i]],
				normals[index.normal[i]],
				colors[index.color[i]]
			};
			triangleVertexs[i].second = vertexData;
		}

		PointTriangle trianglePoints;
		for (int i = 0; i < 3; i++) {
			trianglePoints[i] = vertexShader(
				triangleVertexs[i].first,
				get<0>(triangleVertexs[i].second),
				get<1>(triangleVertexs[i].second),
				get<2>(triangleVertexs[i].second),
				textures[index.texture]
			);
		}

		if (ViewVolumnCull(trianglePoints)) {
			continue;
		}

		auto lines = WireframeNearFarPlaneClipAndGetLines(trianglePoints);
		for (auto line : lines) {
			if (LineClip(line)) {
				DrawLine(line);
			}
		}
	}
}
