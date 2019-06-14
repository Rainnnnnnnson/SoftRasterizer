#pragma once
#include"Assertion.h"
#include<array>

//�̶�����������
//��ֹ�����δ����ظ�
//ʹ��Stream���Ӵ���ɶ���
//ֻ���������ڵ���2����Ҫ�õ��������
template<typename Type, std::size_t size, typename = std::enable_if_t<(size >= 2U)>>
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
		Array<Return, size> returnArray;
		for (std::size_t i = 0; i < size; i++) {
			returnArray[i] = func(elements[i]);
		}
		return returnArray;
	};
	
	/*
		ѭ����ȡ
		���� �������������� 0 1 2
		���÷ֱ�õ� func(0,1) func(1,2) func(2,0) 
	*/
	template<typename Function, typename Return = std::invoke_result_t<Function, Type, Type>>
	Array<Return, size> Circle(Function func) const {
		Array<Return, size> returnArray;
		std::size_t last = size - 1;
		for (std::size_t i = 0; i < last; i++) {
			returnArray[i] = func(elements[i], elements[i + 1]);
		}
		returnArray[last] = func(elements[last], elements[0]);
		return returnArray;
	}

	Type Sum() const {
		Type result = elements[0];
		for (int i = 1; i < size; i++) {
			result = result + elements[i];
		}
		return result;
	}

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

template<std::size_t size>
Array<std::size_t, size> ArrayIndex() {
	Array<std::size_t, size> result;
	for (std::size_t i = 0; i < size; i++) {
		result[i] = i;
	}
	return result;
}

//���趨һ�������������������ջ����
//���ڴ洢С�������ǲ�ȷ��������ʱ��
//�ڼ��õ�ʱ��ʹ��
template<typename Type,std::size_t capacity, typename = std::enable_if_t<(capacity >= 2U)>>
class MaxCapacityArray {
public:
	MaxCapacityArray() : elements{}, size(0) {}
	void Push(const Type& element) {
		assert(size < capacity);
		elements[size] = element;
		size += 1;
	}

	std::size_t Size() {
		return size;
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