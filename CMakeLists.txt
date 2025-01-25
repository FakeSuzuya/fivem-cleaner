cmake_minimum_required(VERSION 3.10)

project(FiveMCleaner)

set(CMAKE_CXX_STANDARD 17)

add_executable(FiveMCleaner
    main.cpp
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
)

# Inclure les répertoires d'en-tête
include_directories(imgui glfw/include)

# Spécifier les répertoires de bibliothèque
link_directories(${CMAKE_SOURCE_DIR}/glfw/lib)

if (WIN32)
    # Utilisez 'glfw3' au lieu de 'glfw' si le fichier est nommé 'glfw3.lib'
    target_link_libraries(FiveMCleaner glfw3 ${CMAKE_DL_LIBS} opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(FiveMCleaner glfw3 ${CMAKE_DL_LIBS} OpenGL::GL)
endif()