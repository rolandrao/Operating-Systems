#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


int lecex2_child(){
	int fd = open("data.txt", O_RDONLY);
	char buffer[1];
	lseek(fd, 5, SEEK_SET);
	int rc = read(fd, buffer, 1);
	if(fd == -1){
		perror("File data.txt not found");
		abort();
	}else if(rc == 0){
		perror("There are less than 6 characters in data.txt");
		abort();
	}
	return (int) buffer[0];

}


int lecex2_parent(){
	int status;

	waitpid(-1, &status, 0);
	if(WIFSIGNALED(status)){
		printf("PARENT: child process terminated abnormally\n");
		return EXIT_FAILURE;
	}
	else{
		printf("PARENT: child process reported '%c'\n", (char) WEXITSTATUS(status));	
	}
	return EXIT_SUCCESS;	
}
