#pragma once
#include <corecrt.h>
#include <format>
#include <stdexcept>

extern "C"
_ACRTIMP void __cdecl _wassert(
	_In_z_ wchar_t const* _Message,
	_In_z_ wchar_t const* _File,
	_In_   unsigned       _Line
);

inline void custom_assert_throw(const char* msg, const char* file, unsigned int line) {
	throw std::logic_error(std::format("Assertion Failed!\n{}\nFile: {}\nLine: {}", msg, file, line));
}

#define gs_assert(expression) (void)(                                                   \
	(!!(expression)) ||                                                              \
	(_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)),    \
	 custom_assert_throw((#expression), (__FILE__), (unsigned)(__LINE__)), 0))
