#include "ptrace.h"
#include "bpf.h"
#include <algorithm>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

using namespace PTrace;

bool TraceApi::attach(pid_t pid) {
    if(ptrace(PTRACE_ATTACH, pid, NULL, NULL))
        return false;
    ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_EXITKILL | PTRACE_O_TRACEEXEC);
    Tracee tracee(pid);
    m_procs.emplace_back(tracee);
    return true;
}

bool TraceApi::exec(std::string path, std::vector<std::string> args) {
    pid_t pid = fork();
    if(pid == 0) {
        int i = 0;
        char** argv = static_cast<char**>(malloc(sizeof(char*) * (args.size() + 1)));
        for(std::string arg : args) {
            argv[i++] = strdup(arg.c_str());
        }
        argv[i] = NULL;

        ptrace(PTRACE_TRACEME, NULL, NULL, NULL);

        if(!BPF::setup()) {
            printf("Failed to install BPF\n");
            exit(1);
        }

        execv(path.c_str(), argv);
    }
    else if(pid > 0) {
        Tracee tracee(pid);
        int status;
        waitpid(pid, &status, 0);
        if(WSTOPSIG(status) != SIGTRAP)
            return false; //This should never happen
        ptrace(PTRACE_SETOPTIONS, tracee.get_pid(), NULL, PTRACE_O_EXITKILL | PTRACE_O_TRACEEXEC | PTRACE_O_TRACESECCOMP);
        ptrace(PTRACE_CONT, pid, NULL, NULL);

        m_procs.emplace_back(tracee);
        
        return true;
    }
    return false;
}

bool TraceApi::loop() {
    int status = 0;
    std::vector<pid_t> endedTracees;

    for(Tracee tracee : m_procs) {
        if(waitpid(tracee.get_pid(), &status, WNOHANG) == -1)
            endedTracees.emplace_back(tracee.get_pid());
        if(status)
            if(m_eventHandler)
                m_eventHandler(tracee, status);
    }

    if(endedTracees.size()) {
        m_procs.erase(std::remove_if(m_procs.begin(), m_procs.end(), [endedTracees](Tracee t) {
            return std::find(endedTracees.begin(), endedTracees.end(), t.get_pid()) != endedTracees.end();
        }), m_procs.end());
    }

    return m_procs.size() > 0;
}

Tracee::Tracee(pid_t pid) : m_pid(pid) {}

void Tracee::cont(int signal) {
    ptrace(PTRACE_CONT, get_pid(), NULL, signal);
}

user_regs_struct Tracee::get_registers() {
    user_regs_struct regs;
    ptrace(PTRACE_GETREGS, get_pid(), NULL, &regs);
    return regs;
}

std::string Tracee::read_string(unsigned long long addr) {
    std::vector<long> readbuf;
    long tmp;

    auto contains_null_byte = [](long octet) {
        return !((octet >> 56) && ((octet >> 48) & 0xFF) && ((octet >> 40) & 0xFF) && ((octet >> 32) & 0xFF) && ((octet >> 24) & 0xFF) && ((octet >> 16) & 0xFF) && ((octet >> 8) & 0xFF) && (octet & 0xFF));
    };
    
    do {
        tmp = ptrace(PTRACE_PEEKDATA, get_pid(), addr, NULL);
        readbuf.emplace_back(tmp);
        addr += 8;
    } while(!contains_null_byte(tmp));

    return std::string(static_cast<char*>(static_cast<void*>(readbuf.data())));
}