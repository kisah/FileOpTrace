#pragma once
#include <string>
#include <vector>
#include <functional>
#include <sys/types.h>
#include <sys/user.h>

namespace PTrace {

class Tracee;
    
class TraceApi {
public:
    bool exec(std::string path, std::vector<std::string> args);
    void register_handler(std::function<void(Tracee&, int)> handler) { m_eventHandler = handler; }

    bool loop();
private:
    std::vector<Tracee> m_procs;
    std::function<void(Tracee&, int)> m_eventHandler;
};

class Tracee {
friend class TraceApi;

public:
    pid_t get_pid() { return m_pid; }
    std::string get_binpath() { return m_binpath; }
    void cont(int signal);
    user_regs_struct get_registers();
    std::string read_string(unsigned long long addr);
    unsigned long long syscall_ret_value();

protected:
    Tracee(pid_t pid, std::string binpath);
    
private:
    pid_t m_pid;
    std::string m_binpath;
};

}