// scheduler.cpp
// CPU Scheduling Simulator: FCFS, SJF (non-preemptive), Round Robin
// Compile: g++ -std=c++17 -O2 -o scheduler scheduler.cpp

#include <bits/stdc++.h>
using namespace std;

struct Segment {
    int pid;    // 0 for IDLE, otherwise process id (1..n)
    int start;
    int end;
};

struct Process {
    int pid;
    int at;
    int bt;
    int ct = 0;
    int tat = 0;
    int wt = 0;
    int remaining = 0;
    bool done = false;
};

void print_table(const vector<Process>& procs) {
    cout << "\nProcess\tAT\tBT\tCT\tTAT\tWT\n";
    cout << "-----------------------------------------\n";
    for (auto &p : procs) {
        cout << "P" << p.pid << "\t"
             << p.at << "\t"
             << p.bt << "\t"
             << p.ct << "\t"
             << p.tat << "\t"
             << p.wt << "\n";
    }
}

void print_averages(const vector<Process>& procs) {
    double sumTAT = 0, sumWT = 0;
    for (auto &p : procs) { sumTAT += p.tat; sumWT += p.wt; }
    double avgTAT = sumTAT / procs.size();
    double avgWT = sumWT / procs.size();
    cout << fixed << setprecision(2);
    cout << "\nAverage TAT = " << avgTAT << "\n";
    cout << "Average WT  = " << avgWT << "\n";
}

void print_gantt(const vector<Segment>& gantt) {
    if (gantt.empty()) { cout << "\nGantt chart: (none)\n"; return; }
    cout << "\nGantt Chart:\n";
    // top line with process labels
    for (auto &s : gantt) {
        cout << "| ";
        if (s.pid == 0) cout << "IDLE ";
        else cout << "P" << s.pid << " ";
    }
    cout << "|\n";

    // print times below (start of each segment, then final end)
    for (size_t i = 0; i < gantt.size(); ++i) {
        cout << gantt[i].start << "\t";
    }
    cout << gantt.back().end << "\n";
}

// FCFS simulation
vector<Segment> simulate_fcfs(vector<Process> procs) {
    int n = procs.size();
    // Sort by arrival time (stable by pid)
    sort(procs.begin(), procs.end(), [](const Process &a, const Process &b){
        if (a.at != b.at) return a.at < b.at;
        return a.pid < b.pid;
    });
    vector<Segment> gantt;
    int time = 0;
    for (auto &p : procs) {
        if (time < p.at) {
            // CPU idle
            gantt.push_back({0, time, p.at});
            time = p.at;
        }
        int start = time;
        time += p.bt;
        p.ct = time;
        p.tat = p.ct - p.at;
        p.wt = p.tat - p.bt;
        gantt.push_back({p.pid, start, time});
    }
    // map results back in original pid order
    // build result vector in original pid order
    vector<Process> ordered(n+1); // index by pid
    for (auto &p : procs) ordered[p.pid] = p;
    vector<Process> result;
    for (int id = 1; id <= n; ++id) result.push_back(ordered[id]);

    print_table(result);
    print_averages(result);
    print_gantt(gantt);
    return gantt;
}

// SJF non-preemptive
vector<Segment> simulate_sjf(vector<Process> procs) {
    int n = procs.size();
    // copy and sort by arrival
    sort(procs.begin(), procs.end(), [](const Process &a, const Process &b){
        if (a.at != b.at) return a.at < b.at;
        return a.bt < b.bt;
    });
    vector<Segment> gantt;
    int time = 0, completed = 0;
    // mark not done initially
    for (auto &p : procs) p.done = false;

    while (completed < n) {
        // collect ready processes
        int idx = -1;
        int bestBT = INT_MAX;
        for (int i = 0; i < n; ++i) {
            if (!procs[i].done && procs[i].at <= time) {
                if (procs[i].bt < bestBT) {
                    bestBT = procs[i].bt;
                    idx = i;
                }
            }
        }
        if (idx == -1) {
            // no one ready -> jump to next arrival
            int nextAT = INT_MAX;
            for (int i = 0; i < n; ++i) if (!procs[i].done) nextAT = min(nextAT, procs[i].at);
            if (time < nextAT) {
                gantt.push_back({0, time, nextAT});
                time = nextAT;
            }
            continue;
        }
        // run chosen process fully
        int start = time;
        time += procs[idx].bt;
        procs[idx].ct = time;
        procs[idx].tat = procs[idx].ct - procs[idx].at;
        procs[idx].wt = procs[idx].tat - procs[idx].bt;
        procs[idx].done = true;
        completed++;
        gantt.push_back({procs[idx].pid, start, time});
    }

    // order by pid for table print
    vector<Process> ordered(n+1);
    for (auto &p : procs) ordered[p.pid] = p;
    vector<Process> result;
    for (int id = 1; id <= n; ++id) result.push_back(ordered[id]);

    print_table(result);
    print_averages(result);
    print_gantt(gantt);
    return gantt;
}

