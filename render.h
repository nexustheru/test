#pragma once

#include "imguii.h"
#include <GLFW\glfw3.h>
#include <GL/freeglut.h>

using namespace std;

GLFWwindow* window;
Immguii* mygu;

bool glwsetup()
{
    if (!glfwInit())
    {
        return false;
    }
    window = glfwCreateWindow(640, 480, "Open gl", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
  mygu = new Immguii();
 mygu->setup(window);
    return true;

}
void render()
{
    glwsetup();
        
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        mygu->render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
