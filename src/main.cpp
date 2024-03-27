#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad/gl.h"
#include "imageio.hpp"

typedef struct AppData {
    GLFWwindow* window;             // GLFW window
    int width;                      // output image/video width
    int height;                     // output image/video height
} AppData;

int main(int argc, char** argv)
{
    // Create application data
    AppData app;

    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Error initializing GLFW\n");
        return EXIT_FAILURE;
    }

    // Create application window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    app.window = glfwCreateWindow(256, 96, "Stacked 360 Depth", NULL, NULL);
    if (!app.window)
    {
        fprintf(stderr, "Error creating OpenGL Window\n");
        return EXIT_FAILURE;
    }

    // Make window's context current
    glfwMakeContextCurrent(app.window);
    glfwSwapInterval(1);
    

    // Initialize GLAD
    if (!gladLoadGL(glfwGetProcAddress))
    {
        fprintf(stderr, "Error initializing Glad (OpenGL Extension Loader)\n");
        return EXIT_FAILURE;
    }

    const unsigned char* gl_version = glGetString(GL_VERSION);
    const unsigned char* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
    printf("Using OpenGL %s, GLSL %s\n", gl_version, glsl_version);


    // Calculate depth maps
    // ... TODO


    // Clean up
    gladLoaderUnloadGL();
    glfwDestroyWindow(app.window);
    glfwTerminate();
    

    return EXIT_SUCCESS;
}