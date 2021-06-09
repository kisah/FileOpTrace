#include <sys/wait.h>
#include "testhandler.h"

using namespace Tests;
using namespace PTrace;

TestHandler::TestHandler(TraceApi& tracer) {
    tracer.register_handler([this](Tracee& tracee, int status) {
        trace_handler(tracee, status);
    });
}

void TestHandler::trace_handler(Tracee& tracee, int status) {
    if(WSTOPSIG(status) == SIGTRAP && (status >> 16) == 0 && m_test) {
        auto regs = tracee.get_registers();
        m_test(tracee, regs.rax);
    }
    tracee.cont(0);
}