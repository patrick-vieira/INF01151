cmake_minimum_required(VERSION 3.16)
project(INF01151_TF)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        INF01151_TF
        src/main.cpp
        src/aux_shared/logger.cpp
        src/aux_shared/logger.h
        src/aux_shared/args_parser.cpp
        src/aux_shared/args_parser.h
        src/server/server.cpp
        src/server/server.h
        src/client/client.cpp
        src/client/client.h
        src/server/user/user.cpp
        src/server/user/user.h
        src/server/memory/serverMemory.cpp
        src/server/memory/serverMemory.h
        src/client/user_menu/user_menu.cpp
        src/client/user_menu/user_menu.h)

target_link_libraries(${PROJECT_NAME} ncurses)