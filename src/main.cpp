#include <iostream>
#include "ptrace.h"
#include "syscall.h"

using namespace PTrace;

int main() {
    TraceApi traceApi;
    SyscallHandler sysHandler(traceApi);
    if(!traceApi.exec("/bin/cat", std::vector<std::string>{ "cat", "/tmp/test" })) {
        std::cout << "Failed to start a process\n";
        return 1;
    }
    /*traceApi.register_handler([](Tracee tracee, int status) {
        std::cout << "Signal " << WSTOPSIG(status) << " from pid " << tracee.get_pid() << "\n";
        std::cout << "PTrace event: " << (status >> 16) << "\n";
        tracee.cont(0);
    });*/
    while(traceApi.loop());
    return 0;
}