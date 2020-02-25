#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;

	printf("current pid : %d\n", getpid());
	if ((pid = fork()) < 0) {
		perror("fork");  
		exit(-1);
	}
	else if (pid == 0) {
		sleep(1);
		printf("child pid : %d, my parent pid : %d\n", 
				getpid(), getppid());
		/* if (execl("/bin/ls", "ls", "-a", "-l", "/etc", NULL) < 0) { */
			/* perror("execl"); */
		/* }   */
		/* exit(2); */
	}
	else {
		pid = wait(NULL); /* wait child exit */ 
		printf("parent pid : %d, catch child(pid : %d) exit\n", 
				getpid(), pid);
	}

	return 0;
}  

