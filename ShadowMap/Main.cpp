#include "../SoftRasterizer/Rasterizer.h"
#include "../SoftRasterizer/Application.h"
#include "../SoftRasterizer/Geometry.h"
#include "../SoftRasterizer/Shadow.h"
#include<windows.h>
#undef max
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	unsigned width = 800;
	unsigned height = 600;
	Application application(width, height);
	Rasterizer rasterizer(width, height);
	ShadowBuilder builder(500, 500);
	using Texture = Vector3;
	array<unsigned, 3> zero = {0, 0, 0};

	vector<Point2> coordinates{{}};
	vector<Vector3> normals{{}};
	vector<Color> colors{{}};
	vector<Texture> textures{
		//正 左 上 右 下 后 和Cube保持一致
		{0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}
	};

	//方块
	GeometryDate cubeData = GenerateCube();
	vector<Point3> cubePoints = std::move(cubeData.points);
	vector<DataIndex> cubeIndexs;
	unsigned cubeFace = 6;
	for (unsigned i = 0; i < cubeFace; i++) {
		unsigned leftUp = 2 * i;
		unsigned rightDown = 2 * i + 1;
		cubeIndexs.push_back({cubeData.index[leftUp].point, zero, zero, zero, i});
		cubeIndexs.push_back({cubeData.index[rightDown].point, zero, zero, zero, i});
	}
	vector<array<unsigned, 3>> cubePointIndexs;
	for (unsigned i = 0; i < 12; i++) {
		cubePointIndexs.push_back(cubeData.index[i].point);
	}
	//平面
	vector<Point3> planePoints{{-5.0f, -2.0f, 0.0f}, {-5.0f, -2.0f, 10.0f}, {5.0f, -2.0f, 10.0f}, {5.0f, -2.0f, 0.0f}, {0.0f, -2.0f, 5.0f}};
	vector<DataIndex> planeIndexs{{{1, 4, 0}}, {{1, 2, 4}}, {{2, 3, 4}}, {{4, 3, 0}}};
	vector<array<unsigned, 3>> planePointIndexs{{1, 4, 0}, {1, 2, 4}, {2, 3, 4}, {4, 3, 0}};

	Point3 cameraLocation = {0.0f, 0.0f, 0.0f};
	Point3 lightLocation = {3.5f, 5.0f, 2.0f};
	Color intensity = {1.0f, 1.0f, 1.0f};
	auto lightView = CameraLookTo(lightLocation, Vector3{-1.0f, -1.0f, 1.0f}.Normalize(), Vector3{-1.0f, 1.0f, 1.0f}.Normalize());
	auto lightPerspective = Perspective(1.0f, 10.0f, -0.4f, 0.4f, -0.4f, 0.4f);
	auto cameraPerspective = PerspectiveByAspect(1.0f, 10.0f, rasterizer.GetAspectRatio());

	float x = 0.7f;
	float y = 0.5F;
	while (application.Continue()) {
		x += 0.007f;
		y += 0.005f;
		auto cubeRotate = RotateX(x) * RotateY(y);
		auto cubeWorld = Move({0.0f, 0.0f, 5.0f}) * cubeRotate * Scale(0.75f, 0.75f, 0.75f);
		auto cubeNormalRotate = cubeRotate.ToMatrix3X3();
		rasterizer.Clear();
		builder.Clear();

		//计算方块
		auto cubeLightMatrix = lightPerspective * lightView * cubeWorld;
		builder.DrawDepth(cubePoints, cubePointIndexs, [&](Point4 point) {
			auto result = cubeLightMatrix * point;
			return result;
		});


		//计算平面
		auto planeLightMatrix = lightPerspective * lightView;
		builder.DrawDepth(planePoints, planePointIndexs, [&](Point4 point) {
			auto result = planeLightMatrix * point;
			return result;
		});
		auto depthTexture = builder.Generate();

		//得到深度图后计算相机
		auto cubeCameraMatrix = cameraPerspective * cubeWorld;
		auto cubeVertexShader = [&](Point4 point, Point2&, Vector3&, Color&, const Texture&) {
			return cubeCameraMatrix * point;
		};
		auto cubeInverse = lightPerspective * lightView * cameraPerspective.Inverse();
		auto cubePixelShader = [&](Point4 point, Point2, Vector3, Color, const Texture& normal) {
			point = cubeInverse * point;
			Point4 testPoint = point;
			testPoint.z -= 0.02F;
			if (Illuminated(depthTexture, testPoint)) {
				Vector3 n = cubeNormalRotate.TransformVector(normal);
				Vector3 light = (lightLocation - point.ToPoint3()).Normalize();
				Vector3 view = (cameraLocation - point.ToPoint3()).Normalize();
				Vector3 half = (light + view).Normalize();

				Color ambient = Color{0.1f, 0.1f, 0.1f};
				Color diffuse = intensity * Color{0.5f, 0.2f, 0.2f} *std::max(0.0f, n.Dot(light));
				Color specular = intensity * Color{0.5f, 0.5f, 0.5f} *pow(std::max(0.0f, n.Dot(half)), 50.0f);

				return ambient + diffuse + specular;
			} else {
				return Color{0.1f, 0.1f, 0.1f};
			}
		};
		rasterizer.DrawTriangle<Texture>(cubePoints, coordinates, normals, colors, textures, cubeIndexs,
										 cubeVertexShader, cubePixelShader);
		auto planeCameraMatrix = cameraPerspective;
		auto planeVertexShader = [&](Point4 point, Point2&, Vector3&, Color&, const Texture&) {
			return planeCameraMatrix * point;
		};

		auto planeInverse = lightPerspective * lightView * cameraPerspective.Inverse();
		auto planePixelShader = [&](Point4 point, Point2, Vector3, Color, const Texture& normal) {
			point = planeInverse * point;
			Point4 testPoint = point;
			testPoint.z -= 0.02F;
			if (Illuminated(depthTexture, testPoint)) {
				return Color{0.6f, 0.6f, 0.6f};
			} else {
				return Color{0.1f, 0.1f, 0.1f};
			}
		};

		rasterizer.DrawTriangle<Texture>(planePoints, coordinates, normals, colors, textures, planeIndexs,
										 planeVertexShader, planePixelShader);
		RGBImage image = rasterizer.GenerateRGBImage();
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}