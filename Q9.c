#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<ctype.h>
#include<fcntl.h>

int main(int argc, char ** argv) {

	int m = atoi(*(argv+1));
	int fd = open("data.txt", O_RDONLY);
	char buffer[m];
	int rc = read(fd, buffer, m);
	int p[2];
	int pipe_rc = pipe(p);
	if( pipe_rc == -1){
		perror("Pipe failed");
		return EXIT_FAILURE;
	}
	write(p[1], buffer, rc);
	pid_t pid = fork();
	if(pid == -1){
		perror("fork failed");
		return EXIT_FAILURE;
	}

	if(pid == 0){
		close(p[1]);
		char data[rc];
		read(p[0], data, rc);
		char c = data[0];
		int i = 0;
		int digitcounter = 0;
		while(i != rc){
			if(isdigit(c)){
				printf("%c", c);
				digitcounter++;
			}
			i++;
			c = data[i]; 




		}
		printf("\n");
		printf("(filtered %d %s from %d %s)", digitcounter, digitcounter!=1?"digits":"digit", rc, rc!=1?"bytes":"byte"); 
		
	}
	else{
		int status;
		int pid_done = waitpid(pid, &status, 0);		
		if(pid_done == -1){
			perror("Waitpid failed");
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

}

