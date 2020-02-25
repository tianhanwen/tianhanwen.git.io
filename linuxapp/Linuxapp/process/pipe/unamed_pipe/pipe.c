#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() 
{
  pid_t pid;
  char buf[32];
  int pfd[2];

  if (pipe(pfd) < 0) {
    perror("pipe"); 
    exit(-1);
  }
  
  if ((pid = fork()) < 0) {
    perror("fork");  
	  exit(-1);
  }
  else if (pid == 0) {          //  子进程
    strcpy(buf, "child");
    write(pfd[1], buf, 32);
    exit(0);   
  }
  else {        //  父进程
    wait(NULL);
    read(pfd[0], buf, 32);
    printf("%s\n", buf);
  }

  return  0;
}       
  
