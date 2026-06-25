cmake_minimum_required(VERSION 3.15)
project(PROJETC VERSION 1.0 LANGUAGES C CXX)

set(WITH_PIC ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

#set(GLFW_BUILD_WIN32 OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
#set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
#set(GLFW_BUILD_COCOA ON CACHE BOOL "" FORCE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib/CMakeLists.txt")
    add_subdirectory(libs/raylib)
else()
    message(FATAL_ERROR "Raylib introuvable dans libs/raylib")
endif()

add_executable(${PROJECT_NAME} 
    src/main.c
    src/gestion_matrice.c
    src/detection_alignements.c
    src/gestion_niveaux.c
    src/sauvegarde.c
    src/rendering.c
    src/teste.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib/src
)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/bin")