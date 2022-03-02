#ifndef SCHED_H
#define SCHED_H
#include <iostream>
#include "process.h"
using namespace std;

class Scheduler{
	public:
		Scheduler();
		string type;
		int quantum;

		//virtual void add_process(Process *p);
		//virtual Process* get_next_process();
		//virtual bool test_preempt(Process *p, int curtime);
};

class FCFS : public Scheduler{
	public:
		FCFS();
};
#endif
