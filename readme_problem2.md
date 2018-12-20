# Problem 2 Implementation

### struct booth_t

There are 7 integer variables. <br>
**slots**   - Number of slots in active EVM <br>
**val0**    - Related to condition variable cond0 <br>
**val1**    - Related to condition variable cond1 <br>
**val2**    - Related to condition variable cond2 <br>
**val3**    - Related to condition variable cond3 <br>
**val4**    - Related to condition variable cond4 <br>
**voters**  - Number of voters waiting at booth <br>
There is one mutex that maintains mutual exclusion within each of the functions <br>
There are 5 conditional variables for thread synchronization - cond0, cond1, cond2,
cond3, cond4

### booth_init

**slots** is initialy 0 before any EVM is active. **val0** is initialized to 1
so that **cond0** can function as a counting semaphore with limit 1. **val1**, **val2**, **val3** and **val4** are initialized to 0 so that **cond1**, **cond2**, **cond3** and **cond4** can function as binary semaphores. The mutex and condition variables are initialized as normal.

### polling_ready_evm

Evm thread calls wait on **cond0** so that only 1 evm thread will be active per
booth. The remaining evm threads wait. Then it checks if the number of voters remaining at the booth is 0. If so, it signals the next waiting evm and exits.
If not then it checks if the number of voters is less than the number of slots.
If so, then it reduces number of slots to number of voters. Finally it sends
signal to **cond1** in order to wake up as many voter threads as there are slots.

### polling_finished_evm

Evm thread calls signal on **cond2** to signal the selected voter threads that
they can enter the slots. It then calls wait on **cond3** in order to wait for
all voters to enter their slots. Then it calls wait on **cond4** in order to wait
for all voters to finish voting. Finally it calls signal on **cond0** to wake up
next evm.

### voter_wait_for_evm

Voter thread calls wait on **cond1** in order to be assigned an evm.

### voter_in_slot

Voter thread calls wait on **cond2** in order to wait for all slots on the evm
to be assigned to voters. Then it sends signal to **cond3** to signal to the evm
that it is in it's slot.

### voter_finished_voting

Voter thread calls signal in **cond4** to signal that it is done voting. Then it
decrements **voter** count for the booth.

# Input Format
**Number of Booths** - Positive Integer <br>
**Number of EVMs** - Positive Integer <br>
**Number of Voters** - Positive Integer

# How to Run
gcc -pthread prob2.c <br>
./a.out
