#include <iostream>
#include <fcntl.h>
#include <string.h>
#include "stdlogger.h"

void StdErrLogger::open(std::string binpath, pid_t pid, std::string path, int mode, int fd) {
    auto str_mode = mode == O_RDONLY ? "read" : (mode == O_WRONLY ? "write" : "read/write");
    std::cerr << "[\"" << binpath << "\" (PID: " << pid << ")] " << "open at path: \"" << path << "\" for " << str_mode << ", fd: " << fd << std::endl;
}

void StdErrLogger::open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error) {
    std::cerr << "[\"" << binpath << "\" (PID: " << pid << ")] " << "open FAILED at path: \"" << path << "\", reason: " << strerror(error) << std::endl;
}

void StdErrLogger::read(std::string binpath, pid_t pid, std::string path, int fd, size_t len) {
    std::cerr << "[\"" << binpath << "\" (PID: " << pid << ")] read " << len << " bytes from path: \"" << path << "\", fd: " << fd << std::endl;
}

void StdErrLogger::write(std::string binpath, pid_t pid, std::string path, int fd, size_t len) {
    std::cerr << "[\"" << binpath << "\" (PID: " << pid << ")] written " << len << " bytes to path: \"" << path << "\", fd: " << fd << std::endl;
}

void StdErrLogger::close(std::string binpath, pid_t pid, std::string path, int fd) {
    std::cerr << "[\"" << binpath << "\" (PID: " << pid << ")] " << "close: \"" << path << "\", fd: " << fd << std::endl;
}

void StdErrLogger::remove(std::string binpath, pid_t pid, std::string path) {
    std::cerr << "[\"" << binpath << "\" (PID: " << pid << ")] " << "remove: \"" << path << "\"" << std::endl;
}