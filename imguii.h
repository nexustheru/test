#pragma once
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3_loader.h>
#include <backends/imgui_impl_glfw.h >
#include <cmath>
#include <GLFW/glfw3.h>

#include "monos.h"

using namespace std;

class Immguii
{
public:
    Immguii();
    ~Immguii();

    bool setup(GLFWwindow* window);
    void render();

private:
    bool dummy = true;
    Monoi* mo;
};


