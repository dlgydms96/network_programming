#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define BUF_SIZE 10
#define MAX_EVENTS 10

pthread_mutex_t mutex;

int sockfd;
int num_client=0;

void* thread_main(void* arg)
{
	int s;
    struct sockaddr_in client_addr;
	int size;
	int addr_len;
	char buf[BUF_SIZE];
	char escape[10]="aa";

	
	addr_len=sizeof(client_addr);
	s = accept(sockfd,(struct sockaddr*)&client_addr, &addr_len);

	pthread_mutex_lock(&mutex);
	num_client++;
	printf("client number: %d\n",num_client);
	pthread_mutex_unlock(&mutex);
	
	while((size = read(s,buf,BUF_SIZE))>0){
		buf[size-1]='\0';
		if(strncmp(buf,escape,2) ==0){
			shutdown(sockfd, SHUT_WR);
			break;
		}
		write(s,buf,size);
		printf("Host: %s\n",buf);
	}
	pthread_mutex_lock(&mutex);
	num_client--;
	pthread_mutex_unlock(&mutex);

	close(s);
	pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
	pthread_t t_id;
	int arg=10;
	int i;
	char c;
	int newfd;
	int addr_len;
	int result;
    struct sockaddr_in addr;
	char message[BUF_SIZE];
	int size;

	sockfd = socket(AF_INET, SOCK_STREAM,0);

	addr.sin_family= AF_INET;
	addr.sin_port = htons(9000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(addr.sin_zero),0,8);
	
	if(bind(sockfd,(struct sockaddr *) &addr, sizeof(addr)) ==-1){
	      perror("bind");
	      return 0;
	    }
	if(listen(sockfd,5)<0){
	       perror("listen");
	       return 0;
	 }

	for(i=0;i<5;i++){
		result=pthread_create(&t_id, NULL, thread_main, NULL);
		if(result != 0){
			errno= result;
			perror("create");
		}
		pthread_detach(t_id);
	}

	c = getchar();
	close(sockfd);
	return 0;
}

