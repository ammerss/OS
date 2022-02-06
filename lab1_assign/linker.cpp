#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <utility>
using namespace std;

int flag = 1;
char buff[512]={0,};
int num_lines;
int cnt_lines;
ifstream f;
//map<string,int> table;
vector<string,int> table;
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
	if(i==-1) {
		flag=-1;
		return -1; // NULL
	}	
	if(i>9999) return 9999;
}
void readSym(){
	char *s = getToken();
	cout <<"\nSymbol : " << s << "offset : " << s-buff<<"\n";
	table.insert({s,s-buff});
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
	while(true){
		getDef();
		if(flag==-1)break;
		getUse();
		getIns();
	}
}
void printSymbolTable(){
	cout<<"Symbol Table\n";
	
	for(map<string,int> const& x : table){
		cout<<x[0]<<"\n";
	}

	/*for(int i=0;i<symbol_table.size();i++){
		cout<<symbol_table[i]<<"\n";
	}*/
}
int main(int argc, char *argv[]){
	f.open(argv[1]);
	if (f.fail()){
		cerr<<"file open error"<<"\n";
		return -1;
	}
	passOne();
	printSymbolTable();
	return 0;
}
