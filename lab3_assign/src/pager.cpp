#include <iostream>
#include <vector>
#include <fstream>
#include "pager.h"
#include "pte.h"
#include "fte.h"
#include "proc.h"
using namespace std;
Pager::Pager(){}
int Pager::select_victim_frame(vector<fte_t> &frametable, vector<proc> &process){return 0;}
FIFO::FIFO(){
	this->hand = NULL;
}
int FIFO::select_victim_frame(vector<fte_t> &frametable, vector<proc> &process){
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
int Random::select_victim_frame(vector<fte_t> &frametable, vector<proc> &process){
	if(rand_cnt == RAND_NUM) rand_cnt = 0;
	int num = rand_list[rand_cnt];
	this->rand_cnt++;
	return num % frametable.size();
}
Clock::Clock(){
	this->hand = NULL;
}
int Clock::select_victim_frame(vector<fte_t> &frametable, vector<proc> &process){
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
NRU::NRU(){
	this->hand = NULL;
	this->ins_cnt = 0;
}
void NRU::reset_referenced(vector<fte_t> frametable, vector<proc> &process){
	for(int i=0;i<frametable.size();i++){
		int pid = frametable[i].proc_num;
		int page = frametable[i].page_num;
		process[pid].pagetable[page].referenced=0;
	}
	this->ins_cnt=0;
}
int NRU::select_victim_frame(vector<fte_t> &frametable, vector<proc> &process){
	int flag=0;
	fte_t *start=hand;
	fte_t *cand1=NULL;
	fte_t *cand2=NULL;
	fte_t *cand3=NULL;
	while(1){
		if(hand==&frametable[frametable.size()-1] || hand==NULL){
		       	hand = &frametable[0];
			if(start==NULL) start=hand;
		}
		else hand++;
		int pid= hand->proc_num;
		int page = hand->page_num;
		int r = process[pid].pagetable[page].referenced;
		int m = process[pid].pagetable[page].modified;
		int rm = 2*r+m;
		if(rm==0) break;
		else if(rm==1 && cand1==NULL) cand1 = hand;
		else if(rm==2 && cand2==NULL) cand2 = hand;
		else if(rm==3 && cand3==NULL) cand3 = hand;
		if(hand==start&&flag){
			if(cand1!=NULL) this->hand = cand1;
			else if(cand2!=NULL) this->hand = cand2;
			else if(cand3!=NULL) this->hand = cand3;
			break;
		}
		flag = 1;
	}
	if(ins_cnt>=50)reset_referenced(frametable, process);
	return hand-&frametable[0];
}
Aging::Aging(){
	this->hand = NULL;
}
string toBinary(unsigned int n){
	string r;
	for(int i=0;i<32;i++){
		r=(n%2==0 ? "0":"1")+r;
		n/=2;
	}
	return r;
}
int Aging::select_victim_frame(vector<fte_t> &frametable, vector<proc> &process){
	int min = -1;
	unsigned int minAge = 0xffffffff;
	//fte_t *start = hand;
	//int flag = 0;
	for(int cnt=0;cnt<frametable.size();cnt++){
		if(hand==&frametable[frametable.size()-1] || hand==NULL)hand = &frametable[0];
		else hand++;
		int pid = hand->proc_num;
		int page = hand->page_num;
		int i = hand-&frametable[0];
		//unsigned int age = hand->age;
		//printf("hand %d age\t",i);
		//cout << toBinary(frametable[i].age)<<" ";
		frametable[i].age = frametable[i].age >> 1;
		if(process[pid].pagetable[page].referenced==1){
			//frametable[i].age = frametable[i].age >> 1;
			frametable[i].age = (frametable[i].age | 0x80000000);
			process[pid].pagetable[page].referenced=0;
		}
		if(frametable[i].age < minAge){
			minAge = frametable[i].age;
			min = i;
		}
	}
	this->hand = &frametable[min];
	frametable[min].age=0;
	return min;
}
WorkingSet::WorkingSet(){
	this->hand = NULL;
	this->ins_cnt = 0;
}
int WorkingSet::select_victim_frame(vector<fte_t> &frametable, vector<proc> &process){
	int min = -1;
	unsigned int minAge = 0xffffffff;
	for(int cnt=0;cnt<frametable.size();cnt++){
		if(hand==&frametable[frametable.size()-1] || hand==NULL)hand=&frametable[0];
		else hand++;
		int pid = hand->proc_num;
		int page = hand->page_num;
		int i = hand-&frametable[0];
		if(process[pid].pagetable[page].referenced==1){
			frametable[i].age = ins_cnt;
			process[pid].pagetable[page].referenced=0;
		}
		else{
			if(ins_cnt - frametable[i].age >= 50){
				min = i;
				break;
			}
			else{
				if(frametable[i].age < minAge){
					minAge = frametable[i].age;
					min = i;
				}
			}
		}
		//printf("hand %d \t\t",i);
		//cout << frametable[i].age << endl;
	}
	if(min==-1){
		if(hand==&frametable[frametable.size()-1] || hand==NULL) hand = &frametable[0];
		else hand++;
		min = hand-&frametable[0];
	}
	//cout << "cur time : " << ins_cnt << endl;
	this->hand= &frametable[min];
	frametable[min].age= ins_cnt;
	return min;
}
