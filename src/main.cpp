#include <iostream>
#include <string>
#include <map>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad/gl.h"
#include "glslloader.h"
#include "imageio.hpp"

typedef struct GlslProgram {
    GLuint program;
    std::map<std::string,GLint> uniforms;
} GlslProgram;

typedef struct AppData {
    GLFWwindow* window;             // GLFW window
    int window_width;               // Width of window
    int window_height;              // Height of window
    int block_size;                 // Width/height of blocks to compare
    GLuint framebuffer;             // RTT framebuffer object
    GLuint framebuffer_texture;     // RTT framebuffer texture
    GlslProgram glsl_program;       // GLSL program
    GLuint vertex_position_attrib;  // Vertex position attribute
    GLuint vertex_texcoord_attrib;  // Vertex texture coordinate attribute
} AppData;

void init(AppData *app_ptr);
bool createRttFramebuffer(AppData *app_ptr);
void loadShader(AppData *app_ptr, std::string shader_filename_base);

int main(int argc, char **argv)
{
    // Create application data
    AppData app;
    app.window_width = 256;
    app.window_height = 96;

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


    // Initialize application
    init(&app);

    // Calculate depth maps
    // ... TODO


    // Clean up
    gladLoaderUnloadGL();
    glfwDestroyWindow(app.window);
    glfwTerminate();
    

    return EXIT_SUCCESS;
}

void init(AppData *app_ptr)
{
    // Set OpenGL settings
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, app_ptr->window_width, app_ptr->window_height);

    // Create RTT framebuffer
    if (!createRttFramebuffer(app_ptr))
    {
        fprintf(stderr, "Error creating RTT framebuffer object\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialize vertex attributes
    app_ptr->vertex_position_attrib = 0;
    app_ptr->vertex_texcoord_attrib = 1;

    // Load shader programs
    loadShader(app_ptr, "resrc/shaders/nolight_texture");
}

bool createRttFramebuffer(AppData *app_ptr)
{
    // Create RTT texture
    glGenTextures(1, &(app_ptr->framebuffer_texture));
    glBindTexture(GL_TEXTURE_2D, app_ptr->framebuffer_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app_ptr->block_size, app_ptr->block_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create RTT depth buffer
    GLuint depthbuffer;
    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, app_ptr->block_size, app_ptr->block_size);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Create RTT framebuffer (and attach texture / depth buffer to it)
    glGenFramebuffers(1, &(app_ptr->framebuffer));
    glBindFramebuffer(GL_FRAMEBUFFER, app_ptr->framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, app_ptr->framebuffer_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

    // Set framebuffers list of draw buffers
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Check if framebuffer was created successfully
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void loadShader(AppData *app_ptr, std::string shader_filename_base)
{
    // Compile GPU program
    std::string vert_filename = shader_filename_base + ".vert";
    std::string frag_filename = shader_filename_base + ".frag";
    app_ptr->glsl_program.program = glsl::createShaderProgram(vert_filename.c_str(), frag_filename.c_str());

    // Specify input and output attributes for the GPU program
    glBindAttribLocation(app_ptr->glsl_program.program, app_ptr->vertex_position_attrib, "vertex_position");
    glBindAttribLocation(app_ptr->glsl_program.program, app_ptr->vertex_texcoord_attrib, "vertex_texcoord");
    glBindFragDataLocation(app_ptr->glsl_program.program, 0, "FragColor");

    // Link compiled GPU program
    glsl::linkShaderProgram(app_ptr->glsl_program.program);

    // Get handles to uniform variables defined in the shaders
    glsl::getShaderProgramUniforms(app_ptr->glsl_program.program, app_ptr->glsl_program.uniforms);
}
