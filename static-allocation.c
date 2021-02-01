#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main() {
  int x = 5;
  printf("x is %d\n", x);
  printf("Sizeof int is %ld\n", sizeof(int));
#ifdef DEBUG_MODE
  int * y = NULL; /* pointers are usually given 8 bytes */
  y = &x ;
  printf("Y points to %d\n", *y);
  
#endif
  char cptr[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ;
  printf("cptr is: %s\n", cptr);
  cptr[2] = 'Q';
  printf("new cptr is: %s\n", cptr);

  return 0;
}
