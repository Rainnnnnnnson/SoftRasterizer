#include<windows.h>
#include<gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
#include"Display.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	constexpr int width = 800;
	constexpr int height = 600;
	Display display(hInstance, nShowCmd, width, height);
	RGBImage fileImage = display.GetImage(L"../EZ.PNG");
	display.Update([&]() {
		RGBImage image(width, height);
 		for (int i = 0; i < fileImage.GetHeight(); i++) {
			for (int j = 0; j  < fileImage.GetWidth(); j++) {
				image.SetPixel(i, j, fileImage.GetPixel(i, j));
			}
		}
		return image;
	});
	return 0;
}