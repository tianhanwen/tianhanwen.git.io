#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>


int main(int argc, char *argv[])
{
  key_t key;
  char *addr;
  int  shmid;
  
  if ((key = ftok(".", 520)) == -1) 
  {
    perror("key");
    exit(-1);
  }   

  if ((shmid = shmget(key, 1024, IPC_CREAT|0666)) < 0) 
  {
    perror("shmget");
    exit(-1);
  }
     
  if ((addr = (char *)shmat(shmid, NULL, 0)) == (char *)-1)
  {
    perror("shmat");
    exit(-1);
  }
  
  fgets(addr, 32, stdin);
  shmdt(addr);
  shmctl(shmid, IPC_RMID, NULL);

  return 0;
}
