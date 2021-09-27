cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 14)
project(chatTCPServer)

set(source_files
        main.cpp
)


add_executable(${CMAKE_PROJECT_NAME} ${source_files})
target_link_libraries(${CMAKE_PROJECT_NAME} ws2_32)

