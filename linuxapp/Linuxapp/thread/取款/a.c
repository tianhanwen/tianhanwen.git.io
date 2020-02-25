#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct
{
	int code;
	double balance;
	pthread_rwlock_t rwlock;
}account;

typedef struct
{
	char name[32];
	account *account_p;
	double amt;
}oparg;

account *create_account(int code,double balance)
{
	account *a = (account *)malloc(sizeof(account));
	assert(a!=NULL);
	a->code=code;
	a->balance=balance;
	pthread_rwlock_init(&a->rwlock,NULL);
	return a;
}
void destory_account(account *a)
{
	assert(a!=NULL);
	pthread_rwlock_destroy(&a->rwlock);
	free(a);
}
double withdraw(account *a,double amt)
{
	assert(a!=NULL);
	pthread_rwlock_wrlock(&a->rwlock);
	if(amt<0||amt>a->balance)
	{
		pthread_rwlock_unlock(&a->rwlock);
		return 0.0;
	}
	double balance = a->balance;
	sleep(1);
	balance -= amt;
	a->balance = balance;
	pthread_rwlock_unlock(&a->rwlock);
	return amt;
}
double deposite(account *a,double amt)
{
	assert(a!=NULL);
	pthread_rwlock_wrlock(&a->rwlock);
	if(amt<0)
	{
		pthread_rwlock_unlock(&a->rwlock);
		return 0.0;
	}
	double balance = a->balance;
	sleep(1);
	balance += amt;
	a->balance = balance;
	pthread_rwlock_unlock(&a->rwlock);
	return amt;
}
double get_balance(account *a)
{
	assert(a!=NULL);
	pthread_rwlock_rdlock(&a->rwlock);
	double balance = a->balance;
	pthread_rwlock_unlock(&a->rwlock);
	return balance;
}

void *withdraw_fn(void *arg)
{
	oparg *oa;
	double amt;
	oa=(oparg *)arg;
	amt = withdraw(oa->account_p,oa->amt);
	printf("%8s(0x%lx)withdraw %f from account %d\n",
			oa->name,pthread_self(),amt,oa->account_p->code);
	return 0;
}
void *deposite_fn(void *arg)
{
	oparg *oa;
	double amt;
	oa=(oparg *)arg;
	amt = deposite(oa->account_p,oa->amt);
	printf("%8s(0x%lx) deposite %f from account %d\n",
			oa->name,pthread_self(),amt,oa->account_p->code);
	return 0;
}

int main(int argc, const char *argv[])
{
	pthread_t boy,girl;
	account *a = create_account(100001,10000);
	oparg o1,o2;
	strcpy(o1.name,"boy");
	o1.account_p = a;
	o1.amt = 10000;
	strcpy(o2.name,"girl");
	o2.account_p = a;
	o2.amt = 10000;

	if(pthread_create(&boy,NULL,
				withdraw_fn,(void *)&o1)!=0)
		perror("pthread_create");
	if(pthread_create(&girl,NULL,
				withdraw_fn,(void *)&o2)!=0)
		perror("pthread_create");
	pthread_join(boy,NULL);
	pthread_join(girl,NULL);
	
	printf("account:%lf\n",get_balance(a));
	destory_account(a);
	return 0;
}
