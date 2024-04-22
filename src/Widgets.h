#pragma once

#include "FileDialog.h"
#include <vector>

namespace CustomWidget
{
    void BrowseFile(
        const std::string& strSubject,
        const std::string& strLabel,
        bool bSelDir,
        std::string& strSelectPath);

    bool BrowseToContainer(
        const std::string& strLabel,
        std::vector<std::wstring>& vContainer,
        const ImVec2& btnSize);

    void TextTable(
        const std::string& strLabel,
        const std::string& strTitle,
        const ImVec2& childSize,
        const ImVec2& tableSize,
        const std::vector<std::string>& vContainer);
}