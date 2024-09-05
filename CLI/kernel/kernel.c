// #include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "framebf.h"
#include "mbox.h"
#include "menu.h"

void main() {
    uart_init();
    print_logo();

    uart_puts("To display the acceptable commands, please enter only 'help' for further assistance.\n");
    uart_puts("\nThen enter the help and command name for detailed information. \nThe correct format is: 'help <command_name>'\n\n");
    uart_puts("========================================================================================\n\n");

    while (1) {
        cli();
    }
}
