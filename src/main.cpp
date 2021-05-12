#include <iostream>
#include "ptrace.h"
#include "stdlogger.h"
#include "syscall.h"

using namespace PTrace;

int main() {
    TraceApi traceApi;
    StdErrLogger logger;
    SyscallHandler sysHandler(traceApi, logger);
    if(!traceApi.exec("/bin/cat", { "cat", "/tmp/test" })) {
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