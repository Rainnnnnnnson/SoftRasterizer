#include "Application.h"
#include "Assertion.h"
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Application::Application(PixelPointRange range) : buffer(range) {
	int width = static_cast<int>(range.width);
	int height = static_cast<int>(range.height);
	//用这个来获取句柄
	hInstance = GetModuleHandle(NULL);

	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	//注册窗体样式
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);
	// Create the window.

	//不允许最大化 不允许拉伸窗口 保证刚好是bitMap的大小
	DWORD dwstyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;

	RECT r{0, 0, range.width, height};
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
	assertion(bitMap != nullptr);
	size_t width = static_cast<size_t>(bitMap->GetWidth());
	size_t height = static_cast<size_t>(bitMap->GetHeight());
	RGBImage image({width, height});
	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			Gdiplus::Color color;
			bitMap->GetPixel(x, y, &color);
			image.SetImagePixel({x, y}, RGBColor{color.GetR(), color.GetG(), color.GetB()});
		}
	}
	return image;
}

bool Application::Continue() {
	size_t width = buffer.GetWidth();
	size_t height = buffer.GetHeight();
	Gdiplus::Graphics graphics(hwnd);
	Gdiplus::Bitmap bitmap(width, height, PixelFormat24bppRGB);
	Gdiplus::Rect rect(0, 0, width, height);
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			PAINTSTRUCT ps;
			Gdiplus::BitmapData bitmapData;
			BeginPaint(hwnd, &ps);
			bitmap.LockBits(&rect, 0, PixelFormat24bppRGB, &bitmapData);
			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					RGBColor rgb = buffer.GetImagePixel({x, y});
					auto p = reinterpret_cast<unsigned char*>(bitmapData.Scan0);
					int index = y * width + x;
					int pixelIndex = index * 3;
					//bitmap 内存是反过来的 BGR排序的
					p[pixelIndex + 0] = rgb.b;
					p[pixelIndex + 1] = rgb.g;
					p[pixelIndex + 2] = rgb.r;
				}
			}
			bitmap.UnlockBits(&bitmapData);
			graphics.DrawImage(&bitmap, 0, 0);
			EndPaint(hwnd, &ps);
			return true;
		}
	}
	return false;
}

void Application::CopyInBuffer(RGBImage image) {
	assertion(buffer.GetWidth() == image.GetWidth());
	assertion(buffer.GetHeight() == image.GetHeight());
	buffer = std::move(image);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}