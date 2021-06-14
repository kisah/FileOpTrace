#include <iostream>
#include <CLI/CLI.hpp>
#include "frontend.h"
#include "ptrace.h"
#include "pipelogger.h"
#include "stdlogger.h"
#include "syscall.h"

using namespace FileOpTrace;
using namespace PTrace;

/**
 * \brief Entry point
 */
int main(int argc, char** argv) {
    CLI::App app{"A ptrace based file operation logger"};
    app.prefix_command();

    app.get_formatter()->label("OPTIONS", "options] <path> [args");

    std::string pipe_path = "/tmp/fileoptrace";
    app.add_option("-p,--pipe", pipe_path, "Path to a frontend connection pipe")->option_text("<path>");

    bool use_stderr = false;
    app.add_flag("-s,--stderr", use_stderr, "Use stderr instead of the frontend");

    bool start_frontend = false;
    app.add_flag("-f,--frontend", start_frontend, "Start a frontend, no path required");

    CLI11_PARSE(app, argc, argv);

    if(start_frontend) {
        FrontEnd frontEnd(pipe_path);
        while(true) {
            if(!frontEnd.bind()) {
                std::cerr << "Failed to create a frontend pipe" << std::endl;
                return 1;
            }
            frontEnd.run();
        }
    }
    else {
        auto args = app.remaining();
        if(args.size()) {
            TraceApi traceApi;
            StdErrLogger stdLogger;
            PipeLogger pipeLogger;
            if(!use_stderr && !pipeLogger.connect(pipe_path)) {
                std::cerr << "Failed to connect to the frontend, falling back to using stderr" << std::endl;
                use_stderr = true;
            }
            SyscallHandler sysHandler(traceApi, use_stderr ? dynamic_cast<Logger*>(&stdLogger) : dynamic_cast<Logger*>(&pipeLogger));
            if(!traceApi.exec(args[0], args)) {
                std::cout << "Failed to start a process" << std::endl;
                return 1;
            }
            while(traceApi.loop());
        }
        else {
            std::cout << app.help();
        }
    }
    return 0;
}