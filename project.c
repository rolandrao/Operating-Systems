/* project.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <math.h>
#include <time.h>
#include <string.h>

struct process
{
	//Constant
	char id;
	int* cpuBtimes;
	int* ioBtimes;
	int size;
	int arrivalTime;

	//Change during the alg
	char* state;		//ready, running, terminated, IO, context switch
	int burstNum;		//Current index for CPU/IO burst
	int cpuB;			//current CPU Burst
	int ioB;			//current IO Burst
	int tau;			//For SJF and SRT alg
	int waitT;
	int turnT;
	int csT;
};

struct queue
{
	struct process readyQueue[26];
	int lastRpI;	//Last process in the ready state for the queue
	int lastIOpI;	//Last process in the IO state for the queue and acts as a counter for the processors in the queue
	int size;
};

//Values for the sim output
struct sim
{
	float avg_CPUbt;
	float avg_Wt;
	float avg_Tt;
	int numOfCS;
	int numOfPre;
	float cpuUsePer;
};

//Prototype functions
struct process generator(double l, int uB);
void resetProcessor(struct process* pA, int nP);
struct sim FCFS(struct process* pA, int nP, int cS);
struct sim RR(struct process* pA, int nP, int cS, int tS);
void SJF(struct process* pA, int nP, int cS, double alpha);
int exp_avg(int tau, int t, double alpha);

//Prototype functions for the queue
int sortingP(struct process p1, struct process p2);
void printQ(struct queue qu);
void pushQ(struct queue * q, struct process p );
void popQ(struct queue * q, int index);
void insertQ(struct queue * q, struct process p);
struct process frontQ(struct queue * q);
void reorganizeQ(struct queue * q, int alg);


int main(int argc, char const *argv[])
{
	int numOfP = atoi(argv[1]);		//# of Processors
	int seed = atoi(argv[2]);		//seed for the random number generator
	double lambda = atof(argv[3]);	//Lambda
	int upperBound = atoi(argv[4]);	//UpperBounds for valid pseudo-random numbers
	int tCS = atoi(argv[5]);		//Time(ms) for Context Switch
	int alpha = atof(argv[6]);		//constant α, (Used for SJF and SRT alg.)

	/*
	int tSlice = argv[7];		//Time Slice(ms) for RR alg.
	char* rrAdd = "END";		//Optional argument used in RR alg., define whether processes are added to the end or the
	if(argc == 9)				//beginning of the ready queue when they arrive or complete I/O
		{rrAdd = argv[8];}		// rrAdd = BEGINNING or END, End being the defult
	*/

	srand48(seed);	//set the random range with the seed
	struct process* pArray = calloc(numOfP, sizeof( struct process ));

	for (int i = 0; i < numOfP; ++i)
	{
		struct process p;
		p = generator(lambda, upperBound);
		p.id = (char)('A'+ i);

		pArray[i] = p;

		#ifdef DEBUG_MODE1
			printf("Process %c:\n\tArrival Time: %d\n", p.id, p.arrivalTime);
			printf("\tBurst Times\tIO Times\n");
			for (int i = 0; i < p.size; ++i)
				printf("\t%d\t\t%d\n", p.cpuBtimes[i], p.ioBtimes[i]);
		#endif
	}

	resetProcessor(pArray, numOfP);

	#ifdef DEBUG_MODE2
		struct process p1;
		p1.id = 'A';
		p1.state = "csOut";
		p1.cpuB = 5;
		p1.ioB = 30;
		struct process p2;
		p2.id = 'B';
		p2.state = "csIn";
		p2.cpuB = 5;
		p2.ioB = 30;
		struct process p3;
		p3.id = 'C';
		p3.state = "IO";
		p3.cpuB = 5;
		p3.ioB = 30;
		struct process p4;
		p4.id = 'D';
		p4.state = "READY";
		p4.cpuB = 5;
		p4.ioB = 30;
		struct process p5;
		p5.id = 'E';
		p5.state = "IO";
		p5.cpuB = 5;
		p5.ioB = 30;
		struct process p6;
		p6.id = 'F';
		p6.state = "TERMINATED";
		p6.cpuB = 5;
		p6.ioB = 30;

		struct queue qTest;
		qTest.size = 0;
		pushQ(&qTest, p1);
		pushQ(&qTest, p2);
		pushQ(&qTest, p3);
		pushQ(&qTest, p4);
		pushQ(&qTest, p5);
		pushQ(&qTest, p6);

		qTest.lastRpI = 1;
		qTest.lastIOpI = 3;
		reorganizeQ(&qTest, 0);

		printf("qTest order:\nID\tSTATE\n");
		for(int i = 0; i < qTest.size; i++)
		{
			printf("%c\t%s\n", qTest.readyQueue[i].id, qTest.readyQueue[i].state);
		}
	#endif

	// struct sim fcfs_val = FCFS(pArray, numOfP, tCS);
	SJF(pArray, numOfP, tCS, alpha);

	// printf("Algorithm FCFS\n");
	// printf("-- average CPU burst time: %.03f ms\n", fcfs_val.avg_CPUbt);
	// printf("-- average wait time:  %.03f ms\n", fcfs_val.avg_Wt);
	// printf("-- average turnaround time:  %.03f ms\n", fcfs_val.avg_Tt);
	// printf("-- total number of context switches: %d\n", fcfs_val.numOfCS);
	// printf("-- total number of preemptions: 0\n");
	// printf("-- CPU utilization: %.03f%c\n", fcfs_val.cpuUsePer, '%');

	/*
	char * name = "outfile.txt";
	int fd = open( name, O_WRONLY | O_CREAT | O_TRUNC, 0660 );
	if ( fd == -1 )
	{
		perror( "open() failed" );
		return EXIT_FAILURE;
	}
	*/


	return 0;
}


