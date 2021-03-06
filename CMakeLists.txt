cmake_minimum_required(VERSION 3.16)
project(INF01151)

set(CMAKE_CXX_STANDARD 20)

set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(src/lib/nlohmann_json)


add_executable(
        ${PROJECT_NAME}
        src/main.cpp

        src/aux_shared/message_types.h

        src/aux_shared/logger.cpp
        src/aux_shared/logger.h

        src/aux_shared/args_parser.cpp
        src/aux_shared/args_parser.h

        src/server/server_communication_manager/server_communication_manager.cpp
        src/server/server_communication_manager/server_communication_manager.h


        src/server/user/user.cpp
        src/server/user/user.h

        src/server/persistence/serverMemory.cpp
        src/server/persistence/serverMemory.h

        src/client/user_gui/user_gui.cpp
        src/client/user_gui/user_gui.h

        src/aux_shared/session/session.cpp
        src/aux_shared/session/session.h

        src/client/client_communication_manager/client_communication_manager.cpp
        src/client/client_communication_manager/client_communication_manager.h


        src/server/server.cpp
        src/server/server.h

        src/client/client.cpp
        src/client/client.h

        src/client/client_thread_runner.h
        src/server/server_thread_runner.h
        src/server/replica_manager/replica_manager.cpp
        src/server/replica_manager/replica_manager.h
        src/server/replica_manager/replica_thread_runner.h)


target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json ncurses pthread)

