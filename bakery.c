/*
** Title: Bakery.c
** Description: Implementation of the Bakery Algorithm in C
** Author: Blaize K. Rodrigues
** Date: March 13th, 2018
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#define THD_MIN 1
#define THD_MAX 256


volatile bool entering[THD_MAX];
volatile int tickets[THD_MAX];
volatile int resource;
volatile int threadCount;

int getThreadCount(int numarg, char *input[]){
char*thread[2]; // array to cast input to

if(numarg < 2 || numarg > 2){
	printf("Error: Please provide a valid thread count.\n");
	return -1;
}

else if (numarg >1 )
{
thread[1] = input[1];
threadCount = atoi(thread[1]);
	if(threadCount < THD_MIN){
		printf("Error: Please provide a thread count greater than 1 and less than 256.\n");
	return -1;

	}
	if (threadCount > THD_MAX)
	{
		printf("Error: Please provide a thread count greater than 1 and less than 256.\n");
	exit(1);
	}
}

}


void lock(int i){ //bakery algorithm following the pseudo-code from the assignement

entering[i]= true; //set entering to true

int maxTicket = 0; //declare max ticket variable
int k;
for(k = 0; k<threadCount; ++k){

maxTicket = tickets[k] > maxTicket ? tickets[k] : maxTicket;  //sets maxTicket to the maximum ticket value
}

tickets[i] = 1 + maxTicket; //the element in the array gets the value of max ticket + 1

entering[i] = false; //reset entering to false


int j;
for(j = 0; j <= threadCount; ++j){

	while(entering[j]){ 
	/*do nothing*/
	}

	while( tickets[j]!=0 && (tickets[j]< tickets[i]) || (tickets[j] == tickets[i]) && (j < i)) {
		/*do nothing*/
		}
	}
}


void unlock(int i){

	tickets[i] = 0; //sets tickets to 0
}


void useResource(int threadNum){
if(resource != -1){
printf("Error: resource acquired by <%d> but still in use by <%d> \n",threadNum, resource );
exit(1);
}

	resource = threadNum;
	printf("Thread <%d> using resource...\n", threadNum);
	sleep(1);
	resource = -1; //reset resource to -1

}

void *thdFunction(void * val){
long thread = (long) val; //sets the passed value to a new variable of type long 
	lock(thread); 
	useResource(thread);
	unlock(thread);
return NULL;
}





int main(int argc, char *argv[]){

getThreadCount(argc, argv); // calls getThreadcount function to check thread count

*entering = (bool*) malloc(sizeof(entering)); //dynamically allocate memory for entering
memset((void*)tickets,0, sizeof(tickets));// dynamically allocate  memory for tickets. 
										//Had to use memset cause I couldn't figure out how to 
										//dynamically allocate tickets with malloc. 



pthread_t threads[threadCount];// create a p thread array called threads per assignment 

int i;
for(i =0; i < threadCount; ++i){
	
	entering[i] = false; //set entering to false
	tickets[i] = 0; // set tickets to 0
	resource = -1; // set resource value to -1

	pthread_create(&threads[i], NULL, &thdFunction, (void*)((long)i)); //creates pthread and calls thdfunction
	pthread_join(threads[i], NULL);	//suspends execution of thread call until target thread terminates

	/* NOTE: I had to put pthread_join in here in order for it to run without getting killed in CARDIAC. See note below */
}

/* NOTE: uncomment this section to run how I had originally intended. Process would Kill while running
in CARDIAC SSH, but would work in JAVA SSH.

int j;

for (j=0; j <threadCount; j++){

	pthread_join(threads[j], NULL);  //suspends execution of thread call until target thread terminates
}
*/


pthread_exit(NULL);//terminates the thread


return EXIT_SUCCESS;



}


