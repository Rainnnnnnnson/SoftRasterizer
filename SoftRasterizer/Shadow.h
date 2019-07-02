#pragma once
#include<functional>
#include<array>
#include "Texture2D.h"
#include "Math.h"
using std::function;
using std::array;

/*
	用光栅化原理生成ShadowMap
	大部分代码可以直接Copy过来
*/
Texture2D<float> GenerateShadowMap(unsigned width, unsigned height,
								   const vector<Point3>& points,
								   const vector<array<unsigned, 3>> & indexs,
								   function<Point4(Point4)> transfrom);


