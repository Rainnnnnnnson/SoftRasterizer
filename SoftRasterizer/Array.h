#pragma once
#include"Assertion.h"
#include<array>
//�̶�����������
//��ֹ�����δ����ظ�
//ʹ��Stream���Ӵ���ɶ���
template<typename Type, std::size_t size>
class Array {
public:
	Array() : elements{} {}
	Array(std::initializer_list<Type> list) {
		assert(list.size() <= size);
		auto it = elements.begin();
		for (auto& t : list) {
			*it = t;
			it += 1;
		}
	}
	//ͨ��lambda�õ���һ�����͵�����
	template<typename Function, typename Return = std::invoke_result_t<Function, Type>>
	Array<Return, size> Stream(Function func) const {
		Array<Return, size> returnArray{};
		std::size_t i = 0;
		for (Return& returnElement : returnArray) {
			returnElement = func(elements[i]);
			i += 1;
		}
		return returnArray;
	};

	Type* begin() {
		return &elements[0];
	}

	const Type* begin() const{
		return &elements[0];
	}

	Type* end() {
		return &elements[0] + size;
	}

	const Type* end() const{
		return &elements[0] + size;
	}

	Type& operator[](std::size_t pos) {
		return elements[pos];
	}

	const Type& operator[](std::size_t pos) const {
		return elements[pos];
	}
private:
	std::array<Type, size> elements;
};

//���趨һ�������������������ջ����
//���ڴ洢С�������ǲ�ȷ��������ʱ��
//�ڼ��õ�ʱ��ʹ��
template<typename Type,std::size_t capacity>
class MaxCapacityArray {
public:
	MaxCapacityArray() : elements{}, size(0) {}
	void Push(const Type& element) {
		assert(size != capacity);
		elements[size] = element;
		size += 1;
	}

	Type* begin() {
		return &elements[0];
	}

	const Type* begin() const {
		return &elements[0];
	}
	
	Type* end() {
		return &elements[0] + size;
	}

	const Type* end() const {
		return &elements[0] + size;
	}
private:
	std::array<Type, capacity> elements;
	std::size_t size;
};