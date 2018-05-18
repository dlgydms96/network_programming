#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

void* thread_main(void* arg)
{
	int i;
	int cnt = *((int *)arg);
	for(i=0;i<cnt;i++){
		sleep(1);
		puts("Running thread");
	}
	pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
	pthread_t t_id;
	int arg=10;
	int result;

	result=pthread_create(&t_id, NULL, thread_main, (void*)&arg);
	if(result != 0){
		errno= result;
		perror("create");
	}
	sleep(4);
	puts("End of program");
	return 0;
}

