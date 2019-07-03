#include"../SoftRasterizer/Rasterizer.h"
#include"../SoftRasterizer/Application.h"
#include"../SoftRasterizer/Geometry.h"
#include <windows.h>
#undef max
#include<cmath>
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	unsigned width = 800;
	unsigned height = 600;
	Application application(width, height);
	Rasterizer rasterizer(width, height);
	using Texture = int;
	GeometryDate sphereData = GenerateSphere(30, 30);
	vector<Point3> points = std::move(sphereData.points);
	vector<Point2> coordinates{{}};
	vector<Vector3> normals = std::move(sphereData.normals);
	vector<Color> colors{{}};
	vector<Texture> textures{{}};
	vector<DataIndex> indexs;
	for (auto& data : sphereData.index) {
		indexs.push_back({data.point, {0, 0, 0}, data.normal});
	}


	float x = 0.7f;
	float y = 0.5f;

	constexpr Color intensity = {1.0F, 1.0F, 1.0F};
	constexpr Point3 position = {5.0f, 5.0f, -5.0f};
	constexpr Point3 camera = {0.0f, 0.0f, 0.0f};

	while (application.Continue()) {
		rasterizer.Clear();
		x += 0.007f;
		y += 0.005f;
		auto perspective = PerspectiveByAspect(1.0f, 5.0f, rasterizer.GetAspectRatio());
		auto perspectiveInverse = perspective.Inverse();
		auto rotate = RotateX(x) * RotateY(y);
		auto VectorRotate = rotate.ToMatrix3X3();
		auto rotateMove = Move({0.0f, 0.0f, 3.0f}) * rotate;
		auto vertexShader = [&](Point4 point, Point2&, Vector3& normal, Color&, const Texture&) {
			normal = VectorRotate.TransformVector(normal);
			point = rotateMove * point;
			return perspective * point;
		};
		auto pixelShader = [&](Point4 point, Point2, Vector3 normal, Color, const Texture&) {
			point = perspectiveInverse * point;

			Vector3 light = (position - point.ToPoint3()).Normalize();
			Vector3 view = (camera - point.ToPoint3()).Normalize();
			Vector3 half = (light + view).Normalize();

			Color ambient = Color{0.1f, 0.1f, 0.1f};
			Color diffuse = intensity * Color{0.2f, 0.5f, 0.2f} *std::max(0.0f, normal.Dot(light));
			Color specular = intensity * Color{0.5f, 0.5f, 0.5f} *pow(std::max(0.0f, normal.Dot(half)), 20.0f);

			return ambient + diffuse + specular;
		};
		rasterizer.DrawTriangle<Texture>(points, coordinates, normals, colors, textures, indexs,
										 vertexShader, pixelShader);
		RGBImage image = rasterizer.GenerateRGBImage();
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}