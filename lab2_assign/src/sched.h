#ifndef SCHED_H
#define SCHED_H
#include <iostream>
#include <deque>
#include "process.h"
using namespace std;

class Scheduler{
	public:
		Scheduler();
		bool prior;
		string type;
		int quantum;
		deque <Process*> runQ;
		virtual void add_process(Process *p);
		virtual Process* get_next_process();
		//virtual bool test_preempt(Process *p, int curtime);
};
class PRIO : public Scheduler{
	public :
		deque <Process*> expQ;
		deque <Process*> *activeQ;
		deque <Process*> *expireQ;
		PRIO();
		void add_process(Process *p);
		Process* get_next_process();
};
class RR : public Scheduler{
	public:
		RR();
		void add_process(Process *p);
		Process* get_next_process();
};
class FCFS : public Scheduler{
	public:
		FCFS();
		void add_process(Process *p);
		Process* get_next_process();
};
class LCFS : public Scheduler{
	public:
		LCFS();
		void add_process(Process *p);
		Process* get_next_process();
};
class SRTF : public Scheduler{
	public:
		SRTF();
		void add_process(Process *p);
		Process* get_next_process();
};
#endif
