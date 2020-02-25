#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

char message[32] = "Hello";

void  thread_func(void *arg) 
{
    sleep(1);
    strcpy(message, "pthread");
    pthread_exit("waiting...");
}

int main() 
{
   pthread_t  a_thread;
   void *result;

   if(pthread_create(&a_thread, NULL, thread_func, NULL) != 0) 
   {
      printf("fail to pthread_create");  
      exit(-1);
   }

   pthread_join(&a_thread, &result);
   printf("result  is  %s\n", (char *)result);
   printf("message  is  %s\n", message);

   return 0;
}
  
