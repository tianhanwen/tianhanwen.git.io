#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N 64
typedef struct message{
    char buf[N];
    int len;
}msg_t;

pthread_mutex_t mymutex;

void* reverse_msgbuf(void* arg)
{
    msg_t *msg = (msg_t *)arg;
    int i = 0;
    char tmp;
    while(1)
    {
        pthread_mutex_lock(&mymutex);
        for(i = 0; i < msg->len/2; i ++)
        {
            tmp          = msg->buf[i];
            msg->buf[i]  = msg->buf[msg->len - i - 1];
            msg->buf[msg->len - i -1] = tmp;
        }
        pthread_mutex_unlock(&mymutex);
    }
}

void* printf_msgbuf(void* arg)
{
    msg_t *msg = (msg_t *)arg;
    while(1)
    {
        pthread_mutex_lock(&mymutex);
        printf("buf :%s\n",msg->buf);
        pthread_mutex_unlock(&mymutex);
        sleep(1);
    }
}

int main(int argc, const char *argv[])
{
    msg_t msg = {"123456789",9};

    pthread_t tid[2];
    pthread_mutex_init(&mymutex,NULL);

    pthread_create(&tid[0],NULL,reverse_msgbuf,(void *)&msg);
    pthread_create(&tid[1],NULL,printf_msgbuf,(void *)&msg);

    pause();

    return 0;
}