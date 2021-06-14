#pragma once
#include <string>
#include <vector>
#include <functional>
#include <limits.h>
#include <sys/types.h>
#include <sys/user.h>

namespace PTrace {

class Tracee;

enum TraceEvent {
    TRACE_CHILD_CREATED,
    TRACE_SIGNAL
};
    
/**
 * \brief A simple c++ ptrace wrapper
 */
class TraceApi {
public:
    /**
     * \brief Start a process and start tracing it
     * \param path Executable path
     * \param args Arguments for the process
     * \return Success status
     */
    bool exec(std::string path, std::vector<std::string> args);

    /**
     * \brief Register a handler of trace events
     * \param handler The handler callback
     */
    void register_handler(std::function<void(TraceEvent, Tracee&, int)> handler) { m_eventHandler = handler; }

    /**
     * \brief Poll all traced processes
     * \return True if there are some processes running, false if there are no running processes left
     * 
     * The loop is responsible for catching the necessary ptrace events and
     * handling things like fork/clone and exec,
     * all of the other event types are passed to the registered handler
     */
    bool loop();

protected:
    /**
     * \brief Get an executable path for a given process id
     * \warning For internal use only
     * \param pid A pid
     * \return Executable path
     */
    std::string binpath_for_pid(pid_t pid);

private:
    /// A vector of all traced processes
    std::vector<Tracee> m_procs;

    /// Trace event handler callback
    std::function<void(TraceEvent, Tracee&, int)> m_eventHandler;
};

/**
 * \brief Represents a traced process
 * \warning This class should only be created by the TraceApi
 * 
 * This class is a part of the c++ wrapper for ptrace
 */
class Tracee {
friend class TraceApi;

public:
    /**
     * \brief Get the process id
     * \return A process id
     */
    pid_t get_pid() { return m_pid; }

    /**
     * \brief Get the executable path
     * \return The executable path
     */
    std::string get_binpath() { return m_binpath; }

    /**
     * \brief Set the executable path
     * \param binpath The executable path
     */
    void set_binpath(std::string binpath) { m_binpath = binpath; }

    /**
     * \brief Get the parent process id
     * \return Parent process id or -1
     */
    pid_t get_parent_pid() { return m_ppid; }

    /**
     * \brief Continue the tracee while sending a signal to it
     * \param signal The signal code to send
     */
    void cont(int signal);

    /**
     * \brief Get a current snapshot of the CPU registers
     * \return Registers
     */
    user_regs_struct get_registers();

    /**
     * \brief Read a string from tracee's memory
     * \param addr Start address
     * \param max_len The maximum string length (4096 by default)
     */
    std::string read_string(unsigned long long addr, size_t max_len = PATH_MAX);

    /**
     * \brief Get a system call result value
     * \return Result
     */
    unsigned long long syscall_ret_value();

protected:
    /**
     * \brief Constructor
     * \warning For internal use only
     * \param pid Tracee process id
     * \param binpath Tracee process executable path
     * \param ppid Parent process id (can be -1)
     */
    Tracee(pid_t pid, std::string binpath, pid_t ppid = -1);

    /**
     * \brief Get the boolean flag
     * \warning For internal use only
     * \return Flag value
     */
    bool get_flag() { return m_flag; }

    /**
     * \brief Set the boolean flag
     * \warning For internal use only
     * \param flag Flag value
     */
    void set_flag(bool flag) { m_flag = flag; }
    
private:
    /// Process id
    pid_t m_pid;

    /// Executable path
    std::string m_binpath;

    /// Parent process id
    pid_t m_ppid;

    /// A boolean flag, set when the tracee forks/clones or execs
    bool m_flag = false;
};

}