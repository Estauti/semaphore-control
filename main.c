#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS 2

struct semaphore {
  int id;
  int qtd_cars;
};

struct semaphore sem[NUM_THREADS];

int popCar(struct semaphore *sem) {
  return --sem->qtd_cars;
}

void initializeSemaphores() {
  srand (time(NULL));

  for (int i = 0; i < NUM_THREADS; i++) {
    sem[i].id = i;
    sem[i].qtd_cars = rand() % 10;

    printf("rua %d, com %d carros\n", i, sem[i].qtd_cars);
  }
}

int incomingCarsAt(int street) {
  printf("incomingCarsAt\n");
  int qty_cars = rand() % 3 + 1;
  sem[street].qtd_cars += qty_cars;
  printf("na rua '%d', chegaram %d carros, ficando com %d\n", street, qty_cars, sem[street].qtd_cars);
  return sem[street].qtd_cars;
}

int main() {
  initializeSemaphores();

  while(1) {
    sleep(1);
    incomingCarsAt(rand() % NUM_THREADS);
  }

  return 0;
}