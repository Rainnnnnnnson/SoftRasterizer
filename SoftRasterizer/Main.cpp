#include<windows.h>
#include "Application.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {

	constexpr int width = 800;
	constexpr int height = 600;
	Application application(width, height);
	RGBImage fileImage = application.GetImage(L"../EZ.PNG");
	while (application.Continue()) {
		RGBImage image(width, height);
		for (int i = 0; i < fileImage.GetHeight(); i++) {
			for (int j = 0; j < fileImage.GetWidth(); j++) {
				image.SetPixel(i, j, fileImage.GetPixel(i, j));
			}
		}
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}