#include "imguii.h"

Immguii::Immguii()
{
    mo = new Monoi();

}

Immguii::~Immguii()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
       
    }
}


bool Immguii::setup(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();;
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
   // glfwSetMouseButtonCallback(window, MouseButtonCallback);
    if (mo->isRunning)
    {
        mo->PluginOnLoad(nullptr);
    }
    return true;
}

void Immguii::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (mo->isRunning)
    {
        mo->PluginOnUpdate(nullptr);
    }

    ImGui::Begin("Menu");
    if (ImGui::Button("Scann for raw Script"))
    {
        mo->Get_Scripts();      
    }
    if (ImGui::Button("Compile Scripts"))
    {
        mo->compile();
    }
    if (ImGui::Button("Load Scripts"))
    {
        mo->Get_Dlls();
    }
    if (ImGui::Button("update Mono"))
    {
        mo->Init();
    }
    if (ImGui::Button("Exit Mono"))
    {
        mo->~Monoi();
    }

    if (ImGui::TreeNode("Configuration##2"))
    {
        ImGui::Checkbox("io.ConfigMacOSXBehaviors", &dummy);
        ImGui::Text("Also see Style->Rendering for rendering options.");
        ImGui::Separator();
        ImGui::Checkbox("io.ConfigDebugIsDebuggerPresent", &dummy);
        ImGui::SameLine();
    }
    ImGui::TreePop();
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