struct process generator(double l, int uB)
{
	struct process p;
	double r; 	//random number
	double x;

	while(1)
	{

		r = drand48();
		x = - log( r ) / l;
		if( x <= uB) { break; }
	}


	p.arrivalTime = x;

	int cpuBursts = (drand48() * 100) + 1;
	p.size = cpuBursts;
	//printf("%d\n", cpuBursts);

	int* cpuBurstTimes = calloc( cpuBursts, sizeof( int ) );
	int* ioBurstTimes = calloc( cpuBursts, sizeof( int ) );

	//set first IO burst as arrivial time
	ioBurstTimes[0] = p.arrivalTime;


	//Create burst times for CPU and IO
	for(int i = 0; i < cpuBursts; i++ )
	{
		if(i > 0)
		{
			while(1)
			{
				r = drand48();
				x = ceil(((-log( r )) / l));

				if( x <= uB) { break; }
			}

			x *= 10;
			ioBurstTimes[i] = x;
		}

		while(1)
		{
			r = drand48();
			x = ceil(((-log( r )) / l));

			if( x <= uB) { break; }
		}

		cpuBurstTimes[i] = ceil(x);

	}

	p.cpuBtimes = cpuBurstTimes;
	p.ioBtimes = ioBurstTimes;

	return p;
}

//Resets the processors values so that they are ready to be used for a scheduling algorithms
void resetProcessor(struct process* pA, int nP)
{
	for (int i = 0; i < nP; ++i)
	{
		//pA[i].ioB = pA[i].arrivalTime;
		pA[i].ioB = pA[i].ioBtimes[0];
		pA[i].cpuB = pA[i].cpuBtimes[0];
		pA[i].waitT = 0;
		pA[i].turnT = 0;
		pA[i].burstNum = 0;
	}

}


//Scheduling ALGs

struct sim FCFS(struct process* pA, int nP, int cS)
{

	struct queue q;
	q.size = 0;
	q.lastRpI = 0;
	q.lastIOpI = 0;

	int contextSwitches = 0;//# of context switches that occur
	int cpuUtilization = 0; //Time the cpu is being used
	int totalTurnt_Sum = 0;
	int totalWaitTime = 0;
	int totalCPU_BurstNum = 0;


