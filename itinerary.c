//Methodios Zacharioudakis 4384
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define N 4
#define T 10

char* Departments[] = {"Math","Physics","Chemistry","CSD"};
char* StopA[200];
char* Bus[200];
char* StopB[200];
char* University[200];

pthread_mutex_t mutex;
pthread_barrier_t barrier;
sem_t arrival;
sem_t boarding;
sem_t nextroute;
sem_t stopb;
sem_t returning;
int count=0;
int globalpos=0;
int MaxDepInBus[] = {0,0,0,0};

void printLists(){
  int i = 1;
  printf("\nStop A:"); //print Stop A list
  while(strcmp(StopA[i],"\0")!=0){
    if(strcmp(StopA[i],"_")!=0){
      printf(" [%d, %s]",i,StopA[i]);
    }
    i++;
  }
  printf("\nBus:"); //print Bus list
  i=1;
  while(strcmp(Bus[i],"\0")!=0){
    if(strcmp(Bus[i],"_")!=0){
      printf(" [%d, %s]",i,Bus[i]);
    }
    i++;
  }
  printf("\nUniversity:"); //print University list
  i=1;
  while(strcmp(University[i],"\0")!=0){
    if(strcmp(University[i],"_")!=0){
      printf(" [%d, %s]",i,University[i]);
    }
    i++;
  }
  printf("\nStopB:"); //print Stop B list
  i=1;
  while(strcmp(StopB[i],"\0")!=0){
    if(strcmp(StopB[i],"_")!=0){
      printf(" [%d, %s]",i,StopB[i]);
    }
    i++;
  }
  printf("\n");
}

void* Studying(void* vargp){
  int studytime = rand() % (15-5+1) + 5;
  int dep = rand() % (3+1);
  int id,position=1; //1 for stopA, 2 for Bus, 3 for stopB, 4 for University
  pthread_mutex_lock(&mutex); //lock is needed to keep count stable
  printf("\nStudent %d (%s) created.\n",1+count++,Departments[dep]);
  id = count;
  StopA[id] = Departments[dep];
  printLists();
  pthread_mutex_unlock(&mutex);
  pthread_barrier_wait(&barrier);
  while(id>1+globalpos){
    sem_wait(&boarding);
    sem_post(&boarding);
  }
  if(MaxDepInBus[dep]>=N/4) {
    printf("\nStudent %d (%s) cannot enter the bus\n",id,Departments[dep]);
    Bus[id] = "_";
    StopB[id] = "_";
    University[id] = "_";
    globalpos++;
    sem_post(&boarding);
    sem_wait(&nextroute);
    printf("\nStudent %d (%s) boarded to the bus.\n",id,Departments[dep]);
    Bus[id] = Departments[dep];
    StopA[id] = "_";
    pthread_mutex_lock(&mutex);
    printLists();
    pthread_mutex_unlock(&mutex);
  } else {
    printf("\nStudent %d (%s) boarded to the bus.\n",id,Departments[dep]);
    Bus[id] = Departments[dep];
    StopA[id] = "_";
    MaxDepInBus[dep]++;
    globalpos++;
    pthread_mutex_lock(&mutex);
    printLists();
    pthread_mutex_unlock(&mutex);
    sem_post(&boarding);
  }
  sem_wait(&arrival);
  printf("\nStudent %d (%s) got off the bus.\n",id,Departments[dep]);
  StopB[id] = Departments[dep];
  Bus[id] = "_";
  pthread_mutex_lock(&mutex);
  printLists();
  pthread_mutex_unlock(&mutex);
  sem_post(&arrival);
  printf("\nStudent %d (%s) went to University.\n",id,Departments[dep]);
  StopB[id] = "_";
  University[id] = Departments[dep];
  pthread_mutex_lock(&mutex);
  printLists();
  pthread_mutex_unlock(&mutex);
  sleep(studytime);
  printf("\nStudent %d (%s) studied for %d seconds, and now is heading to Stop B\n",id,Departments[dep],studytime);
  StopB[id] = Departments[dep];
  University[id] = "_";
  pthread_mutex_lock(&mutex);
  printLists();
  pthread_mutex_unlock(&mutex);
  sem_wait(&stopb);
  printf("\nStudent %d (%s) boarded to the bus.\n",id,Departments[dep]);
  Bus[id] = Departments[dep];
  StopB[id] = "_";
  pthread_mutex_lock(&mutex);
  printLists();
  pthread_mutex_unlock(&mutex);
  sem_post(&stopb);
  sem_wait(&returning);
  printf("\nStudent %d (%s) went home.\n",id,Departments[dep]);
  Bus[id] = "_";
  count--;
  pthread_mutex_lock(&mutex);
  printLists();
  pthread_mutex_unlock(&mutex);
  sem_post(&returning);
  return NULL;
}

void* BusMove(void* vargp){
  pthread_barrier_wait(&barrier); //Wait for initialization of students
  while(count){
    sleep(3); //Bus waits 3 seconds to stop A
    puts("\nBus is on the way to University");
    sleep(T); //Bus is moving
    puts("\nBus arrived at University");
    sem_wait(&returning);
    sem_post(&stopb);
    pthread_mutex_lock(&mutex);
    printLists();
    pthread_mutex_unlock(&mutex);
    sem_post(&arrival);
    sleep(3); //Bus waits 3 seconds to stop B
    puts("\nBus is on the way to stop A");
    sem_wait(&stopb);
    sem_wait(&arrival);
    sleep(T); //Bus is moving
    puts("\nBus arrived at stop A");
    sem_post(&nextroute);
    sem_post(&returning);
  }
  return NULL;
}

int main(){
  int NoS,i;
  pthread_t bus;
  puts("Enter the number of students:");
  scanf("%d",&NoS);
  if(NoS<1){ //Check for Invalid input
    perror("Invalid Number of Students! Must be 1 or more!\n");
    return 1;
  }
  pthread_mutex_init(&mutex,NULL);
  pthread_barrier_init(&barrier,NULL,NoS+1);
  sem_init(&arrival,0,0);
  sem_init(&boarding,0,0);
  sem_init(&nextroute,0,0);
  sem_init(&stopb,0,0);
  sem_init(&returning,0,1);
  for(i=0;i<200;i++){
    StopA[i] = "\0";
    Bus[i] = "\0";
    StopB[i] = "\0";
    University[i] = "\0";
  }
  if(pthread_create(&bus,NULL,BusMove,NULL)!=0){
    perror("Failed to create bus thread\n");
  }
  pthread_t students[NoS]; //Declare student threads
  for(i=0;i<NoS;i++){
    if( pthread_create(&students[i],NULL,&Studying,NULL) != 0){
      perror("Failed to create student thread\n");
    }
  }
  for(i=0;i<NoS;i++){
    if (pthread_join(students[i],NULL) != 0){
      perror("Failed to join student thread\n");
    }
  }
  if(pthread_join(bus,NULL) != 0){
    perror("Failed to join bus thread\n");
  }
  pthread_mutex_destroy(&mutex);
  pthread_barrier_destroy(&barrier);
  sem_destroy(&arrival);
  sem_destroy(&boarding);
  sem_destroy(&nextroute);
  sem_destroy(&stopb);
  sem_destroy(&returning);
  puts("\nAll students from stop A went to the University and came back");
  return 0;
}
