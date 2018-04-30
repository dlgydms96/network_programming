#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define BUF_SIZE 100
int main(int argc, char* argv[])
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM,0);

	addr.sin_family= AF_INET;
	addr.sin_port = htons(12345);
	inet_aton("127.0.0.1",&addr.sin_addr);
	memset(&(addr.sin_zero),0,8);

	connect(sockfd, (struct sockaddr*)&addr,sizeof(addr));

	int str_len;		//read byte
	int op_num;		//number of operand
	int op_temp;		//temp var for operand_number	
	int real_op;
	int bit_mask=255;
	char op_list[BUF_SIZE]={0,};	//operand list
	int lp;			//op_list pointer
	char op_tor;		//operator		
	char packet[BUF_SIZE];	//op_num+op_list+op_tor
	int i,j;
	int pp;			//pp=packet pointer
	float result;		//recv
	int op=0;

	while(1){
		////////////////////////////////op_num/////////////////////////////
		printf("Number of OP : ");
		scanf("%d",&op_temp);	
		pp=1;
		op_num=op_temp;
		////////////////////////////////op_nd//////////////////////////////
		lp=0;
		for(i=0;i<op_num;i++)
		{
			printf("Operand %d : ",i+1);
			scanf("%d",&real_op);
			for(j=0;j<4;j++)
			{
				op=real_op>>8*j;
				op_list[lp]=op&bit_mask;
				lp++;
			}
			pp+=4;
		}
		//printf("packet.op_nd : %d\n", (int)packet[1]);

		////////////////////////////////op_tor/////////////////////////////
		printf("operator : ");
		scanf("%s", &op_tor);
		////////////////////////////////make packet////////////////////////
		char packet[pp+1];
		packet[0]=(char)op_num;
		for(i=1;i<pp;i++)
		{
			packet[i]=op_list[i-1];
		}
		packet[pp]=op_tor;
		//printf("%c",packet[pp]);

		////////////////////////////////socket comm////////////////////////
		write(sockfd, packet, sizeof(packet));
		str_len = read(sockfd,&result,sizeof(&result));
		printf("result : %.3lf\n", result);
	}
	close(sockfd);
}
