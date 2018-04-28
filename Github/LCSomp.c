#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

int NUM_THREADS;
int NUM_LINE;
#define LINE_SIZE 5000
#define FILELINES 1000
#define NUM_RUN 2

int numlines = 0; //Total lines of file readed
int lineReaded = 0; //number of lines readed in current batch read
int filepos; //

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

	int k, i, j;
  int len1 = strlen(word1), len2 = strlen(word2);

  //Set up dynamic programming table
  int *data = (int *) malloc(len1 * len2 * sizeof(int));
  int **longestTable = (int **) malloc(len1 * sizeof(int*));
  for(k = 0; k < len1; k++)
    longestTable[k] = &(data[len2*k]);

  int max = 0, maxI, maxJ;

  for( i = 0; i < len1; i++){
      char letter1 = word1[i];
    for( j = 0; j < len2; j++){
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

  //Build longest substring from longestTable
  char *longestSubstring;
    longestSubstring = strndup(word1+(maxI-(max-1)), max);
    free(data);
    free(longestTable);
  return longestSubstring;
}

void processLine(int ID){

  int startLine, endLine, x;
  char *longS;

  #pragma omp private(ID, lineReaded, startLine, endLine, x, longS)
  {
  //use thread ID to choose what lines to process
    startLine = ID * ((lineReaded-1)/NUM_THREADS);
    endLine = startLine + ((lineReaded-1)/NUM_THREADS);
    if(ID == NUM_THREADS-1)
      endLine += (lineReaded-1) % NUM_THREADS;

  //For each pair of lines find the longest common substring
    printf("Thread %d procesing lines %d-%d\n", ID, filepos+startLine, filepos+endLine);
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
  int x, i;

  NUM_THREADS = atoi(argv[1]);
  NUM_LINE = atoi(argv[2]);

  omp_set_num_threads(NUM_THREADS);

  //Time
  struct timeval t1, t2;
  double elapsedTime, AvgTime;
  double times[11];

  for(x = 0; x <= NUM_RUN; x++)
  {
    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
    gettimeofday(&t1, NULL); //Start Timer
    numlines = 0;
    for(filepos = 0; filepos < NUM_LINE; filepos += FILELINES)
    {
      //set up 2d array for reading file
      line_array = alloc2dChar(FILELINES, LINE_SIZE);
      output = alloc2dChar(FILELINES, LINE_SIZE);
      lineReaded = 0;
      //Read in Wiki Lines
      do{
        err = fscanf(fd, "%[^\n]\n", line_array[lineReaded++]);
      }while(err != EOF && lineReaded < FILELINES && ++numlines < NUM_LINE);

      printf("File Loaded\n");

      #pragma omp parallel
      {
       processLine(omp_get_thread_num());
      }
    //Output result
      for(i = 0; i < lineReaded-1; i++){
        printf("%d-%d: %s\n", filepos+i, filepos+i+1, output[i]);
      }

    }

    gettimeofday(&t2, NULL); //End Timer
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
    times[x] = elapsedTime;
    printf("DEBUG_%d, OMP, %f\n", x, elapsedTime);
    fclose(fd);
}

//Find average
int count = 0;
double sum = 0;
for(x=1; x <= NUM_RUN; x++){
  if(times[x] > 0){
    sum += times[x];
    count++;
  }
}
AvgTime = sum / count;

  printf("DATA, OMP, %d, %d, %f\n", NUM_THREADS, NUM_LINE, AvgTime);
}
