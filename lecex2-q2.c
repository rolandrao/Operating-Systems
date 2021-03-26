#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main() {
	setvbuf(stdout, NULL, _IOLBF, 0);
	printf("PARENT: okay, start here.\n");
	pid_t p, w1, w2;
	p = fork();
	int status;

	if (p == -1){
		perror("Fork failed");
		return EXIT_FAILURE;
	}

	if(p > 0 ){
		w1 = waitpid(p, &status, 0);
		if(w1 == -1){
			printf("Waitpid failed");
			return EXIT_FAILURE;
		}

		p = fork();
		if(p == -1){
			printf("Fork 2 failed");
			return EXIT_FAILURE;
		}
		if(p == 0){
			printf("CHILD B: and happy birthday to me!\n");
			printf("CHILD B: see ya later....self-terminating!\n");

		} else{
			w2 = waitpid(p, &status, 0);
			if(w2 == -1){
				printf("Waitpid failed");
				return EXIT_FAILURE;
			}
			printf("PARENT: both child processes terminated successfully.\n");
			printf("PARENT: phew, i'm glad they're gone!\n");
		}

	}
	else{
		printf("CHILD A: happy birthday to me!\n");
		printf("CHILD A: i'm bored....self-terminating....good-bye!\n");
	}

}
