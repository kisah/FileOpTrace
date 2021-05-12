#pragma once
#include <map>
#include <syscall.h>
#include "ptrace.h"
#include "logger.h"

class SyscallHandler {
public:
    SyscallHandler(PTrace::TraceApi& tracer, Logger& logger);
    
protected:
    std::string path_for_fd(pid_t pid, int fd);
    void remove_fd(int fd);
    std::string resolve_path(std::string& path, pid_t pid, int parent_fd);
    void trace_handler(PTrace::Tracee& tracee, int status);

private:
    std::string m_cwd;
    std::map<int, std::string> m_fd_map;
    Logger& m_logger;
};