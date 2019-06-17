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

		auto vertexShader = [&](Point3 point, Point2 textureCoodinate, const array<RGBImage, 10> & normal) {
			return matrix * point.ToPoint4();
		};

		auto pixelShader = [&](Point4 point4, Point2 textureCoodinate, const array<RGBImage, 10> & texture) {
		    /*  
				ʹ���������˲�
				ͼ��APIʵ�ֺ����ﲻһ�� �ÿ򶨵ķ�Χ��ȷ�� level = log2(D) D�������ط�Χӳ��������Χ��U ��V�е����ֵ
				����Ҳ�޷�ʵ�ָ������Թ�����

				�������������Ƶķ�������
				����ʹ��z�������ȷ�� ����ƽ�洹ֱ�����ƽ�� ���ֽϴ�ʧ��
				������ƽ��ƽ�����ƽ��� ʱ��Ч������

				͸�ӻ�ѹ����οռ��z�� z�������
					  ��͸��         ��һ��             log����                �õ��ȼ�
				[0,1] ======> [n,f] ========> [1,f/n] =======> [0,log2(f/n)] =======>[0,levelMax]
				���������ȼ��Ƿ������� level0 �Ƿֱ�����ߵ� level9 ����͵�
				level = log2((Pn/P0)*X)
				Pn�� ��ƽ������ķֱ��� ��ΪͼƬ����˫�����˲� ��������2��
				p0��level0 �ķֱ���
				X �Ǿ�������ľ���
				
				������ �� level ȡ����ʱ ����Ӧ��ƽ�� �ֱ����������� ������ʧ��

				ע�������͸�Ӿ���Ĳ����й� Ĭ��b = -1 t = 1ʱ ���������Ҫ��������
			*/
			float levelMax = 9.0f;
			float pn = 600.0f * 2.0f;
			float p0 = 1024.0f;
			float z = point4.z / point4.w;
			//��������
			float newZ = f / (f - ((f - n) * z));
			float logZ = log2((pn / p0) * newZ);
			float logLevelLine = std::clamp(logZ, 0.0f, levelMax);
			//����ȼ�
			int logLowLevel = static_cast<int>(floor(logLevelLine));
			int logHighLevel = std::clamp(logLowLevel + 1, 0, static_cast<int>(levelMax));
			//��һ��������͸�һ��������ϵ��
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