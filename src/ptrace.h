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
    bool attach(pid_t pid);
    bool exec(std::string path, std::vector<std::string> args);
    void register_handler(std::function<void(Tracee, int)> handler) { m_eventHandler = handler; }

    bool loop();
private:
    std::vector<Tracee> m_procs;
    std::function<void(Tracee, int)> m_eventHandler;
};

class Tracee {
friend class TraceApi;

public:
    pid_t get_pid() { return m_pid; }
    void cont(int signal);
    user_regs_struct get_registers();
    std::string read_string(unsigned long long addr);

protected:
    Tracee(pid_t pid);
    
private:
    pid_t m_pid;
};

}