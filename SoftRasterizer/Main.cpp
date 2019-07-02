#include<windows.h>
#include "Rasterizer.h"
#include "Application.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	unsigned width = 512;
	unsigned height = 512;
	Application application(width,height);
	Rasterizer rasterizer(width, height);
	RGBImage image = application.GetImage(L"../level0.PNG");
	using Texture = RGBImage;
	vector<Point3> points{{-1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}};
	vector<Point2> coordinates{{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}};
	vector<Vector3> normals{{}};
	vector<Color> colors{{}};
	vector<Texture> textures{std::move(image)};
	vector<DataIndex> indexs{{{0, 1, 2}, {0, 1, 2}}, {{0, 2, 3}, {0, 2, 3}}};
	auto vertexShader = [](Point4 point, Point2&, Vector3&, Color&, const Texture&) {
		return point;
	};
	auto pixelShader = [](Point4, Point2 coordinate, Vector3, Color, const Texture& texture) {
		return BilinearFilter(texture, coordinate);
	};
	while (application.Continue()) {
		rasterizer.Clear();
		rasterizer.DrawTriangle<Texture>(points, coordinates, normals, colors, textures, indexs,
										 vertexShader, pixelShader);
		rasterizer.DrawWireframe<Texture>(points, coordinates, normals, colors, textures, indexs,
										  vertexShader, pixelShader);
		RGBImage image = rasterizer.GenerateRGBImage();
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}