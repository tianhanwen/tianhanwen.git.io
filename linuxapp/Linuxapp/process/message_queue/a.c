#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

struct msg_buf
{
    long mtype;
    char data[255];
};
 
int main()
{
    key_t key;
    int msgid;
    int ret;
    struct msg_buf msgbuf;
    int msgtype =  getpid();

    if ((key = ftok(".", 520)) == -1) 
    {
        perror("ftok");  
        exit(-1);
    }

    
    if ((msgid = msgget(key, IPC_CREAT|0666)) < 0) 
    {
        perror("msgget"); 
        exit(-1);
    }

   	msgbuf.mtype = msgtype; //        getpid();

    strcpy(msgbuf.data,"hiaaaaa");
    if(msgsnd(msgid,&msgbuf, sizeof(msgbuf.data), IPC_NOWAIT)<0)
    {
        perror("msgsnd");
        exit(-1);
    }
    sleep(1);

    memset(&msgbuf,0,sizeof(msgbuf));
    
    if(msgrcv(msgid, &msgbuf, sizeof(msgbuf.data), msgtype, IPC_NOWAIT)<0)
    {

        perror("msgrcv");
        exit(-1);
    }
    printf("recv msg: %s\n",msgbuf.data);
 
}
