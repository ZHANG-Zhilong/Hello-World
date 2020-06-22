cmake_minimum_required(VERSION 3.16)
project(server)

set(CMAKE_CXX_STANDARD 14)
include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(base)
add_subdirectory(net)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(s00)
endif()

add_executable(server main.cpp)
target_link_libraries(server server_base server_net)





