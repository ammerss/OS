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
int last_token_len;
ifstream f;
map<string,int> table;
const int DEF_LIMIT=16;
const int BUFF_SIZE=512;
const char* DELIMITER=" \t";
const char* eof;

void parse_error(int errorcode);
char* getToken();
void isInt(char *s);
void isSymbol(char *s);
void getDef();
void getUse();
void isIEAR(char *s);
void getIns();	
void passOne();
void checkSymLen(char *s);
	
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
		tok = strtok(NULL, DELIMITER); 
		if(tok==NULL) flag = 1;
	}
	if(flag==1){
		do{
		f.getline(buff,BUFF_SIZE);
		printf("read line : %s\n", buff);
		tok = strtok(buff,DELIMITER);
		flag = 0;
		if(tok == NULL) flag=-1;
		else linenum++;
		}while(tok == NULL && !f.eof());
		
	} 
	lineoffset = tok - buff + 1;
	return tok;
	}

void isInt(char *s){
	bool check = true;
	if(s == NULL) check = false;
	if(strlen(s)>30) check = false;
	if(check==true){
		while(*s){
			if(!(*s >= '0' && *s <= '9')){
				check = false;
				break;
			}
			s++;
		}
	}
	cout.flush();
	if(check == false){
		parse_error(0);
		exit(1);
	}
}
bool null(char *s){
	if(s==NULL) return true;       
	return false;
}

void isSymbol(char *s){
	bool check = true;
	if(s == NULL) check = false;
	else if(!isalpha(*s)) check = false;
	if(check==true){
		while(*s){
			if(!isalpha(*s) && !isdigit(*s)) check = false;
			s++;
		}
	}
	cout.flush();
	if(check==false){
		parse_error(1);
		exit(1);
	}
	checkSymLen(s);
}
void checkSymLen(char *s){
	bool check = true;
	if(strlen(s)>16) check = false;
	cout.flush();
	if(check==false){
		parse_error(3);
		exit(1);
	}
}
void getDef(){
	char *defCount_s = getToken();
	if(f.eof()) return; // end of file
	isInt(defCount_s);
	int defCount = stoi(defCount_s);
	if(defCount > DEF_LIMIT){
		parse_error(4);
		exit(1);
	}
		
	for(int i=0;i<defCount;i++){
		char *symbol = getToken();
		isSymbol(symbol);
		table.insert({symbol,lineoffset+cur_pointer});
	
		char *addr_s = getToken();
		isInt(addr_s);
	}
}
void getUse(){
	char *useCount_s = getToken();
	isInt(useCount_s);
	int useCount = stoi(useCount_s);
	
	if(useCount > DEF_LIMIT){
		parse_error(5);
		exit(1);
	}

	for(int i=0;i<useCount;i++){
		char *s = getToken();
		isSymbol(s);
	}
}
void isIEAR(char *s){
	bool check = true;
	
	if(s==NULL) check = false;
	else if(strlen(s)>1) check = false;
	else if(*s != 'I' && *s != 'E' && *s!= 'A' && *s!= 'R') check = false;
	
	cout.flush();
	if(check == false){
		parse_error(2);
		exit(1);
	}
}
void getIns(){
	char *insCount_s = getToken();
	isInt(insCount_s);
	int insCount = stoi(insCount_s);
	if(insCount >= 511){
		parse_error(6);
		exit(1);
	}
	cur_pointer += insCount;
	for(int i=0;i<insCount;i++){
		char * ins = getToken();
		isIEAR(ins);
		char * op = getToken();
		isInt(op);
	}
}
void passOne(){
	while(true){
		getDef();
		if(f.eof()){
			printf("reach eof\n");
			break;
		}
		getUse();
		getIns();
	}
	return;
}
void printSymbolTable(){
	cout<<"Symbol Table\n";	
	for(auto const& x : table){
		cout << x.first<<"="<<x.second<<endl;
	}
}
int main(int argc, char *argv[]){
	f.open(argv[1]);
	if (f.fail()){
		cerr<<"file open error"<<"\n";
		return -1;
	}
	passOne();
	f.close();
	printSymbolTable();
	return 0;
}
//when i try to print the null pointer the program seems to stop printing
