#pragma once
#include "Math.h"
#include <vector>
#include <array>
using std::vector;
using std::array;

struct GeometryIndex {
	array<unsigned, 3> point;
	array<unsigned, 3> normal;
};

struct GeometryDate {
	vector<Point3> points;
	vector<Vector3> normals;
	vector<GeometryIndex> index;
};

/*
	����������ԭ���������
	������ÿһ���������
	������߳�Ϊ2
	����������ָ�򶥵�

	����һ�����ĸ�����ΪABCD
	B C
	A D
	��������������������Ϊ(A B C)(A C D)
	�� �� �� �� �� �� �� ��˳�����
*/
GeometryDate GenerateCube();

/*
	��������ͺ��򶥵����������� 
	��������������,���ܿ�����������
	������ԭ�� �߳�Ϊ1
	������Բ��ָ�򶥵�

	longitudePointCount �򾭶��ϵĵ�ĸ��� (�������3)
	latitudePointCount  ��γ���ϵĵ�ĸ��� (�������1)
*/
GeometryDate GenerateSphere(unsigned longitudePointCount, unsigned latitudePointCount);