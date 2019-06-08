#pragma once
#include<array>
#include<functional>
using std::array;
using std::function;

//��������ͨ��lambda �������һ���������
//������Java ��Stream�� �� C#��linq
//��Ҫ��������ζ�������ظ�
template<typename VariableArray, typename Function,
	typename Return = std::invoke_result_t<Function, typename VariableArray::value_type>,
	std::size_t Size = std::tuple_size<VariableArray>::value>
	array<Return, Size> Stream(const VariableArray& variableArray, Function f) {
	array<Return, Size> returnArray;
	for (std::size_t i = 0; i < Size; i++) {
		returnArray[i] = f(variableArray[i]);
	}
	return returnArray;
}