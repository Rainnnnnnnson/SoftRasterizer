#pragma once
#include<array>
#include<functional>
using std::array;
using std::function;

//遍历数组通过lambda 计算出下一数组的类型
//类似于Java 的Stream库 和 C#的linq
//主要解决三角形顶点代码重复
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