#include <sys/wait.h>
#include "testhandler.h"

using namespace Tests;
using namespace PTrace;

TestHandler::TestHandler(TraceApi& tracer) {
    tracer.register_handler([this](TraceEvent event, Tracee& tracee, int status) {
        trace_handler(event, tracee, status);
    });
}

void TestHandler::trace_handler(TraceEvent event, Tracee& tracee, int status) {
    if(event == TRACE_SIGNAL && WSTOPSIG(status) == SIGTRAP && (status >> 16) == 0 && m_test) {
        auto regs = tracee.get_registers();
        m_test(tracee, regs.rax);
    }
    tracee.cont(0);
}