#include<windows.h>
#include"../SoftRasterizer/Display.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	Display display(width, height);
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

	vector<Color> colors;
	colors.push_back({0.0f, 0.0f, 0.0f});
	colors.push_back({0.0f, 1.0f, 0.0f});
	colors.push_back({1.0f, 1.0f, 0.0f});
	colors.push_back({1.0f, 0.0f, 0.0f});
	colors.push_back({0.0f, 0.0f, 1.0f});
	colors.push_back({0.0f, 1.0f, 1.0f});
	colors.push_back({1.0f, 1.0f, 1.0f});
	colors.push_back({1.0f, 0.0f, 1.0f});

	vector<ColorIndexData> indexDatas;
	//����
	indexDatas.push_back({{0, 1, 2}, {0, 1, 2}});
	indexDatas.push_back({{0, 2, 3}, {0, 2, 3}});
	//���
	indexDatas.push_back({{4, 5, 1}, {4, 5, 1}});
	indexDatas.push_back({{4, 1, 0}, {4, 1, 0}});
	//����
	indexDatas.push_back({{1, 5, 6}, {1, 5, 6}});
	indexDatas.push_back({{1, 6, 2}, {1, 6, 2}});
	//�ұ�
	indexDatas.push_back({{3, 2, 6}, {3, 2, 6}});
	indexDatas.push_back({{3, 6, 7}, {3, 6, 7}});
	//����
	indexDatas.push_back({{4, 0, 3}, {4, 0, 3}});
	indexDatas.push_back({{4, 3, 7}, {4, 3, 7}});
	//����
	indexDatas.push_back({{7, 6, 5}, {7, 6, 5}});
	indexDatas.push_back({{7, 5, 4}, {7, 5, 4}});
	
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

		renderer.DrawTriangleByColor(points, colors, indexDatas, [&](Point3 p) {
			return matrix * p.ToPoint4();
		});

		return renderer.GenerateImage();
	});
	return 0;
}