//Methodios Zacharioudakis 4384
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define N 4
#define T 10

char* Departments[] = {"Math","Physics","Chemistry","CSD"};

typedef enum {
  MATH,
  PHYSICS,
  CHEMISTRY,
  CSD
} departments_t;

typedef struct Student{
  int uid;
  departments_t department;
  struct Student *next;
}student;

student *StopA = NULL, *Bus = NULL, *University = NULL, *StopB = NULL; //Initialize lists for StopA, Bus, University, StopB
pthread_mutex_t mutex;
pthread_barrier_t barrier;
sem_t arrival;
int count=0;
int BusLOcation = 1; //1 for StopA / 2 for StopB
int MaxDepInBus[] = {0,0,0,0};

void printList(student* list){
  while(list!=NULL){
    printf(" [%d, %s]",list->uid,Departments[list->department]);
    list = list->next;
  }
}

void printLists(){
  printf("\nStop A:"); //print Stop A list
  printList(StopA);
  printf("\nBus:"); //print Bus list
  printList(Bus);
  printf("\nUniversity:"); //print University list
  printList(University);
  printf("\nStopB:"); //print Stop B list
  printList(StopB);
  printf("\n");
}

void* Studying(void* vargp){
  int studytime = rand() % (15-5+1) + 5;
  int dep = rand() % (3+1);
  int nextjumps=0;
  pthread_mutex_lock(&mutex); //lock is needed to keep count stable
  printf("\nStudent %d (%s) created.\n",1+count++,Departments[dep]);
  student *new = (student*)malloc(sizeof(student));
  new->uid = count;
  new->department = dep;
  new->next = NULL;
  if(StopA==NULL) StopA=new; //add students to StopA
  else {
    student* temp = StopA;
    while(temp->next!=NULL){
      temp = temp->next;
    }
    temp->next = new;
  }
  printLists();
  pthread_mutex_unlock(&mutex);
  pthread_barrier_wait(&barrier);
  pthread_mutex_lock(&mutex);
  if(Bus==NULL){
    printf("\nStudent %d (%s) boarded to the bus.\n",StopA->uid,Departments[StopA->department]);
    Bus = StopA;
    StopA = StopA->next;
    Bus->next = NULL;
    printLists();
  } else if(MaxDepInBus[StopA->department]>=N/4) {
      printf("\nStudent %d (%s) cannot enter the bus\n",StopA->uid,Departments[StopA->department]);
      printLists();
      nextjumps++;
  } else {
    printf("\nStudent %d (%s) boarded to the bus.\n",StopA->uid,Departments[StopA->department]);
    MaxDepInBus[StopA->department]++;
    student* temp = Bus;
    while(temp->next!=NULL){
      temp = temp->next;
    }
    student* jump = StopA;
    for(int i=0;i<nextjumps;i++){
      jump=jump->next;
    }
    nextjumps=0;
    temp->next = jump;
    jump = jump->next;
    temp->next->next = NULL;
    printLists();
  }
  pthread_mutex_unlock(&mutex);
  sem_wait(&arrival);
  printf("\nEntered University\n");
  sem_post(&arrival);
  sleep(studytime);
  printf("Leaving after studying for %d seconds\n",studytime);
  return NULL;
}

void* BusMove(void* vargp){
  pthread_barrier_wait(&barrier); //Wait for initialization of students
  while(StopA!=NULL || StopB!=NULL){
    sleep(3); //Bus waits 3 seconds to stop A
    puts("\nBus is on the way to University");
    sleep(T); //Bus is moving
    puts("\nBus arrived at University");
    sem_post(&arrival);
    sleep(3); //Bus waits 3 seconds to stop B
    puts("\nBus is on the way to stop A");
    sleep(T); //Bus is moving
    puts("\nBus arrived at stop A");
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
  puts("\nAll students from stop A went to the University and came back");
  return 0;
}
