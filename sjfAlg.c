struct sim SJF(struct process* pA, int nP, int cS, double alpha)
{
	struct queue q;
	q.size = 0;
	q.lastRpI = 0;
	q.lastIOpI = 0;

	int tPs = 0;
	int time = 0;
	bool cpuInUse = false;
	bool newcsIn = false;
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

		printf("Process %c [NEW] (arrival time %d ms) %d CPU burst%s (tau %dms)\n", pA[i].id, pA[i].arrivalTime, pA[i].size, (pA[i].size == 1) ? "" : "s", pA[i].tau);

	}
	reorganizeQ(&q, 0);
	reorganizeQ(&q, 1);


	printf("time 0ms: Simulator started for SJF [Q <empty>]\n");
	// while(time < 10000){
	while(tPs < nP){
		//checking if new processes have arrived
		newcsIn = false;
		struct queue rQ;
		rQ.size = 0;
		for(int i = 0; i < q.size; ++i){
			if((strcmp(q.readyQueue[i].state, "READY") == 0) || (strcmp(q.readyQueue[i].state, "csIn") == 0)){
				pushQ(&rQ, q.readyQueue[i]);
			}
		}

		for(int i = 0; i < q.size; ++i){
			if(time == 0){
				if(strcmp(q.readyQueue[i].state, "READY") == 0){
					printf("time %dms: Process %c (tau %dms) arrived; placed on ready queue ", time, q.readyQueue[i].id, q.readyQueue[i].tau);
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
							printf("time %dms: Process %c (tau %dms) completed a CPU burst; %d burst%s to go ", time, q.readyQueue[i].id, q.readyQueue[i].tau, bursts, (bursts == 1) ? "" : "s");
							printQ(rQ);

							int tau = q.readyQueue[i].tau;
							int t = q.readyQueue[i].cpuBtimes[q.readyQueue[i].burstNum-1];

							q.readyQueue[i].tau = exp_avg(tau, t, alpha);
							printf("time %dms: Recalculated tau (%dms) for process %c ", time, q.readyQueue[i].tau, q.readyQueue[i].id);
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
						printf("time %dms: Process %c (tau %dms) started using the CPU for %dms burst ", time, q.readyQueue[i].id, q.readyQueue[i].tau, q.readyQueue[i].cpuB);
						printQ(rQ);
					}
				}

				else if(strcmp(q.readyQueue[i].state, "READY") == 0){
					q.readyQueue[i].waitT++;
					q.readyQueue[i].turnT++;

					if(!cpuInUse){
						q.readyQueue[i].state = "csIn";
						newcsIn = true;
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
						insertQ(&rQ, q.readyQueue[i]);
						if(q.readyQueue[i].burstNum == 0){
							printf("time %dms: Process %c (tau %dms) arrived; placed on ready queue ", time, q.readyQueue[i].id, q.readyQueue[i].tau);
						}else{
							printf("time %dms: Process %c (tau %dms) completed I/O; placed on ready queue ", time, q.readyQueue[i].id, q.readyQueue[i].tau);
						}
						printQ(rQ);
						q.readyQueue[i].state = "READY";
						if(!cpuInUse){
							q.readyQueue[i].state = "csIn";
							newcsIn = true;
							if(rQ.size == 1 && strcmp(q.readyQueue[0].state, "csOut") != 0){
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
				// }else if(newcsIn){
				// 	if(strcmp(q.readyQueue[0].state, "csIn") == 0){
				// 		if(sorting_SJF(q.readyQueue[0], q.readyQueue[i]) == 1){
				// 			q.readyQueue[0].state = "READY";
				// 			q.readyQueue[i].state = "csIn";
				// 			q.readyQueue[0].turnT-= (cS/2);
				// 			q.readyQueue[i].csT = q.readyQueue[0].csT;
				// 			struct process p = q.readyQueue[i];
				// 			for(int j = i; j > 0; --j){
				// 				q.readyQueue[j] = q.readyQueue[j-1];
				// 			}
				// 			q.readyQueue[0] = p;
				// 		}
				// 	}else if(strcmp(q.readyQueue[1].state, "csIn") == 0){
				// 		if(sorting_SJF(q.readyQueue[1], q.readyQueue[i]) == 1){
				// 			q.readyQueue[1].state = "READY";
				// 			q.readyQueue[i].state = "csIn";
				// 			q.readyQueue[1].turnT-= (cS/2);
				// 			q.readyQueue[i].csT = q.readyQueue[1].csT;
				// 			struct process p = q.readyQueue[i];
				// 			for(int j = i; j > 1; --j){
				// 				q.readyQueue[j] = q.readyQueue[j-1];
				// 			}
				// 			q.readyQueue[1] = p;
				// 		}
				// 	}
				// }
			}
		}
		reorganizeQ(&q, 0);
		reorganizeQ(&q, 1);

		if(tPs >= nP){
			printf("time %dms: Simulator ended for SJF ", time);
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

// ReorganizeQ

	if(alg == 1)
	{
			for (int i = 0; i < q->size; ++i)
			{
				struct process p = q->readyQueue[i];
				int index = i;

				for (int j = i; j < q->size; ++j)
				{
					if(strcmp(q->readyQueue[j].state, "READY") == 0 && strcmp(p.state, "READY") == 0){
						if (sorting_SJF(p, q->readyQueue[j]) == 1)
						{
							q->readyQueue[index] = q->readyQueue[j];
							q->readyQueue[j] = p;
							index = j;
							//break;
						}
					}

				}
			}
			// if(strcmp(q->readyQueue[i].state, "READY") == 0 && strcmp(q->readyQueue[i+1].state, "READY") == 0){
			// 	if(sorting_SJF(q->readyQueue[i], q->readyQueue[i+1])){
			//
			// 	}
			// }
			// 	if(q->readyQueue[i].tau > q->readyQueue[i+1].tau){
			// 		struct process temp = q->readyQueue[i+1];
			// 		q->readyQueue[i+1] = q->readyQueue[i];
			// 		q->readyQueue[i] = temp;
			// 	}
			// 	if(q->readyQueue[i].tau == q->readyQueue[i+1].tau){
			// 		if(q->readyQueue[i].id > q->readyQueue[i+1].id){
			// 			struct process temp = q->readyQueue[i+1];
			// 			q->readyQueue[i+1] = q->readyQueue[i];
			// 			q->readyQueue[i] = temp;
			// 		}
			// 	}
			// }
		}


int sorting_SJF(struct process p1, struct process p2){
	if(p1.tau > p2.tau) return 1;
	else if(p1.tau == p2.tau && p1.id > p2.id) return 1;
	else return 0;
}


void insertQ(struct queue * q, struct process p){
	// printf("%c - %d\t%c - %d", p.id, p.tau, q->readyQueue[q->size-1].id, q->readyQueue[q->size-1].tau);
	if(q->size == 0){
		pushQ(q, p);
	}
	// else if(p.tau >= q->readyQueue[q->size-1].tau){
	// 	pushQ(q, p);
	// }
	else{
		for(int i = 0; i < q->size; ++i){
			if(sorting_SJF(p, q->readyQueue[i]) == 0){
				for(int j = q->size; j > i; --j){
					q->readyQueue[j] = q->readyQueue[j-1];
				}
				q->readyQueue[i] = p;
				q->size++;
				return;
			}
		}
		pushQ(q, p);
	}
}

void tie_breaker(struct queue * q){
	for(int i = 0; i < q->size-1; i++){
		if(q->readyQueue[i].tau == q->readyQueue[i+1].tau){
			if(q->readyQueue[i].id > q->readyQueue[i+1].id){
				struct process temp = q->readyQueue[i+1];
				q->readyQueue[i+1] = q->readyQueue[i];
				q->readyQueue[i] = temp;
			}
		}
	}
}



//calculates tau_(i+1)
int exp_avg(int tau, int t, double alpha){
		return ceil((t*alpha) + (tau*(1-alpha)));
}
