#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int sem_setval(int semid, int val)
{
	union semun su;
	su.val = val;
	if(semctl(semid, 0, SETVAL, su)<0)
	{
		perror("semctl");
		exit(-1);
	}
	return 0;
}

int sem_getval(int semid, int val)
{
	union semun su;
	su.val = val;
	if(semctl(semid, 0, GETVAL, su)<0)
	{
		perror("semctl");
		exit(-1);
	}
	return 0;
}

int sem_d(int semid)
{
	int ret;
	if(semctl(semid, 0, IPC_RMID, 0)<0)
	{
		perror("semctl");
		exit(-1);
	}
	return 0;
}

int sem_p(int semid)
{
	int ret;
	struct sembuf sp = {0, -1, 0};
	if((ret=semop(semid, &sp, 1))<0)
	{
		perror("semop");
		exit(-1);
	}
	return ret;
}

int sem_v(int semid)
{
	int ret;
	struct sembuf sp = {0, 1, 0};
	if((ret=semop(semid, &sp, 1))<0)
	{
		perror("semop");
		exit(-1);
	}
	return ret;
}
     
     
int main(int argc, char *argv[])
{
	key_t key;
	int semid;
	if ((key = ftok(".", 520)) == -1) 
    {
        perror("ftok");  
        exit(-1);
    }

    if(semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL)<0)
	{
		perror("semget"); 
        exit(-1);
	}

	sem_setval(semid,5);
	sem_getval(semid,1);
	sleep(5);
	sem_d(semid);
	
	return 0;
}
