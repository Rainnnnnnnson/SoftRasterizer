#include "Math.h"

float ComputeLine(float k, float x, float b) {
	return k * x + b;
}

float ComputeLineEquation(Point2 p, Point2 p0, Point2 p1) {
	return (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y + p0.x * p1.y - p1.x * p0.y;
}

bool Point2::operator==(const Point2& p) const {
	return x == p.x && y == p.y;
}

Vector3 Point3::operator-(const Point3& p) const {
	return Vector3{x - p.x, y - p.y, z - p.z};
}

Point2 Point3::GetPoint2() const {
	return Point2{x, y};
}

Point4 Point3::ToPoint4() const {
	return Point4{x, y, z, 1};
}

Point3 Point4::ToPoint3() const {
	return Point3{x / w, y / w, z / w};
}

float Vector3::Dot(const Vector3& v) const {
	return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::Cross(const Vector3& v) const {
	return Vector3{y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
}
