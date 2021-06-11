#include <iostream>
#include <fcntl.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <string.h>
#include <unistd.h>
#include "syscall.h"

using namespace FileOpTrace;
using namespace PTrace;

SyscallHandler::SyscallHandler(TraceApi& tracer, Logger* logger) : m_logger(*logger) {
    std::array<char, PATH_MAX> buf;
    getcwd(buf.data(), buf.size());
    m_tracer_cwd = std::string(buf.data());

    tracer.register_handler([this](TraceEvent event, Tracee& tracee, int status) {
        trace_handler(event, tracee, status);
    });
}

void SyscallHandler::add_fd_path(pid_t pid, int fd, std::string& path) {
    auto pair = std::make_pair(pid, fd);
    if(m_fd_map.find(pair) == m_fd_map.end())
        m_fd_map.emplace(pair, path);
}

std::string SyscallHandler::path_for_fd(pid_t pid, int fd) {
    auto pair = std::make_pair(pid, fd);
    auto iter = m_fd_map.find(pair);
    if(iter != m_fd_map.end())
        return (*iter).second;
    else {
        std::array<char, 128> fd_path;
        std::array<char, PATH_MAX> buf;
        
        buf.fill(0);
        snprintf(fd_path.data(), fd_path.size(), "/proc/%d/fd/%d", pid, fd);
        if(readlink(fd_path.data(), buf.data(), buf.size()) >= 0) {
            std::string path(buf.data());
            m_fd_map.emplace(pair, path);
            return path;
        }
        else
            return ":unknown:";
    }
}

void SyscallHandler::remove_fd(pid_t pid, int fd) {
    auto iter = m_fd_map.find(std::make_pair(pid, fd));
    if(iter != m_fd_map.end())
        m_fd_map.erase(iter);
}

std::string SyscallHandler::resolve_path(std::string& path, pid_t pid, int parent_fd) {
    std::string parent_path;
    std::array<char, PATH_MAX> buf;
    buf.fill(0);

    if(parent_fd == AT_FDCWD)
        parent_path = get_cwd(pid);
    else
        parent_path = path_for_fd(pid, parent_fd);
    if(parent_path[parent_path.length() - 1] != '/')
        parent_path += '/';
    auto path_concat = parent_path + path;
    if(!realpath(path_concat.c_str(), buf.data()))
        return path_concat;

    return std::string(buf.data());
}

std::string& SyscallHandler::get_cwd(pid_t pid) {
    auto iter = m_cwd_map.find(pid);
    if(iter != m_cwd_map.end())
        return (*iter).second;
    else
        return m_tracer_cwd;
}

void SyscallHandler::set_cwd(pid_t pid, std::string cwd) {
    auto iter = m_cwd_map.find(pid);
    if(iter != m_cwd_map.end())
        m_cwd_map.erase(iter);
    m_cwd_map.emplace(pid, cwd);
}

