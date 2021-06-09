#pragma once
#include <fstream>

namespace FileOpTrace {

class FrontEnd {
public:
    FrontEnd(std::string pipe_path);
    bool bind();
    void run();

    static void static_sighandler(int sig);
    void sighandler(int sig);

    static FrontEnd* instance;
private:
    std::string m_path;
    std::ifstream m_pipe;
};

}