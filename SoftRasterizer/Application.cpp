#include "Application.h"
#include "Assertion.h"
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Application::Application(int width, int height) 
	: rect(0,0,width,height), bitmap(width, height, PixelFormat24bppRGB) {
	//用这个来获取句柄
	hInstance = GetModuleHandle(NULL);

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

	RECT r{0, 0, width, height};
	AdjustWindowRectEx(&r, dwstyle, 0, 0);
	int w = r.right - r.left;
	int h = r.bottom - r.top;

	hwnd = CreateWindowEx(
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

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

Application::~Application() {
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

RGBImage Application::GetImage(const wchar_t* fileName) const {
	auto bitMap = Gdiplus::Bitmap::FromFile(fileName);
	assert(bitMap != nullptr);
	int width = bitMap->GetWidth();
	int height = bitMap->GetHeight();
	RGBImage image(width, height);
	for (int line = 0; line < height; line++) {
		for (int column = 0; column < width; column++) {
			Gdiplus::Color color;
			bitMap->GetPixel(line, column, &color);
			image.SetPixel(line, column, RGBColor{{color.GetR(), color.GetG(), color.GetB()}});
		}
	}
	return image;
}

bool Application::Continue() {
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//这个没办法放在类里面 
			//如果放入GDI+无法卸载
			Gdiplus::Graphics graphics(hwnd);
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			graphics.DrawImage(&bitmap, 0, 0);
			EndPaint(hwnd, &ps);
			return true;
		}
	}
	return false;
}

void Application::CopyInBuffer(const RGBImage& image) {
	assert(image.GetWidth == bitmap.GetWidth());
	assert(image.GetHeight == bitmap.GetHeight());
	int width = bitmap.GetWidth();
	int height = bitmap.GetHeight();
	Gdiplus::BitmapData bitmapData;
	bitmap.LockBits(&rect, 0, PixelFormat24bppRGB, &bitmapData);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			RGBColor rgb = image.GetPixel(x, y);
			auto p = reinterpret_cast<unsigned char*>(bitmapData.Scan0);
			int index = y * width + height;
			int pixelIndex = index * 3;
			p[pixelIndex + 0] = rgb.b;
			p[pixelIndex + 1] = rgb.g;
			p[pixelIndex + 2] = rgb.r;
		}
	}
	bitmap.UnlockBits(&bitmapData);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}