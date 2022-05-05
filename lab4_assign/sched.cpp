#include <iostream>
#include <vector>
#include "sched.h"
using namespace std;

Scheduler::Scheduler(){}
void Scheduler::add_io(IO *io){
	ioQ.push_back(io);
}
bool Scheduler::empty(){
	if(ioQ.size()==0)return true;
	else return false;
}
IO* Scheduler::get_next_io(int &cur_track, int &dir){
	IO *io=NULL;
	return io;
}
void Scheduler::set_dir(int active_track, int cur_track, int &dir){}

FIFO::FIFO(){}
IO* FIFO::get_next_io(int &cur_track, int &dir){
	IO *io = NULL;
	if(!ioQ.empty()){
		io = ioQ[0];
		ioQ.erase(ioQ.begin());
	}
	return io;
}
void FIFO::set_dir(int active_track, int cur_track, int &dir){
	if(cur_track < active_track)
		dir=1;
	else
		dir=-1;
}

