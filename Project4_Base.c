#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char* findLongestComnonSubstring(char *word1, char *word2){
  int len1 = strlen(word1), len2 = strlen(word2);
  int longestTable [len1][len2];
  int max = 0, maxI, maxJ;

  for(int i = 0; i < len1; i++){
      char letter1 = word1[i];
    for(int j = 0; j < len2; j++){
      char letter2 = word2[j];

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
  char *test1 = "Your ftest string is pie, my words";
  char *test2 = "pies are testing my words";
  printf("Testing with Words %s and %s\n", test1, test2);

  char *testR = findLongestComnonSubstring(test1, test2);

  printf("The longest substring is:%s\n", testR);
}
