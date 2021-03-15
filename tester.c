#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>
int main()
{
	int * r = malloc(sizeof(int));
	float * i = malloc(sizeof(float));
	double ** x = calloc(20, sizeof(double *));
	x[4] = calloc(20, sizeof(double));

}
