#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX 2147483647
int maximum = 0;
int minimum = 2147483647;
int maxbuf[4];
int minbuf[4];
int maxbufindex = 0;
int minbufindex = 0;
int arr[1024];
void init(int[],int);
void *producer(void*);
void *consumer();
struct producer_para
{
	int start;
	int end;
};
sem_t max_buffer_full;
sem_t min_buffer_full;
sem_t max_buffer_mutex;
sem_t min_buffer_mutex;
int main()
{
	int len = sizeof(arr)/sizeof(arr[0]);
	int i=0;
	time_t t;
	srand((unsigned)time(&t));
	pthread_t p1,p2,p3,p4,c1,c2;
	struct producer_para pro1,pro2,pro3,pro4;
	//printf("len = %d\n",len);
	init(arr,len);
	sem_init(&max_buffer_mutex,0,1);
	sem_init(&min_buffer_mutex,0,1);
	sem_init(&max_buffer_full,0,0);
	sem_init(&min_buffer_full,0,0);
	pro1.start = 0;		pro1.end = 255;		
	pro2.start = 256;	pro2.end = 511;		
	pro3.start = 512;	pro3.end = 767;		
	pro4.start = 768;	pro4.end = 1023;	
	pthread_create(&p1,NULL,producer,&pro1);
	pthread_create(&p2,NULL,producer,&pro2);
	pthread_create(&p3,NULL,producer,&pro3);
	pthread_create(&p4,NULL,producer,&pro4);
	pthread_create(&c1,NULL,consumer,NULL);
	pthread_create(&c2,NULL,consumer,NULL);
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	pthread_join(p3,NULL);
	pthread_join(p4,NULL);
	pthread_join(c1,NULL);
	pthread_join(c2,NULL);
	printf("Success! maximum = %d	minimum = %d\n",maximum,minimum);
	return 0;
	
}
void init(int arr[],int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		arr[i] = rand() % (MAX);
		//printf("arr[%d] = %d\n",i,arr[i]);
	}
}
void *producer(void* para)
{
	int max = 0;
	int min = 2147483647;
	int i;
	struct producer_para *p; 
	p = (struct producer_para *)para;
	for(i=p->start;i<=p->end;i++)
	{
		if(arr[i] < min)
		{
			min = arr[i];		
		}
		if(arr[i] > max)
		{
			max = arr[i];
		}
		//printf("buf[%d] = %d\n",i,arr[i]);
	}
	printf("Temporary max = %d and min = %d\n",max,min);
	//max
	sem_wait(&max_buffer_mutex);
		maxbuf[maxbufindex] = max;
		printf("Producer: Put %d into max-buffer at %d\n",max,maxbufindex);
		maxbufindex++;
	sem_post(&max_buffer_mutex);
	sem_post(&max_buffer_full);
	//min
	sem_wait(&min_buffer_mutex);
		minbuf[minbufindex] = min;
		printf("Producer: Put %d into min-buffer at %d\n",min,minbufindex);
		minbufindex++;
	sem_post(&min_buffer_mutex);
	sem_post(&min_buffer_full);
}
void *consumer()
{
	int i=0;
	//max
	sem_wait(&max_buffer_full);
	sem_wait(&max_buffer_mutex);
		for(i=0;i<maxbufindex;i++)
		{
			if(maxbuf[i] > maximum)
			{
				maximum = maxbuf[i];
				printf("Updated! maximum = %d\n",maximum);
			}
		}
	sem_post(&max_buffer_mutex);
	//min
	sem_wait(&min_buffer_full);
	sem_wait(&min_buffer_mutex);
		for(i=0;i<minbufindex;i++)
		{
			if(minbuf[i] < minimum)
			{
				minimum = minbuf[i];
				printf("Updated! minimum = %d\n",minimum);
			}
		}
	sem_post(&min_buffer_mutex);			
}
