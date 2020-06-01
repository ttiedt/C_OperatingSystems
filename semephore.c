/*
Class: CPSC 346-02
Team Member 1: Tyler Tiedt
Team Member 2: N/A 
GU Username of project lead: ttiedt
Pgm Name: Project 6
Pgm Desc: Producer consumer problem and semaphores
Usage: ./a.out
NOTE: This runs fine on my laptop. But I ran it on my desk to
      which  has a diffent processor. About every 3rd run the final 
      values would be diffent from the initial. I think this is due to 
      different processors.
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_MUTEX 1 
#define INIT_EMPTY 100 
#define INIT_FULL  0 
#define EMPTY_ARG  0 //used when argument is optional 

/*
semop sets a semaphore up or down using a struct from sem.h 

  struct sembuf 
  {
    short sem_num;
    short sem_op;  -1 is down, 1 is up
    short sem_flg;
  }
*/
typedef struct sembuf sem_struct;  //give it a more reasonable name

//prototypes for functions used to declutter main 
void set_sem_values(int,int,int);
void get_sem_values(int,int,int);
void set_sem_struct(sem_struct[],int);
int create_semaphore(int);
void cleanup(int, int, int);
void producer(int, int, int, sem_struct[], sem_struct[]);
void consumer(int, int, int, sem_struct[], sem_struct[]);
void cs(int);

int main(int argc, char* argv[])
{
 sem_struct s_wait[1], s_signal[1]; //create semaphore structs
 int mutex, empty, full;   //will hold semaphore identifiers 
 int value, status, ch_stat, i;

 //set wait/signal structs used in semop 
 set_sem_struct(s_wait,-1);
 set_sem_struct(s_signal,1);
 
 //create semaphore sets using arbitrary int unique to the semaphore set. 
 mutex = create_semaphore(0);
 empty = create_semaphore(1);
 full = create_semaphore(2);

 //set semaphores to initial values
 set_sem_values(mutex, empty, full);

 printf("Intial semaphore values\n");
 get_sem_values(mutex, empty, full);
 printf("\n");
 //fork off a process
 if ((value = fork()) < 0)
  printf("error: fork\n");
 else{
  if (value == 0){ 
    consumer(mutex, empty, full, s_wait, s_signal); // Child = Consumer
  }
  else{ 
    producer(mutex, empty, full, s_wait, s_signal); // Parent = Producer
    status = wait(&ch_stat);
  }
 }
/*
 if (value == 0)
   {
    printf("\nFinal semaphore values\n");
    get_sem_values(mutex, empty, full); 
   }
 return 0;
*/
  if(value == 0){
    usleep(2000);
    printf("\nFinal semaphore values\n");
    get_sem_values(mutex, empty, full); 
    cleanup(mutex,empty,full); //remove semaphores
  }
  return 0;
}

//create a semaphore set of 1 semaphore specified by key 
int create_semaphore(int key)
{
 int new_sem;
 if ((new_sem = semget(key, 1, 0777 | IPC_CREAT)) == -1)
   {
    perror("semget failed");
    exit(1);
   }
 return new_sem;
}

//set struct values for down/up ops to be used in semop
//By design, we will always refer to the 0th semaphore in the set
void set_sem_struct(sem_struct sem[], int op_val)
{
 sem[0].sem_num = 0;
 sem[0].sem_op = op_val;
 sem[0].sem_flg = SEM_UNDO;
} 

//set mutex, empty, full to initial values
//these semaphore sets consist of a single semaphore, the 0th 
void set_sem_values(int mutex, int empty, int full)
{
 semctl(mutex, 0, SETVAL, INIT_MUTEX);
 semctl(empty, 0, SETVAL, INIT_EMPTY);
 semctl(full, 0, SETVAL, INIT_FULL);
}

//retrieve value held by the 0th semaphore, which is the second arg.
//0 in the final argument indicates that it is not used 
void get_sem_values(int mutex, int empty, int full)
{
 int m, e, f; 
 m = semctl(mutex, 0, GETVAL, 0);
 e = semctl(empty, 0, GETVAL, 0);
 f = semctl(full, 0, GETVAL, 0);
 printf("mutex: %d empty: %d full: %d\n", m,e,f);
}

//remove semaphores
void cleanup(int mutex, int empty, int full)
{
 semctl(mutex, 1, IPC_RMID, EMPTY_ARG);
 semctl(empty, 1, IPC_RMID, EMPTY_ARG);
 semctl(full, 1, IPC_RMID, EMPTY_ARG);
}

//Child process
void consumer(int mutex, int empty, int full, sem_struct s_wait[], sem_struct s_signal[]){
  for(int i = 0; i < 5; i++){
    printf("%d. Consumer INITAL -> ", i);
    get_sem_values(mutex, empty, full);
    usleep(1000);
    if(semctl(mutex, 0, GETVAL, 0) == 1 && !(semctl(empty, 0, GETVAL, 0) > 100)){    
      //full--
      semop(full, s_wait, 1);
      //mutex--
      semop(mutex, s_wait, 1);  
      //consumer()
      cs(1);
      //mutext++
      semop(mutex, s_signal, 1);
      //empty++
      semop(empty, s_signal, 1);
    }
    printf("%d. Consumer FINAL -> ", i);
    get_sem_values(mutex, empty, full);
    //printf("\n");
  }
}

//Parent process
void producer(int mutex, int empty, int full, sem_struct s_wait[], sem_struct s_signal[]){
  for(int i = 0; i < 5; i++){
    //printf("%d. Producer INITAL -> ", i);
    //get_sem_values(mutex, empty, full);
    usleep(250);
    if(semctl(mutex, 0, GETVAL, 0) == 1 && !(semctl(full, 0, GETVAL, 0) < 0)){   
      //empty--
      semop(empty, s_wait, 1);
      //mutex--
      semop(mutex, s_wait, 1);         
      //Producer()
      cs(0);
      //mutext++
      semop(mutex, s_signal, 1);
      //full++
      semop(full, s_signal, 1);
    }
    //printf("%d. Producer FINAL -> ", i);
    //get_sem_values(mutex, empty, full);
    printf("\n");
  }
}
// Critical  Section
void cs(int crit){
  if(crit == 0){
    printf("Producer making an item\n");
  }
  else{
    printf("Consumer consuming an item\n");
  }
}