	int tPs = 0;			//# of terminated processors
	int time = 0;			//time


	bool cpuInUse = false;

	//Place all processors into the READY or IO state depending on arrival time
	for (int i = 0; i < nP; ++i)
	{

		if (pA[i].arrivalTime == 0)
		{
			pA[i].state = "READY";
			q.lastRpI++;
		}

		else
		{
			pA[i].state = "IO";
			q.lastIOpI++;
		}

		pushQ(&q, pA[i]);
		if(pA[i].size == 1)
			printf("Process %c [NEW] (arrival time %d ms) %d CPU burst\n", pA[i].id, pA[i].arrivalTime, pA[i].size);
		else
			printf("Process %c [NEW] (arrival time %d ms) %d CPU bursts\n", pA[i].id, pA[i].arrivalTime, pA[i].size);
	}

	q.lastRpI--; 				//index would be #ofProcessors in READY and CPU state - 1
	q.lastIOpI += q.lastRpI;	//index would be #ofProcessors in IO state + # in READY and CPU state -1

	reorganizeQ(&q, 0);



	printf("time 0ms: Simulator started for FCFS [Q <empty>]\n");

	while(tPs < nP)
	{
		struct queue rQ; //a queue for just the ready section
		rQ.size = 0;
		for (int i = 0; i <= q.lastRpI; ++i)
			if((strcmp(q.readyQueue[i].state, "READY") == 0) || (strcmp(q.readyQueue[i].state, "csIn") == 0))
				pushQ(&rQ, q.readyQueue[i]);


		for (int i = 0; i < q.size; ++i)
		{
			if(time == 0)
			{
				if (strcmp (q.readyQueue[i].state, "READY") == 0)
				{
					printf("time %dms: Process %c arrived; placed on ready queue ", time, q.readyQueue[i].id);
					printQ(rQ);
					//If CPU isn't being used
					if (!cpuInUse)
					{
						q.readyQueue[i].state = "csIn";
						if(i == 0)
							q.readyQueue[i].csT = (cS/2);
						else
							q.readyQueue[i].csT = cS;

						cpuInUse = true;
						contextSwitches++;
						q.readyQueue[0].turnT += (cS/2);

						popQ(&rQ, 0); //Remove process from Ready Queue
					}
				}
			}

			if (time > 0)
			{

				//If current process is being switched out of the cpu,
				if (strcmp (q.readyQueue[i].state, "csOut") == 0)
				{
					q.readyQueue[i].csT--;

					if((cS/2) < q.readyQueue[i].csT)
					{
						tPs++;
						if(tPs == nP && q.readyQueue[i].burstNum >= q.readyQueue[i].size) {break;}
						else {tPs--;}
					}

					//If process finished it Context Switch
					if(q.readyQueue[i].csT <= 0)
					{
						//If process has gone through all burst times for CPU and IO
						//Say that process has been terminated
						if (q.readyQueue[i].burstNum >= q.readyQueue[i].size )
						{
							tPs++;
							q.readyQueue[i].state = "TERMINATED";

							q.lastIOpI--;	//Move down IO section
						}

						//Change process state to IO, and set its current CPU and IO burst times to the next set
						else
						{
							q.readyQueue[i].state = "IO";
						}

						q.lastRpI--;

					}
				}

				//If current process is in the CPU state, begining of the queue
				else if ((strcmp (q.readyQueue[i].state, "CPU") == 0))
				{

					q.readyQueue[i].cpuB--;
					q.readyQueue[i].turnT++;
					cpuUtilization++;

					//If process finished it CPU burst
					if((q.readyQueue[i].cpuB <= 0))
					{
						q.readyQueue[i].state = "csOut";
						cpuInUse = false;
						q.readyQueue[i].turnT += (cS/2);

						q.readyQueue[i].burstNum++; //Move to next index for burst times
						totalCPU_BurstNum++;
						totalTurnt_Sum += q.readyQueue[i].turnT;
						totalWaitTime += q.readyQueue[i].waitT;

						//Process wil terminate
						if (q.readyQueue[i].burstNum >= q.readyQueue[i].size )
						{
							//Print Event
							printf("time %dms: Process %c terminated ", time, q.readyQueue[i].id );
							printQ(rQ);
							q.readyQueue[i].csT = cS/2;
						}

						//Process will go to IO
						else
						{
							if(rQ.size == 0)
								q.readyQueue[i].csT = cS/2;
							else
								q.readyQueue[i].csT = cS;

							q.readyQueue[i].csT = cS/2;

							q.readyQueue[i].cpuB = q.readyQueue[i].cpuBtimes[q.readyQueue[i].burstNum];
							q.readyQueue[i].ioB = q.readyQueue[i].ioBtimes[q.readyQueue[i].burstNum];

							//Print Event
							int bursts = q.readyQueue[i].size - q.readyQueue[i].burstNum;
							if(bursts > 1)
								printf("time %dms: Process %c completed a CPU burst; %d bursts to go ", time, q.readyQueue[i].id,  bursts);
							else
								{printf("time %dms: Process %c completed a CPU burst; %d burst to go ", time, q.readyQueue[i].id,  bursts);}
							printQ(rQ);

							printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms ",
								time, q.readyQueue[i].id, (time + q.readyQueue[i].csT + q.readyQueue[i].ioB ) );
							printQ(rQ);
						}

					}
				}

				else if (strcmp (q.readyQueue[i].state, "csIn") == 0)
				{
					q.readyQueue[i].csT--;
					if(q.readyQueue[i].csT < cS/2)
						popQ(&rQ, 0); //Remove process from Ready Queue

					//If Context Switch is finished
					if (q.readyQueue[i].csT <= 0)
					{

						q.readyQueue[i].state = "CPU";
						//Print Event
						printf("time %dms: Process %c started using the CPU for %dms burst ", time, q.readyQueue[i].id, q.readyQueue[i].cpuB );
						printQ(rQ);
					}
				}

				else if (strcmp (q.readyQueue[i].state, "READY") == 0)
				{
					q.readyQueue[i].waitT++;
					q.readyQueue[i].turnT++;

					//If CPU isn't being used
					if (!cpuInUse)
					{
						q.readyQueue[i].state = "csIn";
						if(i == 0)
							q.readyQueue[i].csT = (cS/2);
						else
							q.readyQueue[i].csT = cS;

						cpuInUse = true;
						contextSwitches++;
						q.readyQueue[i].turnT += (cS/2);
						q.readyQueue[i].waitT += (cS/2);
					}
				}

				else if (strcmp (q.readyQueue[i].state, "IO") == 0)
				{
					q.readyQueue[i].ioB--;
					//Process finished its ioB time, switch to READY state, or in csIn state if cpu is free
					if(q.readyQueue[i].ioB <= 0)
					{
						q.lastRpI++; //Increase range of Ready Section for reorganization
						q.readyQueue[i].turnT = 0;
						q.readyQueue[i].waitT = 0;

						//Print Event
						pushQ(&rQ, q.readyQueue[i]); //Add to ready queue

						//Print Event
						if(q.readyQueue[i].burstNum == 0)
							{ printf("time %dms: Process %c arrived; placed on ready queue ", time, q.readyQueue[i].id );}
						else
							{ printf("time %dms: Process %c completed I/O; placed on ready queue ", time, q.readyQueue[i].id );}

						printQ(rQ);

						//If CPU isn't being used
						if (!cpuInUse)
						{
							q.readyQueue[i].state = "csIn";
							if(rQ.size == 1 && strcmp(q.readyQueue[0].state, "csOut") != 0)
								q.readyQueue[i].csT = (cS/2);
							else
								q.readyQueue[i].csT = cS;

							cpuInUse = true;
							contextSwitches++;
							q.readyQueue[0].turnT += (cS/2);
							popQ(&rQ, 0); //Remove process from Ready Queue
						}
						else
							{q.readyQueue[i].state = "READY";}
					}
				}
			}
		}

