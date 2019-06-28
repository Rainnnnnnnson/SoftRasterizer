#include<windows.h>
#include "Rasterizer.h"
#include "Application.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	auto range = PixelPointRange{512, 512};
	Application application(range);
	Rasterizer rasterizer(range);
	RGBImage image = application.GetImage(L"../level0.PNG");
	using Texture = RGBImage;
	vector<Point3> vertexs{{-1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}};
	vector<Point2> coordinates{{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}};
	vector<Vector3> normals{{}};
	vector<Color> colors{{}};
	vector<Texture> textures{std::move(image)};
	vector<IndexData> indexs{{{0, 1, 2}, {0, 1, 2}}, {{0, 2, 3}, {0, 2, 3}}};
	auto vertexShader = [](Point3 point, Point2&, Vector3&, Color&, const Texture&) {
		return point.ToPoint4();
	};
	auto pixelShader = [](Point4, Point2 coordinate, Vector3, Color, const Texture& texture) {
		return texture.BilinearFilter(coordinate);
	};
	while (application.Continue()) {
		rasterizer.Clear();
		rasterizer.DrawTriangle<Texture>(vertexs, coordinates, normals, colors, textures, indexs,
										 vertexShader, pixelShader);
		rasterizer.DrawWireframe<Texture>(vertexs, coordinates, normals, colors, textures, indexs,
										  vertexShader, pixelShader);
		RGBImage image = rasterizer.GenerateRGBImage();
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}