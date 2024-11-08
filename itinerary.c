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
sem_t boarding;
sem_t nextroute;
sem_t stopb;
int count=0;
int BusLOcation = 1; //1 for StopA / 2 for StopB
int globalpos=0;
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

void AddAndRemoveToList(student* addlist,student* removelist,student* elem){
  if(addlist==NULL){
    addlist = elem;
    removelist = removelist->next;
    addlist->next = NULL;
  } else {
    student* temp = addlist;
    while(temp->next!=NULL){
      temp = temp->next;
    }
    temp->next = elem;
    student* rem = removelist;
    student* prev;
    while(rem->uid!=elem->uid){
      prev=rem;
      rem=rem->next;
    }
    prev->next = rem->next;
    temp->next->next = NULL;
  }
}

void* Studying(void* vargp){
  int studytime = rand() % (15-5+1) + 5;
  int dep = rand() % (3+1);
  int id,position=1;
  pthread_mutex_lock(&mutex); //lock is needed to keep count stable
  printf("\nStudent %d (%s) created.\n",1+count++,Departments[dep]);
  student *new = (student*)malloc(sizeof(student));
  id = count;
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
  student* temp = StopA;
  while(temp->uid!=id){
    temp=temp->next;
    position++;
  }
  while(position>1+globalpos){
    sem_wait(&boarding);
    sem_post(&boarding);
  }
  if(Bus==NULL){
    printf("\nStudent %d (%s) boarded to the bus.\n",id,Departments[dep]);
    MaxDepInBus[dep]++;
    Bus = temp;
    StopA = StopA->next;
    Bus->next = NULL;
    printLists();
    globalpos++;
    sem_post(&boarding);
  } else if(MaxDepInBus[dep]>=N/4) {
    printf("\nStudent %d (%s) cannot enter the bus\n",id,Departments[dep]);
    globalpos++;
    sem_post(&boarding);
    sem_wait(&nextroute);
  } else {
    printf("\nStudent %d (%s) boarded to the bus.\n",id,Departments[dep]);
    MaxDepInBus[temp->department]++;
    student* new = Bus;
    while(new->next!=NULL){
      new = new->next;
    }
    new->next = temp;
    student* rem = StopA,*prev=StopA;
    while(rem->uid!=temp->uid){
      prev=rem;
      rem=rem->next;
    }
    if(prev->uid==rem->uid){
      prev = prev->next;
    } else {
      prev->next = rem->next;
    }
    new->next->next = NULL;
    printLists();
    globalpos++;
    sem_post(&boarding);
  }
  sem_wait(&arrival);
  if(StopB==NULL){
    printf("\nStudent %d (%s) got off the bus.\n",id,Departments[dep]);
    StopB = temp;
    Bus = Bus->next;
    StopB->next =NULL;
    printLists();
    sem_post(&arrival);
  } else {
    printf("\nStudent %d (%s) got off the bus.\n",id,Departments[dep]);
    student* new = StopB;
    while(new->next!=NULL){
      new = new->next;
    }
    new->next = temp; //FIX BUS LIST
    new->next->next = NULL;
    printLists();
    sem_post(&arrival);
  }
  if(University==NULL){
    printf("\nStudent %d (%s) went to University.\n",id,Departments[dep]);
    University = temp;
    StopB = StopB->next;
    University->next = NULL;
    printLists();
  } else {
    printf("\nStudent %d (%s) went to University.\n",id,Departments[dep]);
    student* new = University;
    while(new->next!=NULL){
      new = new->next;
    }
    new->next = temp; //FIX STOP B LIST
    new->next->next = NULL;
    printLists();
  }
  sleep(studytime);
  if(StopB==NULL){
    printf("\nStudent %d (%s) studied for %d seconds, and now is heading to Stop B\n",id,Departments[dep],studytime);
    StopB = temp;
    University = University->next;
    StopB->next = NULL;
    printLists();
  } else {
    printf("\nStudent %d (%s) studied for %d seconds, and now is heading to Stop B\n",id,Departments[dep],studytime);
    student* new = StopB;
    while(new->next!=NULL){
      new = new->next;
    }
    new->next = temp; //FIX UNIVERSITY LIST
    new->next->next = NULL;
    printLists();
  }
  sem_wait(&arrival);
  printf("\nStudent %d (%s) boarded to the bus.\n",id,Departments[dep]);
  printf("\nStudent %d (%s) went home.\n",id,Departments[dep]);
  return NULL;
}

void* BusMove(void* vargp){
  pthread_barrier_wait(&barrier); //Wait for initialization of students
  while(StopA!=NULL || StopB!=NULL){
    sleep(3); //Bus waits 3 seconds to stop A
    puts("\nBus is on the way to University");
    //sem_wait(&nextroute);
    sleep(T); //Bus is moving
    puts("\nBus arrived at University");
    printLists();
    sem_post(&arrival);
    sleep(3); //Bus waits 3 seconds to stop B
    puts("\nBus is on the way to stop A");
    //sem_wait(&arrival);
    sleep(T); //Bus is moving
    puts("\nBus arrived at stop A");
    sem_post(&nextroute);
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
  puts("\nAll students from stop A went to the University and came back");
  return 0;
}
