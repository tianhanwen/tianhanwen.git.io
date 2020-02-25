#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>


#define FIFO "fifo123"

int main(int argc, const char *argv[]) {
	
	int fd;
	char *s="123456";

	if( access(FIFO, F_OK) == -1 ) { /*检查文件是否存在*/
		if(mkfifo(FIFO, 0666) == -1) { /*创建管道文件*/
			perror("mkfifo");
			return;
		}
	}
	/* if use O_RDWR to open the fifo? */
	fd = open(FIFO, O_WRONLY);
	if(fd < 0) {
		perror("open");
		exit(1);
	}
	if(write(fd, s, strlen(s))<0) {
		perror("write");
		exit(1);
	}
	printf("write buf ok\n");
	unlink(FIFO);
	close(fd);
	return 0;
}
