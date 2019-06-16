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
	//����
	indexDatas.push_back({{0, 1, 2}, {0, 1, 2}, 0});
	indexDatas.push_back({{0, 2, 3}, {0, 2, 3}, 0});
	//���
	indexDatas.push_back({{4, 5, 1}, {0, 1, 2}, 0});
	indexDatas.push_back({{4, 1, 0}, {0, 2, 3}, 0});
	//����
	indexDatas.push_back({{1, 5, 6}, {0, 1, 2}, 0});
	indexDatas.push_back({{1, 6, 2}, {0, 2, 3}, 0});
	//�ұ�
	indexDatas.push_back({{3, 2, 6}, {0, 1, 2}, 0});
	indexDatas.push_back({{3, 6, 7}, {0, 2, 3}, 0});
	//����
	indexDatas.push_back({{4, 0, 3}, {0, 1, 2}, 0});
	indexDatas.push_back({{4, 3, 7}, {0, 2, 3}, 0});
	//����
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

		//��ת
		auto rotate1 = RotateX(x) * RotateY(z);
		//��ת
		auto move2 = Move({0.0f, 0.0f, 10.0f});
		auto rotate2 = RotateY(y);
		//Բ��
		auto move = Move({0.0f, 0.0f, 15.0f});
		auto matrix = perspective * move * rotate2* move2 * rotate1;

		auto light = Point3{5.0f, 5.0f, -1.0};
		auto camera = Point3{0.0f, 0.0f, 0.0f};

		auto vertexShader = [&](Point3 point, Point2 textureCoodinate, const array<RGBImage, 7> & normal) {
			return matrix * point.ToPoint4();
		};

		auto pixelShader = [&](Point4 point4, Point2 textureCoodinate, const array<RGBImage, 7> & texture) {
		    /*
				ʹ���������˲�
				͸�ӻ�ѹ����οռ��z�� z�������
				����ռ�ΪZ ��οռ�ΪZ'
				͸�ӻὫ Z => Z' ������Ҫ�� Z' => Z ��ѹ���� [0,1] Ȼ�� ӳ���� [level - 1,0]
				�����ַ���
					  ��͸��          ƽ��           ����ѹ��        ����ӳ��
				[0,1] ======> [n,f] ======> [0,f-n] =======> [0,1] ========> [level - 1,0]
					  ��͸��              ֱ��ѹ��                   ����ӳ��  (C�Ǹ��Ƚ�С���� �������0)
				[0,1] ======> [n,f] =====================> [n/f,1] ========> [level - 1 + C,0]
				������õڶ���
			*/
			float levelMax = 6.0f;
			float z = point4.z / point4.w;
			float newZ = n / (f - ((f - n) * z));
			float levelLine = -levelMax * newZ + levelMax;
			//��һ��������͸�һ��������ϵ��
			float low = levelLine - floor(levelLine);
			float high = ceil(levelLine) - levelLine;
			//����ȼ�
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