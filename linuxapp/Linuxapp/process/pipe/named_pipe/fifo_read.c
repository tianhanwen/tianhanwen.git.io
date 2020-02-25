#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define FIFO "fifo123"

int main(int argc, const char *argv[]) {

	int fd;
	char buf[8] = {0};

	fd = open(FIFO,O_RDONLY);
	if(fd < 0) {
		perror("open error");
		return;
	}
	while(read(fd, buf, sizeof(buf)) < 0) {
		perror("read");
	}
	printf("read buf: %s\n", buf);
	close(fd);
	return 0;
}
