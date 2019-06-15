#include<windows.h>
#include"../SoftRasterizer/Display.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	Display display(width, height);
	RGBImage fileImage = display.GetImage(L"../EZ.PNG");

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

	vector<Point2> textureCoodinates;
	textureCoodinates.push_back({0.0f, 0.0f});
	textureCoodinates.push_back({0.0f, 1.0f});
	textureCoodinates.push_back({1.0f, 1.0f});
	textureCoodinates.push_back({1.0f, 0.0f});

	vector<RGBImage> textures = {fileImage};

	vector<TextureIndexData> indexDatas;
	//正面
	indexDatas.push_back({{0, 1, 2}, {0, 1, 2},0});
	indexDatas.push_back({{0, 2, 3}, {0, 2, 3},0});
	//左边
	indexDatas.push_back({{4, 5, 1}, {0, 1, 2},0});
	indexDatas.push_back({{4, 1, 0}, {0, 2, 3},0});
	//上面
	indexDatas.push_back({{1, 5, 6}, {0, 1, 2},0});
	indexDatas.push_back({{1, 6, 2}, {0, 2, 3},0});
	//右边
	indexDatas.push_back({{3, 2, 6}, {0, 1, 2},0});
	indexDatas.push_back({{3, 6, 7}, {0, 2, 3},0});
	//下面
	indexDatas.push_back({{4, 0, 3}, {0, 1, 2},0});
	indexDatas.push_back({{4, 3, 7}, {0, 2, 3},0});
	//后面
	indexDatas.push_back({{7, 6, 5}, {0, 1, 2},0});
	indexDatas.push_back({{7, 5, 4}, {0, 2, 3},0});

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

		auto vertexShader = [&](Point3 point, Point2 textureCoodinate, const RGBImage& texture) {
			return matrix * point.ToPoint4();
		};

		auto pixelShader = [&](Point4 point, Point2 textureCoodinate, const RGBImage& texture) {
			return texture.BilinearFiltering(textureCoodinate);
		};

		renderer.DrawTriangleByTexture<RGBImage>(points, textureCoodinates, textures,
												 indexDatas, vertexShader, pixelShader);

		return renderer.GenerateImage();
	});
	return 0;
}