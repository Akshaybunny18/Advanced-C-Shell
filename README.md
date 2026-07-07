# Advanced C-Shell

A massive, multi-phase Operating Systems project covering user-space system programming, network protocol design, and deep kernel-level modifications in xv6.

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Standard](https://img.shields.io/badge/Standard-POSIX-green.svg)](https://pubs.opengroup.org/onlinepubs/9699919799/)

> A comprehensive implementation of a Unix shell and OS concepts developed as part of the Operating Systems and Networks course at IIIT Hyderabad.

## Table of Contents

- [Phase 1-3: Custom C-Shell](#phase-1-3-custom-c-shell)
- [Phase 4: Reliable UDP Networking Protocol](#phase-4-reliable-udp-networking-protocol)
- [Phase 5: xv6 Kernel Modifications](#phase-5-xv6-kernel-modifications)
- [Build & Usage](#build--usage)
- [Architecture & Project Structure](#architecture--project-structure)
- [Implementation Details](#implementation-details)
- [Author](#author)

---

## Phase 1-3: Custom C-Shell

A fully functional, POSIX-compliant UNIX shell built from scratch in C. The shell is capable of executing standard binaries (transparently falling back to `execvp`), managing foreground and background processes, and handling complex I/O pipelines.

### Core Shell Functionality
- **Custom Prompt**: Displays system information, current user, and dynamically updates the current working directory, relative to the initial home directory where the shell was launched (with tilde expansion).
- **Command Parsing**: Robust CFG-based parser supporting complex command chains (`shell_cmd → cmd_group ((& | ;) cmd_group)* &?`).
- **I/O Redirection**: Full support for standard input (`<`), output (`>`), and append (`>>`) redirection.
- **Piping**: Supports arbitrary-length multi-level command pipelines (e.g., `cmd1 | cmd2 | cmd3`).
- **Signal Handling**: Gracefully handles `Ctrl+C` (SIGINT) and `Ctrl+Z` (SIGTSTP) without crashing the shell, delegating signals to foreground children. Handles `EOF` (Ctrl+D) to exit securely.
- **Process Management**: Native support for background execution using the `&` operator. Tracks process states and reaps zombies.

### Built-in Commands
- `hop` (cd equivalent): Navigate through directories with support for absolute and relative paths. Supports `~` (home), `-` (previous), `.`, and `..`.
- `reveal` (ls equivalent): Lists files and directories with detailed information, hidden file toggles (`-a`), and line-by-line output (`-l`).
- `log`: A persistent command history system that stores up to 15 commands. Supports executing previous commands via `log execute <index>` and clearing via `log purge`.
- `activities`: Displays a list of all currently running or stopped background processes spawned by the shell natively.
- `ping <pid> <signal>`: Sends standard UNIX signals (like `SIGTERM`, `SIGKILL`) to running processes directly from the shell.
- `fg <job>` / `bg <job>`: Brings background processes to the foreground, or resumes stopped processes in the background.

---

## Phase 4: Reliable UDP Networking Protocol

A custom reliable data transfer protocol built on top of UDP sockets, simulating TCP-like reliability guarantees using a custom **S.H.A.M.** (Simple Hybrid Assured Messaging) header.

### Features
- **3-Way Handshake**: Connection establishment using `SYN`, `SYN-ACK`, and `ACK` to synchronize initial sequence numbers securely.
- **Sliding Window Protocol**: Implements flow control and reliable delivery for transferring massive files over lossy networks.
- **Timeout & Retransmission**: Uses `select()` to implement a 500ms Retransmission Timeout (RTO) for unacknowledged packets.
- **Cumulative ACKs**: The receiver acknowledges chunks of data cumulatively, ensuring data is written exactly in order.
- **Simulated Packet Loss**: Includes arguments to simulate packet drop rates to test network resilience.
- **Logging Mode**: High-precision microsecond-level timestamps for logging all connection states and flow control events.
- **File Transfer**: Efficient file transfer with MD5 checksum verification upon completion.

---

## Phase 5: xv6 Kernel Modifications

Deep modifications to the MIT xv6 Operating System kernel to add custom system calls and replace the default scheduling algorithm.

### Features
- **New System Call (`getreadcount`)**: Added a custom `sys_getreadcount` syscall that tracks the total number of bytes read globally across all processes using the `read()` syscall.
- **First Come First Serve (FCFS)**: Replaced the default Round Robin scheduler with a non-preemptive FCFS policy based on process creation time.
- **Completely Fair Scheduler (CFS)**: Implemented a simplified version of Linux's CFS. Processes are assigned a `nice` value which dictates their system weight. The scheduler tracks normalized `vruntime` and dynamically assigns time slices to ensure fair CPU distribution.
- **Preemptive MLFQ Bonus**: Implemented a 4-queue Multi-Level Feedback Queue scheduler. Processes start at high priority (1 tick slice) and demote to lower priorities (4, 8, 16 ticks) if they consume their entire CPU slice, with priority boosting every 48 ticks to prevent starvation.

---

## Build & Usage

### Prerequisites
- GCC compiler with C99 support
- POSIX-compliant operating system (Linux, macOS, WSL)
- OpenSSL development libraries (for networking component MD5 checksums)

### Shell

Make the `shell` folder your root directory to build and run the C-Shell:
```bash
cd shell
make all
./shell.out
```

### Networking

Make the `networking` folder your root directory to build and run the S.H.A.M. protocol:
```bash
cd networking
make

# Run the server
./server 8080

# Run the client in another terminal
./client <server_ip> 8080 <input_file> output.bin
```

### xv6

Make the `xv6` folder your root directory to compile and run the modified kernel:
```bash
cd xv6
make qemu SCHEDULER=CFS
```
*(You can also pass `SCHEDULER=FCFS` to test the other policy)*

---

## Architecture & Project Structure

The project is modularly structured, separating concerns into dedicated directories:

```text
Advanced_C-Shell/
├── shell/                      # Phase 1-3: Custom Shell
│   ├── src/                    # Source code files
│   │   ├── input.c, prompt.c   # Fetches user input and displays contextual prompt
│   │   ├── tokenizer.c, parser.c # Lexical analysis and CFG command parsing
│   │   ├── executor.c          # Core engine (forks processes, runs execvp, handles built-ins)
│   │   ├── jobs.c, signals.c   # Manages background processes and handles system signals
│   │   ├── hop.c, reveal.c,    # Built-in command implementations
│   │   └── log.c, ping.c
│   ├── include/                # Header files (.h) exposing module interfaces
│   └── Makefile                # Build instructions
│
├── networking/                 # Phase 4: SHAM Protocol
│   ├── server.c, client.c      # Server and Client UDP implementations
│   ├── sham.c, sham.h          # S.H.A.M Protocol logic and headers
│   └── Makefile                
│
├── xv6/                        # Phase 5: Kernel modifications
│   ├── readcount.c             # System call user program
│   ├── report.md               # Implementation and analysis report
│   └── xv6_modifications.patch # The raw patch file applied to xv6
│
└── autograder_logs_final/      # Autograder test results
```

---

## Implementation Details

- **Memory Management**: Dynamic allocation for command structures with proper cleanup and no core memory leaks.
- **File Descriptor Management**: Proper closing of unused pipe ends, restoration of `stdin`/`stdout` after redirection, and prevention of FD leaks.
- **Concurrency**: Signal-safe code in handlers, race condition prevention, and asynchronous background job cleanup.

## Author

**Chanda Akshay Kumar**  
chanda.kumar@students.iiit.ac.in | 2024102014  
- GitHub: [@Akshaybunny18](https://github.com/Akshaybunny18)  
- Institution: IIIT Hyderabad  
- Course: Operating Systems and Networks (Monsoon 2025)

<div align="center">
**Built with C and POSIX APIs**
</div>
