#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "FFCore/Core/Assert.h"
#include "FFCore/Core/Logger.h"
#include "FFCore/Core/Types.h"
#include "FFCore/Core/HString.h"
#include "FFCore/Math/Vec3.h"
#include "FFVulkan/test.h"

#include "FFEngine/Engine.h"


void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(int argc, char* argv[])
{
    constexpr i32 width = 1080;
    constexpr i32 height = 720;

    ASSERT(glfwInit(), "Failed to initialize GLFW")
    ASSERT(glfwVulkanSupported(), "GLFW does not support Vulkan")

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(width, height, "FF", nullptr, nullptr);
	
    if (!window)
    {
        glfwTerminate();
        LOG_ERROR("Failed to create GLFW window")
        return 3;
    }

    glfwSetKeyCallback(window, GLFW_KeyCallback);
    
    FFE::Engine engine;
    
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}