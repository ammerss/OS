#include <iostream>
#include <fstream>
#include <string.h>
#include <queue>
#include <stdio.h>
#include "sched.h"
#include "process.h"
using namespace std;
//queue<process*> *CREATED;
//queue<process*> *READY;
//queue<process*> *RUNNING;
//queue<process*> *BLOCKED;
void select_sched(char* s){
	cout << "scehd : " << s << endl;
	switch(s[2]){
		case 'F' :
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
	
	while(!feof(f)){
		int at;
		int tc;
		int cb;
		int io;
		fscanf(f, "%d %d %d %d\n", &at, &tc, &cb, &io);
		printf("%d %d %d %d\n", at, tc, cb, io);
		
	}
	fclose(f);
	select_sched(argv[1]);
	cout << "hello world"<<endl;
	return 0;	
}
