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

SSTF::SSTF(){}
IO* SSTF::get_next_io(int &cur_track, int &dir){
	IO *io =NULL;
	if(!ioQ.empty()){
		int min_seektime = abs(ioQ[0]->track - cur_track);
		int pos = 0;
		for(unsigned int i=1; i<ioQ.size();i++){
			int seektime = abs(ioQ[i]->track - cur_track);
			if(seektime < min_seektime){
				min_seektime = seektime;
				pos = i;	
			}
		}
		io = ioQ[pos];
		ioQ.erase(ioQ.begin() + pos);
	}
	return io;
}
void SSTF::set_dir(int active_track, int cur_track, int &dir){
	if(cur_track < active_track) dir = 1;
	else dir = -1;
}

LOOK::LOOK(){}
IO* LOOK::get_next_io(int &cur_track, int &dir){
	IO *io = NULL;
	if(!ioQ.empty()){
		int min_seektime = -1;
		int pos = -1;
		for(unsigned int i=0;i<ioQ.size();i++){
			int seektime = ioQ[i]->track - cur_track;
			if(dir==1 && seektime <0) continue;
			if(dir==-1 && seektime>0) continue;
			seektime = abs(seektime);
			if(seektime < min_seektime || pos == -1){
				min_seektime = seektime;
				pos = i;
			}
		}
		if(pos!=-1){
			io = ioQ[pos];
			ioQ.erase(ioQ.begin() + pos);
		}
	}
	if(io!=NULL) return io;
	dir = -dir;
	if(!ioQ.empty()){
                int min_seektime = -1;
                int pos = -1;
                for(unsigned int i=0;i<ioQ.size();i++){
                        int seektime = ioQ[i]->track - cur_track;
                        if(dir==1 && seektime <0) continue;
                        if(dir==-1 && seektime>0) continue;
                        seektime = abs(seektime);
                        if(seektime < min_seektime || pos == -1){
                                min_seektime = seektime;
                                pos = i;
                        }
                }
                if(pos!=-1){
                        io = ioQ[pos];
                        ioQ.erase(ioQ.begin() + pos);
                }
        }
	return io;
}
void LOOK::set_dir(int active_track, int cur_track, int &dir){}
CLOOK::CLOOK(){flag = 0;}
IO* CLOOK::get_next_io(int &cur_track, int &dir){
	IO *io = NULL;
	if(flag==1){
		dir = 1;
		flag = 2;
	}
        if(!ioQ.empty()){
                int min_seektime = -1;
                int pos = -1;
                for(unsigned int i=0;i<ioQ.size();i++){
                        int seektime = ioQ[i]->track - cur_track;
                        if(dir==1 && seektime <0) continue;
                        if(dir==-1 && seektime>0) continue;
                        seektime = abs(seektime);
                        if(seektime < min_seektime || pos == -1){
                                min_seektime = seektime;
                                pos = i;
                        }
                }
                if(pos!=-1){
                        io = ioQ[pos];
                        ioQ.erase(ioQ.begin() + pos);
                }
        }
	if(io!=NULL) return io;
        if(!ioQ.empty()){
		flag = 1;
		int temp = 0;
                int min_seektime = -1;
                int pos = -1;
                for(unsigned int i=0;i<ioQ.size();i++){
                        int seektime = ioQ[i]->track - temp;
                        if(dir==1 && seektime <0) continue;
                        if(dir==-1 && seektime>0) continue;
                        seektime = abs(seektime);
                        if(seektime < min_seektime || pos == -1){
                                min_seektime = seektime;
                                pos = i;
                        }
                }
                if(pos!=-1){
                        io = ioQ[pos];
                        ioQ.erase(ioQ.begin() + pos);
			dir = -1;
                }
        }
	return io;
}
void CLOOK::set_dir(int active_track, int cur_track, int &dir){
	if(flag == 2) {
		dir = 1;
		flag = 0;
	}
}
FLOOK::FLOOK(){
	activeQ = &ioQ;
	addQ = &anotherQ;
}
void FLOOK::add_io(IO *io){
	addQ->push_back(io);
}
IO* FLOOK::get_next_io(int &cur_track, int &dir){
	if(activeQ->empty()){
		vector<IO*> *temp;
		temp = activeQ;
		activeQ = addQ;
		addQ = temp;
	}
	IO *io = NULL;
	if(!activeQ->empty()){
		int min_seektime = -1;
		int pos = -1;
		int i = -1;
		for(auto a : *activeQ){
			i++;
			int seektime = a->track - cur_track;
			if(dir==1 && seektime<0) continue;
			if(dir==-1 && seektime>0) continue;
			seektime = abs(seektime);
			if(seektime < min_seektime || pos == -1){
				min_seektime = seektime;
				pos = i;
			}
		}
		if(pos!= -1){
			io = activeQ->at(pos);
			activeQ->erase(activeQ->begin() + pos);
		}
	}
	if(io!=NULL) return io;
	dir = -dir;
	if(!activeQ->empty()){
                int min_seektime = -1;
                int pos = -1;
		int i = -1;
                for(auto a : *activeQ){
			i++;
                        int seektime = a->track - cur_track;
                        if(dir==1 && seektime<0) continue;
                        if(dir==-1 && seektime>0) continue;
                        seektime = abs(seektime);
                        if(seektime < min_seektime || pos == -1){
                                min_seektime = seektime;
                                pos = i;
                        }
                }
                if(pos!= -1){
                        io = activeQ->at(pos);
                        activeQ->erase(activeQ->begin() + pos);
                }
        }
	return io;
}
void FLOOK::set_dir(int active_track, int cur_track, int &dir){}

