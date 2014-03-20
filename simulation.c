/****************************************************
This program is written to implement virtual memory.
We choose Round Robin CPU scheduling algorithm.
And there are three page replacement algorithms, FIFO,
LRU and second-chance algorithms.
*****************************************************/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstring>
#include <queue>
#include <stack>
#include <stdlib.h>
#include <iomanip>
using namespace std;

struct mem_final{
	string addr;
	int pro;
};
ofstream out("out4");
mem_final add_final;
int gao = 0;
long long total_pages = 0;
char c;
double current_time = 0;
double remain_time[1000];
double arrive_time[1000];
char process[1000][1000];
char *tmp1;
int sc_memory[1000];
long long process_pagefault[1000];
double elapsed_time[1000];
const char* tail = ".mem";
long long quantum;
double total_idletime = 0;
int count[1000];

queue<int> wait_queue;
queue<int> block_queue;
queue<int> LRU;
stack<int> mystack;

ifstream in_process[100];
string pre_test;
int process_no = 0;
int pointer = 0;
char* s;
const char* name;
string add;
//string memory[10000];
mem_final memory[10000];
int fourth;
int pages;
int total_cycles = 0;
int cycles = 0;
long long pagefault = 0;
long long ref = 0;
long long context_switch = 0;
int process_count[1000];

//modify function is for LRU
void modify(int number)
{
	queue<int> myqueue;	
	while (!LRU.empty())
	{
		int tmp = LRU.front();
		LRU.pop();
		if (tmp == number) continue;
		myqueue.push(tmp);
	}
	while (!myqueue.empty())
	{
		int tmp = myqueue.front();
		myqueue.pop();
		LRU.push(tmp);
	}
	LRU.push(number);
	return ;
}

//FIFO page replacement algorithm
void FIFO(mem_final address)
{
	if (pagefault <= pages)
	memory[pagefault - 1] = address;
	else 
	{
		memory[ref] = address;
		ref = (ref + 1) % pages;
	}
	return ;
}

//least recently used page replacement algorithm
void LRUA(mem_final address)
{
	if (pagefault <= pages)
	{
		memory[pagefault - 1] = address;
		LRU.push(pagefault - 1);
	}	
	else
	{
		int tmp = LRU.front();
		memory[tmp] = address;
		modify(tmp);
	}
	return ;
}

//second chance page replacement algorithm
void SC(mem_final address)
{
	if (pagefault <= pages)
	{
		memory[pagefault - 1] = address;
		sc_memory[pagefault - 1] = 1;
		ref = (ref + 1) % pages;
	}
	else 
	{
		while (true)
		{
			if (sc_memory[ref] == 1)
			{
				sc_memory[ref] = 0;
				ref = (ref + 1) % pages;
				continue;
			}
			else if (sc_memory[ref] == 0)
			{
				memory[ref] = address;
				sc_memory[ref] = 1;
				ref = (ref + 1) % pages;
				break;
			}
		}
	}
	return ;
}

//the modified second chance page replacement algorithm
void SC1(mem_final address)
{
	if (pagefault <= pages)
	{
		memory[pagefault - 1] = address;
		sc_memory[pagefault - 1] = 1;
		ref = (ref + 1) % pages;
	}
	else 
	{
		while (true)
		{
			if (sc_memory[ref] == 1)
			{
				sc_memory[ref] = 0;
				ref = (ref + 1) % pages;
				continue;
			}
			else if (sc_memory[ref] == 0)
			{
				memory[ref] = address;
				sc_memory[ref] = 0;
				ref = (ref + 1) % pages;
				break;
			}
		}
	}
	return ;
}

// the optimal one process page replacement algorithm 
void OPTIMAL(int p,int count,mem_final address)
{
	long long memory_arrive[1000];
	ref = 0;
	if (pagefault <= pages)
	{
		memory[pagefault - 1] = address;
	}
	else
	{
		name = process[pointer];		
		ifstream optimal_test(name);
		long long count_no = 0;
		while (optimal_test>>pre_test)
		{
			if(count_no < count) continue;
			else 
			{
				for (int j = 0;j < pages;++j)
				{
					if (memory[j].addr == pre_test)
					{
						if (memory_arrive[j] == 0 || memory_arrive[j] > count_no)
						memory_arrive[j] = count_no;
					}
				}
			}
			++count_no;
		}
		for (int k = 0;k < pages;++k)
		{
			if (memory_arrive[k] == 0)
			{
				ref = k;
				break;
			}
			else 
			{
				if (memory_arrive[k] > memory_arrive[ref])
				ref = k;
			}
		}
		memory[ref] = address;
	} 
	return;
}




//most frequently used page replacement algorithm
void MFU(mem_final address)
{
	if (pagefault <= pages)
	{
		memory[pagefault - 1] = address;
		++count[pagefault - 1];
		ref = (ref + 1) % pages;
	}
	else
	{
		for (int i = 0;i < pages;++i)
		{
			if (count[i] > count[ref]) ref = i;
		}
		memory[ref] = address;
		count[ref] = 1;
	}
	return ;
}

