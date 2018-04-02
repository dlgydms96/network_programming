#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 1000

int main(int argc, char* argv[])
{
	int str_len;
	char message[BUF_SIZE]={"GET /index.html HTTP/1.1\r\n\r\n"};

	int sockfd;
	struct sockaddr_in addr;
	if(argc !=2){
		printf("Usage : %s <IP> \n", argv[0]);
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd ==-1)
		error_handling("socket() error");
	
	addr.sin_family= AF_INET;
	addr.sin_port = htons(80);
	inet_aton(argv[1],&addr.sin_addr);
	memset(&(addr.sin_zero),0,8);

	if(connect(sockfd, (struct sockaddr*)&addr,sizeof(addr))==-1)
		error_handling("connect() error!");

	write(sockfd,message,strlen(message));
	str_len = read(sockfd,message,BUF_SIZE -1);
	if(str_len == -1) error_handling("read() error!");
	
//	message[str_len]=0;
	printf("Message from server : %s", message);

	close(sockfd);
	return 0;
}

void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n',stderr);
	exit(1);
}
