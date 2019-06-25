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

struct IndexData {
	array<int, 3> vertex;
	array<int, 3> coordinate;
	array<int, 3> normal;
	array<int, 3> color;
	int texture;
};


class Rasterizer {
public:
	Rasterizer(int width, int height);
	void Clear();
	RGBImage GenerateRGBImage() const;
	/*
		vertexs      ��������
		coodinates   ������������
		normals      ��������
		colors       ��ɫ����
		textures     ��������
		indexs       ��������

		vertexShader ������ɫ��
		����ֵ Point4 ����ת����Ķ�������
		���� Point3  ����Ķ���
		���� Point2  �������������
		���� Vector3 ����ķ���
		���� Color   �������ɫ
		���� const Texture& ��Ӧ�󶨵�����

		pixelShader ������ɫ��
		����ֵ Color ��������������ϵ���ɫ(��������gamma����)
		���� Point4  ����λ�ö�Ӧ������(����ͨ������󷵻�����ռ�)
		���� Point2  ������ֵ�������������(˫�����˲�������ʵ��)
		���� Vector3 ������ֵ����ķ���
		���� Color   ������ֵ�������ɫ
		���� const Texture& ��Ӧ�󶨵�����

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
					  const function<Point4(Point3, Point2, Vector3, Color, const Texture&)>& vertexShader,
					  const function<Color(Point4, Point2, Vector3, Color, const Texture&)>& pixelShader);

	/*
		�ӿں�DrawTriangleһ��ֻ��Ҫ�޸ĺ�������
		������ɫ�����н����DrawTriangle����һ��
		������ɫ������ʹ��
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
	//��Ԫ���Խӿ� ����������publicȨ�� ���� #define ������

	// x �����ؿռ�ת������Ļ�ռ� [0, width - 1] => [-1, 1]
	float XPixelToScreen(int x) const;
	// y �����ؿռ�ת������Ļ�ռ� [0, height - 1] => [-1, 1]
	float YPixelToScreen(int y) const;
	// x ����Ļ�ռ�ת�������ؿռ� [-1, 1] => [0, width - 1]
	int XScreenToPixel(float x) const;
	// y ����Ļ�ռ�ת�������ؿռ� [-1, 1] => [0, height - 1]
	int YScreenToPixel(float y) const;
	//�ж� x ��[0, width - 1]��  y ��[0, height - 1]��
	bool XYInPixel(int x, int y) const;
	//�ж� x ��[-1, 1]�� y ��[-1, 1]��
	bool XYInScreen(float x, float y) const;
	//�ж� z �� [0,1]��
	bool ZInViewVolumn(float z) const;




	//���ý�ƽ�� ��Ϊ����˳������ ��Ҫͬʱ���б������
	vector<array<pair<Point4, tuple<Point2, Vector3, Color>>, 3>> TriangleNearPlaneClipAndBackCull(
		const array<pair<Point4, tuple<Point2, Vector3, Color>>, 3> & triangleData);
	/*
		ͨ��ƽ������ĵ���� ��ƽ��Ľ���
	    ��ƽ�� N = (0, 0, -1, 0);
		Զƽ�� N = (0, 0, 1, -1);
	*/
	Point4 ComputePlanePoint(Vector4 N, const array<Point4, 2> & points);
	//��������
	bool BackCull(const array<Point2, 3> & vertexs);

	/*
		�����ι�դ�� 
		�������������� �� ����ϵ��
		����function��ʹ�� ����д��Ҫ��Ϊ����ǿ���庯���ɶ���
	*/
	void TriangleRasterization(const array<Point2, 3> & vertexs,
							   const function<void(int, int, const array<float, 3>&)>& useCoefficient);

	//ͨ����Ļ��������������ֵ ��Ҫ���в�ֵ����
	pair<Point4, tuple<Point2, Vector3, Color>> CaculateCoefficientData(
		const array<pair<Point4, tuple<Point2, Vector3, Color>>, 3> & triangleData,
		const array<float, 3> & coefficients);

	//��ɫд��ZBuffer
	void DrawZBuffer(int x, int y, float z, Color color);





	//��ƽ����� ����������߿�ģʽ����һ�� 
	vector<array<Point4, 3>> WireframeNearPlaneClip(const array<Point4, 3> & vertexs);

