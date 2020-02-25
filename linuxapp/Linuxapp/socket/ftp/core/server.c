#include "ftp.h"

typedef struct
{
	int type;
	int len;
	char path[128];
	char filename[64];
	char file[N][N];
	char data[256];
}MSG;

int do_client(int acceptfd);
int do_client1(int acceptfd);
void do_list(int acceptfd,MSG *msg);

int do_help(int acceptfd,MSG *msg)
{
	DIR *dirp=NULL;
	struct dirent *dp=NULL;
	if((dirp=opendir(msg->path))==NULL)
	{
		perror("opendir");
		return -1;
	}
	else
	{
		printf("path open ok!\n");
	}
    strcpy(msg->data,"OK");
	if(send(acceptfd,msg,sizeof(MSG),0)<0)
	{
		perror("send");
		exit(-1);
	}
	return 1;
}

void do_list(int acceptfd,MSG *msg)
{
	DIR *dirp=NULL;
	struct dirent *dp=NULL;
	if((dirp=opendir(msg->path))==NULL)
	{
		perror("opendir");
		exit(-1);
	}
	msg->len=0;
	while((dp=readdir(dirp))!=NULL)
	{ 
		if(strncmp(dp->d_name,".",1)==0)
			continue;
		strcpy(msg->file[msg->len],dp->d_name);
		msg->len++;
	}
	strcpy(msg->data,"OK");
	if(send(acceptfd,msg,sizeof(MSG),0)<0)
	{
		perror("send");
		exit(-1);
	}
	else
	{
		printf("file list had send ok!\n");
	}
	closedir(dirp);
}

int do_get(int acceptfd,MSG *msg)
{
	int i=0;
	int t=0;
	int flag=0;
	FILE *fp;
	char *ret;
	char fn[128][128]={0};
	char buf[128]={0};
	DIR *dirp=NULL;
	struct dirent *dp=NULL;

	if(chdir(msg->path)<0)
	{
		perror("chdir");
		exit(-1);
	}
	bzero(buf,sizeof(buf));
	if((dirp=opendir(msg->path))==NULL)
	{
		perror("opendir");
		exit(-1);
	}
	while((dp=readdir(dirp))!=NULL)
	{ 
		if(strncmp(dp->d_name,".",1)==0)
			continue;
		strcpy(fn[t],dp->d_name);
			t++;
	}	

	for(i=0;i<t;i++)
	{
		if(strcmp(fn[i],msg->filename)==0)
		{
			if((fp=fopen(msg->filename,"r+"))==NULL)
			{
				perror("get open");
				exit(-1);
			}
		    printf("get %s from %s\n",msg->filename,msg->path);
			do
			{
				ret=fgets(buf,sizeof(buf),fp);
			}while(ret==NULL);
			strcpy(msg->data,buf);
			if(send(acceptfd,msg,sizeof(MSG),0)<0)
			{
				perror("send");
				exit(-1);
			}
			flag=1;
			break;

		}
	}
	if(flag==0)
	{
		strcpy(msg->data,"ERROR");
		if(send(acceptfd,msg,sizeof(MSG),0)<0)
		{
			perror("send");
			exit(-1);
		}
		printf("Not find %s\n",msg->filename);
	}
	fclose(fp);
	return 0;
}
/*int do_get(int acceptfd,MSG *msg)
{
	int i=0;
	int t=0;
	FILE *fp;
	char *ret;
	char fn[128][128]={0};
	char buf[128]={0};
	DIR *dirp=NULL;
	struct dirent *dp=NULL;
	if((dirp=opendir(msg->path))==NULL)
	{
		perror("opendir");
		exit(-1);
	}

	if(chdir(msg->path)<0)
	{
		perror("chdir");
		exit(-1);
	}
	bzero(buf,sizeof(buf));
    printf("get %s from %s\n",msg->filename,msg->path);
	while((dp=readdir(dirp))!=NULL)
	{ 
		if(strncmp(dp->d_name,".",1)==0)
			continue;
		strcpy(fn[t],dp->d_name);
			t++;
	}	

	while((strcmp(fn[i++],msg->filename)!=0)&&(i<t))
	{
		continue;
	}
	
	printf("%s\n",msg->path);

	if(i>=t)
	{
		printf("%s,%s\n",msg->filename,msg->path);
		strcpy(msg->data,"ERROR");
		if(send(acceptfd,msg,sizeof(MSG),0)<0)
		{
			perror("send");
			exit(-1);
		}
		printf("Not find %s\n",msg->filename);
		i=0;
	}
	else
	{
		if((fp=fopen(msg->filename,"r+"))==NULL)
		{
			perror("get open");
			exit(-1);
		}
		do
		{
			ret=fgets(buf,sizeof(buf),fp);
		}while(ret==NULL);
		strcpy(msg->data,buf);
		if(send(acceptfd,msg,sizeof(MSG),0)<0)
		{
			perror("send");
			exit(-1);
		}
	}
	fclose(fp);
	return 0;
}*/
/*int do_get(int acceptfd,MSG *msg)
{
	int fd;
	int ret;
	char buf[128]={0};
	bzero(buf,sizeof(buf));
    printf("get %s from %s\n",msg->filename,msg->path);
	if(chdir(msg->path)<0)
	{
		perror("chdir");
		exit(-1);
	}
	if((fd=open(msg->filename,O_RDONLY))<0)
	{
		perror("get open");
		exit(-1);
	}
	do
	{
		ret=read(fd,buf,sizeof(buf));
	}while(ret<0);
	strcpy(msg->data,buf);
    if(send(acceptfd,msg,sizeof(MSG),0)<0)
	{
		perror("send");
		exit(-1);
	}
	close(fd);
	return 0;
}*/

