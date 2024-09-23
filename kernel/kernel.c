// #include "../uart/uart0.h"
#include "../game/interrupt.h"
#include "../uart/uart1.h"
#include "./cli.h"
#include "./framebf.h"
#include "./mbox.h"
#include "./menu.h"

void main() {
  uart_init();
  framebf_init();
  print_logo();

  uart_puts("To display the acceptable commands, please enter only 'help' for "
            "further assistance.\n");
  uart_puts("=================================================================="
            "======================\n\n");

  while (1) {
    cli();
  }
}
