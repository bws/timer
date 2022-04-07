#include <timer.h>

/* A simple timer example */
int main(int argc, char** argv)
{
  const int PRINTF_TIMER = 1;
  timer_init(10000);
  timer_begin(PRINTF_TIMER);
  printf("Hello World\n");
  timer_end(PRINTF_TIMER);
  timer_destroy();
  return 0;
}
