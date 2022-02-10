#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
using namespace std;

int flag = 1; //for reading new token, flag to getline()
char buff[4096]={0,};
int linenum; //the current line number of getToken()
int lineoffset; //the current offset of getToken()
int module_base; // current address // running sum of modules
int last_line_len; //the length of the last line read
ifstream f;
vector<pair<string,bool>> symbol_used;
const int DEF_LIMIT=16;
const int BUFF_SIZE=4096;
const char* DELIMITER=" \t\n";

struct symbol{
	string name;
	int address;
	bool mul; //multiple times defined
	int n; //n-th defined
	int mod_num;
};
map<string, symbol> sym_table;


void print_error(int errcode, char* c="null")
void parse_error(int errorcode);
char* getToken();
void isInt(char *s);
void isSymbol(char *s);
void checkSymLen(char *s);
char* getDef(int& sym_num,int mod_num);
void getUse();
void isIEAR(char *s);
void getIns(int mod_num, char* symbol=NULL);
void passOne();
void readOp(char* a, int mem_map_cnt, int op, vector<pair<char*,bool>>& use_list, int insCount);
void readIns(int& mem_map_cnt, vector<pair<char*,bool>>& use_list);
void print_never_used_usedlist_warnings(int module_num, vector<pair<char*,bool>>& use_list);
void print_never_used_def_warnings();
void passTwo();
void printSymbolTable();
void init_vars();

