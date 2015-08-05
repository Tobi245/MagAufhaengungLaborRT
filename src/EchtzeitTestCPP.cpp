//============================================================================
// Name        : EchtzeitTestCPP.cpp
// Author      : Tobias Stoffel
// Version     :
// Copyright   : Your copyright notice
// Description : Testprogramm zur Echtzeit in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#include <unistd.h>
#include <iostream>
#include <sched.h>
//#include <wiringPi.h>

#define BCM2708_PERI_BASE        0x3F000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#define NSEC_PER_SEC 1000000000

int  mem_fd;
void *gpio_map;
 
// I/O access
volatile unsigned *gpio;
 
// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
 
#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
 
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
 
#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock
 
void setup_io();
static inline void tsnorm(struct timespec *parameter);

using namespace std;

int main(void)
{
//	wiringPiSetup();
//	pinMode(0, OUTPUT);
//	for(;;)
//	{
//		digitalWrite(0, HIGH); delay (50);
//		digitalWrite(0, LOW); delay (50);
//	}
	static int toggle	=	1;
	
	struct sched_param 	param;
	struct timespec		next, interval;
	
	interval.tv_sec		=	0;
	interval.tv_nsec	=	50000;
	
	param.sched_priority 	=	99;
	if(sched_setscheduler(0, SCHED_FIFO, &param) < 0)
	{
		cout << "Main: Fehler bei sched_setscheduler!" << endl;
		return EXIT_FAILURE;
	}
	
	if(mlockall(MCL_CURRENT | MCL_FUTURE) < 0)
	{
		cout << "Main: Fehler bei mlockall!" << endl;
		return EXIT_FAILURE;
	}
	
	clock_gettime(CLOCK_MONOTONIC, &next);
	
	next.tv_sec 	+=	interval.tv_sec;
	next.tv_nsec 	+=	interval.tv_nsec;
	
	int g;
	
	// Set up gpi pointer for direct register access
	setup_io();
	
	// Switch GPIO 7..11 to output mode
	
	/************************************************************************\
	* You are about to change the GPIO settings of your computer.          *
	* Mess this up and it will stop working!                               *
	* It might be a good idea to 'sync' before running this program        *
	* so at least you still have your code changes written to the SD-card! *
	\************************************************************************/
	
	// Set GPIO pins 7-11 to output
	for (g=7; g<=11; g++)
	{
	INP_GPIO(g); // must use INP_GPIO before we can use OUT_GPIO
	OUT_GPIO(g);
	}
	
	while(1)
	{
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
		next.tv_sec += interval.tv_sec;
		next.tv_nsec += interval.tv_nsec;
		tsnorm(&next);
	
		if(toggle)
			GPIO_SET = 1<<7;
		else
			GPIO_CLR = 1<<7;
		toggle = !toggle;
		
	}
	return EXIT_SUCCESS;
}
 
//
// Set up a memory regions to access GPIO
//
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }
 
   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );
 
   close(mem_fd); //No need to keep mem_fd open after mmap
 
   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }
 
   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
 
 
} // setup_io

static inline void tsnorm(struct timespec *ts)
{
	while (ts->tv_nsec >= NSEC_PER_SEC)
	{
		ts->tv_nsec -= NSEC_PER_SEC;
		ts->tv_sec++;
	}
} // ts norm
