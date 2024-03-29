#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#define NUM_SEMAPHORES 2
#define CARS_PER_RELEASE 2
#define CARS_PER_ARRIVAL 3
#define TIME_TO_RELEASE_ONE_CAR 50

static RT_TASK incoming_cars_t, open_semaphore_t, scan_cars_qty_t;

int semaphore_index_to_release = 0;

struct semaphore {
  int id;
  int cars_count;
};

struct semaphore sem[NUM_SEMAPHORES], *sem_p[NUM_SEMAPHORES];

int popCarAt(int semaphore_index) {
  return --sem_p[semaphore_index]->cars_count;
}

int numberCarsToRelease(int current_cars_count) {
  if (CARS_PER_RELEASE >= current_cars_count) {
    return current_cars_count;
  }
  else {
    return CARS_PER_RELEASE;
  }
}

int releaseSemaphoreCars(int semaphore_index) {
  int qty_cars = numberCarsToRelease(sem_p[semaphore_index]->cars_count);

  for (size_t i = 0; i < qty_cars; i++)
  {
    popCarAt(semaphore_index);
  }

  printf("Liberou %d carros do sinaleiro '%d', ficando com %d carros\n", qty_cars, semaphore_index, sem_p[semaphore_index]->cars_count);
  return qty_cars;
}

void sleepMilliseconds(int milliseconds) {
  struct timespec tim;
  tim.tv_sec = 0;
  tim.tv_nsec = milliseconds*1000000L;
  nanosleep(&tim , NULL);
}

void setBusiestSemaphore() {
  int semaphore_index = 0;
  int max_cars = 0;

  for (size_t i = 0; i < NUM_SEMAPHORES; i++) {
    if (sem_p[i]->cars_count > max_cars) {
      semaphore_index = i;
      max_cars = sem_p[i]->cars_count;
    }
  }
  semaphore_index_to_release = semaphore_index;
}

void openSemaphore() {
  setBusiestSemaphore();

  int number_released_cars = releaseSemaphoreCars(semaphore_index_to_release);
  sleepMilliseconds(number_released_cars * TIME_TO_RELEASE_ONE_CAR);
}

int generateCarCount() {
  srand (time(NULL));
  return rand() % CARS_PER_ARRIVAL + 1;
}

void incomingCarsAt(int semaphore_index) {
  int cars_count = generateCarCount();
  sem_p[semaphore_index]->cars_count += cars_count;

  printf("No sinaleiro '%d', chegaram %d carros, ficando com %d\n", semaphore_index, cars_count, sem_p[semaphore_index]->cars_count);
}

void initializeSemaphores() {
  RTIME start = rt_timer_read();
  srand (time(NULL));

  for (int i = 0; i < NUM_SEMAPHORES; i++) {
    sem[i].id = i;
    sem[i].cars_count = rand() % 10;
    sem_p[i] = &sem[i];

    printf("Sinaleiro %d, com %d carros\n", i, sem_p[i]->cars_count);
  }
  printf("TEMPO PARA INICIALIZAR SINALEIROS: %.6f\n", (rt_timer_read() - start)/1000000.0);
}

void scanCars() {
  for (size_t i = 0; i < NUM_SEMAPHORES; i++) {
    printf("SCAN - Sinaleiro %d. Quantidade de Carros: %d\n", i, sem_p[i]->cars_count);
  }
}

void scanCarsQtyTask() {
  rt_task_set_periodic(NULL, TM_NOW, 200000000);
  while(1) {
    RTIME start = rt_timer_read();
    scanCars();

    printf("TEMPO PARA SCANNEAR CARROS: %.6f\n", (rt_timer_read() - start)/1000000.0);
    rt_task_wait_period(NULL);
  }
}

void openSemaphoreTask() {
  rt_task_set_periodic(NULL, TM_NOW, 200000000);
  while(1) {
    RTIME start = rt_timer_read();
    openSemaphore();

    printf("TEMPO PARA LIBERAR CARROS: %.6f\n", (rt_timer_read() - start)/1000000.0);
    rt_task_wait_period(NULL);
  }
}

void incomingCarsTask() {
  rt_task_set_periodic(NULL, TM_NOW, 400000000);
  while(1) {
    RTIME start = rt_timer_read();
    srand (time(NULL));
    incomingCarsAt(rand() % NUM_SEMAPHORES);

    printf("TEMPO PARA ADICIONAR CARROS: %.6f\n", (rt_timer_read() - start)/1000000.0);
    rt_task_wait_period(NULL);
  }
}

int main() {
  initializeSemaphores();

  rt_task_create(&incoming_cars_t, "incomingCarsTask", 0, 1, 0);
  rt_task_start(&incoming_cars_t, &incomingCarsTask, 0);

  rt_task_create(&open_semaphore_t, "openSemaphoreTask", 0, 1, 0);
  rt_task_start(&open_semaphore_t, &openSemaphoreTask, 0);

  rt_task_create(&scan_cars_qty_t, "scanCarsQtyTask", 0, 1, 0);
  rt_task_start(&scan_cars_qty_t, &scanCarsQtyTask, 0);

  pause();

  return 0;
}