void print_error(int errcode, char* c="null"){
	static char* errstr[] = {
		"Error: Absolute address exceeds machine size; zero used",
		"Error: Relative address exceeds module size; zero used",
		"Error: External address exceeds length of uselist; treated as immediate",
		"is not defined; zero used",
		"Error: This variable is multiple times defined; first value used",
		"Error: Illegal immediate value; treated as 9999",
		"Error: Illegal opcode; treated as 9999",
	};
	if(errcode == 3)printf("Error: %s %s",c,errstr[errcode]);
	else printf("%s",errstr[errcode]);
}
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
	if(flag==0){
		tok = strtok(NULL, DELIMITER); 
		if(tok==NULL) flag = 1;
	}
	if(flag==1){
		do{
		f.getline(buff,BUFF_SIZE);
		//printf("read line : %s\n", buff);
		if(!f.eof()) last_line_len = strlen(buff) + 1;
		tok = strtok(buff,DELIMITER);
		flag = 0; 
		if(!f.eof()) linenum++;
		}while(tok == NULL && !f.eof());
		
	}
	if(f.eof())lineoffset = last_line_len;
	else lineoffset = tok - buff + 1;
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
char* getDef(int& sym_num,int mod_num){
	//def list
	//def count
	char *defCount_s = getToken();
	if(f.eof()) return NULL; // end of file
	isInt(defCount_s);
	int defCount = stoi(defCount_s);
	if(defCount > DEF_LIMIT){
		parse_error(4);
		exit(1);
	}
	//loop defs
	char* symbol=NULL;	
	for(int i=0;i<defCount;i++){
		symbol = getToken();
		isSymbol(symbol);

		char *addr_s = getToken();
		isInt(addr_s);
		int addr = stoi(addr_s);

		if(sym_table.find(symbol) != sym_table.end())
			sym_table[symbol].mul = true;

		else {  
			struct symbol s = {symbol, addr+module_base,false,sym_num,mod_num};
			sym_table.insert({symbol,s});
			sym_num++;
			char* name_cpy = (char*)malloc(strlen(symbol)+1);
			strcpy(name_cpy,symbol);
			symbol_used.push_back({name_cpy,false});
		}
	}
	return symbol;
}
void getUse(){
	//use list
	//use count
	char *useCount_s = getToken();
	isInt(useCount_s);
	int useCount = stoi(useCount_s);
	
	if(useCount > DEF_LIMIT){
		parse_error(5);
		exit(1);
	}
	//loop use
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
void getIns(int mod_num, char* symbol=NULL){
	char *insCount_s = getToken();
	isInt(insCount_s);
	int insCount = stoi(insCount_s);
	if(insCount >= 511){
		parse_error(6);
		exit(1);
	}
	if(symbol){
		int sym_addr = sym_table[symbol].address - module_base;
		if(symbol && sym_addr > insCount-1){
		printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n",mod_num,symbol,sym_addr,insCount-1);
		sym_table[symbol].address-=sym_addr;
		}
	}
	
	module_base += insCount;
	for(int i=0;i<insCount;i++){
		char * ins = getToken();
		isIEAR(ins);
		char * op = getToken();
		isInt(op);
	}
}
void passOne(){
	int sym_num = 0;
	int mod_num = 1;
	while(true){
		char* s=NULL;
		char* s_temp = getDef(sym_num,mod_num);
		if(s_temp){
			s = (char*)malloc(strlen(s_temp)+1);
			strcpy(s,s_temp);
		}
		if(f.eof())break;
		getUse();
		getIns(mod_num,s);
		mod_num++;
	}
	return;
}
void readOp(char* a, int mem_map_cnt, int op, vector<pair<char*,bool>>& use_list, int insCount){
	int op_after;
	int opcode = op/1000;
	int operand = op%1000;
	int error_code = -1;
	char* error_s="null";
	switch(*a){
		case 'R' :
			if(insCount <= operand){
				error_code = 1;
				op_after = opcode * 1000 + module_base;
			}
			else op_after = opcode * 1000 + operand + module_base;
		       	break;
		case 'A' :
			if(operand >= 512){
				error_code = 0;
				op_after = opcode * 1000;
			}
			else op_after = op;
			break;
		case 'E' :
			char* var;
			int global_address;
			int temp;
			if(operand < use_list.size()){ 
				var = use_list[operand].first;
				use_list[operand].second = true;
			}	
			else{
				//exceeds length of uselist
				error_code = 2;
				op_after = op;
				break;
			}
			if(sym_table.find(var) != sym_table.end()){
				global_address = sym_table[var].address;
				int temp = sym_table[var].n;
				symbol_used[temp].second = true;
			}
			else{
				//var is not defined
				error_code = 3;
				error_s = var;
				op_after = opcode * 1000;
				break;
			}
			op_after = opcode * 1000 + global_address;
			break;
		case 'I' : 
			if(op>9999){
				error_code = 5;
				op_after = 9999;
			}
			else op_after = op;
			break;
	}
	if(opcode >= 10 && error_code == -1){
		error_code = 6;
		op_after = 9999;
	}
	printf("%03d: %04d ", mem_map_cnt, op_after);
	if(error_code!=-1) print_error(error_code,error_s);
	cout<<endl;
	return;
}
void readIns(int& mem_map_cnt, vector<pair<char*,bool>>& use_list){
	char * insCount_s = getToken();
	int insCount = stoi(insCount_s);
	for(int i=0;i<insCount;i++){
		char* addressing = getToken();
		char* addr = (char*)malloc(strlen(addressing)+1);
		strcpy(addr, addressing);
		int op = stoi(getToken());
		readOp(addr, mem_map_cnt, op, use_list, insCount);
		mem_map_cnt++;
	}
	module_base += insCount;
}
void print_never_used_usedlist_warnings(int module_num, vector<pair<char*,bool>>& use_list){
	for(int i=0;i<use_list.size();i++){
		if(use_list[i].second==false)
			printf("Warning: Module %d: %s appeared in the uselist but was not actually used\n",module_num,use_list[i].first);
	}
}
void print_never_used_def_warnings(){
	cout<<endl;
	for(int i=0;i<symbol_used.size();i++){
		if(symbol_used[i].second== false){
			string sn = symbol_used[i].first;
			int mn = sym_table[sn].mod_num;
			cout<< "Warning: Module "<< mn <<": " <<sn;
			cout<< " was defined but never used"<<endl;
  						
		}
	}

}
void passTwo(){
	int mem_map_cnt = 0; // to print the cnt number thing in the memory map
	cout << "\nMemory Map" << endl;
	int module_num = 1;

	while(true){
		//def
		char * defCount_s = getToken();
		if(f.eof())break;
		int defCount = stoi(defCount_s);
		for(int i=0;i<defCount;i++){
			getToken();
			getToken();
		}

		//use
		vector<pair<char*,bool>> use_list;
		char * useCount_s = getToken();
		int useCount = stoi(useCount_s);
		for(int i=0;i<useCount;i++){
			char* use_var = getToken();
			char* use_var_cpy = (char*)malloc(strlen(use_var)+1);
			strcpy(use_var_cpy,use_var);
			use_list.push_back({use_var_cpy,false});
		}

		//instruction
		readIns(mem_map_cnt, use_list);
		print_never_used_usedlist_warnings(module_num,use_list);
		
		module_num++;
	}
	print_never_used_def_warnings();
}
void printSymbolTable(){
	cout<<"Symbol Table\n";	
	for(auto const& x : sym_table){
		cout << x.second.name<<"="<<x.second.address;
		if(x.second.mul) 
			cout << " Error: This variable is multiple times defined; first value used"<<endl;
		else cout <<endl;
	}
}
void init_vars(){
	module_base = 0;
	flag = 1;
	linenum = 0;
	lineoffset = 0;
}
int main(int argc, char *argv[]){
	f.open(argv[1]);
	passOne();
	f.close();
	
	printSymbolTable();
	init_vars();

	f.open(argv[1]);
	passTwo();
	f.close();

	return 0;
}
//when i try to print the null pointer the program seems to stop printing
