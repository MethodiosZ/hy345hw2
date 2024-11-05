//Methodios Zacharioudakis 4384
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define N 12
#define T 10

sem_t mutex;

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

void* Studying(void* vargp){
  int studytime = rand() % (15-5+1) + 5;
  sem_wait(&mutex);
  printf("Entered University\n");
  sleep(studytime);
  printf("Leaving after studying for %d seconds\n",studytime);
  sem_post(&mutex);
}

void* BusMove(void* vargp){
  sem_wait(&mutex);
  printf("Bus is on the way to %s\n\n",vargp);
  sleep(T);
  printf("Bus arrived at %s\n\n",vargp);
  sem_post(&mutex);
}

int main(){
  int NoS,CoS,i;
  pthread_t bus;
  pthread_barrier_t barrier;
  student *StopA, *Bus, *University, *StopB; //Initialize lists for StopA, Bus, University, StopB
  StopA = NULL;
  Bus = NULL;
  University = NULL;
  StopB = NULL;
  sem_init(&mutex,0,1);
  puts("Enter the number of students:");
  scanf("%d",&NoS);
  if(NoS<1){ //Check for Invalid input
    perror("Invalid Number of Students! Must be 1 or more!\n");
    return 1;
  }
  pthread_t students[NoS]; //Declare student threads
  for(CoS=1;CoS<=NoS;CoS++){
    pthread_create(&students[CoS-1],NULL,Studying,NULL);
    int dep = rand() % (3+1);
    printf("\nStudent %d (%s) created.\n",CoS,Departments[dep]);
    student *new = (student*)malloc(sizeof(student));
    new->uid = CoS;
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
    printf("\nStop A:"); //print Stop A list
    student* temp = StopA;
    while(temp!=NULL){
      printf(" [%d, %s]",temp->uid,Departments[temp->department]);
      temp = temp->next;
    }
    printf("\nBus:"); //print Bus list
    temp = Bus;
    while(temp!=NULL){
      printf(" [%d, %s]",temp->uid,Departments[temp->department]);
      temp = temp->next;
    }
    printf("\nUniversity:"); //print University list
    temp = University;
    while(temp!=NULL){
      printf(" [%d, %s]",temp->uid,Departments[temp->department]);
      temp = temp->next;
    }
    printf("\nStopB:"); //print Stop B list
    temp = StopB;
    while(temp!=NULL){
      printf(" [%d, %s]",temp->uid,Departments[temp->department]);
      temp = temp->next;
    }
    printf("\n");
  }
  while(StopA!=NULL){
    printf("\nStudent %d (%s) boarded to the bus.\n",StopA->uid,Departments[StopA->department]);
    if(Bus==NULL){
      Bus = StopA;
      StopA = StopA->next;
      Bus->next = NULL;
    } else {
      student* temp = Bus;
      while(temp->next!=NULL){
	temp = temp->next;
      }
      temp->next = StopA;
      StopA = StopA->next;
      temp->next->next = NULL;
    }
    printf("\nStop A:"); //print Stop A list
    student* temp = StopA;
    while(temp!=NULL){
      printf(" [%d, %s]",temp->uid,Departments[temp->department]);
      temp = temp->next;
    }
    printf("\nBus:"); //print Bus list
    temp = Bus;
    while(temp!=NULL){
      printf(" [%d, %s]",temp->uid,Departments[temp->department]);
      temp = temp->next;
    }
    printf("\nUniversity:"); //print University list
    temp = University;
    while(temp!=NULL){
      printf(" [%d, %s]",temp->uid,Departments[temp->department]);
      temp = temp->next;
    }
    printf("\nStopB:"); //print Stop B list
    temp = StopB;
    while(temp!=NULL){
      printf(" [%d, %s]",temp->uid,Departments[temp->department]);
      temp = temp->next;
    }
    printf("\n");
  }
  pthread_create(&bus,NULL,BusMove,"University");
  pthread_join(bus,NULL);
  pthread_create(&bus,NULL,BusMove,"Stop B");
  pthread_join(bus,NULL);
  sem_destroy(&mutex);
  puts("All students from stop A went to the University and came back");
  return 0;
}
