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
#define SET_BLOCK 101
#define GET_BLOCK 113
#define SET_STRUCT 114
/*#define SET_TILE 116
#define SET_POS 112
#define SET_BLOCK 98*/

int sockfd;//socket descriptor
int block=0;//block id
int pos[3];//player pos x,y,z

void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n',stderr);
	exit(1);
	return;
}

void setToServer(char* msg, int* val, int len)
{
	char buf[4];
	int i;
	for(i=0;i<len;i++)
	{
		sprintf(buf,"%d",val[i]);
		strncat(msg,buf,4);
 		if(i != len-1) strncat(msg,",",1);
		else 	strncat(msg,")\n",2);
	}
	printf("%s",msg);
	
	write(sockfd,msg,strlen(msg));
}

char* getToServer(char* msg,int* val, int len)
{
	int str_len=0;
	char res[BUF_SIZE]={0,};
	char buf[4];
	int i;
	for(i=0;i<len;i++)
	{
		sprintf(buf,"%d",val[i]);
		strncat(msg,buf,4);
 		if(i != len-1) strncat(msg,",",1);
		else 	strncat(msg,")\n",2);
	}
	printf("%s",msg);
	
	write(sockfd,msg,strlen(msg));
	str_len = read(sockfd,res,BUF_SIZE -1);
	if(str_len == -1) error_handling("read() error!");
	return res;
}

int getHeight(int* val)
{
	char msg[BUF_SIZE]="world.getHeight(";
	int xz[2];
	char* res;
	int y;

	xz[0]=val[0];
	xz[1]=val[2];
  res= getToServer(msg,xz,2);
	y=atoi(res);
	if(y==(val[1]+1))//jump
		return 1;
	else if(y>(val[1]+1))//can't go
		return -1;
	return 0;
}

void getPose()
{
	int crt=0; //crt == 0 is x , crt == 1 is y, and 3 is z
	char msg_arr[100]={0,};
	char val[10]={0,};
	char check[2];
	char* res;
	int i;
	char message[BUF_SIZE];
	strncpy(message,"player.getTile()\n",BUF_SIZE);
	res=getToServer(message,NULL,0);
	strcpy((char*)msg_arr,res);
 	for(i=0; msg_arr[i] != '\0'; i++)
	  {
		  strncat(val,&msg_arr[i],1);
		  check[0]=msg_arr[i];
		  check[1]='\0';
		  if(strcmp(check,",") == 0)
		   {
		     pos[crt]=atoi(val);
			   crt++;
			   val[0]='\0';
			 }
		 }
  pos[crt]=atoi(val);//end of atoi
}

void movePlayer(int move)
{
	int i,j;
	int crt=0; //crt == 0 is x , crt == 1 is y, and 3 is z
	char msg_arr[100]={0,};
	char val[10]={0,};
	char check[2];
	char msgs[BUF_SIZE];
	char buf[4];
	int movflag;//0 = go, 1 = jump , -1= invalid move

	getPose();
	strcpy(msgs,"player.setTile(");
	
	switch(move)
	{
		case UP:
			pos[2]++;
		break;
		case DOWN:
			pos[2]--;
			break;
		case LEFT:
			pos[0]++;
			break;
		case RIGHT:
			pos[0]--;
			break;
		default:
			break;
	}
  movflag=getHeight(pos);
	if(movflag == 1) pos[1]++;
	if(movflag != -1) setToServer(msgs,pos,3);
	
}

void getBlock()
{
	int crt=0; //crt == 0 is x , crt == 1 is y, and 3 is z
	char msg_arr[100]={0,};
	char val[10]={0,};
	char check[2];
	char* res;
	int i;
  char msgs[BUF_SIZE];
	
	getPose();
	pos[2]++;
	strcpy(msgs,"world.getBlock(");

	res= getToServer(msgs,pos,3);
	block=atoi(res);
	printf("get block number %d\n",block);
}

void setBlock()
{
	int block_pos[4];
	int i;
	char msgs[BUF_SIZE];
	getPose();
	for(i=0;i<3;i++) block_pos[i]=pos[i];
	block_pos[2]++;
	block_pos[3]=block; //block id
	strcpy(msgs,"world.setBlock(");
	setToServer(msgs,block_pos,4);
}

void setStruct()
{
	int point;
	
}

int main(int argc, char* argv[])
{
	char message[BUF_SIZE];
	struct sockaddr_in addr;
	int key;	//input
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
		key = getch();
		switch(key)
		{
			case UP:
			case DOWN:
			case LEFT:
			case RIGHT:
				movePlayer(key);
				break;
			case GET_BLOCK:
				getBlock();
				break;
			case SET_BLOCK:
				setBlock();
				break;
			case SET_STRUCT:
				setStruct();
				break;
			default:
				printf("%d",key);
				break;
		}
	}
	close(sockfd);
	return 0;
}
