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
        ptrace(PTRACE_SETOPTIONS, tracee.getPid(), NULL, PTRACE_O_EXITKILL | PTRACE_O_TRACEEXEC | PTRACE_O_TRACESECCOMP);
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
        if(waitpid(tracee.getPid(), &status, WNOHANG) == -1)
            endedTracees.emplace_back(tracee.getPid());
        if(status)
            if(m_eventHandler)
                m_eventHandler(tracee, status);
    }

    if(endedTracees.size()) {
        m_procs.erase(std::remove_if(m_procs.begin(), m_procs.end(), [endedTracees](Tracee t) {
            return std::find(endedTracees.begin(), endedTracees.end(), t.getPid()) != endedTracees.end();
        }), m_procs.end());
    }

    return m_procs.size() > 0;
}

Tracee::Tracee(pid_t pid) : m_pid(pid) {}

void Tracee::cont(int signal) {
    ptrace(PTRACE_CONT, getPid(), NULL, signal);
}