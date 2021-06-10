#pragma once
#include <fstream>
#include "logger.h"

namespace FileOpTrace {

/**
 * \brief A logger that outputs messages to a named pipe, which is connected to a running FrontEnd
 * 
 * A logger is responsible for outputting messages
 * about file operations in a nice format
 */
class PipeLogger : public Logger {
public:
    /**
     * \brief Connect the logger to a specified named pipe
     * \param pipe_path Path to a named pipe
     * \return Success status
     */
    bool connect(std::string pipe_path);

    virtual void open(std::string binpath, pid_t pid, std::string path, int mode, int fd);
    virtual void open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error);
    virtual void read(std::string binpath, pid_t pid, std::string path, int fd, size_t len);
    virtual void write(std::string binpath, pid_t pid, std::string path, int fd, size_t len);
    virtual void close(std::string binpath, pid_t pid, std::string path, int fd);
    virtual void rename(std::string binpath, pid_t pid, std::string old_path, std::string new_path);
    virtual void remove(std::string binpath, pid_t pid, std::string path);
    virtual void mkdir(std::string binpath, pid_t pid, std::string path);
private:
    /// A file stream of the specified named pipe
    std::ofstream m_pipe;
};

}