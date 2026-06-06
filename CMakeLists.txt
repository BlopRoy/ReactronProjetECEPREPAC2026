cmake_minimum_required(VERSION 3.15)
# 📌 FIX 1 : On ajoute CXX pour activer le compilateur C++
project(PROJETC VERSION 1.0 LANGUAGES C CXX)

# 1. Configuration des options de Raylib avant de l'inclure
set(WITH_PIC ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

# Configuration pour Arch Linux (Wayland / Desktop)
set(PLATFORM "Desktop" CACHE STRING "" FORCE)
set(GRAPHICS "GRAPHICS_API_OPENGL_33" CACHE STRING "" FORCE)

# 2. Inclusion de la librairie Raylib depuis ton dossier libs
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib/CMakeLists.txt")
    add_subdirectory(libs/raylib)
else()
    message(FATAL_ERROR "Raylib introuvable dans libs/raylib ! Vérifie le chemin.")
endif()

# 3. Déclaration de ton exécutable
# 📌 FIX 2 : Change l'extension de ton fichier en .cpp si tu veux faire du C++ !
add_executable(${PROJECT_NAME} 
    src/main.c
    src/gestion_matrice.c
    src/detection_alignements.c
    src/gestion_niveaux.c
    src/sauvegarde.c
    src/rendering.c
)

# 📌 FORCE LE BINAIRE À ALLER DANS LE DOSSIER 'bin'
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)

# Configuration de C++23
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# 4. INCLUSION DES DOSSIERS DE HEADERS (.h)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib/src
)

# 5. Liage de Raylib et des dépendances Arch Linux
target_link_libraries(${PROJECT_NAME} PRIVATE 
    raylib
    wayland-client
    wayland-cursor
    wayland-egl
    xkbcommon
)

# 6. Gestion des assets (Copie automatique dans le dossier build/bin)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/bin")
else()
    message(WARNING "Le dossier 'assets' à la racine est introuvable.")
endif()
