cmake_minimum_required(VERSION 4.0)
project(superpong C)

set(CMAKE_C_STANDARD 23)

include_directories("C:/informatique ing 1v2/projet2/superpong/MinGW_avec_Allegro4/MinGW avec Allegro 4/MinGW/include")
link_directories("C:/informatique ing 1v2/projet2/superpong/MinGW_avec_Allegro4/MinGW avec Allegro 4/MinGW/lib")

add_executable(superpong main.c
        niveaux.c
        niveaux.h
        jeu.c
        graphismes.h
        graphismes.c
        jeu.h
        joueurs.c
        joueurs.h
        menu.c
        menu.h
        logique.c
        logique.h
        ihm.c
        ihm.h
        types.h)

target_link_libraries(superpong alleg44)