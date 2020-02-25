#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

int main(int argc, const char *argv[])
{
	FILE *fp;
	//命令执行结果放置在fp指向的结构体缓存中
	fp = popen("cat /etc/passwd","r");
	char buf[512];
	memset(buf,0,sizeof(buf));
	while(fgets(buf,sizeof(buf),fp)!=NULL)
	{
		printf("%s",buf);
	}
	pclose(fp);

	printf("-------------------------\n");
	//向fp结构体写入管道数据
	fp=popen("wc -l","w");
	fprintf(fp,"1\n2\n3\n");
	pclose(fp);

	return 0;
}
