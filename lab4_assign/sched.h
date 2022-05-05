#ifndef SCHED_H
#define SCHED_H
#include <iostream>
#include <vector>
#include "IO.h"
using namespace std;

class Scheduler{
	public:
		Scheduler();
		vector<IO*> ioQ;
		void add_io(IO *io);
		bool empty();
		virtual IO* get_next_io(int &cur_track, int &dir);
		virtual void set_dir(int active_track, int cur_track, int &dir);
};
class FIFO : public Scheduler{
	public:
		FIFO();
		IO* get_next_io(int &cur_track, int &dir);
		void set_dir(int active_track, int cur_track, int &dir);
};
class SSTF : public Scheduler{
	public:
		SSTF();
		IO* get_next_io(int &cur_track, int &dir);
		void set_dir(int active_track, int cur_track, int &dir);
};
class LOOK : public Scheduler{
	public:
		LOOK();
		IO* get_next_io(int &cur_track, int &dir);
		void set_dir(int active_track, int cur_track, int &dir);
};
#endif
