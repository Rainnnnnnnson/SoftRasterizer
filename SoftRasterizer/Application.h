#pragma once
#include<windows.h>
#include<gdiplus.h>
#include"RGBImage.h"
#pragma comment (lib,"Gdiplus.lib")

class Application {
public:
	Application(unsigned width, unsigned height);
	~Application();
	RGBImage GetImage(const wchar_t* fileName) const;
	bool Continue();
	void CopyInBuffer(RGBImage image);
private:
	HINSTANCE hInstance;
	HWND hwnd;
	RGBImage buffer;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
};