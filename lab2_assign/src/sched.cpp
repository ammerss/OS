#include <iostream>
#include <deque>
#include "sched.h"

using namespace std;

Scheduler::Scheduler(){
	this->prior = false;
	this->preempt = false;
}
void Scheduler::add_process(Process *p){}
Process* Scheduler::get_next_process(){Process *p;return p;}
//PREPRIO
PREPRIO::PREPRIO(){
	this->preempt = true;
}
//PRIO
PRIO::PRIO(){
	activeQ = &runQ; 
	expireQ = &expQ;
	this->prior = true;
}
void PRIO::add_process(Process *p){
	if(p->dprio<0){//reset and enter into expiredQ
		p->dprio = p->prio-1; 
		int pos = 0;
		for(auto e : *expireQ){
			if(e->dprio < p->dprio)break;
			pos++;
		}
		expireQ->insert(expireQ->begin() + pos, p);
	}
	else{//add to activeQ
		int pos = 0;
		for(auto a : *activeQ){
			if(a->dprio < p->dprio)break;
			pos++;
		}
		activeQ->insert(activeQ->begin() + pos, p);
	}
}
Process* PRIO::get_next_process(){
	Process *p = NULL;
	if(activeQ->empty()){ //swap active and expire
		deque<Process*> *temp;
		temp = activeQ;
		activeQ = expireQ;
		expireQ = temp;
	}
	if(!activeQ->empty()){
		p = activeQ->front();
		activeQ->pop_front();
	}
	return p;
}
//RR
RR::RR(){}
void RR::add_process(Process *p){
	runQ.push_back(p);
}
Process* RR::get_next_process(){
	Process *p = NULL;
	if(!runQ.empty()){
		p = runQ.front();
		runQ.pop_front();
	}
	return p;
}
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
