#include <iostream>
#include <fstream>
#include <stdio.h>
#include <getopt.h>
#include <vector>
#include <deque>
#include <map>
#include <cstring>
#include "pte.h"
#include "pager.h"
#include "fte.h"
#include "proc.h"
using namespace std;
int MAX_FRAMES;
typedef struct vma{
	int pid;
	int sv;
	int ev;
	int wp;
	int fp;
};
vector<struct proc> process;
vector<vma> vmas;
vector<fte_t> frametable;
deque<int> free_pool;
struct proc *cur_proc;
int prc_cnt;
Pager* algo;
unsigned long long cost;
unsigned long inst_cnt, ctx_switches, process_exits;
vma* verify(int pid, int n);
bool O=false, P=false, F=false, S=false;
void print_pagetable(){
	for(int i=0;i<process.size();i++){
		pte_t *pte = process[i].pagetable;
		printf("PT[%d]: ",i);
		for(int j=0;j<64;j++){
			if(verify(i,j)==NULL)printf("*");
			else{
				if(pte[j].present == 0){
				       	if(pte[j].pagedout==1&&pte[j].file_mapped==0) printf("#");
					else printf("*");
				}
				else{
					printf("%d:",j);
					if(pte[j].referenced) cout <<"R";
					else cout <<"-";
					if(pte[j].modified) cout << "M";
					else cout <<"-";
					if(pte[j].pagedout&&pte[j].file_mapped==0) cout<<"S";
					else cout <<"-";
				}
			}
			if(j!=63)printf(" ");
		}
		printf("\n");
		

	}
}
void print_frametable(){
	printf("FT: ");
	for(int i=0;i<frametable.size();i++){
		if(frametable[i].proc_num == -1) cout << "*";
		else printf("%d:%d",frametable[i].proc_num, frametable[i].page_num);
	       if(i<frametable.size()-1) cout<<" ";	
	}
	cout << endl;
}
void print_statistics(){
	for(int i=0;i<process.size();i++){
		printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
				process[i].pid, process[i].unmaps, process[i].maps, process[i].ins, process[i].outs, 
			       process[i].fins, process[i].fouts, process[i].zeros, process[i].segv, process[i].segprot);	
	}
	printf("TOTALCOST %lu %lu %lu %llu %lu\n",
			inst_cnt, ctx_switches, process_exits, cost, sizeof(pte_t));
}
void init(){
	//init free frames
	for(int i=0;i<MAX_FRAMES;i++){
		fte_t fte ={-1,-1,0};
		frametable.push_back(fte);
		free_pool.push_back(i);
	}
	//init pte
	for(int i=0;i<prc_cnt;i++){
		struct proc prc;
		for(int j=0;j<MAX_VPAGES;j++){
			struct pte_t pte = {0,0,0,0,0};
			prc.pid = i;
			prc.pagetable[j] = pte;
			prc.unmaps=0;
			prc.maps=0;
			prc.ins=0;
			prc.outs=0;
			prc.fins=0;
			prc.fouts=0;
			prc.zeros=0;
			prc.segv=0;
			prc.segprot=0;
		}
		process.push_back(prc);
	}
}
int get_frame(){
	int idx;
	if (!free_pool.empty()){
		idx = free_pool.front();
		free_pool.pop_front();
	}
	//else page fault
	else
		idx = algo->select_victim_frame(frametable,process);
	return idx;
}
vma* verify(int pid, int n){
	for(int i=0;i<vmas.size();i++){
		if(vmas[i].pid==pid){
			if(vmas[i].sv <= n && vmas[i].ev >=n)return &vmas[i];
		}
	}
	return NULL;
}
int unmap(int pid, int pte_index, bool exit){
	pte_t *pagetable = process[pid].pagetable;
	if(pagetable[pte_index].present==0){
	       	if(exit)pagetable[pte_index].pagedout=0;
		return -1;
	}
	pagetable[pte_index].present=0;
	pagetable[pte_index].referenced=0;
        if(O) printf(" UNMAP %d:%d\n", pid, pte_index);
	cost += 400;
	process[pid].unmaps++;
        if(pagetable[pte_index].modified==1){
        	pagetable[pte_index].modified=0;
                pagetable[pte_index].pagedout=1;
		if(pagetable[pte_index].file_mapped==1){
			if(O)printf(" FOUT\n");
			cost += 2400;
			process[pid].fouts++;
		}
		else if(!exit){
			if(O)printf( " OUT\n");
			cost += 2700;
			process[pid].outs++;
		}
        }
	if(exit){
                pagetable[pte_index].referenced=0;
                pagetable[pte_index].modified=0;
                pagetable[pte_index].pagedout=0;
        }

	return pagetable[pte_index].frame_num;
}

