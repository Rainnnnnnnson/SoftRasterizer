#include<windows.h>
#include"../SoftRasterizer/Display.h"

using std::array;

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

	vector<Point2> textureCoodinates;
	textureCoodinates.push_back({0.0f, 0.0f});
	textureCoodinates.push_back({0.0f, 1.0f});
	textureCoodinates.push_back({1.0f, 1.0f});
	textureCoodinates.push_back({1.0f, 0.0f});

	vector<array<RGBImage, 7>> textures;
	textures.emplace_back(array<RGBImage, 7>{
		display.GetImage(L"../level0.PNG"),
		display.GetImage(L"../level1.PNG"),
		display.GetImage(L"../level2.PNG"),
		display.GetImage(L"../level3.PNG"),
		display.GetImage(L"../level4.PNG"),
		display.GetImage(L"../level5.PNG"),
		display.GetImage(L"../level6.PNG")
	});

	vector<TextureIndexData> indexDatas;
	//正面
	indexDatas.push_back({{0, 1, 2}, {0, 1, 2}, 0});
	indexDatas.push_back({{0, 2, 3}, {0, 2, 3}, 0});
	//左边
	indexDatas.push_back({{4, 5, 1}, {0, 1, 2}, 0});
	indexDatas.push_back({{4, 1, 0}, {0, 2, 3}, 0});
	//上面
	indexDatas.push_back({{1, 5, 6}, {0, 1, 2}, 0});
	indexDatas.push_back({{1, 6, 2}, {0, 2, 3}, 0});
	//右边
	indexDatas.push_back({{3, 2, 6}, {0, 1, 2}, 0});
	indexDatas.push_back({{3, 6, 7}, {0, 2, 3}, 0});
	//下面
	indexDatas.push_back({{4, 0, 3}, {0, 1, 2}, 0});
	indexDatas.push_back({{4, 3, 7}, {0, 2, 3}, 0});
	//后面
	indexDatas.push_back({{7, 6, 5}, {0, 1, 2}, 0});
	indexDatas.push_back({{7, 5, 4}, {0, 2, 3}, 0});

	float aspect = static_cast<float>(width) / static_cast<float>(height);
	float x = 0.0f;
	float y = 3.14f;
	float z = 0.0f;
	float n = 1.0f;
	float f = 100.0f;
	auto perspective = Perspective(n, f, -aspect, aspect, -1.0f, 1.0f);

	display.Update([&]() {
		renderer.Clear();

		x += 0.014f;
		y += 0.010f;
		z += 0.012f;

		//自转
		auto rotate1 = RotateX(x) * RotateY(z);
		//公转
		auto move2 = Move({0.0f, 0.0f, 10.0f});
		auto rotate2 = RotateY(y);
		//圆心
		auto move = Move({0.0f, 0.0f, 15.0f});
		auto matrix = perspective * move * rotate2* move2 * rotate1;

		auto light = Point3{5.0f, 5.0f, -1.0};
		auto camera = Point3{0.0f, 0.0f, 0.0f};

		auto vertexShader = [&](Point3 point, Point2 textureCoodinate, const array<RGBImage, 7> & normal) {
			return matrix * point.ToPoint4();
		};

		auto pixelShader = [&](Point4 point4, Point2 textureCoodinate, const array<RGBImage, 7> & texture) {
		    /*
				使用三线性滤波
				透视会压缩其次空间的z轴 z轴非线性
				相机空间为Z 齐次空间为Z'
				透视会将 Z => Z' 这里需要将 Z' => Z 并压缩至 [0,1] 然后 映射至 [level - 1,0]
				有两种方案
					  逆透视          平移           线性压缩        线性映射
				[0,1] ======> [n,f] ======> [0,f-n] =======> [0,1] ========> [level - 1,0]
					  逆透视              直接压缩                   线性映射  (C是个比较小的数 不会等于0)
				[0,1] ======> [n,f] =====================> [n/f,1] ========> [level - 1 + C,0]
				这里采用第二种
			*/
			float levelMax = 6.0f;
			float z = point4.z / point4.w;
			float newZ = n / (f - ((f - n) * z));
			float levelLine = -levelMax * newZ + levelMax;
			//低一级的纹理和高一级的纹理系数
			float low = levelLine - floor(levelLine);
			float high = ceil(levelLine) - levelLine;
			//纹理等级
			int lowLevel = static_cast<int>(floor(levelLine));
			int highLevel = lowLevel + 1;
			return (texture[lowLevel].BilinearFiltering(textureCoodinate) * low)
				+ (texture[highLevel].BilinearFiltering(textureCoodinate) * high);
		};

		renderer.DrawTriangleByTexture<array<RGBImage, 7>>(points, textureCoodinates, textures,
												   indexDatas, vertexShader, pixelShader);

		return renderer.GenerateImage();
	});
	return 0;
}