cmake_minimum_required(VERSION 3.10.0)
add_executable(tests test.cpp testlogger.cpp ../main/ptrace.cpp ../main/syscall.cpp ../main/bpf.cpp)
target_link_libraries(tests PUBLIC doctest::doctest default)
doctest_discover_tests(tests)