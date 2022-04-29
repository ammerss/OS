#include <iostream>
#include <getopt.h>
#include <fstream>

using namespace std;
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

	}

	return 0;
}
