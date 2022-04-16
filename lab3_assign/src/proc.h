#ifndef PROC_h
#define PROC_h
#include <iostream>
#include "pte.h"
using namespace std;
const int MAX_VPAGES=64;
typedef struct proc{
	int pid;
	struct pte_t pagetable[MAX_VPAGES];
};
#endif