int main(int argc, char *argv[]){
	extern char *optarg;
	char c;
	while((c=getopt(argc,argv,"a:o:f:")) != -1){
		switch(c){
			case 'a':
				if (optarg[0] == 'f') algo = new FIFO();
				else if(optarg[0] =='r') algo = new Random(argv[argc-1]);
				else if(optarg[0] =='c') algo = new Clock();
				else if(optarg[0] =='e') algo = new NRU();
				else if(optarg[0] =='a') algo = new Aging();
				else if(optarg[0] =='w') algo = new WorkingSet();
				break;
			case 'f':
				sscanf(optarg, "%d", &MAX_FRAMES);
				break;
			case 'o':
				for(int i=0;i<strlen(optarg);i++){
					if(optarg[i] == 'O') O=true;
					else if(optarg[i]=='P') P=true;
					else if(optarg[i]=='F') F=true;
					else if(optarg[i]=='S') S=true;
				}
				break;
		}
	}
	ifstream f;
	f.open(argv[argc-2]);
	string line;
	int num_process=-1,num_vmas=0;
	while(getline(f, line)){
		if(line[0] == '#' || line.length() == 0)continue;
		if(num_process==-1)
			sscanf(line.c_str(),"%d", &num_process);
		else if(num_process>0 && num_vmas==0){
			sscanf(line.c_str(),"%d", &num_vmas);
			num_process--;
			prc_cnt++;
		}
		else if(num_vmas>0){
			int sv, ev, wp, fm;
			sscanf(line.c_str(), "%d %d %d %d", &sv, &ev, &wp, &fm);
			num_vmas--;
			vma temp = {prc_cnt-1, sv, ev, wp, fm};
			vmas.push_back(temp);
		}
		if(num_process==0 && num_vmas==0)break;
	}
	init();
	int cnt = 0;
	while(getline(f,line)){
		if(line[0] == '#' || line.length() == 0)continue;
		char ins;
		int n;
		int frame_index;
		sscanf(line.c_str(), "%c %d", &ins, &n);
		if(O) printf("%d: ==> %c %d\n",cnt,ins,n);
		cnt++;
		algo->ins_cnt++;
		inst_cnt++;
		if(ins=='c'){
			cur_proc = &process[n];
			ctx_switches++;
			cost+=130;
		}
		else if(ins=='e'){
			printf("EXIT current process %d\n",cur_proc->pid);
			for(int i=0;i<64;i++){
				//unmap the files
				int frame_num = unmap(cur_proc->pid,i,true);
				if(frame_num==-1) continue;
				//return frames
                		free_pool.push_back(frame_num);
				frametable[frame_num].page_num=-1;
				frametable[frame_num].proc_num=-1;
				frametable[frame_num].age=0;
			}
			process_exits++;
			cost+=1250;
		}
		else{
			cost++;
			pte_t *pagetable = cur_proc->pagetable;
			if(pagetable[n].present==0){
				vma* vpage = verify(cur_proc->pid,n);
				if(vpage==NULL){
					cout << " SEGV" <<endl;;
					cost += 340;
					process[cur_proc->pid].segv++;
				       	continue;
				}
				//verify if this page is in vma or not
				frame_index = get_frame();
				
				int pc_index=frametable[frame_index].proc_num;
				int pt_index=frametable[frame_index].page_num;
				
				//unmap old frame
				if(pt_index!=-1){
					unmap(pc_index,pt_index,false);
				}
				//map new frame
				pagetable[n].frame_num = frame_index;
                                pagetable[n].present = 1;
                                pagetable[n].write_protect = vpage->wp;
                                pagetable[n].file_mapped = vpage->fp;
                                frametable[frame_index].page_num = n;
                                frametable[frame_index].proc_num = cur_proc->pid;
                                
				if(pagetable[n].file_mapped==1){
					if(O)printf(" FIN\n");
					cost+=2800;
					process[cur_proc->pid].fins++;
				}
				else{
					if(pagetable[n].pagedout==1){
					       	if(O)printf(" IN\n");
						cost+=3100;
						process[cur_proc->pid].ins++;
					}
					else {
						if(O)printf(" ZERO\n");
						cost+=140;
						process[cur_proc->pid].zeros++;
					}
				}
				printf(" MAP %d\n",frame_index);
				cost += 300;
				process[cur_proc->pid].maps++;
				
			}
			
			//proceed instruction
			pagetable[n].referenced = 1;
			if(ins=='w'){
				if(pagetable[n].write_protect==0)pagetable[n].modified = 1;
				else{
				       	cout<<" SEGPROT"<<endl;
					cost += 420;
					process[cur_proc->pid].segprot++;
				}
			}
		}
	}
	f.close();
	if(P)print_pagetable();
	if(F)print_frametable();
	if(S)print_statistics();

	return 0;
}
