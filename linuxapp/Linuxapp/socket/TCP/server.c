#include <errno.h>

#include "tcpnet.h"

int main(void)
{
	int fd = -1, newfd;
	int ret = -1;
	struct sockaddr_in sin;
	char buf[BUFSIZ];

	/*1.创建IPV4的TCP套接字 */	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		perror("socket error!");
		exit(1);	
	}
	/* 2.绑定在服务器的IP地址和端口号上*/
	 /* 2.1 填充struct sockaddr_in结构体*/
	 bzero(&sin, sizeof(sin));
	 sin.sin_family = AF_INET;
	 sin.sin_port = htons(SERV_PORT);
	 sin.sin_addr.s_addr = inet_addr(SERV_IPADDR); 
       //  bzero(sin.sin_zero, 8);		
	 /* 2.2 绑定*/
	if(bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("bind");
	       	exit(1);	
	}	
	/*3.把主动套接字变为被动套接字 */
	listen(fd, 5);
        
	/*4.阻塞等待客户端连接请求 */
	if( (newfd = accept(fd, NULL, NULL)) < 0) { /*不关心连接客户端信息*/
		perror("accept");
		exit(1);	
	}

	/*5.和客户端进行信息的交互(读写) */
	while(1) {
		bzero(buf, BUFSIZ);
	
		do {
			ret =read(newfd, buf,BUFSIZ-1);
		}while(ret < 0 && EINTR == errno);
		if(ret < 0) {
			continue;
		}
		/* 处理数据*/
		//....
		printf("Receive data: %s\n",buf);
		if(!ret || !strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))) { //客户端发送了quit字符
			break;	
		}
	}
	close(newfd);

	close(fd);

	return 0;
}