		reorganizeQ(&q, 0);

		if(tPs == nP)
		{
			printf("time %dms: Simulator ended for FCFS ", time);
			printQ(rQ);
			break;
		}
		time++;
	}

	float avgCPUb = (float)cpuUtilization/(float)totalCPU_BurstNum;
	float avgWaitT = (float)totalWaitTime/(float)totalCPU_BurstNum;
	float avgTurnT = (float)totalTurnt_Sum/(float)totalCPU_BurstNum;
	float per = ((float)cpuUtilization/(float)time)*100;

	struct sim s;
	s.avg_CPUbt = avgCPUb;
	s.avg_Wt = avgWaitT;
	s.avg_Tt = avgTurnT;
	s.numOfCS = contextSwitches;
	s.numOfPre = 0;
	s.cpuUsePer = per;

	return s;
}

// struct sim RR(struct process* pA, int nP, int cS, int tS)
// {
//
// }


void SJF(struct process* pA, int nP, int cS, double alpha)
{
	struct queue q;
	q.size = 0;
	q.lastRpI = 0;
	q.lastIOpI = 0;

	int tPs = 0;
	int time = 0;
	bool cpuInUse = false;
	int totalCPU_BurstNum = 0;
	int totalTurnt_Sum = 0;
	int totalWaitTime = 0;
	int contextSwitches = 0;
	int cpuUtilization = 0;

	for(int i = 0; i < nP; ++i){
		if(pA[i].arrivalTime == 0){
			pA[i].state = "READY";
		}else{
			pA[i].state = "IO";
		}

		pushQ(&q, pA[i]);

		printf("Process %c [NEW] (arrvial time %d ms) %d CPU burst%s\n", pA[i].id, pA[i].arrivalTime, pA[i].size, (pA[i].size == 1) ? "" : "s");
		reorganizeQ(&q, 1);
	}


	printf("time 0ms: Simulator started for SJF [Q <empty>]\n");
	// while(time < 10000){
	while(tPs < nP){
		//checking if new processes have arrived
		struct queue rQ;
		rQ.size = 0;
		for(int i = 0; i < q.size; ++i){
			if((strcmp(q.readyQueue[i].state, "READY") == 0) || (strcmp(q.readyQueue[i].state, "csIn") == 0)){
				insertQ(&rQ, q.readyQueue[i]);
			}
		}

		for(int i = 0; i < q.size; ++i){
			if(time == 0){
				if(strcmp(q.readyQueue[i].state, "READY") == 0){
					printf("time %dms: Process %c arrived; placed on ready queue ", time, q.readyQueue[i].id);
					printQ(rQ);

					if(!cpuInUse){
						q.readyQueue[i].state = "csIn";
						if(i == 0){
							q.readyQueue[i].csT = (cS/2);
						}else{
							q.readyQueue[i].csT = cS;
						}

						cpuInUse = true;
						contextSwitches++;
						q.readyQueue[i].turnT += (cS/2);

						popQ(&rQ, 0);
					}
				}
			}

			if(time > 0){
				if(strcmp(q.readyQueue[i].state, "csOut") == 0){
					q.readyQueue[i].csT--;

					if((cS/2) < q.readyQueue[i].csT){
						tPs++;
						if(tPs == nP && q.readyQueue[i].burstNum >= q.readyQueue[i].size) {
							break;
						}else{
							tPs--;
						}
					}

					if(q.readyQueue[i].csT <= 0) {
						if(q.readyQueue[i].burstNum >= q.readyQueue[i].size){
							tPs++;
							q.readyQueue[i].state = "TERMINATED";
						}
						else{
							q.readyQueue[i].state = "IO";
						}
					}
				}

				else if(strcmp(q.readyQueue[i].state, "CPU") == 0){
					q.readyQueue[i].cpuB--;
					q.readyQueue[i].turnT++;
					cpuUtilization++;

					if(q.readyQueue[i].cpuB <= 0) {
						q.readyQueue[i].state = "csOut";
						cpuInUse = false;
						q.readyQueue[i].turnT += (cS/2);
						q.readyQueue[i].burstNum++;
						totalCPU_BurstNum++;
						totalTurnt_Sum += q.readyQueue[i].turnT;
						totalWaitTime += q.readyQueue[i].waitT;

						if(q.readyQueue[i].burstNum >= q.readyQueue[i].size){
							printf("time %dms: Process %c terminated ", time, q.readyQueue[i].id);
							printQ(rQ);
							q.readyQueue[i].csT = cS/2;
						}else{
							if(rQ.size == 0){
								q.readyQueue[i].csT = cS/2;
							}else{
								q.readyQueue[i].csT = cS;
							}

							q.readyQueue[i].csT = cS/2;


							q.readyQueue[i].cpuB = q.readyQueue[i].cpuBtimes[q.readyQueue[i].burstNum];
							q.readyQueue[i].ioB = q.readyQueue[i].ioBtimes[q.readyQueue[i].burstNum];

							int bursts = q.readyQueue[i].size - q.readyQueue[i].burstNum;
							printf("time %dms: Process %c completed a CPU burst; %d burst%s to go ", time, q.readyQueue[i].id, bursts, (bursts == 1) ? "" : "s");
							printQ(rQ);

							printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms ",
								time, q.readyQueue[i].id, (time + q.readyQueue[i].csT + q.readyQueue[i].ioB));
							printQ(rQ);
						}
					}
				}

				else if(strcmp(q.readyQueue[i].state, "csIn") == 0){
					q.readyQueue[i].csT--;
					if(q.readyQueue[i].csT < cS/2){
						popQ(&rQ, 0);
					}

					if(q.readyQueue[i].csT <= 0){
						q.readyQueue[i].state = "CPU";
						printf("time %dms: Process %c started using the CPU for %dms burst ", time, q.readyQueue[i].id, q.readyQueue[i].cpuB);
						printQ(rQ);
					}
				}

				else if(strcmp(q.readyQueue[i].state, "READY") == 0){
					q.readyQueue[i].waitT++;
					q.readyQueue[i].turnT++;

					if(!cpuInUse){
						q.readyQueue[i].state = "csIn";
						if(i == 0){
							q.readyQueue[i].csT = (cS/2);
						}else{
							q.readyQueue[i].csT = cS;
						}

						cpuInUse = true;
						contextSwitches++;
						q.readyQueue[i].turnT += (cS/2);
						q.readyQueue[i].waitT += (cS/2);
					}
				}

				else if(strcmp(q.readyQueue[i].state, "IO") == 0){
					q.readyQueue[i].ioB--;

					if(q.readyQueue[i].ioB <= 0){
						q.readyQueue[i].turnT = 0;
						q.readyQueue[i].waitT = 0;

						pushQ(&rQ, q.readyQueue[i]);
						if(q.readyQueue[i].burstNum == 0){
							printf("time %dms: Process %c arrived; placed on ready queue ", time, q.readyQueue[i].id);
						}else{
							printf("time %dms: Process %c completed I/O; placed on ready queue ", time, q.readyQueue[i].id);
						}
						printQ(rQ);

						if(!cpuInUse){
							q.readyQueue[i].state = "csIn";
							if(rQ.size == 1 && strcmp(q.readyQueue[0].state, "csOut") != 0){
								q.readyQueue[i].csT = (cS/2);
							}else{
								q.readyQueue[i].csT = cS;
							}

							cpuInUse = true;
							contextSwitches++;
							q.readyQueue[i].turnT += (cS/2);
							popQ(&rQ, 0);
						}else{
							q.readyQueue[i].state = "READY";
						}
					}
				}
			}
		}
		reorganizeQ(&q, 1);

		if(tPs >= nP){
			printf("time %dms: Simulator ended for SJF ", time);
			printQ(rQ);
			break;
		}
		time++;
	}

}


