/*
	主要是为了避免微软的引入assert.h的同时会引入一堆宏 导致问题的出现
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
