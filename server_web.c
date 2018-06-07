#include <sys/types.h>
#include <sys/stat.h>
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

#define BASE_DIR "./"
#define DEFAULT_HTML "./index.html"

void* http_req(void *sock);
void http_res(int type);
int parseMsg(char *req_msg);
char name[10]={0,};
static size_t file_size (const char * file_name) 
{
	struct stat sb; 
	if (stat (file_name, & sb) != 0)
	{ 
		fprintf (stderr, "'stat' failed for '%s': %s.\n", file_name, strerror (errno));
		return 0;
	}
	return sb.st_size; 
} 

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
	int fd[10];	//client's port array
	int i=0;
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
		if(i==10) i=0;
		fd[i++]=newfd;
		pthread_create(&t_id, NULL,(void*) http_req,(void*) &fd[i-1]);
		pthread_detach(t_id);
	}
	close(sockfd);
}
void error_page(int s)
{
	int res_msg[BUF_SIZE];
	int str_len;
	int filesize;
	char size[4];
	int filefd;
	int file_buf[BUF_SIZE];

	strcpy(res_msg,"HTTP/1.1 404 Not Found\r\n");
	str_len=strlen(res_msg);
	write(s,res_msg,str_len);
	strcpy(res_msg,"Content-Type: text/html\r\n");
	str_len=strlen(res_msg);
	write(s,res_msg,str_len);

	strcpy(name,"./error.html");
	filesize=file_size(name);
	sprintf(size,"%d",filesize);
	strcpy(res_msg,"Content-Length: ");
	strcat(res_msg,size);
	strcat(res_msg,"\r\n\r\n");
	str_len=strlen(res_msg);
	write(s,res_msg,str_len);

	filefd=open(name,O_RDONLY);
	if(filefd==NULL)
	{
		printf("error");
		return;
	}
	memset(file_buf,0x00,BUF_SIZE);
	while((str_len=read(filefd,file_buf,BUF_SIZE))!=0)
	{
		write(s,file_buf,str_len);
	}
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
	int ntype;
	int filefd;
	int filesize;
	char size[4];

	while(1)
	{
		str_len = read(sockfd,message,BUF_SIZE);
		if(str_len==0)	break;
		printf("Recv %d bytes\n",str_len);
		printf("%s",message);
		for(i=0; i<str_len; i++)
			if(message[i]=='\n')
			{
				strncpy(req_msg,message,++i);
				printf("%s",req_msg);
				ntype=parseMsg(req_msg);
				if(ntype == -1);	//error
				printf("length:%d\n",strlen(req_msg));
				break;
			}

		strcpy(res_msg,"HTTP/1.1 200 OK\r\n");
		str_len=strlen(res_msg);
		write(sockfd,res_msg,str_len);
		if(ntype == 0) strcpy(res_msg,"Content-Type: text/html\r\n");
		else if(ntype ==1) strcpy(res_msg,"Content-Type: image/jpg\r\n");
		else if(ntype == 2) strcpy(res_msg,"Content-Type: image/png\r\n");
		str_len=strlen(res_msg);
		write(sockfd,res_msg,str_len);

		if(strcmp(name,BASE_DIR)==0) strcpy(name,DEFAULT_HTML);
		if((filesize=file_size(name))==0)
			error_page(sockfd);
		sprintf(size,"%d",filesize);
		strcpy(res_msg,"Content-Length: ");
		strcat(res_msg,size);
		strcat(res_msg,"\r\n\r\n");
		str_len=strlen(res_msg);
		write(sockfd,res_msg,str_len);

		filefd=open(name,O_RDONLY);
		if(filefd==NULL)
		{
			printf("error");
			return;
		}
		memset(file_buf,0x00,BUF_SIZE);
		while((str_len=read(filefd,file_buf,BUF_SIZE))!=0)
			write(sockfd,file_buf,str_len);
	}
	close(sockfd);
}

int parseMsg(char *req_msg)
{
	int i,j;
	char file_msg[10]={0,};
	int ntype=0;
	int str_len;
	char type[5]={0,};

	memset(name,0x00,10);
	strcpy(name,BASE_DIR);
	for(i=4;req_msg[i]!=' ';i++)
		strncat(file_msg,&req_msg[i],1);	
	printf("file: %s\n",file_msg);
	for(i=1;i<strlen(file_msg);i++)
	{
		if(file_msg[i]=='.') j=i+1;
		strncat(name,&file_msg[i],1);
	}
	for(;j<strlen(file_msg);j++)
		strncat(type,&file_msg[j],1);
	printf("name: %s\n", name);
	printf("type: %s\n", type);

	if(strcmp(type,"")==0 || strcmp(type,"html")==0)
		ntype=0;
	else if(strcmp(type,"jpg")==0)
		ntype=1;
	else if(strcmp(type,"png")==0)
		ntype=2;
	else 
	{
		ntype =-1;
		printf("Can't parse message\n%d\n",ntype);
	}
	return ntype;
}
