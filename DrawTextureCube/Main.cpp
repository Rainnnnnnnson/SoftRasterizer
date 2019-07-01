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
	RGBImage image = application.GetImage(L"../EZ.PNG");
	using Texture = RGBImage;
	GeometryDate cubeData = GenerateCube();
	vector<Point3> points = std::move(cubeData.points);
	vector<Point2> coordinates{{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}};
	vector<Vector3> normals{{}};
	vector<Color> colors{{}};
	vector<Texture> textures{std::move(image)};
	vector<IndexData> indexs;
	indexs.reserve(12);
	int cubeFace = 6;
	for (int i = 0; i < cubeFace; i++) {
		int leftUp = 2 * i;
		int rightDown = 2 * i + 1;
		indexs.push_back({cubeData.index[leftUp].point, {0, 1, 2}});
		indexs.push_back({cubeData.index[rightDown].point, {0, 2, 3}});
	}

	auto matrix = PerspectiveByAspect(1.0f, 5.0f, rasterizer.GetAspectRatio())
		* Move({0.0f, 0.0f, 3.0f}) * Scale(0.5f, 0.5f, 0.5f);
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
		auto pixelShader = [](Point4, Point2 coordinate, Vector3, Color, const Texture& texture) {
			return BilinearFilter(texture, coordinate);
		};
		rasterizer.DrawTriangle<Texture>(points, coordinates, normals, colors, textures, indexs,
										 vertexShader, pixelShader);
		RGBImage image = rasterizer.GenerateRGBImage();
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}