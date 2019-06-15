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

	vector<Point2> textureCoodinates;
	textureCoodinates.push_back({0.0f, 0.0f});
	textureCoodinates.push_back({0.0f, 1.0f});
	textureCoodinates.push_back({1.0f, 1.0f});
	textureCoodinates.push_back({1.0f, 0.0f});

	vector<Vector3> textures;
	//正面
	textures.push_back({0.0f, 0.0f, -1.0f});
	//左边
	textures.push_back({-1.0f, 0.0f, 0.0f});
	//上面
	textures.push_back({0.0f, 1.0f, 0.0f});
	//右边
	textures.push_back({1.0f, 0.0f, 0.0f});
	//下面
	textures.push_back({0.0f, -1.0f, 0.0f});
	//后面
	textures.push_back({0.0f, 0.0f, 1.0f});


	vector<TextureIndexData> indexDatas;
	//正面
	indexDatas.push_back({{0, 1, 2}, {0, 1, 2}, 0});
	indexDatas.push_back({{0, 2, 3}, {0, 2, 3}, 0});
	//左边
	indexDatas.push_back({{4, 5, 1}, {0, 1, 2}, 1});
	indexDatas.push_back({{4, 1, 0}, {0, 2, 3}, 1});
	//上面
	indexDatas.push_back({{1, 5, 6}, {0, 1, 2}, 2});
	indexDatas.push_back({{1, 6, 2}, {0, 2, 3}, 2});
	//右边
	indexDatas.push_back({{3, 2, 6}, {0, 1, 2}, 3});
	indexDatas.push_back({{3, 6, 7}, {0, 2, 3}, 3});
	//下面
	indexDatas.push_back({{4, 0, 3}, {0, 1, 2}, 4});
	indexDatas.push_back({{4, 3, 7}, {0, 2, 3}, 4});
	//后面
	indexDatas.push_back({{7, 6, 5}, {0, 1, 2}, 5});
	indexDatas.push_back({{7, 5, 4}, {0, 2, 3}, 5});

	float aspect = static_cast<float>(width) / static_cast<float>(height);
	float x = 0.7f;
	float y = 0.5f;
	auto perspective = Perspective(1.0f, 100.0f, -aspect, aspect, -1.0f, 1.0f);
	auto inverse = perspective.Inverse();

	display.Update([&]() {
		renderer.Clear();

		x += 0.007f;
		y += 0.005f;

		auto rotate = RotateX(x) * RotateY(y);
		auto move = Move({0.0f, 0.0f, 5.0f});
		auto matrix = perspective * move * rotate;

		auto vectorRotationMatrix = rotate.GetMatrix3X3();
		
		auto light = Point3{5.0f, 5.0f, -1.0};
		auto camera = Point3{0.0f, 0.0f, 0.0f};

		auto vertexShader = [&](Point3 point, Point2 textureCoodinate, const Vector3& normal) {
			return matrix * point.ToPoint4();
		};

		auto pixelShader = [&](Point4 point4, Point2 textureCoodinate, const Vector3& texture) {
			auto normal = vectorRotationMatrix * texture;
			//逆矩阵变成透视前的点
			auto point = (inverse * point4).ToPoint3();
			auto toLight = (light - point).Normalize();
			auto toCamera = camera - point;
			//半矢量
			auto half = (toLight + toLight).Normalize();

			auto ambient = Color{0.0f,0.0f,0.0f};
			auto diffuse = Color{0.5f, 0.1f, 0.1f} *std::max(0.0f, normal.Dot(toLight));
			auto highLight = Color{0.8f, 0.8f, 0.8f} *std::max(0.0f, pow(half.Dot(normal), 10.0f));
			return ambient + diffuse + highLight;
		};

		renderer.DrawTriangleByTexture<Vector3>(points, textureCoodinates, textures,
												indexDatas, vertexShader, pixelShader);

		return renderer.GenerateImage();
	});
	return 0;
}