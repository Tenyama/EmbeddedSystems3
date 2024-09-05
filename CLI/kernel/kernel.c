// #include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "framebf.h"
#include "mbox.h"

// int string_compare(const char *str1, const char *str2) {
//     while (*str1 && *str2) {
//         if (*str1 != *str2) {
//             return 0;
//         }
//         str1++;
//         str2++;
//     }
//     return (*str1 == '\0' && *str2 == '\0');
// }

// int string_starts_with(const char* str, const char* prefix) {
//     while (*prefix) {
//         if (*str++ != *prefix++) {
//             return 0;
//         }
//     }
//     return 1;
// }

// void string_copy(char* dest, const char* src) {
//     while (*src) {
//         *dest++ = *src++;
//     }
//     *dest = '\0';
// }

// void handle_command(char command[]) {
//     if (string_compare(command, "showinfo")) {
//         uart_puts("Executing 'showinfo' command...\n");
//     } else if (string_compare(command, "clear")) {
//         uart_puts("Executing 'clear' command...\n");
//     } else if (string_compare(command, "exit")) {
//         uart_puts("Executing 'exit' command...\n");
//     } else if (string_compare(command, "draw image")) {
//         uart_puts("Executing 'draw image' command...\n");
//     } else if (string_compare(command, "baudrate")) {
//         uart_puts("Executing 'baudrate' command...\n");
//     } else if (string_compare(command, "stopbit")) {
//         uart_puts("Executing 'stopbit' command...\n");
//     } else {
//         uart_puts("Unknown command: ");
//         uart_puts(command);
//         uart_puts("\n");
//     }
// }

// void cli() {
//     char cli_buffer[MAX_CMD_SIZE];
//     int index = 0;

//     uart_puts("*.* Group1_OS *.* > ");

//     while (1) {
//         char c = uart_getc();

//         // Handle backspace
//         if (c == BACKSPACE || c == DELETE) {
//             if (index > 0) {
//                 index--;
//                 uart_puts("\b \b");
//             }
//         } else if (c == '\r' || c == '\n') {  // Handle Enter key
//             cli_buffer[index] = '\0';  // Null-terminate the string
//             uart_puts("\n");
//             break;
//         } else if (index < MAX_CMD_SIZE - 1) {
//             cli_buffer[index++] = c;
//             uart_sendc(c);  // Echo the character back
//         }
//     }

//     if (string_starts_with(cli_buffer, "help ")) {
//         char command[MAX_CMD_SIZE];
//         string_copy(command, cli_buffer + 5);  // Skip "help " and copy the rest
//         handle_command(command);
//     }
//     else if (string_compare(cli_buffer, "help")) 
//     {
//       draw_command_table();  // Show the command menu if just "help" is entered
//     } 
//     else {
//         uart_puts("Invalid command format. Use 'help <command_name>'.\n");
//     }
// }

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
