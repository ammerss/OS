#include <iostream>
#include "sched.h"

using namespace std;

Scheduler::Scheduler(){
	this->quantum = 0;
	this->type = "";
}
void Scheduler::add_process(Process *p){}
Process* Scheduler::get_next_process(){Process *p;return p;}
//bool test_preempt(Process *p, int curtime){return false;}
FCFS::FCFS(){
}
void FCFS::add_process(Process *p){
	runQ.push_back(p);
}
Process* FCFS::get_next_process(){
	Process *p=NULL;
	if(!runQ.empty()){
		p = runQ.front();
		runQ.pop_front();
	}
	return p;
}

