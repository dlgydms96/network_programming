#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 100
float calculation(int cnt, int* num, char* opt)
{
	int i;
	float result=num[0];

	if(!strcmp(opt,"+"))
	{
		for(i=1;i<cnt;i++)
			result+=num[i];
	}
	else if(!strcmp(opt,"-"))
	{
		for(i=1;i<cnt;i++)
			result-=num[i];
	}
	else if(!strcmp(opt,"*"))
	{
		for(i=1;i<cnt;i++)
			result*=num[i];
	}
	else if(!strcmp(opt,"/"))
	{
		for(i=1;i<cnt;i++)
		{
			result/=num[i];
		}
	}
	else printf("wrong opt\n");
	return result;
}


int main(int argc, char* argv[])
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM,0);

	addr.sin_family= AF_INET;
	addr.sin_port = htons(12345);
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
	int str_len;
	int newfd;
	char message[BUF_SIZE];
	struct sockaddr_in client_addr;
	int addr_len;
	int i;
	int cnt=0;

	/////parsing var//////
	int op_num;
	char op_nd[4]={0,};
	int* op_ptr;
	int op_list[BUF_SIZE]={0,};
	char op_tor[2];
	int lp;
	int j,k;
	/////cal var///////
	float result=0;

	addr_len = sizeof(client_addr);

	while(1){
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
			//////////////////////parsing//////////////////////////
			op_num=message[0];

			lp=0;
			for(j=1;j<str_len-1;j+=4)
			{
				for(k=0;k<4;k++)
					op_nd[k]=message[j+k];
				op_ptr=op_nd;
				op_list[lp]=*op_ptr;
				printf("op_list: %d\n",op_list[lp]);
				lp++;
			}
			op_tor[0]=message[str_len-1];
			op_tor[1]=0;
			/////////////////////calculation///////////////////////
			result=calculation(op_num, op_list, op_tor);
			printf("result : %.3lf\n",result);
			write(newfd,&result,sizeof(&result));
		}
		close(newfd);
	}
	close(sockfd);
}
