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

	vector<array<RGBImage, 10>> textures;
	textures.emplace_back(array<RGBImage, 10>{
		display.GetImage(L"../level0.png"),
		display.GetImage(L"../level1.png"),
		display.GetImage(L"../level2.png"),
		display.GetImage(L"../level3.png"),
		display.GetImage(L"../level4.png"),
		display.GetImage(L"../level5.png"),
		display.GetImage(L"../level6.png"),
		display.GetImage(L"../level7.png"),
		display.GetImage(L"../level8.png"),
		display.GetImage(L"../level9.png")
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

		auto vertexShader = [&](Point3 point, Point2 textureCoodinate, const array<RGBImage, 10> & normal) {
			return matrix * point.ToPoint4();
		};

		auto pixelShader = [&](Point4 point4, Point2 textureCoodinate, const array<RGBImage, 10> & texture) {
		    /*  
				使用三线性滤波
				图形API实现和这里不一样 用框定的范围来确定 level = log2(D) D代表像素范围映射至纹理范围ΔU ΔV中的最大值
				所以也无法实现各项异性过滤器

				所以这里用类似的方法处理
				这里使用z轴比例来确定 会在平面垂直于相机平面 出现较大失真
				但是在平面平行相机平面的 时候效果不错

				透视会压缩其次空间的z轴 z轴非线性
					  逆透视         归一化             log处理                得到等级
				[0,1] ======> [n,f] ========> [1,f/n] =======> [0,log2(f/n)] =======>[0,levelMax]
				这里的纹理等级是反过来的 level0 是分辨率最高的 level9 是最低的
				level = log2((Pn/P0)*X)
				Pn是 近平面需求的分辨率 因为图片采用双线性滤波 所以需求2倍
				p0是level0 的分辨率
				X 是距离相机的距离
				
				意义是 在 level 取整数时 所对应的平面 分辨率是完美的 采样不失真

				注意这里和透视矩阵的参数有关 默认b = -1 t = 1时 如果不是需要比例缩放
			*/
			float levelMax = 9.0f;
			float pn = 600.0f * 2.0f;
			float p0 = 1024.0f;
			float z = point4.z / point4.w;
			//返回线性
			float newZ = f / (f - ((f - n) * z));
			float logZ = log2((pn / p0) * newZ);
			float logLevelLine = std::clamp(logZ, 0.0f, levelMax);
			//纹理等级
			int logLowLevel = static_cast<int>(floor(logLevelLine));
			int logHighLevel = std::clamp(logLowLevel + 1, 0, static_cast<int>(levelMax));
			//低一级的纹理和高一级的纹理系数
			float high = logLevelLine - floor(logLevelLine);
			float low = 1.0f - high;
			return (texture[logLowLevel].BilinearFiltering(textureCoodinate) * low)
				+ (texture[logHighLevel].BilinearFiltering(textureCoodinate) * high);
		};

		renderer.DrawTriangleByTexture<array<RGBImage, 10>>(points, textureCoodinates, textures,
															indexDatas, vertexShader, pixelShader);

		return renderer.GenerateImage();
	});
	return 0;
}