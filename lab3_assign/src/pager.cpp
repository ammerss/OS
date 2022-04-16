#include <iostream>
#include <vector>
#include <fstream>
#include "pager.h"
#include "pte.h"
#include "fte.h"
using namespace std;
Pager::Pager(){}
int Pager::select_victim_frame(vector<fte_t> frametable, pte_t *pagetable){return 0;}
FIFO::FIFO(){
	this->hand = NULL;
}
int FIFO::select_victim_frame(vector<fte_t> frametable, pte_t *pagetable){
	if(hand==NULL) hand = &frametable[0];
	else{
		cout <<" hand is " << hand <<endl;

		if(hand==&frametable[frametable.size()-1]) {
			hand = &frametable[0];
			cout << "init hand back to front of frametable" << endl;
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
int Random::select_victim_frame(vector<fte_t> frametable, pte_t *pagetable){
	if(rand_cnt == RAND_NUM) rand_cnt = 0;
	int num = rand_list[rand_cnt];
	this->rand_cnt++;
	return num % frametable.size();
}

