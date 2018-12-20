# Problem 3 Performance Report
### Concurrent Merge Sort with Fork vs Concurrent Merge Sort with Threads vs Normal Merge Sort

**The fastest is the normal merge sort. The next fastest is the concurrent merge sort with threads.
The slowest is the concurrent merge sort with fork.**

## Execution Times:
(The following tests were run on Oracle Virtual Box running Ubuntu 16.04)

### Concurrent Merge Sort with Fork:
**Size of n - Execution Time** <br>
10^4 - 1.326s <br>
2.5 x 10^4 - 2.607s

### Concurrent Merge Sort with Threads:
**Size of n - Execution Time** <br>
10^4 - 0.764s <br>
2.5 x 10^4 - 1.410s <br>
5 x 10^4 - 2.708s <br>
6.5 x 10^4 - 2.780s <br>

### Normal Merge Sort:
**Size of n - Execution Time** <br>
10^4 - 0.030s <br>
2.5 x 10^4 - 0.064s <br>
5 x 10^4 - 0.118s <br>
6.5 x 10^4 - 0.133s <br>
5 x 10^5 - 0.851s <br>

## Explanation:

### Concurrent Merge Sort with Fork
Concurrent merge sort with fork is slowest due to the time it takes to execute fork
and create a new process each time it call the merge_sort() function, and the time
it takes to create and access the shared memory between the processes. Furthermore, there is
a limit to the number of processes that you can create. On my machine, the Fork system
call started to return "resource temporarily unavailable" errors when 'n' exceeded 2.5 * 10^4.
The advantage of concurrency does not make up for the extra overheads in this case.

### Concurrent Merge Sort with Threads
Concurrent merge sort with threads is faster than the implementation with fork as the
time it takes to create a new thread is slightly faster than the time it takes to create
a new process. Also, there is no need to create a specific shared memory for the threads.
So it does not use up extra time in accessing user generated shared memory. However, this implementation
is still slower than the normal implementation because of the time it takes to create threads. Also, for
'n' greater than 6.5 * 10^4, the concurrent merge sort with threads caused a segmentation fault.
The advantage of concurrency does not make up for the extra overheads in this case.

### Normal Merge Sort:
Normal merge sort is the fastest despite not having concurrency as it does not have
to deal with creating new processes or threads or accessing shared memory. Also, the normal
merge sort does not suffer from any limit issues.

# Input Format
**N** - Positive Integer <br>
**N Numbers** - Integers

# How to Run
gcc prob3_fork.c <br>
./a.out

gcc -pthread prob3_thread.c <br>
./a.out
