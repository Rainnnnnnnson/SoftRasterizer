#pragma once
#include<windows.h>
#include<gdiplus.h>
#include"RGBImage.h"
#pragma comment (lib,"Gdiplus.lib")

class Application {
public:
	Application(int width, int height);
	~Application();
	RGBImage GetImage(const wchar_t* fileName) const;
	bool Continue();
	void CopyInBuffer(const RGBImage& image);
private:
	HINSTANCE hInstance;
	HWND hwnd;
	Gdiplus::Rect rect;
	Gdiplus::Bitmap bitmap;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	MSG msg;
};