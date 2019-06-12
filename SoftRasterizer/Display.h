#pragma once
#include<windows.h>
#include<gdiplus.h>
#include"Renderer.h"
#pragma comment (lib,"Gdiplus.lib")

class Display {
public:
	Display(HINSTANCE hInstance, int nShowCmd, int width, int height);
	~Display();
	RGBImage GetImage(const wchar_t* fileName) const;
	void Update(function<RGBImage()> getImage);
private:
	HINSTANCE hInstance;
	HWND hwnd;
	int width;
	int height;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
};