#ifndef PROCESS_H
#define PROCESS_H
#include <iostream>
using namespace std;
class Process{
public:
	int at;
	int tc;
	int cb;
	int io; 

	Process(int a,int t, int c, int i);
};
#endif
