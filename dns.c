#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char* argv[])
{
	struct hostent *hst;

	hst =gethostbyname(argv[1]);
	if(hst == NULL)
	{
	fprintf(stderr, "Cannot find IP adress from %s\n", argv[1]);
	return;
	}
	printf("Official name: %s\n",hst->h_name);
	
	int i;
	for(i=0;hst->h_aliases[i];i++)
	{
	printf("Aliases %d: %s\n", i+1, hst->h_aliases[i]);
	}
	printf("Address type: %s\n",(hst->h_addrtype == PF_INET)? "PF_INET":"Unknown");

	for(i=0;hst->h_addr_list[i];i++){
	printf("IP addr %d: %s\n", i+1, inet_ntoa(*(struct in_addr*)hst->h_addr_list[i]));
	}
}

