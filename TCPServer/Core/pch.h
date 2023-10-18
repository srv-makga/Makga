// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가

#ifdef _USE_32BIT_TIME_T
	#undef _USE_32BIT_TIME_T
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#pragma warning (disable:4786)
#pragma warning (disable:4259)
#pragma warning (disable:4995)
#pragma warning (disable:4996)
#pragma warning (disable:4005)


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <strsafe.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cassert>

#include <iostream>
#include <queue>
#include <type_traits>
#include <functional>
#include <format>
#include <filesystem>
#include <map>

typedef std::error_code error_code;

#define AUTO_GETSET(type, name) private:\
	type m_##name;\
	public:\
	type name() const { return m_##name; }\
	void name(type _##name) { m_##name = _##name; }

#endif //PCH_H
