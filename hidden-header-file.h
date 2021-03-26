#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>

#define SHARED_SHM_KEY 89997

int lecex3_q1_parent( int pipefd ) {
  key_t key = SHARED_SHM_KEY;
  int shmid;
  int numwritten;
  int* data;

  char* data_to_cpy = "abcdefghijklmnopq rstUVWXYz12 34567890yAy!"; /* set this to what ever */

  if (-1 == (shmid = shmget( key, (size_t)strlen(data_to_cpy), IPC_CREAT | IPC_EXCL | 0660 ) ) ) {
    perror("ERROR: shmget() failed");
    return EXIT_FAILURE;
  }

  if ((int*)-1 == (data = shmat( shmid, NULL, 0 ))) {
    perror("ERROR: shmat() failed");
    return EXIT_FAILURE;
  }

  char* cdata = (char*) data;

  cdata = (char*)memcpy((void*)cdata, data_to_cpy, strlen(data_to_cpy));
  numwritten = strlen(data_to_cpy);

  int sendpipe[] = {key, numwritten};

  if (-1 == write(pipefd, sendpipe, sizeof(int) * 2)) {

    perror("ERROR: write() failed");
    return EXIT_FAILURE;
  }

  if (-1 == waitpid(-1, NULL, 0)) {
    perror("ERROR: waitpid() failed");
    return EXIT_FAILURE;
  }

  printf("%s\n", cdata);

  if (-1 == shmctl(shmid, IPC_RMID, NULL)) {
    perror("ERROR: shmctl() failed");
    return EXIT_FAILURE;
  }

  if (-1 == shmdt(data)) {
    perror("ERROR: shmdt() failed");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
