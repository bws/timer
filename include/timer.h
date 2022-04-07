#ifndef TIMER_H
#define TIMER_H
/**
 * A simple timer that uses Linux clock_gettime system call with the 
 * MONOTONIC_CLOCK to perform nanosecond resolution timing. Timer 0
 * is reserved for measuring the timer overhead.
 *
 * Usage example:
 *
 * timer_init(100);
 * timer_begin(1);
 * timer_end(1);
 * timer_destroy();
 *
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_TIMERS 6
#define MAX_NAME_SIZE 16

static struct timespec* timer_begins_by_idx[NUM_TIMERS];
static struct timespec* timer_ends_by_idx[NUM_TIMERS];
static int timer_current_by_idx[NUM_TIMERS];
static char timer_names[NUM_TIMERS][MAX_NAME_SIZE];
static int timer_name_cur = 0;

/**
   Initializes timer storage to hold n iterations
   @param iterations number of timer samples to support
*/
int timer_init(size_t iterations);

/**
   Destroy timer related resources
*/
int timer_destroy();

/**
   @return the timer index associated with name
*/
int timer_set_name(char* name);

/**
   Start the timer
*/
static inline int timer_begin(int tidx);

/**
   Stop the timer
*/
static inline int timer_end(int tidx);

/**
   @return the average of all times stored for timer tidx
*/
double timer_get_avg(int tidx);

/**
   @return the maximum of all times stored for timer tidx
*/
double timer_get_max(int tidx);

/**
   @return the minimum of all times stored for timer tidx
*/
double timer_get_min(int tidx);

/**
   @return the total of all times stored for timer tidx
*/
double timer_get_total(int tidx);

/**
   Prints the timer as a TSV list
   @return 0
*/
int timer_print_tsv(int tidx, bool header);

/* ------------------------ Begin Implementations --------------------- */

int timer_init(size_t iterations)
{
  // Create the timer arrays
  for (int i = 0; i < NUM_TIMERS; i++) {
    /* Initialize the name to just be integers */
    char name[MAX_NAME_SIZE] = {'\0'};
    snprintf(name, MAX_NAME_SIZE - 1, "%d", i);

    /* Create the arrays */
    timer_begins_by_idx[i] = calloc(iterations, sizeof(struct timespec));
    timer_ends_by_idx[i] = calloc(iterations, sizeof(struct timespec));
    timer_current_by_idx[i] = 0;
    strncpy(timer_names[i], name, MAX_NAME_SIZE);
  }

  // Use timer 0 to measure the timer overhead
  int clk = timer_set_name("CLCK");
  struct timespec t;
  for (int i = 0; i < iterations; i++) {
    timer_begin(clk);
    clock_gettime(CLOCK_MONOTONIC, &t);
    timer_end(clk);
  }
  return 0;
}

int timer_destroy() {
  // Print the timer arrays and deallocate resources
  timer_print_tsv(0, true);
  free(timer_begins_by_idx[0]);
  free(timer_ends_by_idx[0]);
  
  for (int i = 1; i < NUM_TIMERS; i++) {
    if (0 < timer_current_by_idx[i]) {
      timer_print_tsv(i, false);
    }
    free(timer_begins_by_idx[i]);
    free(timer_ends_by_idx[i]);
  }
  return 0;
}

int timer_set_name(char* name) {
  memset(timer_names[timer_name_cur], '\0', MAX_NAME_SIZE);
  strncpy(timer_names[timer_name_cur], name, MAX_NAME_SIZE - 1);
  return timer_name_cur++;
}

static double timespec_to_double(struct timespec* t)
{
  double seconds = t->tv_sec;
  double nanos = t->tv_nsec;
  return (seconds + (nanos / 1000000000 ));
}

int timer_begin(int tidx)
{
  return clock_gettime(CLOCK_MONOTONIC, (timer_begins_by_idx[tidx] + timer_current_by_idx[tidx]));
}

int timer_end(int tidx)
{
  clock_gettime(CLOCK_MONOTONIC, (timer_ends_by_idx[tidx] + timer_current_by_idx[tidx]));
  timer_current_by_idx[tidx]++;
  return 0;
}

double timer_get_avg(int tidx)
{
  return (timer_get_total(tidx) / timer_current_by_idx[tidx]);
}

double timer_get_max(int tidx)
{
  double begin = timespec_to_double(timer_begins_by_idx[tidx]);
  double end = timespec_to_double(timer_ends_by_idx[tidx]);
  double max = end - begin;
  for (int i = 0; i <  timer_current_by_idx[tidx]; i++) {
    begin = timespec_to_double(timer_begins_by_idx[tidx] + i);
    end = timespec_to_double(timer_ends_by_idx[tidx] + i);
    double t = end - begin;
    max = (t > max ? t : max);
  }
  return max;
}

double timer_get_min(int tidx)
{
  double begin = timespec_to_double(timer_begins_by_idx[tidx]);
  double end = timespec_to_double(timer_ends_by_idx[tidx]);  
  double min = end - begin;
  for (int i = 0; i <  timer_current_by_idx[tidx]; i++) {
    begin = timespec_to_double(timer_begins_by_idx[tidx] + i);
    end = timespec_to_double(timer_ends_by_idx[tidx] + i);
    double t = end - begin;
    min = (t < min ? t : min);
  }
  return min;
}

double timer_get_total(int tidx)
{
  double begin = timespec_to_double(timer_begins_by_idx[tidx]);
  double end = timespec_to_double(timer_ends_by_idx[tidx]);  
  double total = 0.0;
  for (int i = 0; i <  timer_current_by_idx[tidx]; i++) {
    begin = timespec_to_double(timer_begins_by_idx[tidx] + i);
    end = timespec_to_double(timer_ends_by_idx[tidx] + i);
    total += end - begin;
  }
  return total;
}

int timer_print_tsv(int tidx, bool header)
{
  char* name = timer_names[tidx];
  double min = timer_get_min(tidx);
  double max = timer_get_max(tidx);
  double avg = timer_get_avg(tidx);
  double ttl = timer_get_total(tidx);
  if (header)
    printf("Timer \tMin \tMax \tAvg \tTtl \n");
  printf("%s \t%.2e \t%.2e \t%.2e \t%.2e \n", name, min, max, avg, ttl);
  return 0;
}
#endif
