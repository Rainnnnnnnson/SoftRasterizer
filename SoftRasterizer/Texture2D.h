#pragma once
#include"Assertion.h"
#include<vector>
using std::vector;

template<typename Element>
class Texture2D {
public:
	Texture2D(unsigned width, unsigned height)
		: width(width), height(height), 
		elements(static_cast<size_t>(width)* height, Element{}) {
		assertion(width > 0);
		assertion(height > 0);
	}

	unsigned GetWidth() const {
		return width;
	}

	unsigned GetHeight() const {
		return height;
	}

	Element GetImagePoint(unsigned x, unsigned y) const {
		assertion(x < width);
		assertion(y < height);
		unsigned index = y * width + x;
		return elements[index];
	}

	void SetImagePoint(unsigned x, unsigned y, Element element) {
		assertion(x < width);
		assertion(y < height);
		unsigned index = y * width + x;
		elements[index] = element;
	}

	Element GetScreenPoint(unsigned x, unsigned y) const {
		assertion(x < width);
		assertion(y < height);
		unsigned index = (height - 1 - y) * width + x;
		return elements[index];
	}

	void SetScreenPoint(unsigned x, unsigned y, Element element) {
		assertion(x < width);
		assertion(y < height);
		unsigned index = (height - 1 - y) * width + x;
		elements[index] = element;
	}
private:
	unsigned width;
	unsigned height;
	vector<Element> elements;
};