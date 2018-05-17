#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define BUF_SIZE 1000
void* http_req(void *sock);
void http_res(int type);
int parseMsg(char *file_msg);
void zombie_handler()
{
	int status;
	int spid;
	spid = wait(&status);
}

int main()
{
	int sockfd;
	struct sockaddr_in addr;
	int opt=1;

	sockfd = socket(AF_INET, SOCK_STREAM,0);

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int));
	addr.sin_family= AF_INET;
	addr.sin_port = htons(80);
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

	int newfd;
	struct sockaddr_in client_addr;
	int addr_len;
	struct sigaction act;
	pthread_t t_id;

	act.sa_handler = zombie_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	sigaction(SIGCHLD,&act,0);

	addr_len = sizeof(client_addr);

	while(1){
		newfd = accept(sockfd,(struct sockaddr*)&client_addr, &addr_len);
		printf("Connected : %s %d\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		if(newfd<0) {
			switch(errno){
				case EINTR:
					continue;
				default:
					perror("accept");
			}
			return 0;
		}
		else printf("Connection Established\n");

		pthread_create(&t_id, NULL,(void*) http_req,(void*) &newfd);
		pthread_detach(t_id);
	}
	close(sockfd);
}

void* http_req(void *sock)
{
	int sockfd=*(int*)sock;
	char message[BUF_SIZE];
	int file_buf[BUF_SIZE];
	int str_len;
	int i;
	char req_msg[30];
	char res_msg[BUF_SIZE];
	int type;
	int filefd;
	while((str_len = read(sockfd,message,BUF_SIZE))!=0){
		printf("Recv %d bytes\n",str_len);
		printf("%s",message);
		for(i=0; i<str_len; i++)
			if(message[i]=='\n')
			{
				strncpy(req_msg,message,++i);
				printf("%s",req_msg);
				type=parseMsg(req_msg);
				printf("length:%d\n",strlen(req_msg));
				break;
			}
		switch(type)
		{
			case 0:
				strcpy(res_msg,"HTTP/1.1 200 OK\r\n");
				str_len=strlen(res_msg);
				write(sockfd,res_msg,str_len);
				strcpy(res_msg,"Content-Type: ");
				strcat(res_msg,"index/html\r\n");
				str_len=strlen(res_msg);
				write(sockfd,res_msg,str_len);
				strcpy(res_msg,"Content-Length: ");
				strcat(res_msg,"163\r\n\r\n");
				str_len=strlen(res_msg);
				write(sockfd,res_msg,str_len);

				filefd=open("./index.html",O_RDONLY);
				if(filefd==NULL)
				{
					printf("error");
					return;
				}

				memset(file_buf,0x00,BUF_SIZE);
				str_len=read(filefd,file_buf,BUF_SIZE);
				write(sockfd,file_buf,str_len);
				printf("%d", str_len);
		}
	}
	close(sockfd);
}

int parseMsg(char *req_msg)
{
	int i;
	char file_msg[10]={0,};
	int ntype=0;
	char type[5]={0,};
	int str_len;
	for(i=4;req_msg[i]!=' ';i++)
		strncat(file_msg,&req_msg[i],1);	
	printf("file: %s\n",file_msg);
	for(i=0;i<strlen(file_msg);i++)
		if(file_msg[i++]=='.')
			strcpy(type,&file_msg[i]);
	printf("type: %s\n", type);


//write(newfd,message,str_len);
	if(strcmp(type,"")==0 || strcmp(type,"html")==0)
	ntype=0;
	else if(strcmp(type,"jpg")==0)
	ntype=1;
	else if(strcmp(type,"png")==0)
	ntype=2;
	else 
	{
		ntype =-1;
		("Can't parse message\n");
	}
	return ntype;
}

