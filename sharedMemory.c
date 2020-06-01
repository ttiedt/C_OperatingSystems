/*
Class: CPSC 346-02
Team Member 1: Tyler Tiedt
Team Member 2: N/A 
GU Username of project lead: ttiedt
Pgm Name: proj5.c
Pgm Desc: Shared memory
Usage: ./a.out time_parent time_child time_parent_non_cs time_child_non_cs
*/

#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void parent(char*, int, int);
void child(char*, int, int);
void cs(char*, int);
void non_cs(int);

void main(int argc, char* argv[]){
 
  char*  process;
  int    shmid, time_child, time_child_non_cs, time_parent, time_parent_non_cs; 

  //create a shared memeory segment
  shmid = shmget(0,1,0777 | IPC_CREAT);

  //attach it to the process, cast its address, and 
  process = (char*)shmat(shmid,0,0); 

  //initialize it to 'p'
  *process = 'p';
  printf("Initial value of shared memory segment: %c.\n",*process);

  //check for proper arguments
  if(argc != 5){
    printf("Error: Not enough arguments\n");
    time_child = 1;//argv[2];
    time_child_non_cs = 2;//argv[4];
    time_parent = 2;//argv[1];
    time_parent_non_cs = 2;//argv[3];
  }
  else{
    time_child = atoi(argv[2]);
    time_child_non_cs = atoi(argv[4]);
    time_parent = atoi(argv[1]);
    time_parent_non_cs = atoi(argv[3]);
  }
  //fork here
  if (fork() == 0)
    child(process, time_child, time_child_non_cs);
  else 
    parent(process, time_parent, time_parent_non_cs);
  //remove it 
  shmctl(shmid,IPC_RMID,0);
  // we wait for all process to finish
  sleep(2);
}

void parent(char* process, int time_crit_sect, int time_non_crit_sect){
  int par;
  for (int i = 0; i < 10; i++){
    par = 1;
    //protect this
    while(*process == 'p' && par == 1){
      cs(process, time_crit_sect);
      par = 0;
    }
      non_cs(time_non_crit_sect); 
  }
  //detach shared memory from the process
  shmdt(process);
}

void child(char* process, int time_crit_sect, int time_non_crit_sect){
  int ch;
  for (int i = 0; i < 10; i++){
    ch = 1;
    //protect this
    while(*process == 'c' && ch == 1){ 
      cs(process, time_crit_sect);
      ch = 0;
    }
      non_cs(time_non_crit_sect); 
  }
  //detach shared memory from the process
  shmdt(process); 
}

void cs(char* process, int time_crit_sect){
  // we run this if the it parents turn in cs
  if (*process == 'p'){
    printf("parent in critical sction\n");
    sleep(time_crit_sect);
    printf("parent leaving critical section\n");
    // parent is done is cs, it is now the childs turn
    *process = 'c';
  }
  // we run this if the it child turn in cs
  else{
    printf("child in critical section\n");
    sleep(time_crit_sect);
    printf("child leaving critical section\n");
    // Child is done is cs, it is now the parents turn
    *process = 'p';
  }
}

void non_cs(int time_non_crit_sect){
  sleep(time_non_crit_sect);
}

