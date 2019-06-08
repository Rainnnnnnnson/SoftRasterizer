#include<windows.h>
#include<gdiplus.h>
#include"Renderer.h"
#pragma comment (lib,"Gdiplus.lib")

constexpr int windowWidth = 800;
constexpr int windowHeight = 600;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);
	// Create the window.

	//不允许最大化 不允许拉伸窗口 保证刚好是bitMap的大小
	DWORD dwstyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;

	RECT r{0, 0, windowWidth, windowHeight};
	AdjustWindowRectEx(&r, dwstyle, 0, 0);
	int w = r.right - r.left;
	int h = r.bottom - r.top;

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"SoftRasterizer",    // Window text
		dwstyle,                        // Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, w, h,
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL) {
		return 0;
	}

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		Gdiplus::Graphics graphics(hwnd);
		Gdiplus::Bitmap bitmap(windowWidth, windowHeight, PixelFormat24bppRGB);
		Gdiplus::Rect rect(0, 0, windowWidth, windowHeight);

		// Run the message loop.
		MSG msg = {};
		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else {
				RGBImage* image = new RGBImage(windowWidth, windowHeight);
				//TODO
				PAINTSTRUCT ps;
				Gdiplus::BitmapData bitmapData;
				BeginPaint(hwnd, &ps);
				bitmap.LockBits(&rect, 0, PixelFormat24bppRGB, &bitmapData);
				for (int i = 0; i < windowWidth; i++) {
					for (int j = windowHeight - 1; j >= 0; j--) {
						RGBColor rgb = image->GetPixel(i, j);
						auto p = reinterpret_cast<unsigned char*>(bitmapData.Scan0);
						int pixelIndex = i * windowWidth + windowHeight;
						p[pixelIndex + 0] = rgb.b;
						p[pixelIndex + 1] = rgb.g;
						p[pixelIndex + 2] = rgb.r;
					}
				}
				bitmap.UnlockBits(&bitmapData);
				graphics.DrawImage(&bitmap, 0, 0);
				EndPaint(hwnd, &ps);
			}
		}
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}