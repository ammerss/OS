#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;
int main(int argc, char *argv[]){
	cout <<argc<<endl;
	cout<<argv[0]<<endl;
	cout <<argv[1]<<endl;
	cout <<argv[2]<<endl;
	cout<<argv[3]<<endl;
	ifstream f;
	cout <<"read file : "<<endl;
	f.open(argv[2]);
	char buff[4096]={0,};
	char* tok;
	
	while(f.getline(buff,4096)){
		cout << buff<<endl;
		tok = strtok(buff, " \n\t");
		cout <<tok <<endl;
		
		while(tok = strtok(NULL," \n\t")){
			cout <<tok<<endl;
		}
		
	}
	f.close();

	cout << "hello world"<<endl;
	return 0;	
}
