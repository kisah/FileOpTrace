#pragma once
#include <functional>
#include "../main/ptrace.h"

namespace Tests {

/**
 * \brief A mock trace event handler for testing PTrace::Tracee::read_string
 */
class TestHandler {
public:
/**
 * \brief Constructor
 * \param tracer A PTrace::TraceApi instance to register the created handler in
 */
TestHandler(PTrace::TraceApi& tracer);

/**
 * \brief Registers a test callback for the handler
 * \param test The callback
 * 
 * The handler passes a reference to a Tracee instance and the value of the RAX register to the callback
 */
void register_test(std::function<void(PTrace::Tracee&, unsigned long long)> test) { m_test = test; }

protected:
/**
 * \brief A handler callback, that gets called when a trace event arrives
 * \param event The event type (either TRACE_CHILD_CREATED or TRACE_SIGNAL)
 * \param tracee A reference to a PTrace::Tracee instance, representing a particular process
 * \param status A waitpid status
 * 
 * This handler will invoke the test callback, when the tracee process hits a breakpoint
 */
void trace_handler(PTrace::TraceEvent event, PTrace::Tracee& tracee, int status);

private:
/// The test callback
std::function<void(PTrace::Tracee&, unsigned long long)> m_test;
};

}