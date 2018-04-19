#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_THREADS 4
#define NUM_LINE 15
#define LINE_SIZE 5000

char **line_array;
char **output;

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
char* findLongestComnonSubstring(char *word1, char *word2)
{
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

void processLine(int ID){
   
  int startLine, endLine, x;
  char *longS;

  #pragma omp private(ID, startLine, endLine, x, longS)
  {
  //use thread ID to choose what lines to process
    startLine = ID * ((numlines-1)/NUM_Thread);
    endLine = startLine + ((numlines-1)/NUM_Thread);
    if(ID == NUM_Thread-1)
      endLine += (numlines-1) % NUM_Thread;

  //For each pair of lines find the longest common substring
    for(x = startLine; x < endLine; x++){
      longS = findLongestComnonSubstring(line_array[x], line_array[x+1]);
      //Output is critical section
  #pragma omp critical
	{
         output[x] = longS;
	}
    }
  }
}

void main(int argc, char* argv[]){
  FILE *fd;
  int err;
  int numlines = 0;
  omp_set_num_threads(NUM_THREADS);

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
  #pragma omp parallel 
  {
   processLine(omp_get_thread_num());
  }
//Output result
  for(int x = 0; x < numlines-1; x++){
    printf("%d-%d: %s\n", x, x+1, output[x]);
  }

}

