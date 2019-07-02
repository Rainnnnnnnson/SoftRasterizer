#pragma once
#include<functional>
#include<array>
#include "Texture2D.h"
#include "Math.h"
using std::function;
using std::array;

/*
	�ù�դ��ԭ������ShadowMap
	�󲿷ִ������ֱ��Copy����
*/
Texture2D<float> GenerateShadowMap(unsigned width, unsigned height,
								   const vector<Point3>& points,
								   const vector<array<unsigned, 3>> & indexs,
								   function<Point4(Point4)> transfrom);


