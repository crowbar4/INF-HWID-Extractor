#include "Imgui/imgui.h"
#include "Imgui/imgui_stdlib.h"

#include "Widgets.h"
#include "StringConv.h"

#include <string>

namespace CustomWidget
{
    void BrowseFile(
        const std::string& strSubject,
        const std::string& strLabel,
        bool bSelDir,
        std::string& strSelectPath)
    {
        ImGui::Text(strSubject.c_str());

        std::string label = "##" + strLabel;
        ImGui::InputText(label.c_str(), &strSelectPath, ImGuiInputTextFlags_ReadOnly, NULL, NULL);
        ImGui::SameLine();

        if (ImGui::Button(std::string("Browse" + label + "btn").c_str()))
        {
            strSelectPath = STDWStringToSTDString(OpenFileDialog(bSelDir));
        }
    }

    bool BrowseToContainer(
        const std::string& strLabel,
        std::vector<std::wstring>& vContainer,
        const ImVec2& btnSize)
    {
        std::string label = "##" + strLabel;
        if (ImGui::Button(std::string("Add" + label + "btn").c_str(), btnSize))
        {
            std::wstring wsPath = OpenFileDialog(false);
            if (!wsPath.empty())
            {
                vContainer.push_back(wsPath);
                return true;
            }
        }
        return false;
    }

    void TextTable(
        const std::string& strLabel,
        const std::string& strTitle,
        const ImVec2& childSize,
        const ImVec2& tableSize,
        const std::vector<std::string>& vContainer)
    {
        ImGui::BeginChild(std::string(strLabel + "list").c_str(), childSize);
        ImGui::Text(strTitle.c_str());
        if (ImGui::BeginTable(strLabel.c_str(), 1, ImGuiTableColumnFlags_WidthStretch | ImGuiTableFlags_Borders, tableSize))
        {
            ImGuiListClipper clipper;
            clipper.Begin(vContainer.size());

            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text(vContainer[row_n].c_str());
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
}