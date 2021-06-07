#include <fstream>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
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

    CHECK_NOTHROW(logger.expect_open(-1, "/dev/null", O_WRONLY));
    CHECK_NOTHROW(logger.expect_close(-1, "/dev/null"));

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
    CHECK(full_path.length() > 0);

    CHECK(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "relative" }));

    unlink("../../hello");

    CHECK_NOTHROW(logger.expect_open(-1, full_path, O_RDWR));
    CHECK_NOTHROW(logger.expect_close(-1, full_path));

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
    CHECK(full_path.length() > 0);

    CHECK(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "openatcwd" }));

    unlink("hello");

    CHECK_NOTHROW(logger.expect_open(-1, full_path, O_RDWR));
    CHECK_NOTHROW(logger.expect_close(-1, full_path));

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
    CHECK(file_path.length() > 0);

    buf.fill(0);
    realpath("..", buf.data());
    dir_path = std::string(buf.data());
    CHECK(dir_path.length() > 0);

    CHECK(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "openatfd" }));

    unlink("../hello");

    CHECK_NOTHROW(logger.expect_open(-1, dir_path, O_RDONLY));
    CHECK_NOTHROW(logger.expect_open(-1, file_path, O_RDWR));
    CHECK_NOTHROW(logger.expect_close(-1, file_path));
    CHECK_NOTHROW(logger.expect_close(-1, dir_path));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Non-existent file") {
    TestLogger logger;

    REQUIRE(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "nonexistent" }));

    CHECK_NOTHROW(logger.expect_open_failed(-1, "doesntexist", O_RDONLY, ENOENT));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Chdir") {
    TestLogger logger;
    std::array<char, PATH_MAX> buf;
    std::string full_path;

    REQUIRE(mkdir("dir", 0755) >= 0);

    CHECK(!close(open("dir/hello", O_WRONLY | O_CREAT, 0644)));
    
    buf.fill(0);
    realpath("dir/hello", buf.data());
    full_path = std::string(buf.data());
    CHECK(full_path.length() > 0);

    CHECK(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "chdir" }));

    CHECK_NOTHROW(logger.expect_open(-1, full_path, O_RDWR));
    CHECK_NOTHROW(logger.expect_close(-1, full_path));

    unlink("dir/hello");
    rmdir("dir");

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Fchdir") {
    TestLogger logger;
    std::array<char, PATH_MAX> buf;
    std::string file_path;
    std::string dir_path;

    REQUIRE(mkdir("dir", 0755) >= 0);

    CHECK(!close(open("dir/hello", O_WRONLY | O_CREAT, 0644)));
    
    buf.fill(0);
    realpath("dir/hello", buf.data());
    file_path = std::string(buf.data());
    CHECK(file_path.length() > 0);

    buf.fill(0);
    realpath("dir", buf.data());
    dir_path = std::string(buf.data());
    CHECK(dir_path.length() > 0);

    CHECK(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "fchdir" }));

    unlink("dir/hello");
    rmdir("dir");

    CHECK_NOTHROW(logger.expect_open(-1, dir_path, O_RDONLY));
    CHECK_NOTHROW(logger.expect_open(-1, file_path, O_RDWR));
    CHECK_NOTHROW(logger.expect_close(-1, file_path));
    CHECK_NOTHROW(logger.expect_close(-1, dir_path));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Unlink") {
    TestLogger logger;
    std::array<char, PATH_MAX> buf;
    std::string full_path;

    REQUIRE(!close(open("hello", O_WRONLY | O_CREAT, 0644)));
    
    buf.fill(0);
    realpath("hello", buf.data());
    full_path = std::string(buf.data());
    CHECK(full_path.length() > 0);

    CHECK(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "unlink" }));

    unlink("hello");

    CHECK_NOTHROW(logger.expect_remove(-1, full_path));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Unlinkat cwd") {
    TestLogger logger;
    std::array<char, PATH_MAX> buf;
    std::string full_path;

    REQUIRE(!close(open("hello", O_WRONLY | O_CREAT, 0644)));
    
    buf.fill(0);
    realpath("hello", buf.data());
    full_path = std::string(buf.data());
    CHECK(full_path.length() > 0);

    CHECK(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "unlinkatcwd" }));

    unlink("hello");

    CHECK_NOTHROW(logger.expect_remove(-1, full_path));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Unlinkat with a directory fd") {
    TestLogger logger;
    std::array<char, PATH_MAX> buf;
    std::string file_path;
    std::string dir_path;

    REQUIRE(mkdir("dir", 0755) >= 0);

    CHECK(!close(open("dir/hello", O_WRONLY | O_CREAT, 0644)));
    
    buf.fill(0);
    realpath("dir/hello", buf.data());
    file_path = std::string(buf.data());
    CHECK(file_path.length() > 0);

    buf.fill(0);
    realpath("dir", buf.data());
    dir_path = std::string(buf.data());
    CHECK(dir_path.length() > 0);

    CHECK(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "unlinkat" }));

    unlink("dir/hello");
    rmdir("dir");

    CHECK_NOTHROW(logger.expect_open(-1, dir_path, O_RDONLY));
    CHECK_NOTHROW(logger.expect_remove(-1, file_path));
    CHECK_NOTHROW(logger.expect_close(-1, dir_path));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Rmdir") {
    TestLogger logger;
    std::array<char, PATH_MAX> buf;
    std::string full_path;

    REQUIRE(mkdir("dir", 0755) >= 0);
    
    buf.fill(0);
    realpath("dir", buf.data());
    full_path = std::string(buf.data());
    CHECK(full_path.length() > 0);

    CHECK(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "rmdir" }));

    rmdir("dir");

    CHECK_NOTHROW(logger.expect_remove(-1, full_path));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}

TEST_CASE("Fork") {
    TestLogger logger;

    REQUIRE(run_and_wait(&logger, "../testbuddy/testbuddy", { "testbuddy", "simple", "1" }));

    CHECK(!access("/tmp/testpids", F_OK));

    pid_t main;
    pid_t child;
    std::ifstream pidInfo("/tmp/testpids");
    pidInfo >> main >> child;
    CHECK(main > 0);
    CHECK(child > 0);

    unlink("/tmp/testpids");

    CHECK_NOTHROW(logger.expect_open(child, "/dev/null", O_WRONLY));
    CHECK_NOTHROW(logger.expect_close(child, "/dev/null"));
    CHECK_NOTHROW(logger.expect_open(main, "/dev/null", O_WRONLY));
    CHECK_NOTHROW(logger.expect_close(main, "/dev/null"));

    REQUIRE_NOTHROW(logger.expect_empty_list());
}