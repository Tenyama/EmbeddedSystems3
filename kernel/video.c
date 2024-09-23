#include "../game/interrupt.h"
#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "./framebf.h"
#include "./menu.h"
// #include "./videoFrame.h"

// void playVideo() {
//   clearScreen();
//   uart_puts("\nPLAYING VIDEO");
//   uart_puts("\nPress x to stop playing");

//   char input = '\0'; // Initialize with a value that is not 'x'

  // while (input != 'x') {
  //   for (int i = 0; i < epd_bitmap_allArray_LEN; i++) {
  //     uart_puts("\nDisplaying frame ");
  //     uart_dec(i);

//       // Update the frame by clearing and drawing in one pass
//       drawImageVideo((700 - 640) / 2, (800 - 360) / 2, epd_bitmap_allArray[i],
//                      640, 360);
//       wait_msec(100);
//     }
//     input = uart_getc(); // Read input again
//   }
// }
