#include <iostream>
#include <fstream>
#include <string.h>
#include <queue>
#include <deque>
#include <stdio.h>
#include "sched.h"
#include "process.h"
#include <getopt.h>
#include <algorithm>
using namespace std;
queue<Process*> plist;
deque<Process*> processQ;
Scheduler* scheduler;
ifstream rf;
bool verbose = false;
int maxprio = 4;
int quantim = 10000;
queue<Process*> CREATED;
queue<Process*> READY;
//queue<process*> *RUNNING;
//queue<process*> *BLOCKED;
typedef enum State{
	CREATED_TO_READY,
	READY_TO_RUNNING,
	RUNNING_TO_BLOCKED,
	RUNNING_TO_READY,
	BLOCKED_TO_READY,
	DONE,
};
typedef struct Event{
	int timestamp;
	Process* process;
	State state;
};
deque<Event*> eventQ;
void insertEvent(Event* e);
int myrandom(int burst);
void printV(Event* e, bool verbose, int current_time);
void select_sched(char* s){
	//cout << "scehd : " << s << endl;
	switch(s[0]){
		case 'F' :
			scheduler = new FCFS();
			cout << "FCFS" << endl;
			break;
		case 'L':
			cout << "LCFS" << endl;
			break;
		case 'S':
			cout << "SRTF" << endl;
			break;
		case 'R':
			cout << "RR" << endl;
			break;
		case 'P' :
			cout << "PRIO" << endl;
			break;
		case 'E' :
			cout << "preemtive prio scheduler " << endl;
			break;

	}
}
void simulation(){
	Event *evt;
	bool CALL_SCHEDULER = false;
	while(!eventQ.empty()){
		Event *evt = eventQ.front();
		eventQ.pop_front();
		Process *proc = evt->process;
		int cur_time = evt->timestamp;
		State state = evt->state;
		printV(evt, verbose, cur_time);
		delete(evt);
		switch(state){			
			case RUNNING_TO_BLOCKED:{
				//create event for when process becomes ready again
				proc->it += proc->run_time;
				Event *e = new Event();
				e->timestamp = cur_time+proc->run_time;
				e->process = proc;
				e->state = BLOCKED_TO_READY;
				insertEvent(e);
						}break;	
			case BLOCKED_TO_READY:{
				//must come from blocked?
				//add to run queue?
				//int run_time = proc->run_time;
				proc-> run_time = myrandom(proc->cb);
				Event *e = new Event();
				e->timestamp = cur_time;
				e->process = proc;
				e->state = READY_TO_RUNNING;
				insertEvent(e);
				//CALL_SCHEDULER = true;
					      }break;
			case CREATED_TO_READY:{
				//add to runque
				proc->run_time = myrandom(proc->cb);
				Event *e = new Event();
				e->timestamp = cur_time;
				e->process = proc;
				e->state = READY_TO_RUNNING;
				insertEvent(e);
				scheduler->add_process(proc);
				//CALL_SCHEDULER = true;
					      }break;
			case READY_TO_RUNNING:{
				//create event for when process becomes blocked
				int run_time = proc->run_time;
				if(proc->rem <= run_time){
					run_time = min(run_time, proc->rem);
					proc->rem -= run_time;
					proc->tt += run_time;
					Event *e = new Event();
					e->timestamp = cur_time + run_time;
					e->process = proc;
					e->state = DONE;
					insertEvent(e);
				}
				else{
					proc->rem -= run_time;
					proc->tt += run_time;
					proc->run_time = myrandom(proc->io);
					Event *e = new Event();
					e->timestamp = cur_time + run_time;
					e->process = proc;
					e->state = RUNNING_TO_BLOCKED;
					insertEvent(e);
				}
			        //insertEvent(evt);	
				//CALL_SCHEDULER = true;
					      }break;
			case DONE:{
					  proc->ft = cur_time;
					  proc->tt = cur_time - proc->at;
				printf("finished pid : %d, cur_time :  %d\n", proc->id, cur_time);
				  }break;


		}
		if(CALL_SCHEDULER){
			cout<<"scheduler"<<endl;
			CALL_SCHEDULER=false;
			if(proc->rem <= 0){ //check if current process has finished and create event for new process
				proc = scheduler->get_next_process();
				if(proc){
					Event e = {cur_time, proc, BLOCKED_TO_READY};
					insertEvent(&e);
				}
				else continue;
			}

		}
	}

}
void insertEvent(Event* e){
	int pos = int(eventQ.size());
	for(int i=0;i<eventQ.size();i++){
		if(e->timestamp < eventQ[i]->timestamp){
			pos = i;
			break;
		}
	}
	eventQ.insert(eventQ.begin()+pos,e);
}
int myrandom(int burst){
	int num;
	rf >> num;
	return 1 + (num % burst);

}
void printV(Event* e, bool verbose, int current_time){
	if(verbose){
		Process *process = e->process;
		cout << current_time<<" "<<e->process->id<<" "<<e->process->time_in_prev<< ": ";
		if(e->state == CREATED_TO_READY){
			cout << "CREATED -> READY" <<endl;
		}
		else if(e->state == READY_TO_RUNNING){
			cout << "READY -> RUNNG";
			cout << " cb=" << process->run_time;
			cout << " rem=" << process->rem;
		       	cout << " prio=" << process->prio<<endl;
		}
		else if(e->state == RUNNING_TO_BLOCKED){
			cout << "RUNNG -> BLOCK";
			cout << " ib=" << process->run_time;
			cout << " rem=" << process->rem<<endl;
		}
		else if(e->state == BLOCKED_TO_READY){
			cout << "BLOCK -> READY" << endl;
		}
		else if(e->state == DONE){
			cout << "DONE" << endl;
		}
	}
}
void summary(){
	while(!plist.empty()){
		Process *p = plist.front();
		plist.pop();
		printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
				p->id,p->at,p->tc,p->cb,p->io,p->prio,p->ft,p->tt,p->it,p->cw);
	}
}
int main(int argc, char *argv[]){
	cout <<argc<<endl;
	cout<<argv[0]<<endl;
	cout <<argv[1]<<endl;
	cout <<argv[2]<<endl;
	cout<<argv[3]<<endl;
	char c;
	extern char *optarg;
	int id = 0;
	//extern int *optind;
	while( (c = getopt(argc,argv,"vs:")) != -1 ){
		switch(c){
			case 'v':
				verbose = true;
				break;
			case 's':
				char* s = optarg;
				select_sched(s);
				break;
		}
	}
	FILE *f;
	cout <<"read file : "<<endl;
	//cout << argc-2 << endl;
	f = fopen(argv[argc-2],"r");
	rf.open(argv[argc-1]);
	myrandom(1);// skip the first num in rfile
	while(!feof(f)){
		int at;
		int tc;
		int cb;
		int io;
		fscanf(f, "%d %d %d %d\n", &at, &tc, &cb, &io);
		printf("%d %d %d %d\n", at, tc, cb, io);
		void *mem = malloc(sizeof(Process));
		Process* p = new (mem) Process(id,at,tc,cb,io,myrandom(maxprio));
		plist.push(p);
		Event *e = new Event();
	        e->timestamp = p->at;
		e->process = p;
		e->state = CREATED_TO_READY;	
		//e = {p->at, p, CREATED_TO_READY};
		insertEvent(e);
		id++;
	}
	simulation();
	rf.close();
	fclose(f);
	summary();
	cout << "end of program"<<endl;
	return 0;	
}


