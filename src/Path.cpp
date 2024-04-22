#include <windows.h>
#include <shlobj_core.h>
#include "Path.h"

// No '\' tail
std::wstring GetAppPath()
{
    std::wstring wsPath = GetAppExePath();
    return wsPath.substr(0, wsPath.find_last_of(L"\\"));
}

std::wstring GetAppExePath()
{
    std::wstring wsPath(MAX_PATH, 0);
    DWORD dwPathSize = GetModuleFileNameW(nullptr, &wsPath.front(), MAX_PATH);
    wsPath.resize(dwPathSize);
    return wsPath;
}

// Has '\' tail
std::wstring GetUserTempPath()
{
    std::wstring wsPath(MAX_PATH, 0);
    DWORD dwPathSize = GetTempPathW(MAX_PATH, &wsPath.front());
    wsPath.resize(dwPathSize);
    return wsPath;
}

// No '\' tail
std::wstring GetSystem32Path()
{
    std::wstring wsPath(MAX_PATH, 0);
    DWORD dwPathSize = GetSystemDirectoryW(&wsPath.front(), MAX_PATH);
    wsPath.resize(dwPathSize);
    return wsPath;
}

// No '\' tail
std::wstring GetSystemWow64Path()
{
    std::wstring wsPath(MAX_PATH, 0);
    DWORD dwPathSize = GetSystemWow64DirectoryW(&wsPath.front(), MAX_PATH);
    wsPath.resize(dwPathSize);
    return wsPath;
}

std::wstring GetGlobalStartupDirectory()
{
    PWSTR pPath;
    HRESULT hRet = SHGetKnownFolderPath(FOLDERID_CommonStartup, 0, NULL, &pPath);
    std::wstring wsPath;
    if (hRet == S_OK) wsPath = pPath;
    CoTaskMemFree(pPath);
    return wsPath;
}

std::wstring AdjustPathTail(const std::wstring& wsPath)
{
    return wsPath.empty() ? L"" : wsPath + (wsPath.back() == L'\\' ? L"" : L"\\");
}
