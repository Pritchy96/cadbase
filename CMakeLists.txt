cmake_minimum_required(VERSION 3.12)

set(APP_NAME cad_base)
set(THIRDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/thirdparty)
set(IMGUI_SRC_DIR ${THIRDPARTY_DIR}/imgui)

#Set the project name
project(${APP_NAME} VERSION 0.0.1)

set(CMAKE_BUILD_TYPE Debug)


#Put output binary (and shaders) in /bin instead of /build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

include_directories(include)
include_directories(${THIRDPARTY_DIR})
include_directories(${IMGUI_SRC_DIR})

#Glob all .cpp files under src into a SOURCES var
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*cpp")

#Add the executable
add_executable(${APP_NAME} 
    ${SOURCES} 
    ${IMGUI_SRC_DIR}/backends/imgui_impl_glfw.cpp 
    ${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.cpp)

target_compile_options(${APP_NAME} PRIVATE -Wall -O3) #Debug options

#GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${APP_NAME} glfw dl) #dl required for glfw

#OPENGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(${APP_NAME} OpenGL::GL)

#GLEW
find_package(GLEW REQUIRED)
target_link_libraries(${APP_NAME} GLEW::GLEW)

#Dear ImGui
set(IMGUI_SOURCES
    ${IMGUI_SRC_DIR}/imconfig.h
    ${IMGUI_SRC_DIR}/imgui_demo.cpp
    ${IMGUI_SRC_DIR}/imgui_draw.cpp
    ${IMGUI_SRC_DIR}/imgui_tables.cpp
    ${IMGUI_SRC_DIR}/imgui_internal.h
    ${IMGUI_SRC_DIR}/imgui_widgets.cpp
    ${IMGUI_SRC_DIR}/imgui.cpp
    ${IMGUI_SRC_DIR}/imgui.h
    ${IMGUI_SRC_DIR}/imstb_rectpack.h
    ${IMGUI_SRC_DIR}/imstb_textedit.h
    ${IMGUI_SRC_DIR}/imstb_truetype.h
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_link_libraries(${APP_NAME} imgui)

# Copy over shader files.
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/src/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
)
add_dependencies(cad_base copy_assets)

# Generate compile_commands.json and copy to root for clang-tidy.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(
    copy-compile-commands ALL
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
    )

add_dependencies(cad_base copy-compile-commands)
