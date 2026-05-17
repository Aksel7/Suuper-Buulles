cmake_minimum_required(VERSION 3.10)

set(MINGW_DIR "C:/informatique ing 1v2/projet2/superpong/MinGW_avec_Allegro4/MinGW avec Allegro 4/MinGW")
set(CMAKE_C_COMPILER "${MINGW_DIR}/bin/gcc.exe")

project(superpong C)
set(CMAKE_C_STANDARD 99)

add_executable(superpong
        main.c
        niveaux.c niveaux.h
        jeu.c jeu.h
        graphismes.c graphismes.h
        joueurs.c joueurs.h
        menu.c menu.h
        logique.c logique.h
        ihm.c ihm.h
        types.h
)

target_include_directories(superpong PRIVATE "${MINGW_DIR}/include")
target_link_directories(superpong PRIVATE    "${MINGW_DIR}/lib")
target_link_libraries(superpong alleg44 kernel32 user32 gdi32 winmm shell32)

set_target_properties(superpong PROPERTIES LINK_FLAGS "-mwindows -mconsole")