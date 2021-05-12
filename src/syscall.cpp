#include <iostream>
#include <limits.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <string.h>
#include <unistd.h>
#include "syscall.h"

using namespace PTrace;

SyscallHandler::SyscallHandler(TraceApi& tracer, Logger& logger) : m_logger(logger) {
    std::array<char, PATH_MAX> buf;
    getcwd(buf.data(), buf.size());
    m_cwd = std::string(buf.data());

    tracer.register_handler([this](Tracee& tracee, int status) {
        trace_handler(tracee, status);
    });
}

std::string SyscallHandler::path_for_fd(pid_t pid, int fd) {
    auto iter = m_fd_map.find(fd);
    if(iter != m_fd_map.end())
        return (*iter).second;
    else {
        std::array<char, 128> fd_path;
        std::array<char, PATH_MAX> buf;
        
        buf.fill(0);
        snprintf(fd_path.data(), fd_path.size(), "/proc/%d/fd/%d", pid, fd);
        if(readlink(fd_path.data(), buf.data(), buf.size()) >= 0) {
            std::string path(buf.data());
            m_fd_map.emplace(fd, path);
            return path;
        }
        else
            return ":unknown:";
    }
}

void SyscallHandler::remove_fd(int fd) {
    auto iter = m_fd_map.find(fd);
    if(iter != m_fd_map.end())
        m_fd_map.erase(iter);
}

//TODO: Proper path resolution
void SyscallHandler::trace_handler(Tracee& tracee, int status) {
    if(WSTOPSIG(status) == SIGTRAP && (status >> 16) == PTRACE_EVENT_SECCOMP) {
        //std::cout << "syscall handler triggered\n";
        user_regs_struct regs = tracee.get_registers();
        int ret_fd;
        int fd;
        std::map<int, std::string>::iterator iter;
        std::string path;
        switch(regs.orig_rax) {
            case __NR_chdir:
                m_cwd = tracee.read_string(regs.rdi);
                break;
            case __NR_openat:
                ret_fd = static_cast<int>(tracee.syscall_ret_value());
                path = tracee.read_string(regs.rsi);
                //std::cout << "openat parent_fd: " << static_cast<int>(regs.rdi) << ", path: \"" << tracee.read_string(regs.rsi) << "\", ret_fd: " << ret_fd << "\n";
                if(m_fd_map.find(ret_fd) == m_fd_map.end())
                    m_fd_map.emplace(ret_fd, path);
                m_logger.open(tracee.get_binpath(), tracee.get_pid(), path, static_cast<int>(regs.rdx), ret_fd);
                break;
            case __NR_close:
                fd = static_cast<int>(regs.rdi);
                path = path_for_fd(tracee.get_pid(), fd);
                remove_fd(fd);
                m_logger.close(tracee.get_binpath(), tracee.get_pid(), path, fd);
                break;
            default:
                break;
        }
        tracee.cont(0);
    }
    else {
        tracee.cont(WSTOPSIG(status));
    }
}