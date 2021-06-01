#include <fcntl.h>
#include "doctest.h"
#include "../main/ptrace.h"
#include "../main/syscall.h"
#include "testlogger.h"

using namespace PTrace;
using namespace Tests;

bool run_and_wait(Logger* logger, std::string path, std::vector<std::string> args) {
    TraceApi traceApi;
    SyscallHandler sysHandler(traceApi, logger);

    if(!traceApi.exec(path, args))
        return false;

    while(traceApi.loop());

    return true;
}

TEST_CASE("Test of the test framework") {
    TestLogger logger;

    REQUIRE(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy" }));

    REQUIRE_NOTHROW(logger.expect_open(-1, "/dev/null", O_WRONLY));
    REQUIRE_NOTHROW(logger.expect_close(-1, "/dev/null"));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}