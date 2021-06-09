#pragma once
#include <map>
#include <syscall.h>
#include "ptrace.h"
#include "logger.h"

namespace FileOpTrace {

class SyscallHandler {
public:
    SyscallHandler(PTrace::TraceApi& tracer, Logger* logger);
    
protected:
    void add_fd_path(pid_t pid, int fd, std::string& path);
    std::string path_for_fd(pid_t pid, int fd);
    void remove_fd(pid_t pid, int fd);
    std::string resolve_path(std::string& path, pid_t pid, int parent_fd);
    void trace_handler(PTrace::Tracee& tracee, int status);

private:
    std::string m_cwd;
    std::map<std::pair<pid_t, int>, std::string> m_fd_map;
    Logger& m_logger;
};

}