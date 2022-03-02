#include <iostream>
#include "process.h"
using namespace std;
	
Process::Process(int a,int t, int c, int i){
	this->at = a;
	this->tc = t;
	this->cb = c;
	this->io = i;
}


