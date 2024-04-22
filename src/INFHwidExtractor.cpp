#include "INFHwidExtractor.h"
#include "StringConv.h"
#include "Path.h"

#include <fstream>
#include <iostream>
#include <set>
//#include <locale>
#include <setupapi.h>
#pragma comment(lib, "Setupapi.lib")

INFHwidExtractor::INFHwidExtractor()
{
}

INFHwidExtractor::~INFHwidExtractor()
{
}

bool INFHwidExtractor::BeginExtract(const std::wstring& wsOutDir, const std::vector<std::wstring>& vINFPaths)
{
    if (wsOutDir.empty() || vINFPaths.empty())
        return false;

    m_vINFObj.clear();

    for (int i = 0; i < vINFPaths.size(); i++)
    {
        INFObj infObj;

        infObj.wsINFName = GetINFName(vINFPaths[i]);

        if (infObj.wsINFName.empty())
            return false;

        infObj.vHwidRaw = GetRawHwids(vINFPaths[i]);

        if (infObj.vHwidRaw.empty())
            return false;

        infObj.vHwidTrim = GetTrimHwids(infObj.vHwidRaw);

        if (infObj.vHwidTrim.empty())
            return false;

        m_vINFObj.push_back(infObj);
    }
    WriteHwidsToFile(wsOutDir);
    return true;
}

std::vector<INFObj> INFHwidExtractor::GetINFObjects() const
{
    return m_vINFObj;
}

std::wstring INFHwidExtractor::GetINFName(const std::wstring& wsINFPath)
{
    std::size_t pos = wsINFPath.find_last_of(L"/\\");
    if (pos == std::wstring::npos)
    {
        std::wcout << L"Fail to retrieve Inf Name from path: " << wsINFPath << std::endl;
        return L"";
    }
    return wsINFPath.substr(pos + 1);
}

std::vector<std::wstring> INFHwidExtractor::GetRawHwids(const std::wstring& wsINFPath)
{
    if (wsINFPath.empty())
        return {};

    HINF hInf = INVALID_HANDLE_VALUE;
    INFCONTEXT InfContext;

    hInf = SetupOpenInfFileW(wsINFPath.c_str(), NULL, INF_STYLE_WIN4, NULL);

    if (hInf == INVALID_HANDLE_VALUE)
    {
        std::wcout << L"Fail to open INF file: " << wsINFPath << std::endl;
        return {};
    }
    if (!SetupFindFirstLineW(hInf, L"Manufacturer", NULL, &InfContext))
    {
        std::cout << "Fail to find first line of \"Manufacturer\" section" << std::endl;
        return {};
    }

    std::wstring wsBuf(MAX_PATH, L' ');
    DWORD dwRequiredSize{};
    std::vector<std::wstring> vManufacturer;
    std::wstring wsSecPrefix;
    DWORD dwIdx = 1;

    // Get Manufacturer list in Manufacturer section
    while (SetupGetStringFieldW(&InfContext, dwIdx, &wsBuf[0], wsBuf.size(), &dwRequiredSize) ||
        (dwRequiredSize <= 1))
    {
        wsBuf.resize(dwRequiredSize - 1);

        if (dwIdx == 1)
            vManufacturer.push_back(wsBuf);
        else
            vManufacturer.push_back(vManufacturer[0] + L"." + wsBuf);

        dwIdx++;
        ResizeWstring(wsBuf, MAX_PATH);
    }

    std::vector<std::wstring> vHwid;

    for (int i = 0; i < vManufacturer.size(); i++)
    {
        //std::wcout << L"-- [" + vManufacturer[i] + L"]" << std::endl;

        // Get first line
        if (!SetupFindFirstLineW(hInf, vManufacturer[i].c_str(), NULL, &InfContext))
        {
            std::wcout << L"Fail to find first line of \"Manufacturer\": " << vManufacturer[i] << std::endl;
            std::cout << "error: " << GetLastError() << std::endl;
            continue;
        }

        // Hwid locate in second param
        if (SetupGetStringFieldW(&InfContext, 2, &wsBuf[0], wsBuf.size(), &dwRequiredSize) ||
            (dwRequiredSize <= 1))
        {
            wsBuf.resize(dwRequiredSize - 1);
            vHwid.push_back(wsBuf);
            std::wcout << wsBuf << std::endl;
            ResizeWstring(wsBuf, MAX_PATH);
        }
        else
        {
            std::cout << " * Fail to get first String Field" << std::endl;
            return {};
        }

        // Get remaining line
        while (SetupFindNextLine(&InfContext, &InfContext))
        {
            if (SetupGetStringFieldW(&InfContext, 2, &wsBuf[0], wsBuf.size(), &dwRequiredSize) ||
                (dwRequiredSize <= 1))
            {
                wsBuf.resize(dwRequiredSize - 1);
                vHwid.push_back(wsBuf);

                std::wcout << wsBuf << std::endl;

                ResizeWstring(wsBuf, MAX_PATH);
            }
            else
            {
                std::cout << " * Fail to get remaining String Field" << std::endl;
                return {};
            }
        }
    }
    return vHwid;
}

void INFHwidExtractor::ResizeWstring(std::wstring& wsStr, DWORD dwLength)
{
    wsStr.clear();
    wsStr.resize(dwLength, L' ');
}

std::vector<std::wstring> INFHwidExtractor::GetTrimHwids(const std::vector<std::wstring>& vRawHwids)
{
    if (vRawHwids.empty())
    {
        std::cout << "Hwid container is empty" << std::endl;
        return {};
    }

    std::set<std::wstring> hwidSet;

    for (int i = 0; i < vRawHwids.size(); i++)
        hwidSet.insert(TrimHwidString(vRawHwids[i]));

    return std::vector<std::wstring>(hwidSet.begin(), hwidSet.end());
}

std::wstring INFHwidExtractor::TrimHwidString(const std::wstring& wsHwid)
{
    size_t  pos = 0;
    int     cnt = 0;
    size_t  pos2 = 0;

    while (cnt < 2)
    {
        //pos += 1;
        pos = wsHwid.find(L"&", pos);

        if (pos == std::wstring::npos)
            return wsHwid;

        pos++;
        cnt++;

        if (cnt == 2)
        {
            if ((wsHwid.find(L"SUBSYS", pos) != std::wstring::npos)
                || (wsHwid.find(L"REV_", pos) != std::wstring::npos))
            {
                return wsHwid.substr(0, pos - 1);
            }
            return wsHwid;
        }
    }
    return L"";
}

bool INFHwidExtractor::WriteHwidsToFile(const std::wstring& wsOutDir)
{
    if (wsOutDir.empty() || m_vINFObj.empty())
    {
        std::cout << "Cannot create file" << std::endl;
        return false;
    }

    std::wstring wsTxtPath = AdjustPathTail(wsOutDir) + L"hwids.txt";

    std::ofstream file(wsTxtPath);
    //std::wcout << L"Update txt path: " << wsTxtPath << std::endl;

    std::string strLine;

    for (int i = 0; i < m_vINFObj.size(); i++)
    {
        int j = 0;
        for (const std::wstring& hwidClean : m_vINFObj[i].vHwidTrim)
        {
            strLine += STDWStringToSTDString(hwidClean);

            if (i == m_vINFObj.size() - 1 && j == m_vINFObj[i].vHwidTrim.size() - 1)
            {
            }
            else
            {
                strLine += "\n";
            }
            j++;
        }
    }
    file << strLine;
    file.close();
}
