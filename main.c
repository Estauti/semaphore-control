#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include<time.h>

#define NUM_THREADS 2

struct semaphore {
  int id;
  int qtd_cars;
};

struct semaphore sem[NUM_THREADS], *sem_p[NUM_THREADS];

int popCar(struct semaphore *sem) {
  return --sem->qtd_cars;
}

void initializeSemaphores() {
  srand (time(NULL));

  for (size_t i = 0; i < NUM_THREADS; i++) {
    sem[i].id = i;
    sem[i].qtd_cars = rand() % 10;
    sem_p[i] = &sem[i];

    printf("%d\n", sem[i].qtd_cars);
  }
}

int main() {
  initializeSemaphores();

  return 0;
}