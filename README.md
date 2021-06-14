# FileOpTrace
A ptrace based file operation logger for Linux

```
Usage: fileoptrace [options] <path> [args]

Options:
  -h,--help                   Print this help message and exit
  -p,--pipe <path>            Path to a frontend connection pipe
  -s,--stderr                 Use stderr instead of the frontend
  -f,--frontend               Start a frontend, no path required
```

Normally the utility tries to connect to a frontend via a named pipe.  
If there is no running frontend, the log information will be printed to stderr.  
To start a frontend:
```
fileoptrace --frontend
```

# Credits
<https://github.com/tsgates/mbox> for seccomp related stuff