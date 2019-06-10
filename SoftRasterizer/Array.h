#pragma once
#include"Assertion.h"
#include<array>
//固定容量的数组
//防止三角形代码重复
//使用Stream增加代码可读性
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
	//通过lambda得到下一个类型的数组
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

//被设定一个最大容量的容器放在栈上面
//用于存储小容量但是不确定个数的时候
//在剪裁的时候使用
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