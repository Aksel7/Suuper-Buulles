cmake_minimum_required(VERSION 4.0)
project(superpong C)

set(CMAKE_C_STANDARD 23)

add_executable(superpong main.c
        niveaux.c
        niveaux.h
        jeu.c
        boss.c
        boss.h
        graphismes.h
        graphismes.c
        jeu.h)
