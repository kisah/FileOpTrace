#pragma once
#include <fstream>

namespace FileOpTrace {

/**
 * \brief A frontend that received messages on a named pipe and outputs them to stdout
 */
class FrontEnd {
public:
    /**
     * \brief Constructor
     * \param pipe_path Path to a named pipe to bind to
     */
    FrontEnd(std::string pipe_path);

    /**
     * \brief Create a named pipe at the specified path and open it
     * \return Success status
     */
    bool bind();

    /**
     * \brief Receive messages and log them on the stdout
     */
    void run();

    /**
     * \brief A static signal handler stub, calls the non-static version
     * \param sig Signal number
     */
    static void static_sighandler(int sig);

    /**
     * \brief A signal handler responsible for removing the named pipe when the frontend shuts down
     * \param sig Signal number
     */
    void sighandler(int sig);

    /// A static pointer to the active FrontEnd instance (used by FrontEnd::static_sighandler)
    static FrontEnd* instance;
private:
    /// Path to the named pipe
    std::string m_path;

    /// A file stream of the specified named pipe
    std::ifstream m_pipe;
};

}