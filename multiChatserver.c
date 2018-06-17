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
#define MAX_CLIENTS 3

typedef struct clientInfo{
	unsigned int IP;
	char name[10];
	unsigned int socket;
}clientInfo;

pthread_mutex_t mutex;
clientInfo client_arr[MAX_CLIENTS];
int cltIdx=0;
int sockfd;

char body[BUF_SIZE];

void whisperClient(int s)
{
	char dst_name[10];
	char whisper[BUF_SIZE];
	int EON;
	int dst_idx;
	for(int i=0;i<strlen(body);i++)
	{
		if(body[i]==' ')
		{
			EON=i;
			break;
		}
	}

	strncpy(dst_name,body,EON);
	dst_name[EON]='\0';
	strcpy(whisper,&body[EON+1]);
	
	for(int i=0;i<cltIdx;i++)
		if(strcmp(client_arr[i].name,dst_name)==0)
			dst_idx=i;
	
	write(client_arr[dst_idx].socket,whisper,strlen(whisper));

}

void sendHowto(int s)
{
	char buf[BUF_SIZE];
	strcpy(buf," /q: disconnection\n /l: get clients list\n /h: help\n /w: whispering chat\n");
	write(s,buf,sizeof(buf));
}

void sendClients(int s, int cltNum)//opt is notice option
{			
	char buf[BUF_SIZE];
	strcpy(buf,"***client list***");
	write(s,buf,strlen(buf));
	for(int i=0;i<cltIdx;i++)
	{
		printf("socket: %d\n",client_arr[i].socket);
		write(s,client_arr[i].name,strlen(client_arr[i].name));
		if(client_arr[i].socket!=s && cltNum>=0)
		{
			strcpy(buf,client_arr[cltNum].name);
			strcat(buf," connected");
			write(client_arr[i].socket,buf,strlen(buf));
		}
	}
}

char* msgParser(char* msg,int size)
{
	if(size < 10)
		strcpy(body,&msg[2]);
	else 
		strcpy(body,&msg[3]);
	return body;
}

int subPeer(int s, int cltNum)
{
	char buf[BUF_SIZE];
	int delete_clt;
	for(int i=0;i<cltIdx;i++)
	{
		strcpy(buf,client_arr[cltNum].name);
		strcat(buf," disconnected");
		write(client_arr[i].socket,buf,strlen(buf));
		if(client_arr[i].socket==s) delete_clt=i;
	}
	for(int j=delete_clt;j<cltIdx;j++)
		client_arr[j]=client_arr[j+1];
		
	cltIdx--;
}

int addPeer(int s,char* buf)
{
	struct clientInfo client;
	struct sockaddr_in addr;
	int addr_len=sizeof(addr);
	char* name;
	if(getpeername(s,(struct sockaddr *)&addr,&addr_len)<0){
		perror("getpeername");
		exit(1);
	}

	client.socket=s;
	client.IP=addr.sin_addr.s_addr;
	strcpy(client.name,body);

	if(cltIdx<MAX_CLIENTS){
		client_arr[cltIdx++]=client;
		printf("IP: %d , name: %s\n", client.IP,client.name);
	}
	else
		{
			printf("too many clients\n");
			close(s);
		}

	return cltIdx;//client num
}

void* thread_main(void* arg)
{
	int size;
	int s =*(int *)arg;
	char buf[BUF_SIZE];
	int msgSize;
	char type;
	int cltNum;
	char cltname[12];
	free(arg);
	char* body;

	pthread_mutex_init(&mutex,NULL);
	while(1)
	{
		if((size = read(s,buf,BUF_SIZE))<=0) break;
		msgSize=atoi(buf);
		if(size<msgSize) size = read(s,buf,BUF_SIZE);
		if(msgSize < 10) type=buf[1];
		else type=buf[2];
		printf("type %c\n",type);
		body=msgParser(buf,msgSize);
		if(type=='m')
		{
			if(strcmp(body,"/q")==0) break;
			else if(strcmp(body,"/h")==0)sendHowto(s);
			else
			{
				for(int i=0;i<cltIdx;i++)
				{
					strcpy(cltname,client_arr[cltNum].name);
					strcat(cltname,": ");
					write(client_arr[i].socket,cltname,strlen(cltname));
					write(client_arr[i].socket,body,strlen(body));
					printf("Host: %s, size: %d\n",body,size);
				}
			}
		}
		else if(type == 'c')
		{
			pthread_mutex_lock(&mutex);
			printf("cltIdx: %d\n",cltIdx);
			cltNum=addPeer(s,buf);
			cltNum--;
			sendClients(s,cltNum);
			pthread_mutex_unlock(&mutex);
		}
		else if(type =='l')
		{
			sendClients(s,-1);
		}
		else if(type =='w')
		{
			whisperClient(s);
		}
		printf("Host: %s, size: %d\n",buf,size);
		memset(buf,0,BUF_SIZE);
		type='\0';
	}
	subPeer(s,cltNum);
	close(s);
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t t_id;
	void* arg;
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
