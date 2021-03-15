#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){

	setvbuf(stdout, NULL, _IOLBF, 0);
	printf("PARENT: okay, start here.\n");
	pid_t p, w;
	p = fork();
	int status;

	if(p == -1) {
		perror("fork() failed");
		return EXIT_FAILURE;
	}
	if(p > 0) {
		w = waitpid(p, &status, 0);
		if(w == -1) {
			perror("waitpid failed");
			return EXIT_FAILURE;
		}
		printf("PARENT: child process terminated successfully.\n");
		printf("PARENT: sigh, i'm gonna miss that little child process.\n");
	}
	else{
		printf("CHILD: happy birthday to me!\n");
		printf("CHILD: i'm bored....self-terminating....good-bye!\n");
	}

	return EXIT_SUCCESS;
}
