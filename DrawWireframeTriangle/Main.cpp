#include<windows.h>
#include"../SoftRasterizer/Display.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	Display display(hInstance, nShowCmd, width, height);

	Renderer renderer(width, height);

	auto points = vector<Point3>{{0.5f, 0.4f, 0.0f}, {0.4f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}};
	auto textureCoordinates = vector<Point2>{{0.0f, 0.0f}};
	//就算没有纹理也需要添加一个元素
	auto textures = vector<float>{0.0f};
	auto indexDatas = vector<IndexData>{{{0, 1, 2}, {0, 0, 0}, 0}};
	auto vertexShader = [](Point3 p, Point2, const int&) {
		return p.ToPoint4();
	};
	renderer.DrawTriangleByWireframe<float>(points, textureCoordinates, textures, indexDatas, vertexShader);
	display.Update([&]() {
		return renderer.GenerateImage();
	});
	return 0;
}