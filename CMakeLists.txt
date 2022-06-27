cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(PacketGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall")

include_directories(${PROJECT_SOURCE_DIR}/src)

add_library(PacketGenerator STATIC 
    src/PacketGenerator.cpp    
    src/Utils.cpp    
)

target_include_directories(PacketGenerator PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

add_executable(packet_generator 
    src/main.cpp    
)

target_link_libraries(packet_generator PRIVATE PacketGenerator)

add_subdirectory(test)