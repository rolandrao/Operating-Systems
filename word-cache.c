#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>

int hash_finder(char * word, int size) {
	int hash = 0;
	int i = 0;
	char c = *(word);
	while(c != '\0') {
		hash = hash + (int) c;
		++i;
		c = *(word + i);
	}
	return hash % size;
}

void print_cache(char ** cache, int size){
	for(int i = 0; i < size; i++){
		if(*(cache+i) != NULL){
			printf("Cache index %d ==> \"%s\"\n", i, *(cache + i));	
		}
	}

}

int main(int argc, char ** argv) {
	
  if(atoi(*(argv+1)) <= 0){
	fprintf(stderr, "Invalid Cache Size\n");
	return EXIT_FAILURE;
  }
  int cache_size = atoi(*(argv+1));

  char ** cache = calloc(cache_size , sizeof(char *));

  char * word = calloc(128, sizeof(char));
  int hash = 0;
  char * mem_allocation = calloc(8, sizeof(char));
  int charcount = 0;
  

  for(int i = 2; i < argc; i++){

	  
	  FILE *fd = fopen(*(argv + i), "r");
	  
	  if (fd == NULL) {
		  perror("Error opening File\n");
		  return EXIT_FAILURE;
	  }
	  char c;
	  while((c = fgetc(fd)) != EOF){
	
		  if(!isalpha(c)) {
		
			  hash = hash_finder(word,cache_size);
			  if(charcount > 2){
				  if(*(cache+hash) == 0){
					  strcpy(mem_allocation, "calloc");
					  *(cache + hash) = calloc(128, sizeof(char));
				  }else {
					  strcpy(mem_allocation, "realloc");
					  *(cache + hash) = realloc(*(cache+hash), 128);
				  }
				  strcpy(*(cache+hash), word);
				  printf("Word \"%s\" ==> %d (%s)\n", word, hash, mem_allocation);
			  }
			  free(word);
			  word = calloc(128, sizeof(char));
			  charcount = 0;
		  }
		  else {
			  strncat(word, &c, 1);
			  charcount++;
		  }
	  }
  fclose(fd);
  }
  print_cache(cache, cache_size);
  for(int i = 0; i < cache_size; i++){
	if(*(cache+i) != NULL){
		free(*(cache+i));
	}
  }
  free(cache);
  free(word);
  free(mem_allocation);
  return EXIT_SUCCESS;
}


