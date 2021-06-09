#pragma once
#include <functional>
#include "../main/ptrace.h"

namespace Tests {

class TestHandler {
public:
TestHandler(PTrace::TraceApi& tracer);
void register_test(std::function<void(PTrace::Tracee&, unsigned long long)> test) { m_test = test; }

protected:
void trace_handler(PTrace::Tracee& tracee, int status);

private:
std::function<void(PTrace::Tracee&, unsigned long long)> m_test;
};

}