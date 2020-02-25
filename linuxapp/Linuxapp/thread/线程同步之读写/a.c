#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct{
	int value;
	pthread_cond_t rc;
	pthread_mutex_t rm;
	int r_wait;

	pthread_cond_t wc;
	pthread_mutex_t wm;
	int w_wait;

}Storage;

void set_data(Storage *s,int value)
{
	s->value=value;
}
int get_data(Storage *s)
{
	return s->value;
}

void *set_th(void *arg)
{
	Storage *s;
	s=(Storage *)arg;
	int i = 0;
	for(;i<100;i++)
	{
		set_data(s,i+100);
		printf("0x%lx(%-5d) write data: %d\n",
				pthread_self(),i,i+100);
		pthread_mutex_lock(&s->rm);
		while(!s->r_wait)
		{
			pthread_mutex_unlock(&s->rm);
			sleep(1);
			pthread_mutex_lock(&s->rm);
		}
		s->r_wait=0;
		pthread_mutex_unlock(&s->rm);
		pthread_cond_broadcast(&s->rc);
		/*
		 *写线程等待阻塞
		 *等待读线程读取完数据后通知唤醒它
		 *然后继续写入数据
		 * */
		pthread_mutex_lock(&s->wm);
		s->w_wait=1;
		pthread_cond_wait(&s->wc,&s->wm);
		pthread_mutex_unlock(&s->wm);

	}
	return (void*)0;
}

void *get_th(void *arg)
{
	Storage *s;
	s=(Storage*)arg;
	int i=0;
	for(;i<100;i++)
	{
		pthread_mutex_lock(&s->rm);
		s->r_wait=1;
		pthread_cond_wait(&s->rc,&s->rm);
		pthread_mutex_unlock(&s->rm);
		int value=get_data(s);
		printf("0x%lx(%-5d) read data: %d\n",
				pthread_self(),i,value);

		pthread_mutex_lock(&s->wm);
		//判断写线程是否准备好
		while(!s->w_wait)
		{
			pthread_mutex_unlock(&s->wm);
			sleep(1);
			pthread_mutex_lock(&s->wm);
		}
		s->w_wait=0;
		pthread_mutex_unlock(&s->wm);
		pthread_cond_broadcast(&s->wc);
	}
	return (void*)0;
}

int main(int argc, const char *argv[])
{
	pthread_t rth,wth;
	Storage s;
	s.r_wait=0;
	s.w_wait=0;
	pthread_mutex_init(&s.rm,NULL);
	pthread_mutex_init(&s.wm,NULL);
	pthread_cond_init(&s.rc,NULL);
	pthread_cond_init(&s.wc,NULL);

	if(pthread_create(&rth,NULL,
				get_th,(void*)&s)!=0)
	{
		perror("pthread_create failed");
	}
	if(pthread_create(&wth,NULL,
				set_th,(void*)&s)!=0)
	{
		perror("pthread_create failed");
	}

	pthread_join(rth,NULL);
	pthread_join(wth,NULL);


	pthread_mutex_destroy(&s.rm);
	pthread_mutex_destroy(&s.wm);
	pthread_cond_destroy(&s.rc);
	pthread_cond_destroy(&s.wc);
	return 0;
}