//Print Ready section of queue
void printQ(struct queue qu)
{
	printf("[Q");

	//If Queue is empty
	if (qu.size <= 0)
		{ printf(" <empty>]\n");}

	else
	{
		for (int i = 0; i < qu.size; ++i)
				{ printf(" %c", qu.readyQueue[i].id);}
		printf("]\n");
	}

}

//Adds p to the end of the queue
void pushQ(struct queue * q, struct process p )
{
	q->readyQueue[q->size] = p;
	q->size++;
}

//Removes p to from queue
void popQ(struct queue * q, int index)
{
	for (int i = index; i < q->size-1; ++i)
		{ q->readyQueue[i] = q->readyQueue[i+1];}

	q->size--;
}

//removes and returns first item in the Queue
struct process frontQ(struct queue * q){
		struct process p = q->readyQueue[0];
		for(int i = 0; i < q->size-1; ++i){
			q->readyQueue[i] = q->readyQueue[i+1];
		}
		return p;
}

//inserts p into queue at index i
void insertQ(struct queue * q, struct process p){
	if(q->size == 0){
		pushQ(q, p);
	}else{
		for(int i = 0; i < q->size; ++i){
			if(p.tau > q->readyQueue[i].tau){
				for(int k = q->size; k >= i; --k){
					q->readyQueue[k] = q->readyQueue[k-1];
				}
				q->readyQueue[i] = p;

				break;
			}
		}
	}
	q->size++;
}



