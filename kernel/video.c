// #include "videoFrame.h"
// #include "../uart/uart0.h"
// #include "../uart/uart1.h"
// #include "framebf.h"
//
//
// void wait_msec(unsigned int msVal){
//     register unsigned long f, t, r, expiredTime; //64 bits
//
//     // Get the current counter frequency (Hz), 1Hz = 1 pulses/second
//     asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
//
//     // Read the current counter value
//     asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
//
//     // Calculate expire value for counter
//     expiredTime = t + f * msVal / 1000;
//
//     do {
//         asm volatile ("mrs %0, cntpct_el0" : "=r"(r));
//     } while(r < expiredTime);
// }
//
//
// void playVideo() {
//     uart_puts("\nPLAYING VIDEO");
//     uart_puts("\nPress x to stop playing");
//
//     char input = '\0'; // Initialize with a value that is not 'x'
//
//     while (input != 'x') {
//         for (int i = 0; i < epd_bitmap_allArray_LEN; i++) {
//             uart_puts("\nDisplaying frame ");
//             uart_dec(i);
//
//             // Update the frame by clearing and drawing in one pass
//             drawImageVideo((800 - 640) / 2, (600 - 360) / 2,
//             epd_bitmap_allArray[i], 640, 360); wait_msec(100);
//         }
//         input = uart_getc();  // Read input again
//     }
// }
//
//
//
//
