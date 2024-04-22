#include "ImguiHelper.h"
#include "GLFW/glfw3.h"

void SetFitParentWindow(GLFWwindow* window)
{
    int w = 0;
    int h = 0;
    glfwGetWindowSize(window, &w, &h);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(w, h));
}