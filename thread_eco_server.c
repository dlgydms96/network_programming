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

int sockfd;

void* thread_main(void* arg)
{
	int size;
	int s =*(int *)arg;
	char buf[BUF_SIZE];
	char escape[10]="aa";

	free(arg);

	while((size = read(s,buf,BUF_SIZE))>0){
		buf[size-1]='\0';
		if(strncmp(buf,escape,2) ==0){
			shutdown(sockfd, SHUT_WR);
			break;
		}
		write(s,buf,size);
		printf("Host: %s\n",buf);
	}
	close(s);
	pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
	pthread_t t_id;
	int arg=10;
	int newfd;
	int addr_len;
	int result;
    struct sockaddr_in addr,client_addr;
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
	addr_len = sizeof(client_addr);

	while(1){
		newfd = accept(sockfd,(struct sockaddr*)&client_addr, &addr_len);

		arg = malloc(sizeof(int));
		*(int *)arg = newfd;
		result=pthread_create(&t_id, NULL, thread_main,arg);
		if(result != 0){
			errno= result;
			perror("create");
		}

		pthread_detach(t_id);
	}

	close(sockfd);
	return 0;
}

