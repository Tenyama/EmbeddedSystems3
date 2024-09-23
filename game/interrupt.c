/* Functions to delay, set/wait timer */

int globalTimerInterrupt;

void wait_msec(unsigned int msVal) {
  register unsigned long f, t, r, expiredTime; // 64 bits

  // Get the current counter frequency (Hz), 1Hz = 1 pulses/second
  asm volatile("mrs %0, cntfrq_el0" : "=r"(f));

  // Read the current counter value
  asm volatile("mrs %0, cntpct_el0" : "=r"(t));

  // Calculate expire value for counter
  /* Note: both expiredTime and counter value t are 64 bits,
  thus, it will still be correct when the counter is overflow */
  expiredTime = t + f * msVal / 1000;

  do {
    asm volatile("mrs %0, cntpct_el0" : "=r"(r));
  } while (r < expiredTime);
}

void set_wait_timer(int set, unsigned int msVal) {
  static unsigned long expiredTime = 0; // declare static to keep value
  register unsigned long r, f, t;

  if (set) { /* SET TIMER */
    // Get the current counter frequency (Hz)
    asm volatile("mrs %0, cntfrq_el0" : "=r"(f));
    
    // Read the current counter
    asm volatile("mrs %0, cntpct_el0" : "=r"(t));

    // Calculate expired time:
    expiredTime = t + f * msVal / 1000;
  } else { /* WAIT FOR TIMER TO EXPIRE */
    do {
      asm volatile("mrs %0, cntpct_el0" : "=r"(r));
      globalTimerInterrupt = 0;
    } while (r < expiredTime);
    globalTimerInterrupt = 1;
  }
}

/*
set_wait_timer(1, 500);
//LAM GI DO
set_wait_timer(0, 0);
*/