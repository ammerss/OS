#ifndef PTE_h
#define PTE_h

#include <iostream>
using namespace std;
typedef struct pte_t{
	unsigned int present:1;
	unsigned int referenced:1;
	unsigned int modified:1;
	unsigned int write_protect:1;
	unsigned int pagedout:1;
	unsigned int file_mapped:1;
	unsigned int frame_num:26;
};
#endif
