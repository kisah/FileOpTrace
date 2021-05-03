#include <iostream>
#include "syscall.h"

using namespace PTrace;

SyscallHandler::SyscallHandler(TraceApi& tracer) {
    tracer.register_handler([this](Tracee tracee, int status) {
        trace_handler(tracee, status);
    });
}

void SyscallHandler::trace_handler(PTrace::Tracee tracee, int status) {
    std::cout << "handler triggered\n";
    user_regs_struct regs = tracee.get_registers();
    switch(regs.orig_rax) {
        case __NR_openat:
            std::cout << "openat: \"" << tracee.read_string(regs.rsi) << "\"\n";
            break;
        default:
            break;
    }
    tracee.cont(0);
}