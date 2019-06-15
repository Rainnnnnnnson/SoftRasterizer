#include "Display.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



Display::Display(int width, int height)
	: width(width), height(height) {
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

Display::~Display() {
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

RGBImage Display::GetImage(const wchar_t* fileName) const {
	auto bitMap = Gdiplus::Bitmap::FromFile(fileName);
	assert(bitMap != nullptr);
	int width = bitMap->GetWidth();
	int height = bitMap->GetHeight();
	RGBImage image(width, height);
	for (int line = 0; line < height; line++) {
		for (int column = 0; column < width; column++) {
			Gdiplus::Color color;
			bitMap->GetPixel(line, column, &color);
			image.SetPixel(line, column, RGBColor{color.GetR(), color.GetG(), color.GetB()});
		}
	}
	return image;
}

void Display::Update(function<RGBImage()> getImage) {
	Gdiplus::Graphics graphics(hwnd);
	Gdiplus::Bitmap bitmap(width, height, PixelFormat24bppRGB);
	Gdiplus::Rect rect(0, 0, width, height);

	// Run the message loop.
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			RGBImage image = getImage();
			assert(image.GetWidth() == width);
			assert(image.GetHeight() == height);
			PAINTSTRUCT ps;
			Gdiplus::BitmapData bitmapData;
			BeginPaint(hwnd, &ps);
			bitmap.LockBits(&rect, 0, PixelFormat24bppRGB, &bitmapData);
			for (int lineInex = 0; lineInex < height; lineInex++) {
				for (int columnIndex = 0; columnIndex < width; columnIndex++) {
					RGBColor rgb = image.GetPixel(columnIndex, lineInex);
					auto p = reinterpret_cast<unsigned char*>(bitmapData.Scan0);
					int pixelIndex = PixelToIndex(columnIndex, lineInex, width) * 3;
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