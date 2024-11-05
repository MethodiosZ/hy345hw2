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

void* myThread(void* vargp){
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
  pthread_t thread_id,bus;
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
  for(CoS=1;CoS<=NoS;CoS++){ //create students
    int dep = rand() % (3+1);
    printf("\nStudent %d (%s) created.\n\n",CoS,Departments[dep]);
    printf("Stop A:");
    student *new = (student*)malloc(sizeof(student));
    new->uid = CoS;
    new->department = dep;
    new->next = NULL;
    if(StopA==NULL) StopA=new;
    else {
      student* temp = StopA;
      while(temp->next!=NULL){
	temp = temp->next;
      }
      temp->next = new;
    }
    student* temp = StopA;
    while(temp!=NULL){
      printf(" [%d, %s]",temp->uid,Departments[temp->department]);
      temp = temp->next;
    }
    printf("\nBus:\nUniversity:\nStop B:\n");
  }
  pthread_create(&thread_id,NULL,myThread,NULL);
  pthread_create(&bus,NULL,BusMove,"University");
  pthread_join(thread_id,NULL);
  pthread_join(bus,NULL);
  sem_destroy(&mutex);
  puts("All students from stop A went to the University and came back");
  return 0;
}
