#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_DGRAM,0);

	addr.sin_family= AF_INET;
	addr.sin_port = htons(atoi(argv[1]));
	inet_aton("127.0.0.255",&addr.sin_addr);
	memset(&(addr.sin_zero),0,8);

#define BUF_SIZE 10
#define MAX_EVENTS 10

	int str_len;
	int newfd;
	char message[BUF_SIZE];
	struct sockaddr_in client_addr;
	int addr_len;
	int event_cnt;
	int opt=1;
	if(setsockopt(sockfd,SOL_SOCKET, SO_BROADCAST,(void*)&opt, sizeof(opt))<0){
		perror("socketsopt");
		exit(0);
	}

	while(1){
		printf("Input message(Q to quit): ");
		fgets(message,BUF_SIZE,stdin);
		if(strcmp(message,"q\n") == 0 || strcmp(message,"Q\n") == 0){
			break;
		}

		sendto(sockfd, message, strlen(message),0,(struct sockaddr *)&addr, sizeof(addr));
		str_len = recvfrom(sockfd,message,BUF_SIZE -1,0,(struct sockaddr *)&client_addr, &addr_len);
		message[str_len]=0;
		printf("Message from server : %s", message);
	}
	close(sockfd);
}
