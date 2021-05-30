#include <string.h>
#include <unistd.h>
#include "pipelogger.h"

bool PipeLogger::connect(std::string pipe_path) {
    if(access(pipe_path.c_str(), F_OK))
        return false;
    m_pipe = std::ofstream(pipe_path);
    return true;
}

void PipeLogger::open(std::string binpath, pid_t pid, std::string path, int mode, int fd) {
    m_pipe << "[\"" << binpath << "\" (PID: " << pid << ")] " << "open at path: \"" << path << "\", fd: " << fd << std::endl;
}

void PipeLogger::open_failed(std::string binpath, pid_t pid, std::string path, int mode, int error) {
    m_pipe << "[\"" << binpath << "\" (PID: " << pid << ")] " << "open FAILED at path: \"" << path << "\", reason: " << strerror(error) << std::endl;
}

void PipeLogger::read(std::string binpath, pid_t pid, std::string path, int fd, size_t len) {
    m_pipe << "[\"" << binpath << "\" (PID: " << pid << ")] read " << len << " bytes from path: \"" << path << "\", fd: " << fd << std::endl;
}

void PipeLogger::write(std::string binpath, pid_t pid, std::string path, int fd, size_t len) {
    m_pipe << "[\"" << binpath << "\" (PID: " << pid << ")] written " << len << " bytes to path: \"" << path << "\", fd: " << fd << std::endl;
}

void PipeLogger::close(std::string binpath, pid_t pid, std::string path, int fd) {
    m_pipe << "[\"" << binpath << "\" (PID: " << pid << ")] " << "close: \"" << path << "\", fd: " << fd << std::endl;
}