#pragma once
#include "logger.h"

namespace FileOpTrace {

/**
 * \brief A logger that outputs messages to stderr
 * 
 * A logger class is responsible for outputting messages
 * about file operations in a nice format
 */
class StdErrLogger : public Logger {
public:
    virtual void open(std::string binpath, pid_t pid, std::string path, int mode, int fd);
    virtual void open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error);
    virtual void read(std::string binpath, pid_t pid, std::string path, int fd, size_t len);
    virtual void write(std::string binpath, pid_t pid, std::string path, int fd, size_t len);
    virtual void close(std::string binpath, pid_t pid, std::string path, int fd);
    virtual void rename(std::string binpath, pid_t pid, std::string old_path, std::string new_path);
    virtual void remove(std::string binpath, pid_t pid, std::string path);
    virtual void mkdir(std::string binpath, pid_t pid, std::string path);
};

}