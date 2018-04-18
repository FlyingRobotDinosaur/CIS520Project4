#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_LINE 10
#define LINE_SIZE 10000

char **line_array;

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
  int longestTable [len1][len2];
  int max = 0, maxI, maxJ;

  for(int i = 0; i < len1; i++){
      char letter1 = word1[i];
    for(int j = 0; j < len2; j++){
      char letter2 = word2[j];

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

void main(int argc, char* argv[]){
  FILE *fd;
  int err;
  int numlines = 0;

  line_array = alloc2dChar(NUM_LINE, LINE_SIZE);

  //Read in Wiki Lines
  fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
  do{
    err = fscanf(fd, "%[^\n]\n", line_array[numlines++]);
  }while(err != EOF && numlines < NUM_LINE);
  fclose(fd);
  printf("File Loaded\n");

//For each pair of lines find the longest common substring
  for(int x = 0; x < numlines-1; x++){
    char *longS = findLongestComnonSubstring(line_array[x], line_array[x+1]);
    printf("%d-%d: %s\n", x, x+1, longS);
  }
}
