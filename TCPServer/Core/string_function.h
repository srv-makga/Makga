#pragma once

#include <cstdint>
#include <string>
#include <tchar.h>
#include <regex>
#include <sstream>

namespace std
{
using tchar = TCHAR;
using tstring = basic_string <TCHAR, char_traits<TCHAR>, allocator<TCHAR>>;
using tifstream = basic_ifstream<TCHAR, char_traits<TCHAR>>;
using tofstream = basic_ofstream<TCHAR, char_traits<TCHAR>>;
using tregex = basic_regex<TCHAR>;
using tregex_token_iterator = regex_token_iterator<tstring::const_iterator>;
using tistream = basic_istream<TCHAR, char_traits<TCHAR>>;
using tstringstream = basic_stringstream<TCHAR, char_traits<TCHAR>, allocator<TCHAR>>;
}

namespace core {
namespace string {
void ConvertEncoding(uint32_t _InEncoding, uint32_t _OutEncoding, const char* _InBuffer, char* _OutBuffer);
char* MultibyteToUtf8(const char* in, char* out);
char* Utf8ToMultibyte(const char* in, char* out);
bool IsUtf8(const char* _buffer, std::size_t _len);

bool Tokenization(std::tstring& _data, TCHAR _delimiter, std::vector<std::tstring>& _tokens);
bool Tokenization(std::tistream& _data, TCHAR _delimiter, std::vector<std::tstring>& _tokens);
} // namespace string
} // namespace core