cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

add_executable(PacketGeneratorUnitTest
    AppTest.cpp   
)

target_include_directories(PacketGeneratorUnitTest PRIVATE ../src ../lib)

target_link_libraries(PacketGeneratorUnitTest PRIVATE PacketGenerator)