	/*
		Զƽ����� ���ﲢ���Ǽ��ó�������
		���߿�ģʽ�¸�����Ƚ�����������
		����ֻ��Զƽ�� ��һ���� ���ҽ����ظ��߶� ����������
	*/
	vector<array<Point2, 2>> WireframeFarPlaneClipAndGetNotRepeatingLines(const vector<array<Point4, 3>> & vertexs);

	/*
		Liang-Barskyֱ�߶βü�
		����߶���[-1,1] * [-1,1]��
		����false ������ȫ����Ļ���� ����Ҫ���Ƹ�ֱ��
	*/
	bool LineClip(array<Point2, 2> & line);

	//mid-Point�㷨��ֱ��
	void DrawLine(const array<Point2, 2> & line);

	//�߿�ģʽ�»��ư�ɫ���� �Ḳ����ZBuffer��ǰ�� �޷�������Ԫ�ظ���
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
	for (const auto& index : indexs) {
		assert(std::all_of(index.vertex.begin(), index.vertex.end(), [&](int i) {
			return i >= 0 && i < vertexs.size();
		}));
		assert(std::all_of(index.coordinate.begin(), index.coordinate.end(), [&](int i) {
			return i >= 0 && i < coordinates.size();
		}));
		assert(std::all_of(index.normal.begin(), index.normal.end(), [&](int i) {
			return i >= 0 && i < normals.size();
		}));
		assert(std::all_of(index.color.begin(), index.color.end(), [&](int i) {
			return i >= 0 && i < colors.size();
		}));
		assert(index.texture >= 0 && index.texture < textures.size());
		array<Point4, 3> mainVertexs;
		for (int i = 0; i < 3; i++) {
			mainVertexs[i] = vertexShader(
				vertexs[index.vertex[i]],
				coordinates[index.coordinate[i]],
				normals[index.normal[i]],
				colors[index.color[i]],
				textures[index.texture]
			);
		}
		array<pair<Point4, tuple<Point2, Vector3, Color>>, 3> triangle;
		for (int i = 0; i < 3; i++) {
			triangle[i] = {
				mainVertexs[i], {coordinates[index.coordinate[i]], normals[index.normal[i]], colors[index.color[i]]}
			};
		}
		auto clipTriangles = TriangleNearPlaneClipAndBackCull(triangle);
		for (const auto& clipTriangle : clipTriangles) {
			array<Point2, 3> screenTriangle;
			for (int i = 0; i < 3; i++) {
				screenTriangle[i] = clipTriangle[i].first.ToPoint2();
			}
			TriangleRasterization(screenTriangle, [&](int x, int y, const array<float, 3> & coefficient) {
				auto data = CaculateCoefficientData(clipTriangle, coefficient);
				float depth = data.first.z / data.first.w;
				if (ZInViewVolumn(depth)) {
					Color color = pixelShader(data.first, get<0>(data.second), get<1>(data.second),
											  get<2>(data.second), textures[index.texture]);
					DrawZBuffer(x, y, depth, color);
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
									  const function<Point4(Point3, Point2, Vector3, Color, const Texture&)>& vertexShader, 
									  const function<Color(Point4, Point2, Vector3, Color, const Texture&)>& pixelShader) {
	for (const auto& index : indexs) {
		assert(std::all_of(index.vertex.begin(), index.vertex.end(), [&](int i) {
			return i >= 0 && i < vertexs.size();
		}));
		assert(std::all_of(index.coordinate.begin(), index.coordinate.end(), [&](int i) {
			return i >= 0 && i < coordinates.size();
		}));
		assert(std::all_of(index.normal.begin(), index.normal.end(), [&](int i) {
			return i >= 0 && i < normals.size();
		}));
		assert(std::all_of(index.color.begin(), index.color.end(), [&](int i) {
			return i >= 0 && i < colors.size();
		}));
		assert(index.texture >= 0 && index.texture < textures.size());
		array<Point4, 3> mainVertexs;
		for (int i = 0; i < 3; i++) {
			mainVertexs[i] = vertexShader(
				vertexs[index.vertex[i]],
				coordinates[index.coordinate[i]],
				normals[index.normal[i]],
				colors[index.color[i]],
				textures[index.texture]
			);
		}
		auto clipTriangles = WireframeNearPlaneClip(mainVertexs);
		auto lines = WireframeFarPlaneClipAndGetNotRepeatingLines(clipTriangles);
		for (auto& line : lines) {
			if (LineClip(line)) {
				DrawLine(line);
			}
		}
	}
}
