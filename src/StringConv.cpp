#include "StringConv.h"
#include <locale>
#include <codecvt>

//
// Warning: std::wstring_convert Only Work on C++ 11
//

std::wstring STDStringToSTDWString(const std::string& strUtf8)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(strUtf8);
}

std::string STDWStringToSTDString(const std::wstring& wsUtf16)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(wsUtf16);
}

