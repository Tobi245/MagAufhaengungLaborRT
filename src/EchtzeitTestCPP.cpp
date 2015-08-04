//============================================================================
// Name        : EchtzeitTestCPP.cpp
// Author      : Tobias Stoffel
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sched.h>
#include <sys/mman.h>
#include <wiringPi.h>

#define NSEC_PER_SEC 1000000000

using namespace std;

int main(void)
{
	wiringPiSetup();
	pinMode(0, OUTPUT);
	for(;;)
	{
		digitalWrite(0, HIGH); delay (50);
		digitalWrite(0, LOW); delay (50);
	}
	return 0;
//	
//	struct sched_param 	param;
//	struct timespec		next;
//	int					interval;
//	
//	param.sched_priority 	=	99;
//	interval				=	30000000;
//	
//	if(sched_setscheduler(0, SCHED_FIFO, &param) < 0)
//	{
//		cout << "Main: Fehler bei sched_setscheduler!" << endl;
//		return EXIT_FAILURE;
//	}
//	
//	if(mlockall(MCL_CURRENT | MCL_FUTURE) < 0)
//	{
//		cout << "Main: Fehler bei mlockall!" << endl;
//		return EXIT_FAILURE;
//	}
//	
//	clock_gettime(CLOCK_MONOTONIC, &next);
//	
//	while(1)
//	{
//		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
//		next.tv_nsec += interval;
//		
//		while(next.tv_nsec >= NSEC_PER_SEC)
//		{
//			next.tv_nsec -= NSEC_PER_SEC;
//			++next.tv_sec;
//		}
//		
//		
//	}
//	return EXIT_SUCCESS;
}