//calculates tau_(i+1)
int exp_avg(int tau, int t, double alpha){
		return ceil((tau*alpha) + (t*(1-alpha)));
}

//Compares two processes based on their states,
//and if they have the same state and equal bursts, (I.E. a tie)
//Returns 0 if p1 has higher priority than p2, returns 1 for vice-versa
int sortingP(struct process p1, struct process p2)
{
	int p1v = 0;
	int p2v = 0;


	if (strcmp(p1.state, "csOut") == 0)
		{ p1v = 0; }
	else if (strcmp(p1.state, "CPU") == 0)
		{ p1v = 1; }
	else if (strcmp(p1.state, "csIn") == 0)
		{ p1v = 2; }
	else if (strcmp(p1.state, "READY") == 0)
		{ p1v = 3; }
	else if (strcmp(p1.state, "IO") == 0)
		{ p1v = 4; }
	else if (strcmp(p1.state, "TERMINATED") == 0)
		{ p1v = 5; }

	if (strcmp(p2.state, "csOut") == 0)
		{ p2v = 0; }
	else if (strcmp(p2.state, "CPU") == 0)
		{ p2v = 1; }
	else if (strcmp(p2.state, "csIn") == 0)
		{ p2v = 2; }
	else if (strcmp(p2.state, "READY") == 0)
		{ p2v = 3; }
	else if (strcmp(p2.state, "IO") == 0)
		{ p2v = 4; }
	else if (strcmp(p2.state, "TERMINATED") == 0)
		{ p2v = 5; }

	//breaks ties for IO sections
	if ((p1v == p2v) && (p1v == 4))
	{
		if(p1.ioB == p2.ioB)
			if (p1.id > p2.id)
				{ return 1; }
	}


	else if (p1v > p2v)
		{ return 1;}

	return 0;
}

//Reorganizes a Queue based on the current alg, before reorganizing READY and IO sections of theb Queue to fix ties
//alg: 0 = FCFS, 1 = SJF, 2 = SRT, 3 = RR
void reorganizeQ(struct queue * q, int alg)
{
	if(alg == 0)
	{
		for (int i = 0; i <= q->lastIOpI; ++i)
		{
			struct process p = q->readyQueue[i];
			int index = i;

			for (int j = i; j < q->size; ++j)
			{
				if (sortingP(p, q->readyQueue[j]) == 1)
				{
					q->readyQueue[index] = q->readyQueue[j];
					q->readyQueue[j] = p;
					index = j;
					//break;
				}
			}
		}

	}

	if(alg == 1)
	{
		for(int i = 0; i < q->size-1; ++i){
			struct process a = q->readyQueue[i];
			struct process b = q->readyQueue[i+1];
			if(a.tau == b.tau && (int)a.id > (int)b.id){
				struct process temp = b;
				q->readyQueue[i+1] = q->readyQueue[i];
				q->readyQueue[i] = temp;
			}
		}
	}

}
