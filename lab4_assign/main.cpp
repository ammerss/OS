#include <iostream>
#include <getopt.h>
#include <fstream>
#include "IO.h"
#include "sched.h"
#include <vector>
using namespace std;
vector<IO> io_list;
Scheduler* sched;
int main(int argc, char *argv[]){
	extern char *optarg;
	char c;
	while((c=getopt(argc,argv,"s:vd")) != EOF){
		switch(c){
			case 's':
				if (optarg[0] == 'i') sched = new FIFO();
				else sched = new FIFO();
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
	int cnt = 0;
	IO *active = NULL;
	//for(int cnt=0;cnt<io_list.size();cnt++)
	while(1){
		while(cnt <= io_list.size()-1 && io_list[cnt].time_step <= time){
			sched->add_io(&io_list[cnt]);
			cnt++;
		}
		if(active && cur_track == active->track){
			active->end_time = time;
			active = NULL;
		}
		if(active==NULL){
			active = sched->get_next_io(cur_track, dir);
			if(active){
				active->start_time = time;
				sched->set_dir(active->track, cur_track, dir);
				if(active->track == cur_track)continue;
				cout << "starting to search for " <<active->track<<endl;
			}
			if(active==NULL && sched->empty() && cnt>=io_list.size()-2)break;
			//if(active==NULL && cnt < io_list.size())continue;
			//active->start_time = time;
			//sched->set_dir(active->track, cur_track, dir);
		}	
		if(active){
			if(dir==1) cur_track++;
			else cur_track--;
		}
		time++;
		cout << time <<" "<< cur_track << endl;
	
	}
	for(int i=0;i<io_list.size();i++){
		printf("%5d: %5d %5d %5d\n",i,io_list[i].time_step, io_list[i].start_time, io_list[i].end_time);
	}
	return 0;
}
