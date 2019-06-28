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

//Ĭ�ϵ�ʱ��ȫ������0
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
		����ֵ Point4 ת����Ķ���
		���� Point3   ����Ķ���
		���� Point2&  �������������ı��� 
		���� Vector3& ����ķ��ߵı��� 
		���� Color&   �������ɫ�ı��� (�޸Ĵ���ɫ����ʵ��Gouraud Shading)
		���� const Texture& ��Ӧ�󶨵����� (���������Ŷ� Point2& Vector3& Color& �����ֵʱ��ʹ��)
	*/
	template<typename Texture>
	using VertexShader = function<Point4(Point3, Point2&, Vector3&, Color&, const Texture&)>;

	/*
		����ֵ Color ��������������ϵ���ɫ (��������gamma����)
		���� Point4  ����λ�ö�Ӧ������ (����ͨ������󷵻�����ռ�)
		���� Point2  ������ֵ������������� (˫�����˲�������ʵ��)
		���� Vector3 ������ֵ����ķ���
		���� Color   ������ֵ�������ɫ
		���� const Texture& ��Ӧ�󶨵�����
	*/
	template<typename Texture>
	using PixelShader = function<Color(Point4, Point2, Vector3, Color, const Texture&)>;

	/*
		����ʹ�õ�Ԫ��Ҳ��Ҫ����һ������Ϊ1��vector
		index����дΪ0
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
		�ӿڲ�����DrawTriangleһ��ֻ��Ҫ�޸ĺ�������
		������ɫ�����н����DrawTriangle����һ��
		������ɫ������ʹ��
		���Ƴ����ĵ����Ϊ-1 ��֤���ᱻ����
		��ɫ�ض��ǰ�ɫ
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
		ʹ��pair array tuple ���´����Ѷ�
		����ʹ��using������
		.cpp�ļ�����Ҫ����һ��
	*/
	using VertexData = tuple<Point2, Vector3, Color>;
	using Vertex = pair<Point4, VertexData>;
	using VertexTriangle = array<Vertex, 3>;
	using PointTriangle = array<Point4, 3>;
	using GravityCoefficient = array<float, 3>;
	using ScreenTriangle = array<Point2, 3>;
	using ScreenLine = array<Point2, 2>;

	/*
		�ֱ��Ӧ����ģʽ�Ļ���
	*/
	void DrawZBuffer(ScreenPixelPoint point, float depth, Color color);
	void DrawWritePixel(ScreenPixelPoint point);


	/*
		����ζ���ת������Ļ�ϵĵ�
		point(x/w, y/w)
	*/
	Point2 ConvertToScreenPoint(Point4 p) const;

	/*
		p0 p1 Ϊ��ƽ������ĵ�
		����ƽ���ϵ����Բ�ֵϵ��t
		����ʽ�� p = (1 - t) * p0 + t * p1;

		����plane(A,B,C,D)����ƽ��
		���ʽ  Ax + By + Cz + Dw = 0

		��ƽ��  -x           + lw = 0     plane(-1,  0,  0, -1)
		��ƽ��   x           - rw = 0     plane( 1,  0,  0, -1)
		��ƽ��       -y      + bw = 0     plane( 0, -1,  0, -1)
		��ƽ��        y      - tw = 0     plane( 0,  1,  0, -1)
		��ƽ��            -z + nw = 0     plane( 0,  0, -1,  0)
		Զƽ��             z - fw = 0     plane( 0,  0,  1, -1)
	*/
	float CalculatePlaneInterpolationCoefficient(Vector4 plane, Point4 p0, Point4 p1) const;


	/*
		����ռ�ֱ�������Բ�ֵ����
		�������е����ݵĵ��¶���
		���� p = (1 - t) * p0 + t * p1;
	*/
	Vertex CalculateInterpolationVertex(float t, const Vertex& p0, const Vertex& p1) const;

	/*
		ͨ��������ֱ�Ӽ����ƽ���ϵĵ�
		p0 �� p1 ��Ҫ��ƽ������
	*/
	Point4 CalculatePlanePoint(Vector4 plane, Point4 p0, Point4 p1) const;

	/*
		��������6��ƽ�������
		���㶥�㵽ƽ���ϵľ���
		����ľ�������ж���ƽ����ĸ���λ
		distance > 0   point�ڼ��ÿռ���
		distance = 0   point�ڳ�ƽ����
		distance < 0   point�ڼ��ÿռ䷽��
	*/
	float CalculatePlaneDistance(Vector4 plane, Point4 point) const;

	/*
		�����ݶȷ��򷵻���ֱ���Ϸ������·�
		����ֱ�߷���ʽ�õ�����ֵ
		p0 p1 Ϊֱ�߶˵�
		pΪ��Ҫ�жϵĶ���
	*/
	float CalculateLineEquation(Point2 p, Point2 p0, Point2 p1) const;

	/*
		���ж��㶼�ڼ��ÿռ�����������ν�ֱ�ӱ��޳�
		����true���������
	*/
	bool ViewVolumnCull(const PointTriangle& points) const;

	/*
		��ʱ������ ���ݴ��붥���˳��
		����true���������
	*/
	bool BackCull(const PointTriangle& points) const;

	/*
		ʹ������ϵ�����㶥��
		����ֱ��ʹ������������
	*/
	Point4 CalculatePointByCoefficient(const GravityCoefficient& coefficients,
									   const PointTriangle& points) const;

	/*
		ʹ������ϵ�����㶥������
		����ʹ����������ķ�������
		��Ҫʹ�ö�Ӧ�����ε�w����
	*/
	VertexData CalculateVertexData(const GravityCoefficient& coefficients,
								   const VertexTriangle& vertexs) const;

	/*
		��������
		1 ��ƽ�����,�õ�����Ͳ�ֵϵ��
		2 ��������
		3 �������Բ�ֵ
		4 ������������
	*/
	vector<VertexTriangle> TriangleNearPlaneClipAndBackCull(const VertexTriangle& vertexs) const;


	using CalculateScreenPoint = function<void(ScreenPixelPoint, const GravityCoefficient&)>;
	/*
		�����ι�դ����������
		1 ���������λ��
		2 ����λ������ ����ϵ��
		3 ����λ�����������ڵ���function
	*/
	void TriangleRasterization(const ScreenTriangle& points,
							   const CalculateScreenPoint& useCoefficient);

	/*
		��������
		1 ��ƽ����õõ��¶���
		2 Զƽ����õõ��¶���
		3 ���ֱ��,��ƽ�������Ҫ�ֽ�������εı�,Զƽ�����ֻ��Ҫ�õ�Զƽ�������ֱ��
		4 ������������
	*/
	vector<ScreenLine> WireframeNearFarPlaneClipAndGetLines(const PointTriangle& points) const;

	/*
		Liang-Barskyֱ�߶βü�
		��֤�˵���[-1, 1] * [-1, 1]
	*/
	bool LineClip(ScreenLine& points) const;

	/*
		middle-Point�㷨��ֱ��
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
