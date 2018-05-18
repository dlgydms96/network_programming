#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

int thread_result;

void* thread_main(void* arg)
{
	int i;
	int cnt = *((int *)arg);
	for(i=0;i<cnt;i++){
		sleep(1);
		puts("Running thread");
	}
	thread_result =i;
	return (void*)&thread_result;
}

int main(int argc, char *argv[])
{
	pthread_t t_id;
	int arg=10;
	int result;
	int *p;

	result=pthread_create(&t_id, NULL, thread_main, (void*)&arg);
	if(result != 0){
		errno= result;
		perror("create");
	}
	pthread_join(t_id, (void*)&p);
	printf("%d\n",*p);

	//sleep(4);
	puts("End of program");
	return 0;
}

