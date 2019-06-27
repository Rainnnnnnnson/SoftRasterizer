/*
	��Ҫ��Ϊ�˱���΢�������assert.h��ͬʱ������һ�Ѻ� ��������ĳ���
*/
#pragma once

#ifdef _DEBUG
void assertFunction(wchar_t const* _Message, wchar_t const* _File, unsigned _Line);
#define TO_WIDE_CHAR_(s) L ## s
#define TO_WIDE_CHAR(s) TO_WIDE_CHAR_(s)
#define assertion(expression) {                                         \
    if(!(expression)){                                                  \
        assertFunction(L###expression,TO_WIDE_CHAR(__FILE__),__LINE__); \
	}                                                                   \
}
#else
#define assertion(expression) {}
#endif
