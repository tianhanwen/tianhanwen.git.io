#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

void *th_fn(void *arg)
{
	int distance;
	int i;
	int time;
	distance = (int)arg;
	for(i=0;i<distance;i++)
	{
		printf("%lx run %d\n",pthread_self(),i);
		time = (int)(drand48()*100000);
		usleep(time);
	}
	return (void *)distance;
}

int main(int argc, const char *argv[])
{
	int result;
	pthread_t rabbit,turtle;
	if(pthread_create(&rabbit,NULL,th_fn,(void *)50)!=0)
	{
		perror("pthread_create failed");
		exit(1);
	}
	if(pthread_create(&turtle,NULL,th_fn,(void *)50)!=0)
	{
		perror("pthread_create failed");
		exit(1);
	}

	pthread_join(rabbit,(void *)&result);
	printf("rabbit's distance is %d\n",result);
	
	pthread_join(turtle,(void *)&result);
	printf("turtle's distance is %d\n",result);

	printf("control thread id: %lx\n",pthread_self());
	printf("finished!\n");

	return 0;
}
