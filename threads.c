/*
Class: CPSC 346-02
Team Member 1: Tyler Tiedt
Team Member 2: N/A 
GU Username of project lead: ttiedt
Pgm Name: proj2.c
Pgm Desc: Creates 4 threads
          1st thread writes argv[1] random numbers
          the other thread read the number and determine if its prime.
Usage: ./a.out 10

NOTE: If the code looks like its hanging. Its computing the prime of a large number.
      Just give a few seconds to compute.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_THREADS 4
#define MAXSIZE 4

void* writer(void*);
void* reader(void*);
int isPrime(int);

int p[2];
// Struct to pass int arguments with pthread_create
struct arg{
  int arg1;
};

int main(int argc, char* argv[]){
  // Declare thread struct and variables
  pthread_t thread1, thread2, thread3, thread4;
  int status, prime;//, isDone;
  struct arg args, thread;
  int gen = atoi(argv[1]);
  // Can write specified numbers.
  args.arg1 = gen;
  
  
  // Start pipe.  
  if(pipe(p) == -1){
    printf("Pipe Failed\n");
    exit(0);  
  }
  // Thread1 writes
  status = pthread_create(&thread1, NULL, writer, (void *)&args);
  if (status != 0){
    printf("Error in thread 1: %d\n", status);
    exit(1);
  }
  pthread_join(thread1, NULL);// Wait to finish.
  //Thread2 3 4 read.
  for(int i = 1; i < (gen + 1); i++){
    sleep(1);
    if(i % 3 == 0){
      thread.arg1 = 4;
      status = pthread_create(&thread4, NULL, reader, (void *)&thread);
      if (status != 0){
        printf("Error in thread 4: %d\n", status);
        exit(1);
      }
      pthread_join(thread4, NULL);// Wait to finish.
    }else if(i % 2 == 0){
      thread.arg1 = 3;
      status = pthread_create(&thread3, NULL, reader, (void *)&thread);
      if (status != 0){
        printf("Error in thread 3: %d\n", status);
        exit(1);
      }
      pthread_join(thread3, NULL);// Wait to finish.
    }else{
      thread.arg1 = 2;
      status = pthread_create(&thread2, NULL, reader, (void *)&thread);
      if (status != 0){
        printf("Error in thread 2: %d\n", status);
        exit(1);
      }
      pthread_join(thread2, NULL);// Wait to finish.
    }
  }
  // close read end of pipe.
  close(p[0]);
  return 0; 
}
// Wrire num amount of random numbers to the write end of the pipe.
void* writer(void* num){
  // Makes a struct so i can pass an int as an argument.
  struct arg *number = num;
  int generate = number->arg1;
  int r;
  srand(time(NULL));
  for(int i = 0; i < generate; i++){
    r = rand();  
    write(p[1], &r , MAXSIZE);
    printf("Generated: %d\n", r);
  }
  close(p[1]);
}
// Read end of pipe. Takes in an int so it knows what
// thread is doing the reading.
void* reader(void* threadId){
  // Makes a struct so i can pass an int as an argument.
  struct arg *thread = threadId;
  int tid = thread->arg1;
  int prime;
  int result = read(p[0], &prime, MAXSIZE);
  if(result == -1){
    printf("Read Error\n");
    exit(1);
  }else if(result == 0){
    exit(2);
  }else{
    // Determins if number read is prime.
    printf("Reader thread %d: ", tid);
    if(isPrime(prime))
      printf("%d is prime.\n", prime);    
  }
}
// Returns if a number is prime or not.
int isPrime(int num){
  // 2 is the first prime number.
  int i = 2;
  // tries to find a number divisable by the num.
  while (i < num)
  {
    if (num % i == 0){
      printf("%d is divisible by %d\n",num,  i);     
      return 0;
    }
    ++i;
  }
}
