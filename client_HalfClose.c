#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM,0);
	
addr.sin_family= AF_INET;
addr.sin_port = htons(9000);
inet_aton("127.0.0.1",&addr.sin_addr);
memset(&(addr.sin_zero),0,8);

connect(sockfd, (struct sockaddr*)&addr,sizeof(addr));

#define BUF_SIZE 10
int str_len;
int newfd;
char message[BUF_SIZE];
struct sockaddr_in client_addr;
int addr_len;

while(1){
	printf("Input message(Q to quit): ");
	fgets(message,BUF_SIZE,stdin);
	if(strcmp(message,"q\n") == 0 || strcmp(message,"Q\n") == 0){
	break;
	}

	write(sockfd,message,strlen(message));
	str_len = read(sockfd,message,BUF_SIZE -1);
	message[str_len]=0;
	printf("Message from server : %s", message);
}
shutdown(sockfd, SHUT_WR);
str_len=read(sockfd,message,BUF_SIZE);
message[str_len]=0;
printf("Final message from server: %s", message);
close(sockfd);
}
