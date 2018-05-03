#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/select.h>
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
	struct timeval timeout;
	fd_set readfds,readtemp;
	int max_fd=0;
	int result;

	FD_ZERO(&readfds);
	FD_SET(sockfd,&readfds);
	max_fd = sockfd;

	addr_len = sizeof(client_addr);

	while(1){
		readtemp= readfds;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		result = select(max_fd+1, &readtemp, NULL, NULL, &timeout);
		if(result == -1){
			perror("select");
			break;
		}
		else if(result ==0){
			printf("Timeout\n");
			continue;
		}
		for(i=0;i<=max_fd;i++){
			if(FD_ISSET(i<=max_fd;i++)){
				if(i==sockfd){
					newfd = accept(i, (struct sockaddr*)&cleint_addr, &sin_size);
					if(newfd == -1){
						perror("accept");
						exit(0);
					}
					printf("Connected client: %d\n",newfd);
					FD_SET(nesfd, &readfds);
					if(max_fd < newfd) max_fd =newfd;
				}
				else{
					str_len = read(i,message,BUF_SIZE);
					if(str_len ==0){
						FD_CLR(i,&readfds);
						close(i);
						printf("Closed client: %d\n", i);
					}
					else write(i,message,str_len);
			}
		}
	}
	close(sockfd);
}
