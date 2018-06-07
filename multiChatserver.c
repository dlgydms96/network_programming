#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 100


typedef struct clientInfo{
	unsigned int IP;
	char name[10];
	unsigned int socket;
}clientInfo;

clientInfo client_arr[10];
int cltIdx=0;
int sockfd;

int getPeer(int s,char* buf)
{
	struct clientInfo client;
	struct sockaddr_in addr;
	int addr_len=sizeof(addr);

	if(getpeername(s,(struct sockaddr *)&addr,&addr_len)<0){
		perror("getpeername");
		exit(1);
	}

	client.socket=s;
	client.IP=addr.sin_addr.s_addr;
	strcpy(client.name,buf);

	client_arr[cltIdx++]=client;
	printf("IP: %d , name: %s\n", client.IP,client.name);
	return cltIdx-1;
}

void* thread_main(void* arg)
{
	int size;
	int s =*(int *)arg;
	char buf[BUF_SIZE];
	int msgSize;
	char type;
	int cltNum;
	free(arg);

	while(1)
	{
		if((size = read(s,buf,BUF_SIZE))<=0) break;
		msgSize=atoi(buf);
		if(size<msgSize) size = read(s,buf,BUF_SIZE);
		if(msgSize < 10) type=buf[1];
		else type=buf[2];
		printf("type %c\n",type);
		if(type=='m')
		{
			for(int i=0;i<cltIdx;i++) write(client_arr[i].socket,buf,size);
			printf("Host: %s, size: %d\n",buf,size);
		}
		else if(type == 'c')
		{
			printf("cltIdx: %d\n",cltIdx);
			cltNum=getPeer(s,buf);
			strcpy(buf,"***client list***");
			write(s,buf,strlen(buf));
			for(int i=0;i<cltIdx;i++)
			{
				printf("socket: %d\n",client_arr[i].socket);
				write(s,client_arr[i].name,size);
				if(client_arr[i].socket!=s)
				{
					strcpy(buf,client_arr[i].name);
					strcat(buf," connected");
					write(client_arr[i].socket,buf,strlen(buf));
				}
			}
		}
		printf("Host: %s, size: %d\n",buf,size);
		memset(buf,0,BUF_SIZE);
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
	addr.sin_port = htons(atoi(argv[1]));
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
