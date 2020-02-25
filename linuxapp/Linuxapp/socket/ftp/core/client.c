#include "ftp.h"
#define PATH "./"

typedef struct
{
	int type;
	int len;
	char path[128];
	char filename[64];
	char file[N][N];
	char data[256];
}MSG;

int do_menu(int sockfd,MSG *msg);
int do_help(int sockfd,MSG *msg)
{
	msg->type=H;
	while(1)
	{
		printf("Hello,This is a FTP model!\n");
		printf("please input  PATH('#' quit):");
		scanf("%s",msg->path);
		getchar();
		if(strcmp(msg->path,"#")==0)
			break;	
		if(send(sockfd,msg,sizeof(MSG),0)<0)
		{
			perror("path send");
			exit(-1);
		}
		if(recv(sockfd,msg,sizeof(MSG),0)<0)
		{
			perror("recv");
			exit(-1);
		}
		if(strcmp(msg->data,"OK")==0)
		{
			do_menu(sockfd,msg);
		}
		else
		{
			printf("NO such path!");
		}
	}
	return 0;
}


int do_menu(int sockfd,MSG *msg)
{
	int n;
	while(1)
	{
		printf("************************************\n");
		printf("* 1.list   2.get   3.put    4.quit *\n");
		printf("************************************\n");
		printf(">>>>Now you at %s\n",msg->path);
		printf("please choose:");
		scanf("%d",&n);
		getchar();
		switch(n)
		{
			case 1:
				do_list(sockfd,msg);
				break;
			case 2:
				do_get(sockfd,msg);
				break;
			case 3:
				do_put(sockfd,msg);
				break;
			case 4:
				printf("Welcom you next visit!\n");
				close(sockfd);
				exit(0);
				break;
			default:
				printf("Invalid number!\n");
		}
	}
}

int do_list(int sockfd,MSG *msg)
{
	int i;
	msg->type=L;
	if(send(sockfd,msg,sizeof(MSG),0)<0)
	{
		perror("send");
		return -1;
	}
	if(recv(sockfd,msg,sizeof(MSG),0)<0)
	{
		perror("receive");
		return -1;
	}
	for(i=0;i<(msg->len);i++)
	{
		printf("***");
		printf("%s\n",msg->file[i]);
	}
	if(strcmp(msg->data,"OK")==0)
	{
		printf("%s list get ok!\n",msg->path);
	}
	return 0;
}
int do_get(int sockfd,MSG *msg)
{
	int fd;
	char buf[128]={0};
	msg->type=G;
	while(1)
	{
		printf(">>>>>>>>Input filename(# back): ");
		bzero(msg->filename,sizeof(msg->filename));
		scanf("%s",msg->filename);
		getchar();
		if(strcmp(msg->filename,"#")==0)
			break;

		if(send(sockfd,msg,sizeof(MSG),0)<0)
		{
			perror("send");
			return -1;
		}
		if(recv(sockfd,msg,sizeof(MSG),0)<0)
		{
			perror("receive");
			return -1;
		}
		if(strcmp(msg->data,"ERROR")!=0)
		{
			bzero(buf,sizeof(buf));
			strcpy(buf,msg->data);
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
			else
			{
				printf("get ok,you can look it now\n");
			}
			break;
		}		
		else
		{
			printf("NO such file!\n");
			exit(-1);
			continue;
		}
	}
	close(fd);
	return 0;
}

int do_put(int sockfd,MSG *msg)
{
	int i=0;
	int t=0;
	int fd;
	int ret;
	char fn[128][128]={0};
	char buf[128];
	DIR *dirp=NULL;
	struct dirent *dp=NULL;
	msg->type=P;
	bzero(buf,sizeof(buf));
	if(chdir(PATH)<0)
	{
		perror("chdir");
		exit(-1);
	}
	if((dirp=opendir(PATH))==NULL)
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
	while(1)
	{

		printf(">>>>>Input filename(# back): ");
		scanf("%s",msg->filename);
		getchar();
		if(strcmp(msg->filename,"#")==0)
			break;
		while((strcmp(fn[i++],msg->filename)!=0)&&(i<t))
		{
			continue;
		}
		if(i>=t)
		{
			printf("No such file,please rechoose!\n");
			i=0;
			continue;
		}
		else
		{
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
			if(send(sockfd,msg,sizeof(MSG),0)<0)
			{
				perror("send");
				return -1;
			}
			if(recv(sockfd,msg,sizeof(MSG),0)<0)
			{
				perror("receive");
				return -1;
			}
			if(strcmp(msg->data,"OK")==0)
			{
				printf("put success!\n");
			}
			break;
		}
	}

}

int main(int argc, const char *argv[])
{
	int n;
	int sockfd;
	MSG msg;
	struct sockaddr_in serveraddr;
	if(argc<3)
	{
		printf("Usage : %s <IP> <port>\n",argv[0]);
		return -1;
	}
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket");
		exit(-1);
	}
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(atoi(argv[2]));
	serveraddr.sin_addr.s_addr=inet_addr(argv[1]);
	if(connect(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0)
	{
		perror("connect");
		exit(-1);
	}

	while(1)
	{
		printf("********************************\n");
		printf("* 1.Can I Help You?     2.quit *\n");
		printf("********************************\n");
		printf("please choose:");
		scanf("%d",&n);
		getchar();
		switch(n)
		{
			case 1:
				do_help(sockfd,&msg);
				break;
			case 2:
				printf("Welcom you next visit!\n");
				close(sockfd);
				exit(-1);
				break;
			default:
				printf("Invalid number!\n");
		}
	}
	return 0;
}
