#include<windows.h>
#include"../SoftRasterizer/Display.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	Display display(hInstance, nShowCmd, width, height);
	//
	Renderer renderer(width, height);
	vector<Point3> points{
		Point3{-0.5f, 0.5f, 0.0f},
		Point3{0.4f, -0.5f, 0.0f},
		Point3{0.5f, 0.4f, 0.0f},
	};
	vector<Point2> texturesCoordinates{
		Point2{0.0f, 0.0f},
	};
	vector<float> textures{0.0f};
	vector<IndexData> indexDatas = {
		IndexData{0, {0, 1, 2}, {0, 0, 0}}
	};
	auto vertexShader = [](Point3 p, Point2, const int&) {
		return p.ToPoint4();
	};
	renderer.DrawTriangleByWireframe<float>(
		points, texturesCoordinates, textures, indexDatas, vertexShader
	);
	display.Update([&]() {
		return renderer.GenerateImage();
	});
	return 0;
}