//least frequently used page replacement algorithm
void LFU(mem_final address)
{
	if (pagefault <= pages)
	{
		memory[pagefault - 1] = address;
		++count[pagefault - 1];
		ref = (ref + 1) % pages;
	}
	else 
	{
		for (int i = 0;i < pages;++i)
		{
			if (count[i] < count[ref]) ref = i;
		}
		memory[ref] = address;
		count[ref] = 1;
	}
	return; 
}


int main(int argc,char* argv[])
{
	pages = atoi(argv[1]);
	quantum = atoi(argv[2]);
	
	
    //deal with the schedule traces	
	ifstream in(argv[4]);	
	while (in>>process[process_no])
	{
		tmp1 = process[process_no];
		tmp1 = strcat(tmp1,tail);	
		in>>arrive_time[process_no]>>remain_time[process_no];
		++process_no;
		in>>fourth;
	}
	in.close();
	
	//push all the process into the wait queue
	for (int i = 0;i < process_no;++i)
	{
		wait_queue.push(i);
		name = process[i];
		in_process[i].open(name);
		
	}
		
	cout<<"the program begins running"<<endl;

	//there will be a loop until all the process is dealt with
	while(true)
	{		
		cycles = 0;		
		//when there is no process still need to be operating
		if (wait_queue.empty() && block_queue.empty()) break; 
			
	
		int pointer1,pointer2;
		if (wait_queue.empty())
		{
			pointer = block_queue.front();
			block_queue.pop();	
		}
		else if (block_queue.empty())
		{
			pointer = wait_queue.front();
			wait_queue.pop();
		}
		else //choose the process which comes ealier
		{
			pointer1 = wait_queue.front();
			pointer2 = block_queue.front();
			if (arrive_time[pointer1] < arrive_time[pointer2])
			{
				pointer = pointer1;
				wait_queue.pop();
			}
			else 
			{
				pointer = pointer2;
				block_queue.pop();
			}
		}	
				
		
		if (current_time < arrive_time[pointer])
		{
			total_idletime += (arrive_time[pointer] - current_time);
			current_time = arrive_time[pointer];
		}
		while (true)
		{					
			if ((in_process[pointer]>>add) == false)	//means the process has been done		
			{
				elapsed_time[pointer] = current_time;
				break;
			}	
			++process_count[pointer];				
			++total_pages;		
			++cycles;
			current_time += (double)1/100000.0;
			bool flag = false;
			for (int j = 0;j < pages;++j)
			if (memory[j].addr == add && memory[j].pro == pointer)
			{
								
				flag = true;
				sc_memory[j] = 1;
				count[j]++;
				if(strcmp(argv[3],"option1") == 0)
				ref = (j + 1) % pages;
				modify(j);			
				break;
				
			}
			
			if (flag == true)  //page hit
			{				
				if (cycles == quantum)  //to see whether the process has run up its time slice
				{					
					remain_time[pointer] -= quantum/100000.0;
					current_time += float(50)/100000.0;
					arrive_time[pointer] = current_time;
					wait_queue.push(pointer);					
					++context_switch;					
					break;
				}	
				else continue;
			}
			else if (flag == false)  //page fault
			{
				++pagefault;	
				++process_pagefault[pointer];
						
				if (block_queue.empty())
				arrive_time[pointer] = current_time + (float)(1000)/100000.0;
				else
				{
					int tmp = block_queue.back();
					if (arrive_time[tmp] <= current_time)
					arrive_time[pointer] = current_time + (float)(1000)/100000.0;
					else
					arrive_time[pointer] = arrive_time[tmp] + (float)1000/100000.0;
				}				
				
				block_queue.push(pointer);

				add_final.addr = add;
				add_final.pro = pointer;
				
				if (strcmp(argv[3],"fifo") == 0)
				FIFO(add_final);
				else if(strcmp(argv[3],"lru") == 0)
				LRUA(add_final);
				else if(strcmp(argv[3],"2ch-alg") == 0)
				SC(add_final);
				else if(strcmp(argv[3],"option1") == 0)
				SC(add_final);
				else if(strcmp(argv[3],"option2") == 0)
				MFU(add_final);
				else if(strcmp(argv[3],"option3") == 0)
				LFU(add_final);
				else if(strcmp(argv[3],"option4") == 0)
				SC1(add_final);
				else if(strcmp(argv[4],"option5") == 0)
				OPTIMAL(pointer,process_count[pointer],add_final);
				else 
				cout<<"there is something wrong!"<<endl;

				current_time += float(50)/100000.0;
				remain_time[pointer] -= (float)cycles/100000.0;	

				++context_switch;
				break;
			}
			//current_time += float(cycles)/100000.0;	
			
		}		
			
			
	}

	for (int i = 0;i < process_no;++i) //close the files and print the desired output
	{
		in_process[i].close();
		cout<<process[i]<<"'s page fault is "<<process_pagefault[i]<<" and the elapsed time is "<<elapsed_time[i]<<endl;
	}
	cout<<"total elapsed time is "<<current_time<<endl;
	cout<<"total pagefault is "<<pagefault<<endl;
	
	return 0;

}



