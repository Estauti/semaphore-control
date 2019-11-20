#include<stdio.h>
#include <stdbool.h>

struct semaphore {
  int id;
  int qtd_cars;
};

int popCar(struct semaphore *sem) {
  return --sem->qtd_cars;
}

int main() {
  struct semaphore sem[2], *sem_p[2];
  sem[0].id = 0;
  sem[0].qtd_cars = 5;
  sem_p[0] = &sem[0];

  sem[1].id = 1;
  sem[1].qtd_cars = 7;
  sem_p[1] = &sem[1];

  printf("%d\n", sem_p[0]->qtd_cars);
  popCar(sem_p[0]);
  printf("%d\n", sem_p[0]->qtd_cars);

  return 0;
}