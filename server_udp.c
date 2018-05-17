#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_DGRAM,0);

	addr.sin_family= AF_INET;
	addr.sin_port = htons(9001);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(addr.sin_zero),0,8);

	if(bind(sockfd,(struct sockaddr *) &addr, sizeof(addr)) ==-1){
		perror("bind");
		return 0;
	}
#define BUF_SIZE 10
	int str_len;
	char message[BUF_SIZE];
	struct sockaddr_in client_addr;
	int addr_len;

	addr_len = sizeof(client_addr);
	while((str_len = recvfrom(sockfd,message,BUF_SIZE,0,(struct sockaddr*)&client_addr,&addr_len))!=0){
		message[str_len]=0;
		printf("Recv %s bytes\n",message);
		sendto(sockfd,message,str_len,0,(struct sockaddr*)&client_addr, addr_len);
	}
	close(sockfd);
}
