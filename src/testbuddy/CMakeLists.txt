cmake_minimum_required(VERSION 3.10.0)
add_executable(testbuddy testbuddy.cpp)
target_link_libraries(testbuddy PUBLIC CLI11::CLI11 default)