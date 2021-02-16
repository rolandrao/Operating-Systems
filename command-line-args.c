#include<stdio.h>
#include<stdlib.h>

int main(int argc, char ** argv) {
  
  File *f = fopen(*(argv + 1), "r");
  char * line = malloc(128);

  if (f == NULL) {
	  perror("Error opening File");
	  return EXIT_FAILURE;
  }
  else {
	  fgets(line, 128, f);
	  printf("Read: %s\n from %s", line, *(argv+1));
  }
	 
  


  return EXIT_SUCCESS;
}

