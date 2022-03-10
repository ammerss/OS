#include <iostream>
#include "process.h"
using namespace std;
	
Process::Process(int id_, int at_, int tc_, int cb_, int io_){
	this->id = id_;
	this->at = at_;
	this->tc = tc_;
	this->cb = cb_;
	this->io = io_;
}


