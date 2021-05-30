#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include "frontend.h"

FrontEnd* FrontEnd::instance;

FrontEnd::FrontEnd(std::string pipe_path) : m_path(pipe_path) {
    instance = this;
}

bool FrontEnd::bind() {
    signal(SIGINT, FrontEnd::static_sighandler);
    if(mkfifo(m_path.c_str(), 0666))
        return false;
    std::cout << "Waiting for a connection..." << std::endl;
    m_pipe = std::ifstream(m_path);
    std::cout << "A new client connected" << std::endl;
    return true;
}

void FrontEnd::static_sighandler(int sig) {
    instance->sighandler(sig);
}

void FrontEnd::sighandler(int sig) {
    unlink(m_path.c_str());
    exit(0);
}

void FrontEnd::run() {
    std::string line;
    while (true) {
        std::getline(m_pipe, line);
        if(!line.length())
            break;
        std::cout << line << "\n";
    }
    unlink(m_path.c_str());
    std::cout << "Client disconnected" << std::endl;
}