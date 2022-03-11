cmake_minimum_required(VERSION 3.16)
project(INF01151_TF)

set(CMAKE_CXX_STANDARD 20)

set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(src/lib/nlohmann_json)


add_executable(
        INF01151_TF
        src/main.cpp

        src/aux_shared/message_types.h

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

        src/server/persistence/serverMemory.cpp
        src/server/persistence/serverMemory.h

        src/client/user_gui/user_gui.cpp
        src/client/user_gui/user_gui.h

        src/client/session/session.cpp
        src/client/session/session.h

        src/server/session_manager/session_manager.cpp
        src/server/session_manager/session_manager.h

        src/server/communication_manager/communication_manager.cpp
        src/server/communication_manager/communication_manager.h

        src/client/communication_manager/communication_manager.cpp
        src/client/communication_manager/communication_manager.h

        src/client/client_thread_runner.h
)


target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json ncurses pthread)

