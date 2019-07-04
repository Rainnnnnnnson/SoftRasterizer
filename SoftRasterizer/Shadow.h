#pragma once
#include<functional>
#include<array>
#include "Texture2D.h"
#include "Math.h"
using std::function;
using std::array;

using ShadowTexture = Texture2D<float>;

class ShadowBuilder {
public:
	ShadowBuilder(unsigned width, unsigned height);
	float GetAspectRatio() const;
	void Clear();
	ShadowTexture Generate() const;
	void DrawDepth(const vector<Point3>& points, 
				   const vector<array<unsigned, 3>> & indexs,
				   function<Point4(Point4)> transfrom);
private:
	ShadowTexture depths;
};

bool Illuminated(const ShadowTexture& shadow, Point4 point);