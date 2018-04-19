#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_Thread 4
#define NUM_LINE 25
#define LINE_SIZE 5000

char **line_array;
char **output;

pthread_mutex_t mutexOutput; //mutex for output[]

//Set 2d Array
char** alloc2dChar(int row, int col){
  int i;
  char *data = (char *) malloc(row * col * sizeof(char));
  char **charArray = (char **) malloc(row * sizeof(char*));
  for(i = 0; i < row; i++)
    charArray[i] = &(data[col*i]);
  return charArray;
}

//Use dynamic programming to find the longest common substring
char* findLongestComnonSubstring(char *word1, char *word2){
  int len1 = strlen(word1), len2 = strlen(word2);
  printf("len1:%d len2:5=%d\n", len1, len2);
  //Set up dynamic programming table
  int *data = (int *) malloc(len1 * len2 * sizeof(int));
  int **longestTable = (int **) malloc(len1 * sizeof(int*));
  for(int i = 0; i < len1; i++)
    longestTable[i] = &(data[len2*i]);

  int max = 0, maxI, maxJ;

  for(int i = 0; i < len1; i++){
      char letter1 = word1[i];
    for(int j = 0; j < len2; j++){
      char letter2 = word2[j];
      //printf("@ %d,%d\n",i,j );
      //If ith and jth chars of word1 and word2 are equal then
      //it value is longestTable[i-1][j-1] + 1
      if(letter1 == letter2)
      {
        if(i == 0 || j == 0)
          longestTable[i][j] = 1;
        else
          longestTable[i][j] = longestTable[i-1][j-1] + 1;

        //Test if value in greater then max
        int testMax = longestTable[i][j];
        if(max < testMax){
          max = testMax;
          maxI = i;
          maxJ = j;
        }
      }
      else
        longestTable[i][j] = 0;
    }
  }
  printf("Build the long substring\n");
  //Build longest substring from longestTable
  char *longestSubstring;
    longestSubstring = strndup(word1+(maxI-(max-1)), max);
    printf("build Done\n");
  return longestSubstring;
}

void processLine(void *ID){
  int myID = (int) ID;
  int startLine, endLine, x;
  char *longs;
  //use thread ID to choose what lines to process
    startLine = myID * ((numlines-1)/NUM_Thread);
    endLine = startLine + ((numlines-1)/NUM_Thread);
    if(ID == NUM_Thread-1)
      endLine += (numlines-1) % NUM_Thread;

  //For each pair of lines find the longest common substring
    for(x = startLine; x < endLine; x++){
      longS = findLongestComnonSubstring(line_array[x], line_array[x+1]);
      //Output is critical section
      pthread_mutex_lock(&mutexOutput);
      output[x] = longS;
      pthread_mutex_unlock(&mutexOutput);
    }
}

void main(int argc, char* argv[]){
  FILE *fd;
  int err;
  int numlines = 0;

  //pthread varables
  int i, rc;
  pthread_t threads[NUM_Thread];
  pthread_attr_t attr;
  void *status;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  //set up 2d array for reading file
  line_array = alloc2dChar(NUM_LINE, LINE_SIZE);

  //Read in Wiki Lines
  fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
  do{
    err = fscanf(fd, "%[^\n]\n", line_array[numlines++]);
  }while(err != EOF && numlines < NUM_LINE);
  fclose(fd);
  printf("File Loaded\n");

  output = alloc2dChar(numlines-1, LINE_SIZE);


  //Run processLine with pthread
  for(i = 0; i < NUM_Thread; i++){
    rc = pthread_create(&threads[i], &attr, processLine, (void *)i);
    if(rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  pthread_attr_destroy(&attr);
  for(i = 0; i < NUM_Thread; i++){
    rc = pthread_join(threads[i], &status);
    if(rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

//Output result
  for(int x = 0; x < numlines-1; x++){
    printf("%d-%d: %s\n", x, x+1, output[x]);
  }

  pthread_mutex_destroy(&mutexOutput);
  printf("DATA, time");
  pthread_exit(NULL);
}