int do_put(int acceptfd,MSG *msg)
{
	int fd;
	char buf[128];
	bzero(buf,sizeof(buf));
    printf("put %s to %s\n",msg->filename,msg->path);

	strcpy(buf,msg->data);
	if(chdir(msg->path)<0)
	{
		perror("chdir");
		exit(-1);
	}
	if((fd=open(msg->filename,O_WRONLY|O_CREAT,0666))<0)
	{
		perror("open");
		exit(-1);
	}
	if(write(fd,buf,strlen(buf))<0)
	{
		perror("write");
		exit(-1);
	}
	strcpy(msg->data,"OK");
    if(send(acceptfd,msg,sizeof(MSG),0)<0)
	{ 
		perror("send");
		exit(-1);
	}
	close(fd);

	return 0;
}

int main(int argc, const char *argv[])
{
	int sockfd,acceptfd;
	char ipv4_addr[16];
	socklen_t pstrlen;
	char buff[16];
	struct sockaddr_in serveraddr,cliaddr;
	pid_t pid;

	if(argc<2)
	{
		printf("Usage : %s <port>\n",argv[0]);
		return -1;
	}

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket");
		exit(-1);
	}
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(atoi(argv[1]));
	serveraddr.sin_addr.s_addr=htons(INADDR_ANY);

	if(bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0)
	{
		perror("bind");
		exit(-1);
	}

	if(listen(sockfd,5)<0)
	{
		perror("lieten");
		exit(-1);
	}
//handle EXIT_ZOMBIE
	signal(SIGCHLD,SIG_IGN);
	
	pstrlen=sizeof(cliaddr);
	while(1)
	{
		if((acceptfd=accept(sockfd,(struct sockaddr *)&cliaddr,&pstrlen))<0)
		{
			perror("accept");
			exit(-1);
		}
		if(!inet_ntop(AF_INET,(void *)&cliaddr.sin_addr,ipv4_addr,sizeof(buff)))
		{
			perror("inet_ntop");
			exit(-1);
		}
		printf("client (%s:%d)connected\n",ipv4_addr,ntohs(serveraddr.sin_port));
		if((pid=fork())<0)
		{
			perror("fork");
			exit(-1);
		}
		else if(pid==0)
		{//child process is used to handle the message which from the client
			close(sockfd);
//			if(do_client1(acceptfd)>1)
//			{
//				while(do_help(acceptfd)>0)
					do_client1(acceptfd);
//			}
		}
		else
		{//parent process is used to accept the command which from the client
			close(acceptfd);
		}
	}
	return 0;
}

int do_client1(int acceptfd)
{
	MSG msg;
	while(recv(acceptfd,&msg,sizeof(msg),0)>0)
	{
		switch(msg.type)
		{
		case H:
			if(do_help(acceptfd,&msg)>0)
				do_client(acceptfd);
			break;
		default:
			printf("Invalid message!\n");
		}
	}
	printf("client exit\n");
	close(acceptfd);
	exit(0);

	return 0;

}

int do_client(int acceptfd)
{
	MSG msg;
	while(recv(acceptfd,&msg,sizeof(msg),0)>0)
	{
		switch(msg.type)
		{
		case L:
			do_list(acceptfd,&msg);
			break;
		case G:
			do_get(acceptfd,&msg);
			break;
		case P:
			do_put(acceptfd,&msg);
			break;
		default:
			printf("Invalid message!\n");
		}
	}
	printf("client exit\n");
	close(acceptfd);
	exit(0);

	return 0;
}
