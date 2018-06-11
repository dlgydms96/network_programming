#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#define BUF_SIZE 1000
#define MAX_EVENTS 10
int main()
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd=socket(AF_INET, SOCK_STREAM,0);
	if(sockfd==-1) perror("socket");

	addr.sin_family=AF_INET;
	addr.sin_port=htons(9001);
	inet_aton("127.0.0.1",&addr.sin_addr);
	memset(&addr.sin_zero,0,8);
	struct timeval timeout;
	fd_set readfds, readtemp;
	int max_fd=0;
	int result;
	int i;
	int new;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in client_addr;
	int addr_len;
	addr_len=sizeof(client_addr);
	int event_cnt;
	int epfd;
	struct epoll_event ev,ev1, evs[MAX_EVENTS];

	epfd=epoll_create1(0);
	ev.events = EPOLLIN;
	ev.data.fd = 0;
	epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &ev);

	ev1.events = EPOLLIN;
	ev1.data.fd = sockfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev1);

	new=connect(sockfd,(struct sockaddr*)&addr, sizeof(addr));

	while(1){
		event_cnt= epoll_wait(epfd,evs,MAX_EVENTS,-1);

		if(event_cnt ==-1){
			perror("epoll");
			break;
		}
		else if(event_cnt ==0){
			printf("Timeout\n");
			continue;
		}

		for(i=0;i<=event_cnt;i++){	
			if(evs[i].events==EPOLLIN){
				if(evs[i].data.fd==sockfd){
					str_len = read(evs[i].data.fd,message,BUF_SIZE);
					if(str_len==0){
						epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,NULL);
						close(evs[i].data.fd);
						printf("Closed\n");
					}
					else{
						message[str_len]='\0';
						printf("%s",message);
					}
				}
				else if(evs[i].data.fd==0)
				{
					fgets(message,BUF_SIZE,stdin);
					write(sockfd,message,strlen(message));
				}

			}		
		}
	}
}
