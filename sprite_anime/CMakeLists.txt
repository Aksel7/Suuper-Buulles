cmake_minimum_required(VERSION 4.2)
project(sprite_anime C)

set(CMAKE_C_STANDARD 11)

add_executable(sprite_anime main.c
        main.c)
target_link_libraries(sprite_anime -mwin32 -lalleg44)