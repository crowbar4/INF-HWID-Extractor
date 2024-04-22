#pragma once

#include <string>

//
// WARNING: ONLY Work with C++11
//
// Ref:
// https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t/18597384#18597384
//

std::wstring STDStringToSTDWString(const std::string& strUtf8);
std::string STDWStringToSTDString(const std::wstring& wsUtf16);
