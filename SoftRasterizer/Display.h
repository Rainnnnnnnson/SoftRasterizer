#pragma once
#include<windows.h>
#include<gdiplus.h>
#include"Renderer.h"
#pragma comment (lib,"Gdiplus.lib")

class Display {
public:
	Display(HINSTANCE hInstance, int nShowCmd, int width, int height);
	RGBImage GetImage(const wchar_t* FileName) const;
	void Update(function<RGBImage()> getImage);
private:
	HINSTANCE hInstance;
	HWND hwnd;
	int width;
	int height;
};