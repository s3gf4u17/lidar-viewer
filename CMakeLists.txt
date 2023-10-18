cmake_minimum_required(VERSION 3.10.2)

project(grafika3d-projekt1)

set(CMAKE_CXX_FLAGS "-O3")

set(IMGUI lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp lib/imgui/imgui.cpp lib/imgui/backends/imgui_impl_glfw.cpp lib/imgui/backends/imgui_impl_opengl3.cpp)
add_executable(${PROJECT_NAME} src/main.cpp ${IMGUI})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(lib/glfw)
add_subdirectory(lib/glad)
add_subdirectory(lib/glm)

target_include_directories(${PROJECT_NAME} PUBLIC lib/imgui)
target_include_directories(${PROJECT_NAME} PUBLIC lib/imgui/backends)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad glm)
