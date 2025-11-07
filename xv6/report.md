xv6 Scheduler Implementation Report

Project Overview:

The project introduces two new scheduling algorithms, First Come First Serve (FCFS) and Completely Fair Scheduler (CFS), on top of the existing Round Robin scheduler. The project supports conditional compilation of scheduling algorithms, improved process tracking, and thorough testing capabilities.

Part A: System Call Implementation
getreadcount System Call
Implementation: Introduced a global byte counter total_bytes_read that maintains bytes read by all processes since boot time.

Key Changes:

kernel/readcount.c: Fundamental implementation involving sys_getreadcount() and increment_read_count()
kernel/sysfile.c: Altered sys_read() to invoke increment_read_count() upon successful reads
Overflow handling: Wraps around to 0 in case of overflow to avoid undefined behavior

Testing: User program readcount.c showcases functionality by reading 100 bytes and checking the counter increments appropriately.

Part B: Scheduler Implementation

B1: FCFS (First Come First Serve) Scheduler
Implementation: Non-preemptive scheduler that chooses processes based on creation time.

Key Features:

Uses p->ctime field set at process creation
Chooses RUNNABLE process with the earliest creation time
Non-preemptive: processes execute to completion or voluntary yield
Improved procdump() displays creation times for debugging

Algorithm:
for each scheduling decision:
    find RUNNABLE process with minimum ctime
    execute chosen process until it yields/blocks

B2: CFS (Completely Fair Scheduler) Implementation

B2.1: Priority Support

Implementation: Introduced nice value support with weight computation.
Formula: weight = 1024 / (1.25 ^ nice)

Nice 0: weight = 1024 (default)
Nice -20: weight = 88761 (highest priority)
Nice 19: weight = 15 (lowest priority)

Code: calculate_weight() method estimates the exponential relationship with integer arithmetic.

B2.2: Virtual Runtime Tracking
Implementation: Maintains normalized CPU time usage per process.

Key Features:

vruntime = 0 upon process creation
Update rule: vruntime += 1024 / weight on each timer tick
Processes with higher weight accumulate vruntime more slowly (get more CPU time)

B2.3: Scheduling Algorithm [50 Marks]
Implementation: Schedules always process with lowest virtual runtime.

Algorithm:
for each scheduling decision
log all runnable processes and their vruntime
    select process with minimum vruntime
    calculate dynamic time slice
    run selected process for its time slice
    update vruntime during execution

B2.4: Time Slice Calculation

Implementation: Dynamic time slices based on system load.

Formula:
time_slice = target_latency / runnable_processes
Target latency: 48 ticks
Minimum time slice: 3 ticks

Ensures system responsiveness independent of process number

Conditional Compilation
Makefile Improvement:

makefile
ifndef SCHEDULER
SCHEDULER := RR
endif
CFLAGS += -DSCHEDULER_$(SCHEDULER)
Allows building with: make qemu SCHEDULER=FCFS/CFS

CFS Logging Output
With the logging solution, CFS outputs such as:
text
[Scheduler Tick]
PID: 10 | vRuntime: 200
PID: 11 | vRuntime: 150  
PID: 12 | vRuntime: 180
--> Scheduling PID 11 (minimum vRuntime: 150)

PID 11: vRuntime set from 150 to 151 (ticks_used: 1/16)
PID 11: vRuntime set from 151 to 152 (ticks_used: 2/16)
.
PID 11: Time slice exhausted, resulting in 

[Scheduler Tick]
PID: 10 | vRuntime: 200
PID: 11 | vRuntime: 166
PID: 12 | vRuntime: 180
--> Scheduling PID 11 (lowest vRuntime: 166)

This illustrates:
Lowest vruntime process is always chosen
vRuntime values are updated correctly per tick
Time slice management and preemption in action
Scheduling fairly over all processes

Performance Analysis:
Test Setup:
Single CPU setup (CPUS := 1)
10 processes: 5 IO-bound (sleep 200 ticks), 5 CPU-bound (computation loops)
Measurements: average waiting time and turnaround time

Scheduler Comparison:

Scheduler     Avg Wait Time   Avg Turnaround  CPU Utilization  Responsiveness
Round Robin      Medium          Medium           High            Good
FCFS             High            High             High            Poor
CFS              Low             Low              High          Excellent

Analysis:

Round Robin:
Advantages: Easy, equitable time allocation, suitable for interactive workloads
Disadvantages: Tied time quantum might not be suitable for all process types

FCFS:
Advantages: Easy to implement, no preemption overhead
Cons: Slow response time, convoy effect with CPU-intensive processes

CFS:
Pros: Very good fairness, sensitive to various process types, priority support
Cons: More sophisticated implementation, minimal scheduling overhead

Key Observations:
FCFS exhibits bad performance for subsequent processes due to non-preemptive
CFS achieves best global fairness and responsiveness
Round Robin provides balanced performance for general-purpose systems
CFS virtual runtime mechanism effectively avoids process starvation
Dynamic time slicing in CFS is well-suited to changing system loads

Testing and Verification:
Test Programs:
schedulertests.c: Generates mixed CPU-bound and IO-bound workloads
readcount.c: Exercises system call functionality
procdump improvement: Displays scheduler-specific process data

Verification Techniques:
Process state observation using improved procdump
vRuntime execution progress logging in CFS
Order of execution analysis with varying schedulers
Timing-based performance comparisons

Conclusion:
The implementation effectively illustrates three different scheduling methodologies:
FCFS: Predictable, simple, but unfair
Round Robin: Balanced scheduling with time-slicing
CFS: Complex fair scheduling with priority support

The CFS implementation specifically exemplifies contemporary scheduler design practices with virtual runtime monitoring, dynamic time slicing, and full fairness measures. The logging facility affords transparent insights into scheduling choices, verifying accurate algorithm conduct.

All of the schedulers conditionally compile and exhibit anticipated behavior patterns, fulfilling project specifications and offering realistic experience with core operating system scheduling principles.