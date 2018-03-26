#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM,0);
	
	addr.sin_family= AF_INET;
	addr.sin_port = htons(13579);
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

	#define BUF_SIZE 10
	int str_len;
	int newfd;
	char message[BUF_SIZE];
	struct sockaddr_in client_addr;
	int addr_len;
	int i;

	addr_len = sizeof(client_addr);

	for(i =0;i < 5; i++){
		newfd = accept(sockfd,(struct sockaddr*)&client_addr, &addr_len);
		if(getsockname(newfd,(struct sockaddr *)&addr,&addr_len) <0){
			perror("getsockname");
			exit(1);
		}
		printf("Socket IP: %s, PORT: %u\n", inet_ntoa(addr.sin_addr), (unsigned)ntohs(addr.sin_port));
		if(getpeername(newfd,(struct sockaddr *)&addr,&addr_len) <0){
			perror("getpeername");
			exit(1);
		}
		printf("peer IP: %s, PORT: %u\n", inet_ntoa(addr.sin_addr), (unsigned)ntohs(addr.sin_port));

		if(newfd==-1) perror("listen");
		else printf("ConnectionEstablished\n");

		while((str_len = read(newfd,message,BUF_SIZE))!=0){
		printf("Recv %d bytes\n",str_len);
		write(newfd,message,str_len);
		}
		close(newfd);
	}
	close(sockfd);
}
