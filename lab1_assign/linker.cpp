#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <ctype.h>
#include <string.h>
using namespace std;

int flag = 1;
char buff[512]={0,};
int linenum;
int lineoffset;
int cur_pointer;
ifstream f;
map<string,int> table;

void parse_error(int errorcode);
char* getToken();
int readInt();
bool isInt(char *s);

void parse_error(int errcode){
	static char* errstr[] = {
		"NUM_EXPECTED",
		"SYM_EXPECTED",
		"ADDR_EXPECTED",
		"SYM_TOO_LONG",
		"TOO_MANY_DEF_IN_MODULE",
		"TOO_MANY_USE_IN_MODULE",
		"TOO_MANY_INSTR",
	};
	printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
}
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
		linenum++;
		cout << "read line :" << buff <<"\n";
		tok = strtok(buff,delim);
		cout << "tok is : " << tok << "\n";
		flag = 0;
	} 
	if(tok!=NULL) lineoffset = tok - buff;
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
	if(i>9999) return 9999;
	return i;
}
int isInt(){
	int i = 0;
	char *s = getToken();
	if(s==NULL) return -1;
	while(*s){
		if(*s >= '0' && *s <= '9'){
			i = i*10 + (*s - '0');
			s++;
		}
		else {
			parse_error(0);
			exit(1);
		}
	}
	cout << "isInt : " << i <<"\n";
	if(i>9999) return 9999;	
	return i;
}
int checkEOF(int i){ 
	if(i==-1) {
		flag=-1;
		return -1; // NULL
	}	
	return i;
}
bool isSym(char *s){
	if(s == NULL) return false;
	if(!isalpha(*s)) return false;
	while(*s){
		if(!isalpha(*s) && !isdigit(*s)) return false;
		s++;
	}
	return true;
}
void readSym(){
	char *s = getToken();
	if(!isSym(s)){
		parse_error(1);	
		exit(1);
	}
	if( strlen(s) > 16 ){
		parse_error(3);
		exit(1);
	} 
	cout <<"\nSymbol : " << s << "offset : " << s-buff<<"\n";
	table.insert({s,lineoffset + cur_pointer});
}
void getDef(){
	int defCount = isInt();
	if(checkEOF(defCount)==-1) return;
	cout<<"defcount :" <<defCount<<"\n";
	for(int i=0;i<defCount;i++){
		readSym();
		//cout << "\tSymbol :" << getToken();//Symbol sym = readSym();
		cout << "\tval :"<< isInt();//int val = readInt();
		cout <<"\n";
	}
	cout <<"\n";
}
void getUse(){
	int useCount = isInt();
	cout << "useCount :" << useCount<<"\n";
	for(int i=0;i<useCount;i++){
		readSym();
		cout <<"\n";
	}
	cout <<"\n";
}
void isIEAR(){
	char *s = getToken();
	if( (*s != 'I' && *s != 'E' && *s!= 'A' && *s!= 'R') || strlen(s)>1 ){
		parse_error(2);
		exit(1);
	}
	cout << s << strlen(s) ;
}
void getIns(){
	int insCount = isInt();
	if(insCount >= 512-1){
		parse_error(6);
		exit(1);
	}
	cur_pointer += insCount;
	cout << "insCount :"<< insCount<<"\n";
	for(int i=0;i<insCount;i++){
		isIEAR();
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
	for(auto const& x : table){
		cout<<x.first<<"="<<x.second<<"\n";
	}
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
