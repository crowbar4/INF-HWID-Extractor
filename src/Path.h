#pragma once

#include <string>

std::wstring GetAppPath();
std::wstring GetAppExePath();
std::wstring GetUserTempPath();
std::wstring GetSystem32Path();
std::wstring GetSystemWow64Path();
std::wstring AdjustPathTail(const std::wstring& wsPath);
std::wstring GetGlobalStartupDirectory();
