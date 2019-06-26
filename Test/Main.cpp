#include"../SoftRasterizer/Rasterizer.h"
#include"../SoftRasterizer/Application.h"
#include<windows.h>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	Application application(width, height);
	Rasterizer rasterizer(width, height);
	using Texture = int;
	vector<Point3> vertexs{{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}};
	vector<Point2> coordinates{{}};
	vector<Vector3> normals{{}};
	vector<Color> colors{{}};
	vector<Texture> textures{{}};
	vector<IndexData> indexs{{{0, 1, 2}}};
	auto vertexShader = [](Point3 point, Point2, Vector3, Color, const Texture&){
		return point.ToPoint4();
	};
	auto pixelShader = [](Point4, Point2, Vector3, Color, const Texture&) {
		return Color{};
	};
	while (application.Continue()) {
		rasterizer.Clear();
		rasterizer.DrawWireframe<Texture>(vertexs, coordinates, normals, colors, textures, indexs, 
										  vertexShader, pixelShader);
		RGBImage image = rasterizer.GenerateRGBImage();
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}