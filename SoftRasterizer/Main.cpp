#include<windows.h>
#include "Application.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {

	PixelPointRange range{800, 600};
	Application application(range);
	RGBImage fileImage = application.GetImage(L"../EZ.PNG");
	while (application.Continue()) {
		RGBImage image(range);
		size_t width = fileImage.GetWidth();
		size_t height = fileImage.GetHeight();
		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				ImagePixelPoint point = {x, y};
				image.SetImagePixel(point, fileImage.GetImagePixel(point));
			}
		}
		application.CopyInBuffer(std::move(image));
	}
	return 0;
}