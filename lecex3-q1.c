#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>


int lecex3_q1_child(int pipefd){
  int key = 0;
  int size = 0;
  int * buffer = calloc(1, sizeof(int));
  read(pipefd, buffer, sizeof(int));
  key = * buffer;
  read(pipefd, buffer, sizeof(int));
  size = *buffer;

  int shmid = shmget(key, size, 0660);
  if(shmid == -1){
    perror("ERROR: shmget() failed");
    return EXIT_FAILURE;
  }

  char * data = shmat(shmid, NULL, 0);
  if(strcmp(data, "-1") == 0){
    perror("ERROR: shmat() failed");
    return EXIT_FAILURE;
  }
  int i = 0;
  while(i < size){
    if(isalpha(data[i])){
      data[i] = toupper(data[i]);
    }
    i++;
  }


  return 0;
}
