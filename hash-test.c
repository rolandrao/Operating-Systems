#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int hash_finder(char * word, int size) {
	int hash = 0;
	int i = 0;
	char c = *(word+1)+0;
	while(c != '\0') {
		hash = hash + (int) c;
		++i;
		c = *(word + i);
	}

	return hash % size;
}



int main(int argc, char ** argv) {
	
	int hash = 0;
	char * word = calloc(128, sizeof(char));
  for(int i = 2; i < argc; i++){

	  FILE *fd = fopen(*(argv + i), "r");
	  
	  if (fd == NULL) {
		  perror("Error opening File");
		  return EXIT_FAILURE;
	  }
	  char c;
	  while((c = fgetc(fd)) != EOF){
	
		  if(c == ' ' || c == '\n') {
		
			  hash = hash_finder(word,atoi(*(argv+1)));
			  printf("%d\n", hash);
			 
		  }
		  else {
			  strncat(word, &c, 1);
		  }
	  }
  }
}