// Round Robin
vector<Segment> simulate_rr(vector<Process> procs, int quantum) {
    int n = procs.size();
    // sort by arrival time for enqueuing
    vector<Process> sorted = procs;
    sort(sorted.begin(), sorted.end(), [](const Process &a, const Process &b){
        if (a.at != b.at) return a.at < b.at;
        return a.pid < b.pid;
    });
    for (auto &p : sorted) p.remaining = p.bt;
    vector<Segment> gantt;
    queue<int> q; // will store indices into sorted
    int time = 0;
    int i = 0; // next to arrive
    // enqueue initial arrivals at time 0
    while (i < n && sorted[i].at <= time) { q.push(i); i++; }
    if (q.empty() && i < n) {
        // CPU idle until first arrival
        gantt.push_back({0, time, sorted[i].at});
        time = sorted[i].at;
        while (i < n && sorted[i].at <= time) { q.push(i); i++; }
    }
    int finished = 0;
    while (finished < n) {
        if (q.empty()) {
            // idle until next arrival
            if (i < n) {
                gantt.push_back({0, time, sorted[i].at});
                time = sorted[i].at;
                while (i < n && sorted[i].at <= time) { q.push(i); i++; }
            }
            continue;
        }
        int idx = q.front(); q.pop();
        int exec = min(sorted[idx].remaining, quantum);
        int start = time;
        time += exec;
        sorted[idx].remaining -= exec;
        // add any newly arrived processes during this interval
        while (i < n && sorted[i].at <= time) { q.push(i); i++; }
        if (sorted[idx].remaining > 0) {
            // not finished, requeue
            q.push(idx);
        } else {
            // finished
            sorted[idx].ct = time;
            sorted[idx].tat = sorted[idx].ct - sorted[idx].at;
            sorted[idx].wt = sorted[idx].tat - sorted[idx].bt;
            finished++;
        }
        gantt.push_back({sorted[idx].pid, start, time});
    }

    // map back to original order for the table
    vector<Process> ordered(n+1);
    for (auto &p : sorted) ordered[p.pid] = p;
    vector<Process> result;
    for (int id = 1; id <= n; ++id) result.push_back(ordered[id]);

    print_table(result);
    print_averages(result);
    print_gantt(gantt);
    return gantt;
}

int main() {
    cout << "CPU Scheduling Simulator (FCFS, SJF non-preemptive, RR)\n";
    cout << "-----------------------------------------------------\n";

    int useSample = 0;
    cout << "Use sample input from PDF? (1 = yes, 0 = no): ";
    cin >> useSample;

    int n;
    vector<Process> procs;
    if (useSample == 1) {
        // Sample: Processes: 3, AT: [0,1,2], BT: [5,3,8]
        n = 3;
        procs = vector<Process>(n);
        procs[0] = {1, 0, 5};
        procs[1] = {2, 1, 3};
        procs[2] = {3, 2, 8};
        for (auto &p : procs) p.remaining = p.bt;
        cout << "Sample loaded: 3 processes (AT: 0,1,2 BT: 5,3,8)\n";
    } else {
        cout << "Enter number of processes: ";
        cin >> n;
        procs.resize(n);
        for (int i = 0; i < n; ++i) {
            int at, bt;
            cout << "P" << (i+1) << " Arrival Time: ";
            cin >> at;
            cout << "P" << (i+1) << " Burst Time  : ";
            cin >> bt;
            procs[i] = {i+1, at, bt};
            procs[i].remaining = bt;
        }
    }

    cout << "\nChoose algorithm:\n1 - FCFS\n2 - SJF (non-preemptive)\n3 - Round Robin\nSelect (1/2/3): ";
    int choice;
    cin >> choice;
    if (choice == 1) {
        cout << "\n--- FCFS ---\n";
        simulate_fcfs(procs);
    } else if (choice == 2) {
        cout << "\n--- SJF (non-preemptive) ---\n";
        simulate_sjf(procs);
    } else if (choice == 3) {
        int q;
        cout << "Enter quantum (e.g., 2 or 3): ";
        cin >> q;
        if (q <= 0) q = 2;
        cout << "\n--- Round Robin (quantum=" << q << ") ---\n";
        simulate_rr(procs, q);
    } else {
        cout << "Invalid choice.\n";
    }

    cout << "\nDone. Use Ctrl+C to exit if running repeatedly.\n";
    return 0;
}
