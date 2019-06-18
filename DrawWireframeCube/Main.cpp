#include<windows.h>
#include"../SoftRasterizer/Display.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	constexpr float aspect = static_cast<float>(width) / static_cast<float>(height);

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

	vector<WireframeIndexData> wireframeIndexDatas;
	//����
	wireframeIndexDatas.push_back({{0, 1, 2}});
	wireframeIndexDatas.push_back({{0, 2, 3}});
	//���
	wireframeIndexDatas.push_back({{4, 5, 1}});
	wireframeIndexDatas.push_back({{4, 1, 0}});
	//����
	wireframeIndexDatas.push_back({{1, 5, 6}});
	wireframeIndexDatas.push_back({{1, 6, 2}});
	//�ұ�
	wireframeIndexDatas.push_back({{3, 2, 6}});
	wireframeIndexDatas.push_back({{3, 6, 7}});
	//����
	wireframeIndexDatas.push_back({{4, 0, 3}});
	wireframeIndexDatas.push_back({{4, 3, 7}});
	//����
	wireframeIndexDatas.push_back({{7, 6, 5}});
	wireframeIndexDatas.push_back({{7, 5, 4}});

	float x = 0.0f;
	float y = 0.0f;

	display.Update([&]() {
		renderer.Clear();
		x += 0.007f;
		y += 0.005f;
		//���Կ������е�ֱ�� ����ƽ���Զƽ������˻�����µ������� ���ܻ��е�����
		auto per = PerspectiveByAspect(1.0f, 2.0f, aspect);
		auto rotate = RotateX(x) * RotateY(y);
		auto move = Move({0.0f, 0.0f, 1.6f});
		auto scale = Scale(0.5f, 0.5f, 0.5f);
		auto matrix = per * move * rotate * scale;

		renderer.DrawTriangleByWireframe(points, wireframeIndexDatas, [&](Point3 p) {
			return matrix * p.ToPoint4();
		});

		return renderer.GenerateImage();
	});
	return 0;
}