#include <iostream>
#include <fstream>
#include <string.h>
#include <queue>
#include <deque>
#include <stdio.h>
#include "sched.h"
#include "process.h"
using namespace std;
deque<Event*> eventQ;
deque<Process*> processQ;
Scheduler* scheduler;
queue<Process*> CREATED;
queue<Process*> READY;
//queue<process*> *RUNNING;
//queue<process*> *BLOCKED;
typedef enum State{
	CREATED_TO_READY,
	READY_TO_RUNNING,
	RUNNING_TO_BLOCKED,
	BUNNING_TO_READY,
	BLOCKED_TO_READY,
	DONE,
};
typedef struct Event{
	int timestamp;
	Process process;
	State state;
};
void select_sched(char* s){
	cout << "scehd : " << s << endl;
	switch(s[2]){
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
	while(!eventQ.empty){
		evt = eventQ.front();
		Process *proc = evt->process;
		cur_time = evt->timestamp;
		State state = evt->state;
		switch(state){
			case BLOCKED_TO_RUNNING:
				//create event for blocking
				CALL_SCHEDULER = true;
				break;
			case RUNNING_TO_BLOCKED:
				//create event for when process becomes ready again
				CALL_SCHEDULER = true;
				break;	
			case BLOCKED_TO_READY:
				//must come from blocked?
				//add to run queue?
				CALL_SCHEDULER = true;
				break;
			case CREATED_TO_READY:
				//create event
				Event e ={}; 
				//add to run queue
				CALL_SCHEDULER = true;
		}
		if(CALL_SCHEDULER){
			cout<<"scheduler"<<endl;
			CALL_SCHEDULER=false;

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
int main(int argc, char *argv[]){
	cout <<argc<<endl;
	cout<<argv[0]<<endl;
	cout <<argv[1]<<endl;
	cout <<argv[2]<<endl;
	cout<<argv[3]<<endl;
	FILE *f;
	cout <<"read file : "<<endl;
	f = fopen(argv[2],"r");
	char buff[4096]={0,};
	char* tok;
	int id = 0;
	while(!feof(f)){
		int at;
		int tc;
		int cb;
		int io;
		fscanf(f, "%d %d %d %d\n", &at, &tc, &cb, &io);
		printf("%d %d %d %d\n", at, tc, cb, io);
		Process* p = new Process(id,at,tc,cb,io);
		CREATED.push(p);
		Event e = {p->at, p, CREATED_TO_READY};
		insertEvent(e);
		id++;
	}
	while(!CREATED.empty()){
		cout << CREATED.front()->at <<endl;
		//make events
		READY.push(CREATED.front());
		CREATED.pop();

	}
	fclose(f);
	select_sched(argv[1]);
	cout << "end of program"<<endl;
	return 0;	
}


