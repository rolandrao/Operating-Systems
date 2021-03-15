#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<ctype.h>
#include<fcntl.h>

int childfunction(int index, int bytes_per, char search, int pid, int bytes, int children);

int main(int argc, char ** argv){
	setvbuf(stdout, NULL, _IOLBF, 0);
	int children = atoi(*(argv+1));
	char * lookup = *(argv+2);	
	struct stat * info = calloc(1, sizeof(struct stat));
	stat("data.txt", info);
	int bytes = (int) info->st_size;
	pid_t * pids = calloc(children, sizeof(pid_t));
	int bytes_per = bytes / children;
	for(int i = 0; i < children; i++){
		pid_t pid = fork();
		if(pid == 0){
			childfunction(i, bytes_per, lookup[0], pid, bytes, children);
			exit(0);
		}

		pids[i] = pid;
	}

	for(int j = 0; j < children; j++){
		int status;
		int pid_done = waitpid(pids[j], &status, 0);
		if(pid_done == -1){
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
	

}


int childfunction(int index, int bytes_per, char search, int pid, int bytes, int children){	
	int fd = open("data.txt", O_RDONLY);
	lseek(fd, index*bytes_per, SEEK_SET);
	int start = bytes_per * index;
	if(index == children-1){
		bytes_per = bytes - bytes_per * index;	
	}
	int end = start + bytes_per-1;
	char buffer[bytes_per];
	read(fd, buffer, bytes_per);
	int i = 0;
	int charcount = 0;
	char c = buffer[0];
	while(i != bytes_per){
		if(c == search){
			charcount++;
		}
		i++;
		c = buffer[i];
	}
	close(fd);
	printf("Counted %d occurrence%s of '%c' in byte range %d-%d\n", charcount, charcount!=1?"s":"", search, start, end );
	return EXIT_SUCCESS;

}



