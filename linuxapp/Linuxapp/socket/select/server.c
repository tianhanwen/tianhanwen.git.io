#include <pthread.h>
#include <signal.h>
#include "net.h"

void cli_data_handle (void *arg);

void sig_child_handle(int signo)
{
	if(SIGCHLD == signo) {
		waitpid(-1, NULL,  WNOHANG);
	}
}
int main (void)
{

	int fd = -1;
	struct sockaddr_in sin;
	
	signal(SIGCHLD, sig_child_handle);	

	/* 1. 创建socket fd */
	if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror ("socket");
		exit (1);
	}

	/*优化4： 允许绑定地址快速重用 */
	int b_reuse = 1;
	setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &b_reuse, sizeof (int));


	/*2. 绑定 */
	/*2.1 填充struct sockaddr_in结构体变量 */
	bzero (&sin, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons (SERV_PORT);	//网络字节序的端口号

	/*优化1: 让服务器程序能绑定在任意的IP上 */
#if 1
	sin.sin_addr.s_addr = htonl (INADDR_ANY);
#else
	if (inet_pton (AF_INET, SERV_IP_ADDR, (void *) &sin.sin_addr) != 1) {
		perror ("inet_pton");
		exit (1);
	}
#endif
	/*2.2 绑定 */
	if (bind (fd, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
		perror ("bind");
		exit (1);
	}

	/*3. 调用listen()把主动套接字变成被动套接字 */
	if (listen (fd, BACKLOG) < 0) {
		perror ("listen");
		exit (1);
	}
	printf ("Server starting....OK!\n");
	int newfd = -1;
	/*4. 阻塞等待客户端连接请求 */
	
        struct sockaddr_in cin;
        socklen_t addrlen = sizeof (cin);
	while(1) {
		pid_t pid = -1;
		if ((newfd = accept (fd, (struct sockaddr *) &cin, &addrlen)) < 0) {
                        perror ("accept");
                        break;
                }
		/*创建一个子进程用于处理已建立连接的客户的交互数据*/
		if((pid = fork()) < 0) {
			perror("fork");
			break;
		}
		
		if(0 == pid) {  //子进程中
			close(fd);
			char ipv4_addr[16];
                
			if (!inet_ntop (AF_INET, (void *) &cin.sin_addr, ipv4_addr, sizeof (cin))) {
                        	perror ("inet_ntop");
                        	exit (1);
               	 	}

               	 	printf ("Clinet(%s:%d) is connected!\n", ipv4_addr, ntohs(cin.sin_port));	
			cli_data_handle(&newfd);		
			return 0;	
		
		} else { //实际上此处 pid >0, 父进程中 
			close(newfd);
		}
		

	}		


	close (fd);
	return 0;
}

void cli_data_handle (void *arg)
{
	int newfd = *(int *) arg;

	printf ("Child handling process: newfd =%d\n", newfd);

	//..和newfd进行数据读写


	int ret = -1;
	char buf[BUFSIZ];
	char resp_buf[BUFSIZ+10];
	while (1) {
		bzero (buf, BUFSIZ);
		do {
			ret = read (newfd, buf, BUFSIZ - 1);
		} while (ret < 0 && EINTR == errno);
		if (ret < 0) {

			perror ("read");
			exit (1);
		}
		if (!ret) {				//对方已经关闭
			break;
		}
		printf ("Receive data: %s\n", buf);
		
		if (!strncasecmp (buf, QUIT_STR, strlen (QUIT_STR))) {	//用户输入了quit字符
			printf ("Client(fd=%d) is exiting!\n", newfd);
			break;
		}

		bzero(resp_buf, BUFSIZ+10);
		
		strncpy(resp_buf, SERV_RESP_STR, strlen(SERV_RESP_STR));
		strcat(resp_buf, buf); 	
		do {
			ret = write(newfd, resp_buf, strlen(resp_buf));	
		}while(ret < 0 && EINTR == errno);
		
	}
	close (newfd);

}
