cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 99)
project(bbb2pcap VERSION 1.0.0)

add_executable(bbb2pcap bbb2pcap.cpp)
target_link_libraries(bbb2pcap ../pcap/Lib_x64/wpcap)
target_link_libraries(bbb2pcap ../pcap/Lib_x64/Packet)
#include_directories(${CMAKE_SOURCE_DIR}/bcl_lib)
include_directories(${CMAKE_SOURCE_DIR}/pcap/include)
