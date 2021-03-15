#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/types.h>
#include<fcntl.h>

void read_and_parse_file(char *, int, int);

int * p;
int wordcount = 0, bytecount = 0, linecount = 0, digitcount = 0;

int main(int argc, char ** argv){
	setvbuf(stdout, NULL, _IOLBF, 0);
	if(argc == 1){
		exit(0);
	}else if(argc > 2){
		printf("PARENT: Collecting counts for %d files and the sentinel...\n", argc-1);
	}else{
		printf("PARENT: Collecting counts for %d file and the sentinel...\n", argc-1);
	}
	int * pids = calloc(argc-1, sizeof(int));
	int * pipes = calloc((argc-1)*2, sizeof(int));
	int * master_data = calloc(4, sizeof(int));
	for(int i = 1; i < argc; i++){
		p = calloc(2, sizeof(int));
		int pipe_rc = pipe(p);
		if(pipe_rc == -1){
			perror("Pipe failed");
		}
		int index = (i - 1)*2;
		*(pipes + index) = *p;
	        *(pipes + index + 1) = *(p + 1);	
		*(pids + i - 1) = fork();
		if(*(pids + i - 1) != 0){
			printf("PARENT: Calling fork() to create child process for \"%s\"\n", *(argv+i));
		}
		if(*(pids+i-1) == 0){
			close(*(pipes+index));
		}else{
			close(*(pipes+index+1));
		}
		read_and_parse_file(*(argv+i), *(pids+i-1), *(pipes + index + 1));
		int read_fd = *(pipes + index);
		if(*(pids + i-1) != 0){
			read(read_fd, master_data, 4*sizeof(int));
			int bytes = *(master_data);
			int words = *(master_data+1);
			int lines = *(master_data+2);
			int digits = *(master_data+3); 
		printf("PARENT: File \"%s\" -- %d byte%s, %d word%s, %d line%s, %d digit%s\n", *(argv+i), bytes, bytes!=1?"s":"", words, words!=1?"s":"", lines, lines!=1?"s":"", digits, digits!=1?"s":"");
		}
		free(p);
	}
	for(int j = 0; j < argc - 1; j++){
		int status = 0;
		int pid_done = waitpid(*(pids+j), &status, 0);
		if(pid_done == -1){
			printf("Wait_Pid failed\n");
		}
		
	}
	for(int k = 0; k < argc-1; k++){
	}
	p = calloc(2, sizeof(int));
	int pipe_rc = pipe(p);
	if(pipe_rc == -1){
		perror("Pipe failed");
	}
	printf("PARENT: Calling fork() to create child process for the sentinel\n");
	int pid_t = fork();

	if(pid_t == 0){
		close(*p);	
		char * write_fd = calloc(1, sizeof(int));
		sprintf(write_fd, "%d", *(p+1));
		printf("CHILD: Calling execl() to execute sentinel.out...\n");
		int rc = execl("./sentinel.out", "sentinel.out", write_fd, NULL);
		if(rc == -1){
			perror("Execl() failed");
		}
		
		free(write_fd);
		exit(0);
	}else{
		close(*(p+1));
		read(*p, master_data, 4*sizeof(int));
		int bytes = *(master_data);
		int words = *(master_data+1);
		int lines = *(master_data+2);
		int digits = *(master_data+3); 
		printf("PARENT: Sentinel -- %d byte%s, %d word%s, %d line%s, %d digit%s\n", bytes, bytes!=1?"s":"", words, words!=1?"s":"", lines, lines!=1?"s":"", digits, digits!=1?"s":"");
		close(*p);
	}

	free(pids);
	free(pipes);
	free(p);
	free(master_data);
	printf("PARENT: All done -- exiting...\n");
}


void read_and_parse_file(char * filename, int pid, int write_fd){
	if(pid == 0){
		printf("CHILD: Processing \"%s\"\n", filename);
		FILE * fd = fopen(filename, "r");
		int * data = calloc(4, sizeof(int));
		struct stat * info = calloc(1, sizeof(struct stat));
		stat(filename, info);
		bytecount = (int) info->st_size;
		char c;
		int charcount = 0;
		c = fgetc(fd);
		while(c != EOF){
			if(!isalpha(c) && !isdigit(c)) {
				if(charcount > 0){
					wordcount++;
				}
				charcount = 0;
			}
			else if(isdigit(c)){
				digitcount++;
			}
			else {
				charcount++;
			}
			if(c == '\n'){
				linecount++;
			}
			c = fgetc(fd);
		
		}  
		*data = bytecount;
		*(data + 1) = wordcount;
		*(data + 2) = linecount;
	        *(data + 3) = digitcount;	
		printf("CHILD: Done processing \"%s\"\n", filename);
		write(write_fd, data, 4*sizeof(int)); 
		free(data);
		free(info);
		
		exit(0);
	}
	else {
		
	}
}


