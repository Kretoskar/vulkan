#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(int argc, char* argv[])
{
    constexpr int32_t width = 1080;
    constexpr int32_t height = 720;
    
    if (!glfwInit())
    {
        return 1;
    }

    if (!glfwVulkanSupported())
    {
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(width, height, "FF", nullptr, nullptr);
	
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    glfwSetKeyCallback(window, GLFW_KeyCallback);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}