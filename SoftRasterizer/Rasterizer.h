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
using std::get;

//默认的时候全部都是0
struct IndexData {
	array<size_t, 3> vertex{0, 0, 0};
	array<size_t, 3> coordinate{0, 0, 0};
	array<size_t, 3> normal{0, 0, 0};
	array<size_t, 3> color{0, 0, 0};
	size_t texture = 0;
};


class Rasterizer {
public:
	Rasterizer(PixelPointRange range);
	void Clear();
	RGBImage GenerateRGBImage() const;

	/*
		返回值 Point4 转化后的顶点
		参数 Point3   传入的顶点
		参数 Point2&  传入的纹理坐标的备份 
		参数 Vector3& 传入的法线的备份 
		参数 Color&   传入的颜色的备份 (修改此颜色用于实现Gouraud Shading)
		参数 const Texture& 对应绑定的纹理 (可以用于扰动 Point2& Vector3& Color& 计算插值时会使用)
	*/
	template<typename Texture>
	using VertexShader = function<Point4(Point3, Point2&, Vector3&, Color&, const Texture&)>;

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
	void DrawTriangle(const vector<Point3>& vertexs,
					  const vector<Point2>& coordinates,
					  const vector<Vector3>& normals,
					  const vector<Color>& colors,
					  const vector<Texture>& textures,
					  const vector<IndexData>& indexs,
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
	void DrawWireframe(const vector<Point3>& vertexs,
					   const vector<Point2>& coordinates,
					   const vector<Vector3>& normals,
					   const vector<Color>& colors,
					   const vector<Texture>& textures,
					   const vector<IndexData>& indexs,
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
	using PointTriangle = array<Point4, 3>;
	using GravityCoefficient = array<float, 3>;
	using ScreenTriangle = array<Point2, 3>;
	using ScreenLine = array<Point2, 2>;

	/*
		分别对应两种模式的绘制
	*/
	void DrawZBuffer(ScreenPixelPoint point, float depth, Color color);
	void DrawWritePixel(ScreenPixelPoint point);


	/*
		将齐次顶点转化成屏幕上的点
		point(x/w, y/w)
	*/
	Point2 ConvertToScreenPoint(Point4 p) const;

	/*
		p0 p1 为超平面两侧的点
		返回平面上的线性插值系数t
		满足式子 p = (1 - t) * p0 + t * p1;

		向量plane(A,B,C,D)代表平面
		表达式  Ax + By + Cz + Dw = 0

		左平面  -x           + lw = 0     plane(-1,  0,  0, -1)
		右平面   x           - rw = 0     plane( 1,  0,  0, -1)
		下平面       -y      + bw = 0     plane( 0, -1,  0, -1)
		上平面        y      - tw = 0     plane( 0,  1,  0, -1)
		近平面            -z + nw = 0     plane( 0,  0, -1,  0)
		远平面             z - fw = 0     plane( 0,  0,  1, -1)
	*/
	float CalculatePlaneInterpolationCoefficient(Vector4 plane, Point4 p0, Point4 p1) const;


	/*
		世界空间直接用线性插值计算
		计算所有的数据的到新顶点
		满足 p = (1 - t) * p0 + t * p1;
	*/
	Vertex CalculateInterpolationVertex(float t, const Vertex& p0, const Vertex& p1) const;

	/*
		通过两个点直接计算出平面上的点
		p0 和 p1 需要在平面两侧
	*/
	Point4 CalculatePlanePoint(Vector4 plane, Point4 p0, Point4 p1) const;

	/*
		带入上面6个平面的向量
		计算顶点到平面上的距离
		这里的距离仅仅判断在平面的哪个方位
		distance > 0   point在剪裁空间外
		distance = 0   point在超平面上
		distance < 0   point在剪裁空间方向
	*/
	float CalculatePlaneDistance(Vector4 plane, Point4 point) const;

	/*
		根据梯度方向返回在直线上方还是下方
		带入直线方程式得到的数值
		p0 p1 为直线端点
		p为需要判断的顶点
	*/
	float CalculateLineEquation(Point2 p, Point2 p0, Point2 p1) const;

	/*
		所有顶点都在剪裁空间外面的三角形将直接被剔除
		返回true则代表消除
	*/
	bool ViewVolumnCull(const PointTriangle& points) const;

	/*
		逆时针消除 根据传入顶点的顺序
		返回true则代表消除
	*/
	bool BackCull(const PointTriangle& points) const;

	/*
		使用重心系数计算顶点
		这里直接使用线性来计算
	*/
	Point4 CalculatePointByCoefficient(const GravityCoefficient& coefficients,
									   const PointTriangle& points) const;

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


	using CalculateScreenPoint = function<void(ScreenPixelPoint, const GravityCoefficient&)>;
	/*
		三角形光栅化步骤如下
		1 计算各像素位置
		2 根据位置生成 重心系数
		3 若该位置在三角形内调用function
	*/
	void TriangleRasterization(const ScreenTriangle& points,
							   const CalculateScreenPoint& useCoefficient);

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
	PixelPointRange range;
	vector<pair<float, Color>> zBuffer;
};

template<typename Texture>
inline void Rasterizer::DrawTriangle(const vector<Point3>& vertexs,
									 const vector<Point2>& coordinates,
									 const vector<Vector3>& normals,
									 const vector<Color>& colors,
									 const vector<Texture>& textures,
									 const vector<IndexData>& indexs,
									 const VertexShader<Texture>& vertexShader,
									 const PixelShader<Texture>& pixelShader) {
	for (const auto& index : indexs) {
		assertion(std::all_of(index.vertex.begin(), index.vertex.end(), [&](size_t i) {
			return i < vertexs.size();
		}));
		assertion(std::all_of(index.coordinate.begin(), index.coordinate.end(), [&](size_t i) {
			return i < coordinates.size();
		}));
		assertion(std::all_of(index.normal.begin(), index.normal.end(), [&](size_t i) {
			return i < normals.size();
		}));
		assertion(std::all_of(index.color.begin(), index.color.end(), [&](size_t i) {
			return i < colors.size();
		}));
		assertion(index.texture < textures.size());

		const Texture& texture = textures[index.texture];

		VertexTriangle triangleVertexs;
		for (int i = 0; i < 3; i++) {
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
				vertexs[index.vertex[i]],
				get<0>(triangleVertexs[i].second),
				get<1>(triangleVertexs[i].second),
				get<2>(triangleVertexs[i].second),
				texture
			);
		}

		if (ViewVolumnCull(trianglePoints)) {
			continue;
		}

		for (int i = 0; i < 3; i++) {
			triangleVertexs[i].first = trianglePoints[i];
		}

		auto clipTriangleVertexs = TriangleNearPlaneClipAndBackCull(triangleVertexs);
		for (const auto& clipTriangleVertex : clipTriangleVertexs) {

			ScreenTriangle screenPoints;
			for (int i = 0; i < 3; i++) {
				screenPoints[i] = ConvertToScreenPoint(clipTriangleVertex[i].first);
			}

			TriangleRasterization(screenPoints, [&](ScreenPixelPoint screenPoint, const GravityCoefficient& coefficient) {

				array<Point4, 3> clipTrianglePoints;
				for (int i = 0; i < 3; i++) {
					clipTrianglePoints[i] = clipTriangleVertex[i].first;
				}

				auto point = CalculatePointByCoefficient(coefficient, clipTrianglePoints);
				float depth = point.z / point.w;

				if (DepthInViewVolumn(depth)) {
					auto triangleData = CalculateVertexData(coefficient, clipTriangleVertex);
					auto coordinate = get<0>(triangleData);
					auto normal = get<1>(triangleData);
					auto color = get<2>(triangleData);
					auto pixelColor = pixelShader(point, coordinate, normal, color, texture);
					DrawZBuffer(screenPoint, depth, pixelColor);
				}
			});
		}
	}
}

template<typename Texture>
inline void Rasterizer::DrawWireframe(const vector<Point3>& vertexs,
									  const vector<Point2>& coordinates,
									  const vector<Vector3>& normals,
									  const vector<Color>& colors,
									  const vector<Texture>& textures,
									  const vector<IndexData>& indexs,
									  const VertexShader<Texture>& vertexShader,
									  const PixelShader<Texture>& pixelShader) {
	for (const auto& index : indexs) {
		assertion(std::all_of(index.vertex.begin(), index.vertex.end(), [&](size_t i) {
			return i < vertexs.size();
		}));
		assertion(std::all_of(index.coordinate.begin(), index.coordinate.end(), [&](size_t i) {
			return i < coordinates.size();
		}));
		assertion(std::all_of(index.normal.begin(), index.normal.end(), [&](size_t i) {
			return i < normals.size();
		}));
		assertion(std::all_of(index.color.begin(), index.color.end(), [&](size_t i) {
			return i < colors.size();
		}));
		assertion(index.texture < textures.size());

		VertexTriangle triangleVertexs;
		for (int i = 0; i < 3; i++) {
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
				vertexs[index.vertex[i]],
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
