#ifndef PAGER_H
#define PAGER_H
#include <iostream>
#include <vector>
#include <fstream>

#include "pte.h"
#include "fte.h"
using namespace std;
class Pager{
	public:
		Pager();
		virtual int select_victim_frame(vector<fte_t> frametable, pte_t *pagetable);
};
class FIFO : public Pager{
	public:
		FIFO();
		fte_t *hand;
		int select_victim_frame(vector<fte_t> frametable, pte_t *pagetable);
};
class Random : public Pager{
	public:
		Random();
		Random(char* rf_path);
		int rand_cnt;
		int RAND_NUM;
		vector<int> rand_list;
		int select_victim_frame(vector<fte_t> frametable, pte_t *pagetable);
};
#endif
