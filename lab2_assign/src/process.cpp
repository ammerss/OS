#include <iostream>
#include "process.h"
using namespace std;
	
Process::Process(int id_, int at_, int tc_, int cb_, int io_){
	this->id = id_;
	this->at = at_;
	this->tc = tc_;
	this->cb = cb_;
	this->io = io_;
	this->prio = 0;
	this->ft = 0;
	this->tt = 0;
	this->it = 0;
	this->cw = 0;
	this->rem = tc_;
}


