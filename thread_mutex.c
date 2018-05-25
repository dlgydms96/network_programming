#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

void* thread_main1(void* arg)
{
	int i;
	for(i=1;i<= 10;i+=2){
		pthread_mutex_lock(&mutex1);
		printf("%d\n",i);
		pthread_mutex_unlock(&mutex2);
	}
	pthread_exit(NULL);
}

void* thread_main2(void* arg)
{
	int i;
	for(i=2;i<= 10;i+=2){
		pthread_mutex_lock(&mutex2);
		printf("%d\n",i);
		pthread_mutex_unlock(&mutex1);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t t_id1, t_id2;

	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);

	pthread_mutex_lock(&mutex2);

	int result;

	result=pthread_create(&t_id1, NULL, thread_main1, NULL);
	result=pthread_create(&t_id2, NULL, thread_main2, NULL);
	if(result != 0){
		errno= result;
		perror("create");
	}

	pthread_join(t_id1,NULL);
	pthread_join(t_id2,NULL);

	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	puts("End of program");
	return 0;
}

