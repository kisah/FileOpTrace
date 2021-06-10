#pragma once
#include <map>
#include <syscall.h>
#include "ptrace.h"
#include "logger.h"

namespace FileOpTrace {

/**
 * \brief A handler class for the trace events
 * 
 * This class handles the incoming events from PTrace::TraceApi,
 * determines what system called was invoked by the tracee and
 * calls the appropriate handler in a provided logger
 */
class SyscallHandler {
public:
    /**
     * \brief Constructor
     * \param tracer A reference to a PTrace::TraceApi instance
     * \param logger A pointer to a Logger type instance
     */
    SyscallHandler(PTrace::TraceApi& tracer, Logger* logger);
    
protected:
    /**
     * \brief Binds a file descriptor of a given process to the path it represents
     * \warning For internal use only
     * \param pid Process id
     * \param fd File descriptor
     * \param path A path for the file descriptor
     */
    void add_fd_path(pid_t pid, int fd, std::string& path);

    /**
     * \brief Retrieves the path from a file descriptor of a given process
     * \warning For internal use only
     * \param pid Process id
     * \param fd File descriptor
     * \return Path
     */
    std::string path_for_fd(pid_t pid, int fd);

    /**
     * \brief Removes information about a closed file descriptor
     * \warning For internal use only
     * \param pid Process id
     * \param fd File descriptor
     */
    void remove_fd(pid_t pid, int fd);

    /**
     * \brief Resolves a relative path
     * \warning For internal use only
     * \param path Relative path
     * \param pid Process id
     * \param parent_fd File descriptor of the parent directory or AT_FDCWD
     * \return Absolute path
     */
    std::string resolve_path(std::string& path, pid_t pid, int parent_fd);

    /**
     * \brief A handler that receives trace events from PTrace::TraceApi and calls into the logger
     * \param tracee A reference to a PTrace::Tracee instance, representing a particular process
     * \param status A waitpid status
     */
    void trace_handler(PTrace::Tracee& tracee, int status);

private:
    /// Current working directory path
    std::string m_cwd;

    /// A map binding file descriptors to their respective path
    std::map<std::pair<pid_t, int>, std::string> m_fd_map;

    /// A reference to an active Logger instance
    Logger& m_logger;
};

}