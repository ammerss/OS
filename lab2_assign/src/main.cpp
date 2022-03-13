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
#include <vector>
using namespace std;
queue<Process*> plist;
Scheduler* scheduler;
ifstream rf;
bool verbose = false;
int maxprio = 4;
int quantum = 10000;
vector<int> rand_list;
int rand_cnt;

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
			scheduler = new LCFS();
			cout << "LCFS" << endl;
			break;
		case 'S':
			scheduler = new SRTF();
			cout << "SRTF" << endl;
			break;
		case 'R':
			quantum = atoi(s+1);
			scheduler = new RR();
			cout << "RR" << " " << quantum << endl;
			break;
		case 'P' :{
			quantum = atoi(strtok(s+1,":"));
			char* temp = strtok(NULL, " ");
			if(temp) maxprio = atoi(temp);
			scheduler = new PRIO();
			cout << "PRIO" << endl;}
			break;
		case 'E' :{
			quantum = atoi(strtok(s+1,":"));
			char* temp = strtok(NULL, " ");
			if(temp) maxprio = atoi(temp);
			scheduler = new PREPRIO();
			cout << "preemtive prio scheduler " << endl;}
			break;

	}
}
void preempt_process(Process *p, int cur_time){
	p->preempted = true;
	//remove pending event
	int pos = int(eventQ.size());
	for(int i=0;i<eventQ.size();i++){
		if(eventQ[i]->process->id == p->id){
			pos = i;
			break;
		}
	}
	//cout << eventQ[pos]->state << endl;
	//cout << eventQ[pos]->timestamp << endl;
	eventQ.erase(eventQ.begin()+pos);
	int supposed_to_run = p->nxt_evt_ts - p->time_in_prev;
	int actually_ran = cur_time - p->time_in_prev;
	int have_to_run_more = p->prev_run_time - actually_ran;
	cout << "pid : " <<p->id<<endl;
	cout << "actually ran: " <<actually_ran << endl;
	cout << "prev run time : " << p->prev_run_time<<endl;
	cout << "nxt_evt_ts : " <<p->nxt_evt_ts<<endl;
	cout <<" was started at : " <<p->time_in_prev<<endl;
	cout << "have to run more : " <<have_to_run_more << endl;
	cout << "p rem : " << p->rem<<endl;
	cout << "p tt : " << p-> tt <<endl;
	cout << "p burst left: " << p->burst_left << endl;
	cout << "p run time : " << p->run_time << endl;
	int diff = min(p->prev_run_time, quantum) - actually_ran;
	if(p->burst_left == 0 && p->rem > 0) rand_cnt--;
	p->rem += diff;
	p->tt += diff;
	p->run_time = have_to_run_more;
	p->burst_left = have_to_run_more;
	p->nxt_evt_ts = cur_time;
	Event *e = new Event();
	e->timestamp = cur_time;
	e->process = p;
	e->state = RUNNING_TO_READY;
	insertEvent(e);
}
void simulation(){
	Event *evt;
	bool CALL_SCHEDULER = false;
	Process* cpu_occupied = NULL;
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
				proc->time_in_prev = cur_time;
				proc->it += proc->run_time;
				//if(scheduler->prior) proc->dprio--;
				Event *e = new Event();
				e->timestamp = cur_time+proc->run_time;
				e->process = proc;
				e->state = BLOCKED_TO_READY;
				insertEvent(e);
				cpu_occupied = NULL;
				CALL_SCHEDULER = true;
						}break;	
			case BLOCKED_TO_READY:{
				//add to run queue?
				CALL_SCHEDULER = true;
				proc->dprio = proc->prio-1;
				if(scheduler->preempt && cpu_occupied){
				printf("---->PRIO preemption %d by %d  ? %d === TS=%d now=%d----------",
                                                cpu_occupied->id, proc->id, cpu_occupied->dprio < proc->dprio,
						cpu_occupied->nxt_evt_ts, cur_time);
				}
				if(scheduler->preempt && cpu_occupied 
						&& proc->dprio > cpu_occupied->dprio 
						&& cpu_occupied->nxt_evt_ts > cur_time){
					printf("YES\n");
                                      preempt_process(cpu_occupied, cur_time);
				 
                                }
				else cout<<endl;
				proc->time_in_prev = cur_time;
				scheduler->add_process(proc);
					      }break;
			case CREATED_TO_READY:{
				//add to runque
				/*proc->run_time = myrandom(proc->cb);
				Event *e = new Event();
???END
				e->timestamp = cur_time;
				e->process = proc;
				e->state = READY_TO_RUNNING;
				insertEvent(e);*/
				CALL_SCHEDULER = true;
				if(scheduler->preempt && cpu_occupied){
				printf("---->PRIO preemption %d by %d  ?===%d --- TS=%d now=%d--------",
                                                cpu_occupied->id, proc->id, cpu_occupied->dprio < proc->dprio,
                                                cpu_occupied->nxt_evt_ts, cur_time);
				}
				if(scheduler->preempt && cpu_occupied 
                                                && proc->dprio > cpu_occupied->dprio 
                                                && cpu_occupied->nxt_evt_ts > cur_time){
					printf("YES\n");
					preempt_process(cpu_occupied, cur_time);
		
				}
				else cout<<endl;
				proc->time_in_prev = cur_time;	
				scheduler->add_process(proc);
				//CALL_SCHEDULER = true;
				
					      }break;
			case READY_TO_RUNNING:{
				//create event for when process becomes blocked
				int total_run_time = proc->run_time;
				int run_time = min(quantum, total_run_time);
				proc->cw += cur_time - proc->time_in_prev;
				proc->time_in_prev = cur_time;
				proc->prev_run_time = total_run_time;
				cpu_occupied = proc;
				if(proc->rem <= run_time){ //process will finish with this run
					run_time = min(run_time, proc->rem);
					proc->rem -= run_time;
					proc->tt += run_time;
					proc->nxt_evt_ts = cur_time + run_time;
					proc->burst_left = 0;
					Event *e = new Event();
					e->timestamp = cur_time + run_time;
					e->process = proc;
					e->state = DONE;
					insertEvent(e);
				}
				else if(total_run_time - run_time > 0){ //process did not finish cpu burst
					proc->rem -= run_time;
					proc->tt += run_time;
					proc->burst_left = total_run_time - run_time;
					proc->run_time = proc->burst_left;
					proc->nxt_evt_ts = cur_time + run_time;
					Event *e = new Event();
					e->timestamp = cur_time + run_time;
					e->process = proc;
					e->state = RUNNING_TO_READY;
					insertEvent(e);
				}
				else{ //process finished cpu burst and will be blocked
					proc->rem -= run_time;
					proc->tt += run_time;
					proc->burst_left = 0;
					proc->nxt_evt_ts = cur_time + run_time;
					proc->run_time = myrandom(proc->io);
					Event *e = new Event();
					e->timestamp = cur_time + run_time;
					e->process = proc;
					e->state = RUNNING_TO_BLOCKED;
					insertEvent(e);
				}
					      }break;
			case RUNNING_TO_READY:{
					cpu_occupied = NULL;
					proc->time_in_prev = cur_time;
					if(scheduler->prior) proc->dprio --;
					scheduler->add_process(proc);
					CALL_SCHEDULER = true;
					proc->preempted = false;
					      }break;

			case DONE:{
					  cpu_occupied = NULL;
					  proc->ft = cur_time;
					  proc->tt = cur_time - proc->at;
					  CALL_SCHEDULER = true;
				  }break;
		}
		if(CALL_SCHEDULER){
			if(!eventQ.empty() && eventQ.front()->timestamp == cur_time) continue;
			CALL_SCHEDULER=false;
			if(!cpu_occupied){
			Process* newp = scheduler->get_next_process();
				if(newp){	 
					/*if(cur_time == 91) {
						cout << myrandom(30)<<endl;
						rand_cnt--;
						cout << myrandom(20)<<endl;
						rand_cnt--;
						cout <<myrandom(10)<<endl;
						
						cout << myrandom(30)<<endl;
                                                rand_cnt--;
                                                cout << myrandom(20)<<endl;
                                                rand_cnt--;
                                                cout <<myrandom(10)<<endl;

						exit(1);
					}*/
					//newp->prev_run_time = newp->run_time;
					cout << newp->id << " " <<newp->burst_left << endl;
					if(newp->burst_left == 0) newp->run_time = myrandom(newp->cb);
                        		Event *e = new Event();
                        		e->timestamp = cur_time;
                        		e->process = newp;
                        		e->state = READY_TO_RUNNING;
                        		insertEvent(e);
				}
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
	cout << "----------------------------------myrandom------------------------------" << endl;
	int num = rand_list[rand_cnt];
	rand_cnt++;
	return 1 + (num % burst);
}
void printV(Event* e, bool verbose, int current_time){
	if(verbose){
		Process *p = e->process;
		cout << current_time<<" "<<p->id<<" "<< current_time - p->time_in_prev<< ": ";
		if(e->state == CREATED_TO_READY){
			printf("CREATED -> READY\n");
		}
		else if(e->state == READY_TO_RUNNING){
			printf("READY -> RUNNG cb=%d rem=%d prio=%d\n",p->run_time,p->rem,p->dprio);
		}
		else if(e->state == RUNNING_TO_BLOCKED){
			printf("RUNNG -> BLOCK ib=%d rem=%d\n",p->run_time, p->rem);
		}
		else if(e->state == RUNNING_TO_READY){
			printf("RUNNG -> READY  cb=%d rem=%d prio=%d\n", p->run_time, p->rem,p->dprio);
		}
		else if(e->state == BLOCKED_TO_READY){
			printf("BLOCK -> READY\n");
		}
		else if(e->state == DONE){
			printf("Done\n");
		}
	}
}
void summary(){
	int time_cpubusy=0, time_iobusy=0, num_processes=0, finishtime=0;
	int total_tt=0, total_cw=0;
	double cpu_util;
	double io_util;
	double avg_tt;
	double avg_cw;
	double tp;
	while(!plist.empty()){
		Process *p = plist.front();
		plist.pop();
		printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
				p->id,p->at,p->tc,p->cb,p->io,p->prio,p->ft,p->tt,p->it,p->cw);
		finishtime = max(finishtime, p->ft);
		time_cpubusy += p->tc;
		time_iobusy += p->it;
		num_processes++;
		total_tt += p->tt;
		total_cw += p->cw;
	}
	cpu_util = 100.0 * (time_cpubusy  / (double) finishtime);
	io_util  = 100.0 * (time_iobusy   / (double) finishtime);
	tp 	 = 100.0 * (num_processes / (double) finishtime);
	avg_tt   = (double) total_tt/num_processes;
	avg_cw   = (double) total_cw/num_processes;
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
			finishtime, cpu_util,io_util,avg_tt,avg_cw,tp);

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
	int num_rand;
	rf >> num_rand;
	for(int i=0;i<num_rand;i++){
		int temp;
		rf >> temp;
		rand_list.push_back(temp);
	}
	rf.close();
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
		insertEvent(e);
		id++;
	}
	fclose(f);

	simulation();
	summary();
	cout << "end of program"<<endl;
	return 0;
}	
