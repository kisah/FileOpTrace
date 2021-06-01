cmake_minimum_required(VERSION 3.10.0)
add_executable(fileoptrace main.cpp frontend.cpp ptrace.cpp bpf.cpp syscall.cpp stdlogger.cpp pipelogger.cpp)
target_link_libraries(fileoptrace PUBLIC doctest::doctest CLI11::CLI11 default)