#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "pipelogger.h"

using namespace FileOpTrace;

bool PipeLogger::connect(std::string pipe_path) {
    if(access(pipe_path.c_str(), F_OK))
        return false;
    m_pipe = std::ofstream(pipe_path);
    return true;
}

void PipeLogger::open(std::string binpath, pid_t pid, std::string path, int mode, int fd) {
    auto str_mode = mode == O_RDONLY ? "read" : (mode == O_WRONLY ? "write" : "read/write");
    m_pipe << "[\"" << binpath << "\" (PID: " << pid << ")] " << "open at path: \"" << path << "\" for " << str_mode << ", fd: " << fd << std::endl;
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

void PipeLogger::rename(std::string binpath, pid_t pid, std::string old_path, std::string new_path) {
    m_pipe << "[\"" << binpath << "\" (PID: " << pid << ")] " << "rename: \"" << old_path << "\" to \"" << new_path << "\"" << std::endl;
}

void PipeLogger::remove(std::string binpath, pid_t pid, std::string path) {
    m_pipe << "[\"" << binpath << "\" (PID: " << pid << ")] " << "remove: \"" << path << "\"" << std::endl;
}

void PipeLogger::mkdir(std::string binpath, pid_t pid, std::string path) {
    m_pipe << "[\"" << binpath << "\" (PID: " << pid << ")] " << "mkdir: \"" << path << "\"" << std::endl;
}