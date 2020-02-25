#include "tcpnet.h"

int main(void)
{
	int fd = -1;
	struct sockaddr_in sin;
	char buf[BUFSIZ];

        /*1.创建IPV4的TCP套接字 */
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if(fd < 0) {
                perror("socket error!");
                exit(1);
        }

	/*2. 连接到服务器*/
	/*2.1 填充服务器的IP地址和端口号*/
	 bzero(&sin, sizeof(sin));
         sin.sin_family = AF_INET;
         sin.sin_port = htons(SERV_PORT);
         sin.sin_addr.s_addr = inet_addr(SERV_IPADDR);

	/*2.2 链接服务器*/
	if(connect(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
                perror("connect");
                exit(1);
        }
	/* 从键盘获取数据发送给服务器*/
	while(1) {
		printf("Pls input string:\n");
		bzero(buf, BUFSIZ);
		fgets(buf, BUFSIZ-1, stdin);
		/*把buf中的数据写到网络套接字中*/	
		write(fd, buf, strlen(buf));
		if(!strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))) { //用户输入了quit字符
                        break;
                }
		
	}
	close(fd);
	return 0;
}
