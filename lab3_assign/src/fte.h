#ifndef FTE_h
#define FTE_h
#include <iostream>
using namespace std;
typedef struct fte_t{
	int page_num;
	int proc_num;
	unsigned int age:32;
};
#endif
