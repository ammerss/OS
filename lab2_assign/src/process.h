#ifndef PROCESS_H
#define PROCESS_H
#include <iostream>
using namespace std;
class Process{
public:
	int id;
	int at;
	int tc;
	int cb;
	int io;
        int prio;
	int ft;
	int tt;
	int it;
	int cw;
	int rem;
	int run_time;
	int time_in_prev;
	int dprio;
	int burst_left;	
	int nxt_evt_ts;
	int prev_run_time;

	Process(int id_, int at_, int tc_, int cb_, int io_, int prio_);
};
#endif
