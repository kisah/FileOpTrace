#include <iostream>
#include <string.h>
#include "stdlogger.h"

void StdErrLogger::open(std::string binpath, pid_t pid, std::string path, int mode, int fd) {
    std::cerr << "[\"" << binpath << "\" (PID: " << pid << ")] " << "open at path: \"" << path << "\", fd: " << fd << "\n";
}

void StdErrLogger::open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error) {
    std::cerr << "[\"" << binpath << "\" (PID: " << pid << ")] " << "open FAILED at path: \"" << path << "\", reason: " << strerror(error) << "\n";
}

void StdErrLogger::read(std::string binpath, pid_t pid, std::string path, int fd, size_t len) {
    //TODO
}

void StdErrLogger::write(std::string binpath, pid_t pid, std::string path, int fd, size_t len) {
    //TODO
}

void StdErrLogger::close(std::string binpath, pid_t pid, std::string path, int fd) {
    std::cerr << "[\"" << binpath << "\" (PID: " << pid << ")] " << "close: \"" << path << "\", fd: " << fd << "\n";
}