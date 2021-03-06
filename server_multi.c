#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#define BUF_SIZE 1000
int main()
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd=socket(AF_INET, SOCK_STREAM,0);
	if(sockfd==-1) perror("socket");

	addr.sin_family=AF_INET;
	addr.sin_port=htons(9000);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	memset(&addr.sin_zero,0,8);
	if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr))==-1)
	{
		perror("bind");
		return 0;
	}

	if(listen(sockfd,5)<0)
	{
		perror("listen");
		return 0;
	}

	struct timeval timeout;
	fd_set readfds, readtemp;
	int max_fd=0;
	int result;
	int i;
	int newfd;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in client_addr;
	int addr_len;
	addr_len=sizeof(client_addr);


	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	FD_SET(0, &readfds);
	max_fd = sockfd;

	while(1){
		readtemp= readfds;
		timeout.tv_sec=5;
		timeout.tv_usec=0;

		result = select(max_fd +1, &readtemp,NULL,NULL, &timeout);

		if(result ==-1){
			perror("select");
			break;
		}
		else if(result ==0){
			printf("Timeout\n");
			continue;
		}

		for(i=0;i<=max_fd;i++){	

			if(FD_ISSET(i, &readtemp)){
				if(i==sockfd){
					newfd=accept(i,(struct sockaddr*)&client_addr, &addr_len);
					if(newfd==-1){
						perror("accept");
						exit(0);
					}
					printf("Connected client: %d\n", newfd);
					FD_SET(newfd, &readfds);
					if(max_fd<newfd)
						max_fd= newfd;
				}
				else if(i==0)
				{
					fgets(message,BUF_SIZE,stdin);
					str_len=write(newfd,message,strlen(message));
				}

				else{
					str_len = read(i,message,BUF_SIZE);
					if(str_len==0){
						FD_CLR(i, &readfds);
						close(i);
						printf("Closed client: %d\n",i);
					}
					else{
						message[str_len]='\0';
						printf("%s",message);
						//write(i,message,str_len);
					}
				}
			}
		}
	}
}
