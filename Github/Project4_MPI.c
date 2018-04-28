#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>

int NUM_Thread;
int NUM_LINE;
#define LINE_SIZE 5000
#define FILELINES 100 //How many lines of a file to read at a time

int numlines = 0; //Total lines of file readed
int lineReaded = 0; //number of lines readed in current batch read
int filepos = 0; //

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
char* findLongestComnonSubstring(char *word1, char *word2){
  int len1 = strlen(word1), len2 = strlen(word2);

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

  //Build longest substring from longestTable
  char *longestSubstring;
    longestSubstring = strndup(word1+(maxI-(max-1)), max);
    free(data);
    free(longestTable);
  return longestSubstring;
}

void processLine(void* ID){
  int startLine, endLine, x;
  int myID = *((int*) ID);
  char *longS;
  //use thread ID to choose what lines to process
    startLine = myID * ((lineReaded-1)/NUM_Thread);
    endLine = startLine + ((lineReaded-1)/NUM_Thread);
    if(myID == lineReaded-1)
      endLine += (lineReaded-1) % NUM_Thread;

  //For each pair of lines find the longest common substring
  printf("Thread %d procesing lines %d-%d\n", myID, filepos+startLine, filepos+endLine);
    for(x = startLine; x < endLine; x++){
      longS = findLongestComnonSubstring(line_array[x], line_array[x+1]);
      //Output is critical section
      output[x] = longS;
    }
}

void main(int argc, char* argv[]){
  FILE *fd;
  int err;

//  NUM_Thread = atoi(argv[1]);
  NUM_LINE = atoi(argv[1]);

  //MPI varables
  int i , x, rc, numtasks, rank;
  MPI_Status Status;

  rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS){
    printf("MPI Error\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  NUM_Thread = numtasks;
  printf("size = %d rank = %d\n", numtasks, rank);
  fflush(stdout);

  //Time
  struct timeval t1, t2;
  double elapsedTime, AvgTime;
  double times[11];

//for()
  if(rank == 0)
    gettimeofday(&t1, NULL);
  //set up 2d array for reading file
  line_array = alloc2dChar(FILELINES, LINE_SIZE);
  output = alloc2dChar(FILELINES, LINE_SIZE);

if(rank == 0)
{
  lineReaded = 0;
  //Read in Wiki Lines
  fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
  do{
    err = fscanf(fd, "%[^\n]\n", line_array[lineReaded++]);
  }while(err != EOF && lineReaded < FILELINES && ++numlines < NUM_LINE);
  fclose(fd);
  printf("File Loaded\n");
}

  //Send Data
  MPI_Bcast(line_array, lineReaded * LINE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Bcast(&lineReaded, 1, MPI_INT, 0, MPI_COMM_WORLD);

  //Process lines
  processLine(&rank);

//Get Output
if(rank == 0){
  for(i = 1; i < numtasks; i++){
    MPI_Recv(output, lineReaded * LINE_SIZE, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);//get output back from each thread
  }
}
  else{
    MPI_Send(output, lineReaded * LINE_SIZE, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
  }



//Output result
if(rank == 0)
{
  for(int i = 0; i < lineReaded-1; i++){
    printf("%d-%d: %s\n", i, i+1, output[i]);
  }

if(rank == 0){
  gettimeofday(&t2, NULL); //End Timer
  elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
  elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
  times[x] = elapsedTime;
  printf("DEBUG_%d, MPI, %f\n", x, elapsedTime);
}

}

MPI_Finalize();
}
