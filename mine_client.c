#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "getch.h"
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1000

#define LEFT 97
#define RIGHT 100
#define DOWN 115
#define UP 119
#define GET_TILE 49
#define GET_POS 50
#define GET_HEIGHT 51
#define GET_BLOCK 52
/*#define SET_TILE 116
#define SET_POS 112
#define SET_BLOCK 98*/

static int x,y,z;//player pos
int sockfd;

void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n',stderr);
	exit(1);
	return;
}

char* sendToServer(char* msg)
{
	int str_len;
	char res[BUF_SIZE]={0,};
	write(sockfd,msg,strlen(msg));
	str_len = read(sockfd,res,BUF_SIZE -1);
	if(str_len == -1) error_handling("read() error!");
	return res;
}

int movePlayer(char* msg)
{
	int i;
	int crt=0; //crt == 0 is x , crt == 1 is y, and 3 is z
	int pos[3]={0,}; //x,y,z
	char temp[100]={0,};
	char val[10]={0,};
	char check[2];
	strcpy((char*)temp,msg);
	for(i=0; temp[i] != '\0'; i++)
	{
		strncat(val,&temp[i],1);
		check[0]=temp[i];
		check[1]='\0';
		if(strcmp(check,",") == 0)
			{
				pos[crt]=atoi(val);
				crt++;
				val[0]='\0';
			}
	}
	pos[crt]=atoi(val);
	return pos[2];
}

int main(int argc, char* argv[])
{
	char message[BUF_SIZE];
	struct sockaddr_in addr;
	int key;	//input
	char* res;//response from Server
	int tempx=0;
	if(argc !=2){
		printf("Usage : %s <IP> \n", argv[0]);
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd ==-1)
		error_handling("socket() error");
	
	addr.sin_family= AF_INET;
	addr.sin_port = htons(4711);
	inet_aton(argv[1],&addr.sin_addr);
	memset(&(addr.sin_zero),0,8);

	if(connect(sockfd, (struct sockaddr*)&addr,sizeof(addr))==-1)
		error_handling("connect() error!");
while(1){
//	printf("Input message(Q to quit): ");
  key = getch();
	switch(key)
		{
			case GET_TILE:
		 		strncpy(message,"player.getTile()\n",BUF_SIZE);
				res=sendToServer(message);
				break;
			case GET_POS:
		 		strncpy(message,"player.getPos()\n",BUF_SIZE);
				res=sendToServer(message);
				break;
			case UP:
		 		strncpy(message,"player.getTile()\n",BUF_SIZE);
				res=sendToServer(message);
				tempx=movePlayer(res);
				break;
			case GET_BLOCK:
		 		strncpy(message,"world.getBlock()\n",BUF_SIZE);
				res=sendToServer(message);
				break;
			default:
			//  message="0";
				break;
		}
//  fgets(message, BUF_SIZE,stdin);
//	printf("%d",key);
  //if(strcmp(message,"q\n") ==0 || strcmp(message,"Q\n") == 0) break;

	printf("Message from server : %s", res);
	printf("X : %d\n",tempx);
  }
	close(sockfd);
	return 0;
}
