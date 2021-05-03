#pragma once
#include <syscall.h>
#include "ptrace.h"

class SyscallHandler {
public:
    SyscallHandler(PTrace::TraceApi& tracer);
    
private:
    void trace_handler(PTrace::Tracee tracee, int status);
};