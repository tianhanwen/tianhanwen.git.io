#include "net.h"

int main(int argc, const char *argv[])
{
	int sockfd;
	char buf[SIZE];
	struct sockaddr_in servaddr;

	if(argc<3)
	{
		printf("Usage : %s <IP> <PORT>\n",argv[0]);
		exit(-1);
	}

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket");
		exit(-1);
	}
    printf("sockfd : %d\n",sockfd);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[2]));
	servaddr.sin_addr.s_addr=inet_addr(argv[1]);

/*
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
	{
		perror("connect");
		exit(-1);
	}
//	printf("sockfd:%d\n",sockfd);
//	*/
 	while(1)
	{
		bzero(buf,SIZE);
		printf("please input('quit' exit): ");
		if(fgets(buf,SIZE-1,stdin)==NULL)
		{
			continue;
		}
		sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
		if(strcmp(buf,"quit\n")==0)
		{
			printf("client is exiting\n");
			break;
		}
	}
	close(sockfd);
	return 0;
}
