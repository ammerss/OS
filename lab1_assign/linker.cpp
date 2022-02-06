#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>
#include <utility>
using namespace std;

int flag = 1;
char buff[512]={0,};
int num_lines;
int cnt_lines;
ifstream f;
vector<pair<string, int> > sym_table;

char* getToken(){
	char* tok;
	const char* delim = " \n\t";
	if(flag==0){
		tok = strtok(NULL, delim); 
		if(tok==NULL) flag = 1;
	}
	if(flag==1){
		cout << "reading new line"<<"\n";
		f.getline(buff,512);
		cout << "read line :" << buff <<"\n";
		tok = strtok(buff,delim);
		flag = 0;
	} 
	return tok;
	/*
	int cnt_line = 0;
	char line[512]={0,};
	char* tok;
	int offset;
	const char* delim=" \n\t";
	while(f.getline(line,512)){
		cout<<"line"<<cnt_line<<":"<<"\n";
		cout<<line<<"\n";
		cnt_line++;
		tok = strtok(line,delim);
		while(tok!=NULL){
			offset = tok - line;
			printf("Token[%s], offset[%d], line[%d]\n",tok,offset,cnt_line);
			tok = strtok(NULL,delim);
		}
	}*/
}
int readInt(){
	int i = 0;
	char *s = getToken();
	if(s==NULL) return -1;
	while( *s >= '0' && *s <= '9'){
		i = i*10 + (*s - '0');
		s++;
	}
	return i;
}
int checkInt(int i){
	if(i==-1) return -1; // NULL
	if(i>9999) return 9999;
}
void readSym(){
	char *s = getToken();
	cout <<"\nSymbol : " << s << "offset : " << s-buff<<"\n";
	sym_table.push_back({s,s-buff});
}
void getDef(){
	int defCount = readInt();
	if(checkInt(defCount)==-1) return;
	cout<<"defcount :" <<defCount<<"\n";
	for(int i=0;i<defCount;i++){
		readSym();
		//cout << "\tSymbol :" << getToken();//Symbol sym = readSym();
		cout << "\tval :"<< readInt();//int val = readInt();
		cout <<"\n";
	}
	cout <<"\n";
}
void getUse(){
	int useCount = readInt();
	cout << "useCount :" << useCount<<"\n";
	for(int i=0;i<useCount;i++){
		cout<< "\tSymbol : " << getToken();
		cout <<"\n";
	}
	cout <<"\n";
}
void getIns(){
	int insCount = readInt();
	cout << "insCount :"<< insCount<<"\n";
	for(int i=0;i<insCount;i++){
		cout<<"\tType :" << getToken();
		cout<<"\toperand :" << getToken();
		cout <<"\n";
	}
	cout <<"\n";
}
void passOne(){
	while(f.get()!=EOF){
		getDef();
		getUse();
		getIns();
	}
}

int main(int argc, char *argv[]){
	f.open(argv[1]);
	if (f.fail()){
		cerr<<"file open error"<<"\n";
		return -1;
	}
	passOne();
	return 0;
}
