#include<windows.h>
#include"../SoftRasterizer/Display.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	Display display(hInstance, nShowCmd, width, height);
	Renderer renderer(width, height);

	vector<Point3> points;
	points.push_back({-1.0f, -1.0f, -1.0f});
	points.push_back({-1.0f, 1.0f, -1.0f});
	points.push_back({1.0f, 1.0f, -1.0f});
	points.push_back({1.0f, -1.0f, -1.0f});
	points.push_back({-1.0f, -1.0f, 1.0f});
	points.push_back({-1.0f, 1.0f, 1.0f});
	points.push_back({1.0f, 1.0f, 1.0f});
	points.push_back({1.0f, -1.0f, 1.0f});

	vector<WireframeIndexData> indexDatas;
	//正面
	indexDatas.push_back({{0, 1, 2}});
	indexDatas.push_back({{0, 2, 3}});
	//左边
	indexDatas.push_back({{4, 5, 1}});
	indexDatas.push_back({{4, 1, 0}});
	//上面
	indexDatas.push_back({{1, 5, 6}});
	indexDatas.push_back({{1, 6, 2}});
	//右边
	indexDatas.push_back({{3, 2, 6}});
	indexDatas.push_back({{3, 6, 7}});
	//下面
	indexDatas.push_back({{4, 0, 3}});
	indexDatas.push_back({{4, 3, 7}});
	//后面
	indexDatas.push_back({{7, 6, 5}});
	indexDatas.push_back({{7, 5, 4}});

	float x = 0.7f;
	float y = 0.5f;

	display.Update([&]() {
		renderer.Clear();
		x += 0.007f;
		y += 0.005f;
		auto rotate = RotateX(x) * RotateY(y);
		auto move = Move({0.0f, 0.0f, 0.35f});
		auto scale = Scale(0.3f, 0.3f, 0.3f);
		auto matrix = move * rotate * scale;

		renderer.DrawTriangleByWireframe(points, indexDatas, [&](Point3 p) {
			return matrix * p.ToPoint4();
		});

		return renderer.GenerateImage();
	});
	return 0;
}