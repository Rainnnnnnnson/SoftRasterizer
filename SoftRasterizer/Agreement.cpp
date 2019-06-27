#include "Agreement.h"
#include <cmath>
size_t PixelPointRange::GetSize() const {
	return width * height;
}

float PixelPointRange::GetAspectRatio() const {
	return static_cast<float>(width) / static_cast<float>(height);
}

bool PixelPointInRange(PixelPoint point, PixelPointRange range) {
	return point.x < range.width && point.y < range.height;
}

bool ImageCoordinateInRangle(ImageCoordinate coordinate) {
	return coordinate.x >= 0.0f && coordinate.x <= 1.0f
		&& coordinate.y >= 0.0f && coordinate.y <= 1.0f;
}

bool ScreenCoordinateInRangle(ScreenCoordinate coordinate) {
	return coordinate.x >= -1.0f && coordinate.x <= 1.0f
		&& coordinate.y >= -1.0f && coordinate.y <= 1.0f;
}

bool DepthInViewVolumn(float depth) {
	return depth >= 0.0f && depth <= 1.0f;
}

float ImagePixelPointToCoordinate(size_t pixel, size_t pixelCount) {
	return static_cast<float>(pixel) / static_cast<float>(pixelCount - 1);
}

int ImageCoordinateToPixelPoint(float coordinate, size_t pixelCount) {
	return round(coordinate * static_cast<float>(pixelCount - 1));
}

float ScreenPixelPointToCoordinate(size_t pixel, size_t pixelCount) {
	float coordinate = static_cast<float>(pixel) / static_cast<float>(pixelCount - 1);
	float normalizeCoordinate = coordinate * 2.0f - 1.0f;
	return normalizeCoordinate;
}

int ScreenCoordinateToPixelPoint(float coordinate, size_t pixelCount) {
	float normalizeCoordinate = (coordinate + 1.0f) * 0.5f;
	return round(normalizeCoordinate * static_cast<float>(pixelCount - 1));
}

size_t ImagePixelPointToIndex(ImagePixelPoint point, PixelPointRange range) {
	return point.y * range.width + point.x;
}

size_t ScreenPixelPointToIndex(ScreenPixelPoint point, PixelPointRange range) {
	size_t reverseY = range.height - 1 - point.y;
	return reverseY * range.width + point.x;
}
