//Methodios Zacharioudakis 4384
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define N 20

void* myThread(void* vargp){
  sleep(1);
  puts("Print inside thread");
  return NULL;
}

int main(){
  int NoS,CoS;
  pthread_t thread_id=1;
  puts("Enter the number of students:");
  scanf("%d",&NoS);
  if(NoS<1){
    perror("Invalid Number of Students! Must be 1 or more!\n");
    return 1;
  }
  CoS=1;
  printf("%d\n",NoS);
  pthread_create(&thread_id,NULL,myThread,NULL);
  pthread_join(thread_id,NULL);
  return 0;
}
