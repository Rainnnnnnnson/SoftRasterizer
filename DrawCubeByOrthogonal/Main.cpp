#include"../SoftRasterizer/Rasterizer.h"
#include"../SoftRasterizer/Application.h"
#include"../SoftRasterizer/Geometry.h"
#include<windows.h>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	unsigned width = 800;
	unsigned height = 600;
	Application application(width, height);
	Rasterizer rasterizer(width, height);
	using Texture = int;

	GeometryDate cubeData = GenerateCube();

	vector<Point3> points = std::move(cubeData.points);
	vector<Point2> coordinates{{}};
	vector<Vector3> normals{{}};
	vector<Color> colors{
		{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}
	};
	vector<Texture> textures{{}};
	vector<IndexData> indexs;
	indexs.reserve(12);
	array<unsigned, 3>zero{0, 0, 0};
	int cubeFace = 6;
	for (int i = 0; i < cubeFace; i++) {
		int leftUp = 2 * i;
		int rightDown = 2 * i + 1;
		indexs.push_back({cubeData.index[leftUp].point, zero, zero, cubeData.index[leftUp].point});
		indexs.push_back({cubeData.index[rightDown].point, zero, zero, cubeData.index[rightDown].point});
	}

	auto matrix = OrthogonalByAspect(1.0f, 2.0f, rasterizer.GetAspectRatio())
		* Move({0.0f, 0.0f, 1.5f}) * Scale(0.5f, 0.5f, 0.5f);
	float x = 0.7f;
	float y = 0.5f;
	while (application.Continue()) {
		rasterizer.Clear();
		x += 0.007f;
		y += 0.005f;
		auto rotate = RotateX(x) * RotateY(y);
		auto vertexShader = [&](Point4 point, Point2&, Vector3&, Color&, const Texture&) {
			return matrix * rotate * point;
		};
		auto pixelShader = [](Point4, Point2, Vector3, Color color, const Texture&) {
			return color;
		};
		rasterizer.DrawTriangle<Texture>(points, coordinates, normals, colors, textures, indexs,
										 vertexShader, pixelShader);
		rasterizer.DrawWireframe<Texture>(points, coordinates, normals, colors, textures, indexs,
										  vertexShader, pixelShader);
		RGBImage image = rasterizer.GenerateRGBImage();
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}