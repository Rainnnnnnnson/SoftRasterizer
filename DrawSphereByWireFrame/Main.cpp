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
	GeometryDate sphereData = GenerateSphere(10, 10);
	vector<Point3> points = std::move(sphereData.points);
	vector<Point2> coordinates{{}};
	vector<Vector3> normals{{}};
	vector<Color> colors{{}};
	vector<Texture> textures{{}};
	vector<DataIndex> indexs;
	for (auto& data : sphereData.index) {
		indexs.push_back({data.point});
	}

	auto matrix = PerspectiveByAspect(1.0f, 5.0f, rasterizer.GetAspectRatio())
		* Move({0.0f, 0.0f, 3.0f}) * Scale(1.0f, 1.0f, 1.0f);
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
		auto pixelShader = [](Point4, Point2, Vector3, Color, const Texture&) {
			return Color{};
		};
		rasterizer.DrawWireframe<Texture>(points, coordinates, normals, colors, textures, indexs,
										 vertexShader, pixelShader);
		RGBImage image = rasterizer.GenerateRGBImage();
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}