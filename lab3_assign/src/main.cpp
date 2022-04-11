#include <iostream>
#include <fstream>
#include <stdio.h>
#include <getopt.h>
using namespace std;
int main(int argc, char *argv[]){
	extern char *optarg;
	char c;
	while((c=getopt(argc,argv,"a:o:f:")) != -1){
		switch(c){
			case 'a':
				break;
			case 'f':
				break;
		}
	}
	ifstream f;
	f.open(argv[argc-2]);
	string line;
	int num_process;
	while(getline(f, line)){
		if(line[0] == '#')continue;
		sscanf(line.c_str(),"%d", &num_process);
		break;
	}
	for(int i=0;i<num_process;i++){
		cout << "#process "<< i << endl;
		while(getline(f,line)){
			int num_vmas;
			if(line[0] == '#')continue;
			sscanf(line.c_str(),"%d", &num_vmas);
			for(int j=0;j<num_vmas;j++){
					
			}
		}
	}
		
	
	while(getline(f,line)){
		
	}
	cout << num_process << endl;

	return 0;
}
