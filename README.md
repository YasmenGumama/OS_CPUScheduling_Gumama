
# OS_CPUScheduling_Gumama

This repository contains a CPU Scheduling Simulator in C++ implementing:
- FCFS
- SJF (non-preemptive)
- Round Robin (RR)


# FCFS
1. First-Come, First-Served (FCFS)

*Concept:*

-  Processes are executed in the order they arrive, like a queue in real life.

- Non-preemptive: once a process starts, it runs until completion.

**How it works:**

1. The CPU looks at the process that arrived first.

2. Runs it completely.

3. Moves on to the next process in arrival order.

**Pros:**

- Simple to implement.

- Fair in terms of arrival order.

**Cons:**

- Convoy effect: a long process delays all shorter processes behind it. 

## OUTPUT
![FCFS OUTPUT](FCFS.png)


# Shortest Job First (SJF – Non-Preemptive)

**Concept:**

- CPU always runs the process with the shortest burst time among those that have arrived.

- Non-preemptive: once chosen, a process runs until it finishes.

**How it works:**

 1. At any given time, select the ready process with the smallest burst time.

 2. Run it until completion.

 3. Repeat until all processes finish.

**Pros:**

- Minimizes average waiting time (mathematically optimal for non-preemptive scheduling).

**Cons:**

- Requires knowledge of burst time in advance.

- Risk of starvation for long processes if short jobs keep arriving.

**OUTPUT**
![alt text](SJF (non-preemptive).png)


# Round Robin (RR)

**Concept:**

- Each process gets a fixed time slice (quantum) in turn.

- Preemptive: if a process does not finish in its quantum, it is paused and put at the end of the ready queue.

**How it works:**

 1. Place processes in a ready queue (by arrival time).

 2. The CPU gives each process up to q units of time.

 3. If the process finishes, it leaves; otherwise, it is requeued.

 4. Cycle repeats until all processes finish.

**Pros:**

- Fair to all processes (everyone gets CPU time regularly).

- Ideal for time-sharing systems.

**Cons:**

- Performance depends heavily on quantum size:

- Too small → too many context switches.

- Too large → behaves like FCFS.

**OUTPUT**
![alt text](Round Robin.png)







