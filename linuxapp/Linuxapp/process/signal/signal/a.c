#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void  myhandle(int num)
{
	printf("recv signal id num : %d \n", num);
	alarm(1);
}

int main(void)
{
	printf("main ....begin\n");
	//注册信号处理函数
	if (signal(SIGALRM, myhandle) == SIG_ERR)
	{
		perror("func signal err\n");
		return 0;
	} 

	if (signal(SIGALRM, myhandle) == SIGINT)
	{
		printf("Ctrl+C\n");
		return 0;
	} 
	
	alarm(1);
	while(1) 
	{
		pause();
		printf("pause return\n");
	}
	return 0;
}