void SyscallHandler::trace_handler(TraceEvent event, Tracee& tracee, int status) {
    if(event == TRACE_SIGNAL) {
        if(WSTOPSIG(status) == SIGTRAP && (status >> 16) == PTRACE_EVENT_SECCOMP) {
            user_regs_struct regs = tracee.get_registers();
            int ret_fd;
            int fd;
            std::map<int, std::string>::iterator iter;
            std::string path;
            std::string path2;
            switch(regs.orig_rax) {
                case __NR_chdir:
                    path = tracee.read_string(regs.rdi);
                    if(path.length() && path[0] != '/')
                        set_cwd(tracee.get_pid(), resolve_path(path, tracee.get_pid(), AT_FDCWD));
                    else
                        set_cwd(tracee.get_pid(), path);
                    break;
                case __NR_fchdir:
                    path = path_for_fd(tracee.get_pid(), static_cast<int>(regs.rdi));
                    set_cwd(tracee.get_pid(), path);
                    break;
                case __NR_openat:
                    ret_fd = static_cast<int>(tracee.syscall_ret_value());
                    path = tracee.read_string(regs.rsi);
                    if(ret_fd < 0) {
                        m_logger.open_failed(tracee.get_binpath(), tracee.get_pid(), path, static_cast<int>(regs.rdx), -ret_fd);
                        break;
                    }
                    if(path.length() && path[0] != '/')
                        path = resolve_path(path, tracee.get_pid(), static_cast<int>(regs.rdi));
                    if(ret_fd >= 0)
                        add_fd_path(tracee.get_pid(), ret_fd, path);
                    m_logger.open(tracee.get_binpath(), tracee.get_pid(), path, static_cast<int>(regs.rdx) & 3, ret_fd);
                    break;
                case __NR_read:
                    fd = static_cast<int>(regs.rdi);
                    m_logger.read(tracee.get_binpath(), tracee.get_pid(), path_for_fd(tracee.get_pid(), fd), fd, tracee.syscall_ret_value());
                    break;
                case __NR_write:
                    fd = static_cast<int>(regs.rdi);
                    m_logger.write(tracee.get_binpath(), tracee.get_pid(), path_for_fd(tracee.get_pid(), fd), fd, tracee.syscall_ret_value());
                    break;
                case __NR_close:
                    fd = static_cast<int>(regs.rdi);
                    path = path_for_fd(tracee.get_pid(), fd);
                    remove_fd(tracee.get_pid(), fd);
                    m_logger.close(tracee.get_binpath(), tracee.get_pid(), path, fd);
                    break;
                case __NR_unlink:
                case __NR_rmdir:
                    path = tracee.read_string(regs.rdi);
                    if(path.length() && path[0] != '/')
                        path = resolve_path(path, tracee.get_pid(), AT_FDCWD);
                    m_logger.remove(tracee.get_binpath(), tracee.get_pid(), path);
                    break;
                case __NR_rename:
                    path = tracee.read_string(regs.rdi);
                    path2 = tracee.read_string(regs.rsi);
                    if(path.length() && path[0] != '/')
                        path = resolve_path(path, tracee.get_pid(), AT_FDCWD);
                    if(path2.length() && path2[0] != '/')
                        path2 = resolve_path(path2, tracee.get_pid(), AT_FDCWD);
                    m_logger.rename(tracee.get_binpath(), tracee.get_pid(), path, path2);
                    break;
                case __NR_renameat:
                case __NR_renameat2:
                    path = tracee.read_string(regs.rsi);
                    path2 = tracee.read_string(regs.r10);
                    if(path.length() && path[0] != '/')
                        path = resolve_path(path, tracee.get_pid(), static_cast<int>(regs.rdi));
                    if(path2.length() && path2[0] != '/')
                        path2 = resolve_path(path2, tracee.get_pid(), static_cast<int>(regs.rdx));
                    m_logger.rename(tracee.get_binpath(), tracee.get_pid(), path, path2);
                    break;
                    break;
                case __NR_unlinkat:
                    path = tracee.read_string(regs.rsi);
                    if(path.length() && path[0] != '/')
                        path = resolve_path(path, tracee.get_pid(), static_cast<int>(regs.rdi));
                    m_logger.remove(tracee.get_binpath(), tracee.get_pid(), path);
                    break;
                case __NR_mkdir:
                    path = tracee.read_string(regs.rdi);
                    if(path.length() && path[0] != '/')
                        path = resolve_path(path, tracee.get_pid(), AT_FDCWD);
                    m_logger.mkdir(tracee.get_binpath(), tracee.get_pid(), path);
                    break;
                case __NR_mkdirat:
                    path = tracee.read_string(regs.rsi);
                    if(path.length() && path[0] != '/')
                        path = resolve_path(path, tracee.get_pid(), static_cast<int>(regs.rdi));
                    m_logger.mkdir(tracee.get_binpath(), tracee.get_pid(), path);
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
    else if(event == TRACE_CHILD_CREATED) {
        if(tracee.get_parent_pid() != -1)
            set_cwd(tracee.get_pid(), get_cwd(tracee.get_parent_pid()));
        else
            set_cwd(tracee.get_pid(), m_tracer_cwd);
    }
}