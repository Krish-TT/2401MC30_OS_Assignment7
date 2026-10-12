#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_THREADS 3
#define INCREMENTS 100000

volatile int counter = 0;

void thread_worker(void *arg) {
  for(int i = 0; i < INCREMENTS; i++) {
    int value = counter;
    for (volatile int delay = 0; delay < 32; delay++)
      ;
    counter = value + 1;
  }
  exit(0);
}

int main(int argc, char *argv[]) {
  for(int i = 0; i < NUM_THREADS; i++) {
    void *stack = sbrk(4096); // Allocate user stack
    int tid = clone(thread_worker, (void*)(long)i, stack);
    if(tid < 0) {
      printf("Error creating thread %d\n", i);
      exit(1);
    }
  }

  for(int i = 0; i < NUM_THREADS; i++) {
    join();
  }

  printf("Expected Counter Value: %d\n", NUM_THREADS * INCREMENTS);
  printf("Actual Counter Value  : %d\n", counter);
  
  if (counter < NUM_THREADS * INCREMENTS) {
    printf("Race condition detected as expected!\n");
  }

  exit(0);
}