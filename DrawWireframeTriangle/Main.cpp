#include<windows.h>
#include"../SoftRasterizer/Display.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	Display display(width, height);

	Renderer renderer(width, height);

	auto points = vector<Point3>{{0.5f, 0.4f, 0.0f}, {0.4f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}};
	auto indexDatas = vector<WireframeIndexData>{{{0, 1, 2}}};
	auto vertexShader = [](Point3 p) {
		return p.ToPoint4();
	};
	renderer.DrawTriangleByWireframe(points, indexDatas, vertexShader);
	display.Update([&]() {
		return renderer.GenerateImage();
	});
	return 0;
}