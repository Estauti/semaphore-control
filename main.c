#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS 2
#define CARS_PER_RELEASE 2

struct semaphore {
  int id;
  int cars_count;
};

struct semaphore sem[NUM_THREADS];

int popCarAt(int semaphore_index) {
  return --sem[semaphore_index].cars_count;
}

void sleepMilliseconds(int milliseconds) {
  struct timespec tim;
  tim.tv_sec = 0;
  tim.tv_nsec = milliseconds*1000000L;
  nanosleep(&tim , NULL);
}

void initializeSemaphores() {
  srand (time(NULL));

  for (int i = 0; i < NUM_THREADS; i++) {
    sem[i].id = i;
    sem[i].cars_count = rand() % 10;

    printf("rua %d, com %d carros\n", i, sem[i].cars_count);
  }
}

int incomingCarsAt(int street) {
  int cars_count = rand() % 3 + 1;
  sem[street].cars_count += cars_count;
  printf("na rua '%d', chegaram %d carros, ficando com %d\n", street, cars_count, sem[street].cars_count);
  return sem[street].cars_count;
}

int busiestSemaphore() {
  int semaphore_index = 0;
  int max_cars = 0;

  for (size_t i = 0; i < NUM_THREADS; i++) {
    if (sem[i].cars_count > max_cars) {
      semaphore_index = i;
      max_cars = sem[i].cars_count;
    }
  }
  return semaphore_index;
}

int numberCarsToRelease(int current_cars_count) {
  if (current_cars_count >= 2) {
    return 2;
  }
  else if (current_cars_count == 1) {
    return 1;
  }
  else {
    return 0;
  }
}

int releaseSemaphoreCars(int semaphore_index) {
  int qty_cars = numberCarsToRelease(sem[semaphore_index].cars_count);

  for (size_t i = 0; i < qty_cars; i++)
  {
    popCarAt(semaphore_index);
  }

  printf("liberou %d carros da rua '%d', ficando com %d carros\n", qty_cars, semaphore_index, sem[semaphore_index].cars_count);
  return qty_cars;
}

void releaseSemaphore(int semaphore_index) {
  // cada semáforo libera 2 carros, demorando 200 ms pra tal
  int number_released_cars = releaseSemaphoreCars(semaphore_index);
  sleepMilliseconds(number_released_cars*100);
}

int main() {
  initializeSemaphores();

  while(1) {
    sleep(2);
    // a cada 400 ms adiciona novos carros
    incomingCarsAt(rand() % NUM_THREADS);

    // a cada 200 ms mede quantidade de carros por semáforo
    // necessário nesse caso onde já tenho contabilizado as quantidades?

    // a cada 200 ms escolhe o semáforo que será aberto
    int semaphore_index = busiestSemaphore();
    releaseSemaphore(semaphore_index);
  }

  return 0;
}