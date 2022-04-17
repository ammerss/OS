#include <iostream>
#include <vector>
#include <fstream>
#include "pager.h"
#include "pte.h"
#include "fte.h"
#include "proc.h"
using namespace std;
Pager::Pager(){}
int Pager::select_victim_frame(vector<fte_t> frametable, vector<proc> &process){return 0;}
FIFO::FIFO(){
	this->hand = NULL;
}
int FIFO::select_victim_frame(vector<fte_t> frametable, vector<proc> &process){
	if(hand==NULL) hand = &frametable[0];
	else{
		if(hand==&frametable[frametable.size()-1]) {
			hand = &frametable[0];
		}
		else hand++;
	}
	return hand-&frametable[0];
}
Random::Random(){}
Random::Random(char* rf_path){
	this->rand_cnt = 0;
	this->RAND_NUM;
	ifstream rf;
	rf.open(rf_path);
	rf >> this->RAND_NUM;
	for(int i=0;i<RAND_NUM;i++){
		int temp;
		rf >> temp;
		this->rand_list.push_back(temp);
	}
	rf.close();
}
int Random::select_victim_frame(vector<fte_t> frametable, vector<proc> &process){
	if(rand_cnt == RAND_NUM) rand_cnt = 0;
	int num = rand_list[rand_cnt];
	this->rand_cnt++;
	return num % frametable.size();
}
Clock::Clock(){
	this->hand = NULL;
}
/*void p(vector<proc> &process){
for(int i=0;i<64;i++){
                printf("page %d referenced %d\n", i, process[0].pagetable[i].referenced);
        }
process[0].pagetable[0].referenced=0;

}*/
int Clock::select_victim_frame(vector<fte_t> frametable, vector<proc> &process){
	while(1){
		if(hand==&frametable[frametable.size()-1] || hand==NULL) hand = &frametable[0];
		else hand++;
		int pid = hand->proc_num;
		int page = hand->page_num;	
		if(process[pid].pagetable[page].referenced==0) break;
		else process[pid].pagetable[page].referenced = 0;
	}
	
	return hand-&frametable[0];
}

