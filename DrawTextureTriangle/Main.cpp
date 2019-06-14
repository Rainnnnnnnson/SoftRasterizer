#include<windows.h>
#include"../SoftRasterizer/Display.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	Display display(hInstance, nShowCmd, width, height);
	RGBImage fileImage = display.GetImage(L"../EZ.PNG");
	Renderer renderer(width, height);

	auto points = vector<Point3>{{0.5f, 0.4f, 0.0f}, {0.4f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}};
	auto textureCoordinates = vector<Point2>{{1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}};
	auto textures = vector<RGBImage>{fileImage};
	auto  indexDatas = vector<TextureIndexData>{{{0, 1, 2}, {0, 1, 2}, 0}};
	auto vertexShader = [](Point3 p, Point2, const RGBImage&) {
		return p.ToPoint4();
	};
	auto pixelShader = [](Point4 p, Point2 t, const RGBImage& i) {
		return i.BilinearFiltering(t);
	};
	renderer.DrawTriangleByTexture<RGBImage>(points, textureCoordinates, textures, indexDatas,
											 vertexShader, pixelShader);
	display.Update([&]() {
		return renderer.GenerateImage();
	});
	return 0;
}