#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
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

TEST_CASE("Simple open and close") {
    TestLogger logger;

    REQUIRE(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "simple" }));

    REQUIRE_NOTHROW(logger.expect_open(-1, "/dev/null", O_WRONLY));
    REQUIRE_NOTHROW(logger.expect_close(-1, "/dev/null"));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Relative path") {
    TestLogger logger;
    std::array<char, PATH_MAX> buf;
    std::string full_path;

    REQUIRE(!close(open("../../hello", O_WRONLY | O_CREAT, 0644)));
    
    buf.fill(0);
    realpath("../../hello", buf.data());
    full_path = std::string(buf.data());
    REQUIRE(full_path.length() > 0);

    REQUIRE(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "relative" }));

    unlink("../../hello");

    REQUIRE_NOTHROW(logger.expect_open(-1, full_path, O_RDWR));
    REQUIRE_NOTHROW(logger.expect_close(-1, full_path));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Openat cwd") {
    TestLogger logger;
    std::array<char, PATH_MAX> buf;
    std::string full_path;

    REQUIRE(!close(open("hello", O_WRONLY | O_CREAT, 0644)));
    
    buf.fill(0);
    realpath("hello", buf.data());
    full_path = std::string(buf.data());
    REQUIRE(full_path.length() > 0);

    REQUIRE(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "openatcwd" }));

    unlink("hello");

    REQUIRE_NOTHROW(logger.expect_open(-1, full_path, O_RDWR));
    REQUIRE_NOTHROW(logger.expect_close(-1, full_path));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Openat with a directry fd") {
    TestLogger logger;
    std::array<char, PATH_MAX> buf;
    std::string file_path;
    std::string dir_path;

    REQUIRE(!close(open("../hello", O_WRONLY | O_CREAT, 0644)));
    
    buf.fill(0);
    realpath("../hello", buf.data());
    file_path = std::string(buf.data());
    REQUIRE(file_path.length() > 0);

    buf.fill(0);
    realpath("..", buf.data());
    dir_path = std::string(buf.data());
    REQUIRE(dir_path.length() > 0);

    REQUIRE(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "openatfd" }));

    unlink("../hello");

    REQUIRE_NOTHROW(logger.expect_open(-1, dir_path, O_RDONLY));
    REQUIRE_NOTHROW(logger.expect_open(-1, file_path, O_RDWR));
    REQUIRE_NOTHROW(logger.expect_close(-1, file_path));
    REQUIRE_NOTHROW(logger.expect_close(-1, dir_path));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Non-existent file") {
    TestLogger logger;

    REQUIRE(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "nonexistent" }));

    REQUIRE_NOTHROW(logger.expect_open_failed(-1, "doesntexist", O_RDONLY, ENOENT));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}