#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
 
void my_handler(int s){
           printf("\nCaught signal %d\n",s);
           exit(1); 
 
}
 
int main(int argc,char** argv)
{
 
   struct sigaction sigHandler;
 
   sigHandler.sa_handler = my_handler;
 
   sigaction(SIGINT, &sigIntHandler, NULL);
 
   pause();
 
   return 0;    
}
