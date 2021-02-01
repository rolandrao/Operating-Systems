#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

int main() {
  char * path = malloc(20); /* dynamically allocates 20 bytes on the heap */

  strcpy (path, "/csci/goldsd/s21/");
  printf("path is %s\n", path);

  char * path2 = malloc(20);
  strcpy (path2, "AAAAAAAAAAAAAAA");
  printf("path2 is %s\n", path2);

  return EXIT_SUCCESS; 
}
