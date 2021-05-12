#pragma once
#include <string>

class Logger {
public:
    Logger() {}
    virtual ~Logger() {}
    virtual void open(std::string binpath, pid_t pid, std::string path, int mode, int fd) = 0;
    virtual void read(std::string binpath, pid_t pid, std::string path, int fd, size_t len) = 0;
    virtual void write(std::string binpath, pid_t pid, std::string path, int fd, size_t len) = 0;
    virtual void close(std::string binpath, pid_t pid, std::string path, int fd) = 0;
};