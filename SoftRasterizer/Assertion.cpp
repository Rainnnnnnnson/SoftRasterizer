#include "Assertion.h"
#include <assert.h>
#ifdef _DEBUG
void assertFunction(wchar_t const* _Message, wchar_t const* _File, unsigned _Line) {
	_wassert(_Message, _File, _Line);
}
#endif