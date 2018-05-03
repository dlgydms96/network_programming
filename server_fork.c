#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
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

	#define BUF_SIZE 100
	int str_len;
	int newfd;
	char message[BUF_SIZE];
	struct sockaddr_in client_addr;
	int addr_len;
	struct sigaction act;

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
		else printf("ConnectionEstablished\n");

		if(fork()==0){
		 close(sockfd);
		 while(1){
			if((str_len = recv(newfd,message,BUF_SIZE,0))>0)
			 send(newfd,message,str_len,0);
			else break;
			}
		close(newfd);
		exit(0);
		}
		close(newfd);
	}
	close(sockfd);
}
