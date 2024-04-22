#pragma once

#include <windows.h>
#include <string>
#include <vector>

struct INFObj
{
    std::wstring wsINFName;
    std::vector<std::wstring> vHwidRaw;
    std::vector<std::wstring> vHwidTrim;
};

class INFHwidExtractor
{
public:
    INFHwidExtractor();
    ~INFHwidExtractor();

    bool BeginExtract(const std::wstring& wsOutDir, const std::vector<std::wstring>& vINFPaths);

    std::vector<INFObj> GetINFObjects() const;

    // Ban Copy & Assign
    INFHwidExtractor(const INFHwidExtractor&) = delete;
    INFHwidExtractor& operator=(const INFHwidExtractor&) = delete;

private:
    std::wstring GetINFName(const std::wstring& wsINFPath);
    std::vector<std::wstring> GetRawHwids(const std::wstring& wsINFPath);
    std::vector<std::wstring> GetTrimHwids(const std::vector<std::wstring>& vRawHwids);

    std::wstring TrimHwidString(const std::wstring& wsHwid);
    void ResizeWstring(std::wstring& wsStr, DWORD dwLen);
    bool WriteHwidsToFile(const std::wstring& wsOutDir);

    std::vector<INFObj> m_vINFObj;
};
