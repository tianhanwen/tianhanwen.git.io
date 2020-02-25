//#include <pthread.h>
#include "net.h"

void cli_data_handle(void * arg);
int main(int argc, const char *argv[])
{
	int serverfd,newserverfd;
//	int ret=0;
	socklen_t addrlen;
	char buf[SIZE];
	struct sockaddr_in servaddr,cliaddr;
	//pthread_t tid;
	char ipv4_addr[16];

	if(argc<2)
	{
		printf("Usage : %s <PORT>\n",argv[0]);
		exit(-1);
	}

	if((serverfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
	{
		perror("socket");
		exit(-1);
	}
	printf("socket serverfd : %d\n",serverfd);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[1]));
//	servaddr.sin_addr.s_addr=inet_addr(INADDR_ANY);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
//	servaddr.sin_addr.s_addr=inet_addr(argv[1]);

	if(bind(serverfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
	{
		perror("bind");
		exit(-1);
	}
	printf("blind success!\n");

/*	if(listen(serverfd,5)==-1)
	{
		perror("listern");
		exit(-1);
	}*/
	addrlen=sizeof(cliaddr);
	while(1)
	{	
		bzero(buf,SIZE);
		if((newserverfd=recvfrom(serverfd,buf,SIZE-1,0,(struct sockaddr *)&cliaddr,&addrlen))<0)
		{
			perror("recvfrom");
			exit(-1);
		}
		if(!inet_ntop(AF_INET,(void *)&cliaddr.sin_addr,ipv4_addr,htons(cliaddr.sin_port)))
		{
			perror("inet_ntop");
			exit(-1);
		}
		printf("Receive from (%s:%d),content:%s\n",ipv4_addr,ntohs(servaddr.sin_port),buf);

		if(strcmp(buf,"quit\n")==0)
		{
			printf("client(%s:%d) is exiting\n",ipv4_addr,ntohs(servaddr.sin_port));
		}
//		pthread_create(&tid,NULL,(void *)cli_data_handle,(void *)&newserverfd);
//		printf("serverfd:%d\n",serverfd);
//		printf("newserverfd:%d\n",newserverfd);
	}

	close(serverfd);
	return 0;
}
/*
void cli_data_handle(void * arg)
{
	int newserverfd=*(int *)arg;
//	printf("handle thread: newserverfd=%d\n",newserverfd);
	int ret=-1;
	char buf[SIZE];
	while(1)
	{
		bzero(buf,SIZE);
		do
		{
			ret=read(newserverfd,buf,SIZE-1);
		}while(ret<0&&EINTR==errno);
		if(ret<0)
		{
			perror("read");
			exit(1);
		}
		if(!ret)
		{
			break;
		}
	
		printf("receive data: %s\n",buf);
		printf("========GREAT==========\n");
		puts("\n");
		if(strcmp(buf,"quit\n")==0)
		{
			printf("client(newserverfd=%d) is exiting\n",newserverfd);
			break;
		}
	}
	close(newserverfd);	
}*/
