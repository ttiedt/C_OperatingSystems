/*
Class: CPSC 346-02
Team Member 1: Tyler Tiedt
Team Member 2: N/A 
GU Username of project lead: ttiedt
Pgm Name: Project 8
Pgm Desc: Virtual memory manager
Usage: ./a.out BACKING_STORE.bin addresses.txt
*/

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TLB_SIZE 16
#define PAGES 256
#define PAGE_MASK 255
#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMORY_SIZE PAGES * PAGE_SIZE 


struct tlbentry 
   {
    unsigned char page_number;
    unsigned char frame_number;
    int tlb_ref;
   };
typedef struct tlbentry tlbentry;  

void init_pagetable(int[]);
bool check_pagetable(int[], int, int*);
int get_physical_address(int[], int);

int main(int argc, char *argv[])
{ 
  int pagetable[PAGES];
  tlbentry tlb[TLB_SIZE];
  signed char main_memory[MEMORY_SIZE];
  signed char *backing;
  int logical_address;
  int offset;
  int logical_page;
  int physical_page;
  int physical_address;
  int numTLBRefs = 0;
  int numTLBHits = 0;
  int faults = 0;
  int cnt = 0;

  //open simulation of secondary storage     
  const char *backing_filename = argv[1]; 					//BACKING_STORE.bin 
  int backing_fd = open(backing_filename, O_RDONLY);
  backing = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0); 	//backing can be accessed as an array  

  //open simulation of address bus and read the first line 
  FILE *ifp = fopen(argv[2],"r"); 						//addresses.txt 
  fscanf(ifp,"%d", &logical_address);
  // initialized pagetable
  init_pagetable(pagetable);
  char value;
  while(!feof (ifp)){
    //extract low order 8 bits from the logical_address. 
    offset = logical_address & OFFSET_MASK;
    //extract bits 8 through 15. This is the page number gotten by shifting right 8 bits 
    logical_page = (logical_address >> OFFSET_BITS) & PAGE_MASK;
    //When you write the program, the physical page will increment by 1 for each copy
    //from simulated backing store to main memory 
    physical_page = faults;
    //int tmp = check_pagetable(pagetable, logical_page, &logical_address); 
    if(check_pagetable(pagetable,logical_page, &physical_page)){
      printf("IN  ");
      physical_address = (get_physical_address(pagetable, logical_page) << OFFSET_BITS) | offset;
      value = main_memory[get_physical_address(pagetable, logical_page) * PAGE_SIZE + offset];
    }else {
      printf("NOT  ");
      faults++;
      memcpy(main_memory + physical_page * PAGE_SIZE, 
             backing + logical_page * PAGE_SIZE, PAGE_SIZE);
      physical_address = (physical_page << OFFSET_BITS) | offset;
      value = main_memory[physical_page * PAGE_SIZE + offset];
    }
    printf("Line: %d Virtual address: %d Physical address: %d Value: %d\n", 
            cnt + 1, logical_address, physical_address, value);
    				//addresses.txt 
    fscanf(ifp,"%d", &logical_address);
    cnt++;
  }
/*
Number of Translated Addresses = 1000
Page Faults = 244
Page Fault Rate = 0.244
TLB Hits = 54
TLB Hit Rate = 0.054
*/
  double rate = (double)faults/cnt;
  printf("Number of Translated: %d\n", cnt);
  printf("Page Faults = %d\n", faults);
  printf("Page Faults Rate = %f\n", rate);
  return 0;
}

// initiatas the page table
void init_pagetable(int pagetable[]){
  for(int i = 0; i < PAGES; i++){
    pagetable[i] = -1;
  }
}

// checks to see if frame number is in the page table
bool check_pagetable(int pagetable[], int logical_page, int *physical_page){
  if(pagetable[logical_page] == -1){
    pagetable[logical_page] = *physical_page;
    return false;
  } 
  return true;
}
// gets the buffer for physical page.
int get_physical_address(int pagetable[], int logical_page){
  return pagetable[logical_page];
}
