#include <iostream>
#include <getopt.h>
#include <fstream>
#include "IO.h"
#include <vector>
using namespace std;
vector<IO> io_list;
int main(int argc, char *argv[]){
	extern char *optarg;
	char c;
	while((c=getopt(argc,argv,"s:vd")) != EOF){
		switch(c){
			case 's':
				if (optarg[0] == 's') break;
				break;
			case 'v':
				break;
			case 'd':
				break;
		}
	}
	ifstream f;
	f.open(argv[argc-1]);
	string line;
	while(getline(f,line)){
		if(line[0] == '#' || line.length() == 0)continue;
		int time_step, track;
		sscanf(line.c_str(),"%d %d", &time_step, &track);
		cout << time_step << track << endl;
		struct IO temp = {time_step, track, 0, 0};
		io_list.push_back(temp);
	}
	f.close();
	int time = 0;
	int cur_track = 0;
	int dir = 1;
	IO *active = NULL;
	//for(int cnt=0;cnt<io_list.size();cnt++)
	while(1){
		while(io_list[cnt].time_step <= time){
			sched.add_io(io_list[cnt]);
		}
		if(active && cur_track == active->track){
			active->end_time = time;
		}
		if(active==NULL){
			active = sched.get_next_io();
			if(active==NULL)break;
			active->start_time = time;
		}	
		if(active){
			if(dir) cur_track++;
			else cur_track--;
		}
		time++;
	}

	return 0;
}
