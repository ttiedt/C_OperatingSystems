/*
Class: CPSC 346-02
Team Member 1: Tyler Tiedt
Team Member 2: N/A 
GU Username of project lead: ttiedt
Pgm Name: Project 4
Pgm Desc: Shell
Usage: ./a.out
*/

#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#define MAX_LINE 80
#define TRUE 80
#define MAX_COMMANDS 9

char** parseInput(char*);
void addtohistory(char[]);

char history[MAX_COMMANDS][MAX_LINE];
char disp_history[MAX_COMMANDS][MAX_LINE];
int historyNumCommands;

int main(int argc, char* argv[])
{
  //A pointer to an array of pointers to char.  In essence, an array of 
  //arrays.  Each of the second level arrays is a c-string. These hold
  //the command line arguments entered by the user.
  //as exactly the same structure as char* argv[]
  char **args;  
  pid_t child;
  char inputBuff[MAX_LINE];

  while (TRUE)
  {
    int length,		/* # of characters in the command line */
            i,j,				/* loop index for accessing inputBuffer array */
            command_number,
            tmpHistoryIndex;
    char tmpString[MAX_LINE];

    /* read what the user enters on the command line */
    do {
        printf("myshell>");
        fflush(stdout);
        char c;
        length = 0;
  
        while((c = getchar()) != '\n'){
          inputBuff[length] = c;
          length++;
        }
        inputBuff[length] = '\0';
        length++;
        args = parseInput(inputBuff);
    }

    // Sets the index for the history commands
    while (inputBuff[0] == '\n'); /* swallow newline characters */
    // Conditional to check if they are using history
    if (inputBuff[0] == '!') {
        // If inputBuffer[] == "!!", we go to the first command.
        if(inputBuff[1] == '!') 
          tmpHistoryIndex = 0;
        // Otherwise, we gather user input at inputBuffer[1]
        else 
            tmpHistoryIndex = inputBuff[1] - '0';

        // If the index the user is trying to access is out of bounds, negative or
        // greater than historyNumCommands, the program exits.
        if(tmpHistoryIndex < 0 || tmpHistoryIndex > historyNumCommands){
            printf("ERROR - unable to access command in history\n");
            break;
            //return 0;
        }
        // we copy history's contents at index tmpHistoryIndex into inputBuffer, which is parsed and run.
        strcpy(inputBuff, history[tmpHistoryIndex]);
    }
    // Adds the input to history
    addtohistory(inputBuff);

    // If the user has entered "quit" break out of the loop.
    if(strcmp(args[0], "quit") == 0)
      break;
    else if(strcmp(args[0], "history") == 0){
      printf("---HISTORY---\n");
            for (int curr = historyNumCommands; curr >= 0; curr--) {
                printf("\t%d: %s\n", curr, disp_history[curr]);
            }
    }
    // Else run command
    else {
      child = fork();
      if(child == 0){
        execvp(args[0], args);
        exit(0);
      } 
      else {
          printf("Wait ");
          wait(NULL);
      }
      printf("Done!\n");
    }
  }
   return 0;
}

/*
inp is a cstring holding the keyboard input
returns an array of cstrings, each holding one of the arguments entered at
the keyboard. The structure is the same as that of argv
Here the user has entered three arguements:
myShell>cp x y
*/ 
char** parseInput(char* inp){
  char** arg = NULL;
  char* str = strtok(inp, " ");
  int numSpaces = 0, i;
  
  while(str){
    // Re-allocates the index of arg for each argument
    arg = realloc(arg, sizeof(char*)* ++numSpaces);
    if(arg == NULL)
      exit(-1);
    // Sets the index of arg to value of str
    arg[numSpaces-1] = str;
    // Gets the next argument.
    str = strtok(NULL, " ");
  }
  // adds NULL charater to end of arg
  arg = realloc(arg, sizeof(char*)* (numSpaces + 1));
  arg[numSpaces] = '\0';

  return arg;
} 

//Add the most recent command to history
void addtohistory(char args[]) {
    int i, j, keepGoing;

    // Here, we move all commands one spacd to the right, to make room for our newest
    // command at index 0.
    for(int curr = historyNumCommands; curr > 0; curr--){
        strcpy(history[curr], history[curr-1]);
    }
    // Once this is done, we add our command to our history array.
    strcpy(history[0], args);


    // Here, we move all commands one space to the right, to make room for our newest
    // command at index 0.
    for(int curr = historyNumCommands; curr > 0; curr--){
        strcpy(disp_history[curr], disp_history[curr-1]);
    }
    keepGoing = 1;
    j=0;
    while(keepGoing){
        switch (args[j]){
            // If we encounter a null or newline character, we are done appending.
            case '\0':
            case '\n':
                disp_history[0][j] = '\0';
                keepGoing = 0;
                break;
            // Any other character gets added to the history.
            default :
                disp_history[0][j] = args[j];
                j++;
                break;
        }
    }

    // These two lines are to ensure we don't let historyNumCommands exceed MAX_COMMANDS
    historyNumCommands++;
    if(historyNumCommands == MAX_COMMANDS) historyNumCommands--;
}
