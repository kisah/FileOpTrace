#pragma once
#include <string>

namespace FileOpTrace {

class Logger {
public:
    Logger() {}
    virtual ~Logger() {}
    virtual void open(std::string binpath, pid_t pid, std::string path, int mode, int fd) = 0;
    virtual void open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error) = 0;
    virtual void read(std::string binpath, pid_t pid, std::string path, int fd, size_t len) = 0;
    virtual void write(std::string binpath, pid_t pid, std::string path, int fd, size_t len) = 0;
    virtual void close(std::string binpath, pid_t pid, std::string path, int fd) = 0;
    virtual void rename(std::string binpath, pid_t pid, std::string old_path, std::string new_path) = 0;
    virtual void remove(std::string binpath, pid_t pid, std::string path) = 0;
    virtual void mkdir(std::string binpath, pid_t pid, std::string path) = 0;
};

}