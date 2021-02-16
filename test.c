#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char ** argv) {
	char * word = "Test";
	strcpy(*(argv + 1), word);
	printf("%s\n", *(argv+1));
	return EXIT_SUCCESS;
}
