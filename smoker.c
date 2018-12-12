/*
** Filename: smoker.c
** Author: Blaize K. Rodrigues
** Date: March 20th, 2018
** Class: CS370 University of Nevada - Las Vegas
*/
#include "pthread.h"
#include "semaphore.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"


volatile int smokeCount;
sem_t table;
sem_t agent;
sem_t smokers[3];

void* agentThdFunc(){


int i,j;
int values[2]; //array of values 0 and 2 to be used in dead smoker checking
values[0] = 0;
values[1] = 2;
int randNum;
int type0count = 0;
int type1count = 0;
int type2count = 0;


for(i=0; i<= 3*smokeCount; i++){
//statements to check if any of the smokers are dead
if (type0count < smokeCount && type1count < smokeCount && type2count < smokeCount)
{
	randNum = rand()%3;
}else if(type0count==smokeCount && type1count < smokeCount && type2count < smokeCount)
{
	randNum = (rand()%2) + 1;
}else if(type0count< smokeCount && type1count == smokeCount && type2count < smokeCount)
{
	randNum = values[(rand()%2)];
}else if(type0count< smokeCount && type1count < smokeCount && type2count == smokeCount){
	randNum = rand()%2;
}else if(type0count < smokeCount && type1count == smokeCount && type2count == smokeCount){
	randNum = 0;
}else if(type0count == smokeCount && type1count < smokeCount && type2count == smokeCount){
	randNum = 1;
}
else if(type0count == smokeCount && type1count == smokeCount && type2count < smokeCount){
	randNum = 2;
}else{
	exit(1);
}

sem_wait(&table); //decrements the table (table is empty)

if(randNum == 0){
	printf("Agent produced tobacco & paper\n");
	 type0count++;
		
}else if(randNum == 1){
	printf("Agent produced matches & tobacco\n");
	type1count++;
	
}else{
	printf("Agent produced matches & paper\n");
	type2count++;
	
}

sem_post(&table); //increment the table by 1
sem_post(&smokers[randNum]); // wake appropriate smoker
sem_wait(&agent); // agent sleeps

	}

return NULL;
}


void*  smokersThdFunc(void *arg){

long type = (long)arg;
int i;
int type0count = 0;
int type1count = 0;
int type2count = 0;

printf("Smoker %d starts...\n",type );


for(i=0; i<= smokeCount ; ++i){
	sem_wait(&smokers[type]); //wait until ready
	sem_wait(&table); // decrement table

	usleep(rand()%1500000);
		switch(type){
		case 0: printf("\033[0;31mSmoker %d completed smoking\033[0m\n", 0); // red
			type0count++;
				if(type0count == smokeCount){
			printf("Smoker %d died of cancer...\n",0 );
	}
			break;
		case 1: printf("\033[0;32mSmoker %d completed smoking\033[0m\n", 1); // green
			type1count++;
					if(type1count == smokeCount){
			printf("Smoker %d died of cancer...\n",1 );
	}
			break;
	
		case 2: printf("\033[0;34mSmoker %d completed smoking\033[0m\n", 2); // blue
			type2count++;
				if(type2count == smokeCount){
			printf("Smoker %d died of cancer...\n",2 );
	}
			break;
		default: printf("This line should not be printed\n");
			break;
		}
	sem_post(&table);//increment table
	sem_post(&agent);//wake up agent

}
return NULL;
}




int main(int argc, char *argv[]){
char *input[2];
int i;
long j =0;


if(argc < 2){
	printf("Error: Please provide a count value. Example: ./smoker '10' \n");
	exit(1);
}

for(i=0;i<2;i++){
	input[i] = argv[i];
}

smokeCount = atoi(input[1]);

if (smokeCount < 3 || smokeCount > 10)
{
	printf("Error: Please provide a count value between 3 and 10 \n");
	exit(1);
}

//Initialize semaphores
for(i=0; i < 3; ++i){
	sem_init(&smokers[i],0,0);
}
sem_init(&table,0,1);
sem_init(&agent,0,0);

pthread_t smoker_threads[3];
pthread_t agent_thread;



for ( i = 0; i < 3; ++i)
{ 
	pthread_create(&smoker_threads[i],NULL,&smokersThdFunc,(void*)(long)i);
}

//create one agent thread
pthread_create(&agent_thread, NULL,&agentThdFunc,(void*)(long)j);

for ( i = 0; i < 3; ++i)
{ 
pthread_join(smoker_threads[i],NULL); //destroy the threads
}


pthread_join(agent_thread,NULL); //destroy agent thread



	return 0;
}

