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
//FCFS
FCFS::FCFS(){}
void FCFS::add_process(Process *p){runQ.push_back(p);}
Process* FCFS::get_next_process(){
	Process *p=NULL;
	if(!runQ.empty()){
		p = runQ.front();
		runQ.pop_front();
	}
	return p;
}
//LCFS
LCFS::LCFS(){}
void LCFS::add_process(Process *p){runQ.push_back(p);}
Process* LCFS::get_next_process(){
	Process *p=NULL;
	if(!runQ.empty()){
		p = runQ.back();
		runQ.pop_back();
	}
	return p;
}
//SRTF
SRTF::SRTF(){}
void SRTF::add_process(Process *p){
	int pos = int(runQ.size());
	for(int i=0;i<runQ.size();i++){
		if(runQ[i]->rem > p->rem){
			pos = i;
			break;
		}
	}
	runQ.insert(runQ.begin()+pos, p);
}
Process* SRTF::get_next_process(){
	Process* p = NULL;
	if(!runQ.empty()){
		p = runQ.front();
		runQ.pop_front();
	}
	return p;
}
