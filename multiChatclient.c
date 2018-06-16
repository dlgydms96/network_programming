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
#define MAX_EVENTS 100

int sockfd;

void ConnectToServer(char* arg)
{
	char output[BUF_SIZE];
	char name[10];
	strcpy(name,arg);
	sprintf(output,"%d",strlen(name));
	strcat(output,"c");
	strcat(output,name);
	send(sockfd,output,strlen(output),0);
}

void* thread_send(void* arg)
{
	int size;
	char buf[BUF_SIZE];
	char output[BUF_SIZE];
	char name[10];
	char header[3];
	ConnectToServer((char*)arg);
	while((size = read(0,buf,BUF_SIZE))>0){
		strncpy(header,buf,3);
		header[2]='\0';
		printf("header: %s\n",header);
		sprintf(output,"%d",size);
		if(strcmp(buf,"/l\n")==0)
		{
			sprintf(output,"%d",strlen("l"));
			strcat(output,"l");
			send(sockfd,output,strlen(output),0);
		}
		else if(strcmp(header,"/w")==0)
		{
			sprintf(output,"%d",strlen("w"));
			strcat(output,"w");
			////////////////////////////////////
			buf[size-1]='\0';
			strcat(output,&buf[3]);
			send(sockfd,output,strlen(output),0);
		}
		else{
			strcat(output,"m");
			////////////////////////////////////
			buf[size-1]='\0';
			strcat(output,buf);
			send(sockfd,output,strlen(output),0);
			//printf("Host: %s\n",output);
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t t_id;
	int arg=10;
	int result;
    struct sockaddr_in addr;
	char message[BUF_SIZE];
	int size;

	if(argc !=4)
	{
		printf("NOTE: filename <IP> <port> <client name>\n");
		exit(0);
	}
	sockfd = socket(AF_INET, SOCK_STREAM,0);

	addr.sin_family= AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1],&addr.sin_addr);
	memset(&(addr.sin_zero),0,8);
	
	connect(sockfd, (struct sockaddr*)&addr,sizeof(addr));

	result=pthread_create(&t_id, NULL, thread_send,argv[3]);
	if(result != 0){
		errno= result;
		perror("create");
	}

	pthread_detach(t_id);

	while((size =recv(sockfd, message, BUF_SIZE,0))>0){
	message[size]='\0';
	printf("%s\n", message);
	}

	close(sockfd);
	return 0;
}
