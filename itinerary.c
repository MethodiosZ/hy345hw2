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
int count=0;

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
  pthread_mutex_lock(&mutex);
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
  //printf("\nEntered University\n");
  //sleep(studytime);
  //printf("Leaving after studying for %d seconds\n",studytime);
  return NULL;
}

void* BusMove(void* vargp){
  sleep(3);
  printf("\nBus is on the way to %s\n",vargp);
  sleep(T);
  printf("\nBus arrived at %s\n",vargp);
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
  if(pthread_create(&bus,NULL,BusMove,"University")!=0){
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
  /* while(StopA!=NULL){
    printf("\nStudent %d (%s) boarded to the bus.\n",StopA->uid,Departments[StopA->department]);
    if(Bus==NULL){
      Bus = StopA;
      StopA = StopA->next;
      Bus->next = NULL;
    } else if(MaxDepInBus[StopA->department]>=N/4) {
      printf("\nStudent %d (%s) cannot enter the bus\n",StopA->uid,Departments[StopA->department]);
    } else {
      MaxDepInBus[StopA->department]++;
      student* temp = Bus;
      while(temp->next!=NULL){
	temp = temp->next;
      }
      temp->next = StopA;
      StopA = StopA->next;
      temp->next->next = NULL;
  }*/
  pthread_mutex_destroy(&mutex);
  puts("\nAll students from stop A went to the University and came back");
  return 0;
}
