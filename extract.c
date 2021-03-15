#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

int main(int argc, char ** argv) {
  int fd = open(*(argv+1), O_RDONLY);
  char * buffer = malloc(1);
  lseek(fd, 6, SEEK_SET);
  while(read(fd, buffer, 1) != 0){
    printf("%s",buffer);
    lseek(fd, 6, SEEK_CUR);
  }
  close(fd);
  printf("\n");
  return EXIT_SUCCESS;
  


  

}
