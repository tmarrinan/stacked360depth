cmake_minimum_required(VERSION 3.12)

# Define project
project(Stacked360Depth)


# Get user home directory
IF (WIN32)
  set(USER_HOME_DIRECTORY $ENV{USERPROFILE})
ELSE()
  set(USER_HOME_DIRECTORY $ENV{HOME})
ENDIF()


# Set GLFW directories
set(GLFW_INC ${USER_HOME_DIRECTORY}/local/include)
find_library(GLFW_LIB
    NAMES glfw3 glfw3dll
    HINTS ${USER_HOME_DIRECTORY}/local/lib
)

# Set GLM directories
set(GLM_INC ${USER_HOME_DIRECTORY}/local/include)
find_library(GLM_LIB
    NAMES glm
    HINTS ${USER_HOME_DIRECTORY}/local/lib
)

# Set project include directories
set(INCLUDE_DIRS ${GLFW_INC} ${GLM_INC} ${PROJECT_SOURCE_DIR}/include)


# Set C++ sources
set(SOURCES src/gl.c src/glslloader.cpp src/main.cpp)


# Create executable application
add_executable(Stacked360Depth ${SOURCES})
target_include_directories(Stacked360Depth PRIVATE ${INCLUDE_DIRS})
target_link_libraries(Stacked360Depth ${GLFW_LIB} ${GLM_LIB})
