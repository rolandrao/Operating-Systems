#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>



#include "lecex3-q2-copy-file.c"




int main(int argc, char ** argv){
  int children = argc - 1;
  int rc;
  pthread_t tid[children];
  int byte_sum;
  for(int i = 0; i < children; i++){

    rc = pthread_create(&tid[i], NULL, copy_file, argv[i+1]);

    if(rc != 0){
      fprintf(stderr, "pthread_create() failed (%d)\n", rc);
    }
	}


	for(int j = 0; j < children; j++){
    int * bytes;
    pthread_join(tid[j], (void **)&bytes);
    byte_sum += *bytes;
    printf("MAIN: Thread completed copying %d bytes for %s\n",
     *bytes, argv[j+1]);
  }

  printf("MAIN: Successfully copied %d byte%s via %d child thread%s\n",
   byte_sum, (byte_sum != 1) ? "s":"", children, (children != 1)? "s":"");



  return EXIT_SUCCESS;
}
