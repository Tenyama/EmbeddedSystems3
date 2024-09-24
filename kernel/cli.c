// ----------------------------------- cli.c ----------------------------------- //
// This file implements the Command Line Interface (CLI) for the OS, including
// features like command history, backspace, delete handling, and tab-completion.

#include "./mbox.h"             // Mailbox for system calls
#include "../game/game.h"        // Game-related functions
#include "../uart/uart1.h"       // UART functions for serial communication
#include "./draw.h"              // Functions to draw on screen
#include "./framebf.h"           // Framebuffer functions for rendering
#include "./menu.h"              // CLI menu
#include "./video.h"             // Video-related functions

// Define constants for the maximum size of commands, history size, backspace, and delete keys
#define MAX_CMD_SIZE 100
#define HISTORY_SIZE 10
#define BACKSPACE 8
#define DELETE 127
#define COMMAND_SIZE 12

// List of valid commands for the CLI
static char *command_list[] = {
    "help", "showinfo", "baudrate", "stopbits", "clear", "clear QEMU", "display image", 
    "game", "exit", "play video", "checkbaudrate", "checkstopbits"
};

// CLI function with command history, backspace, delete, and tab-completion
void cli() {
    static char history[HISTORY_SIZE][MAX_CMD_SIZE]; // Command history buffer
    static char cli_buffer[MAX_CMD_SIZE];            // Buffer for the current command
    static int index = 0;                            // Current position in the command buffer
    static int history_index = 0;                    // Position in history buffer
    static int current_index = 0;                    // Current position in history navigation

    // Display the prompt to the user
    uart_puts("*.* Group1_OS *.* > "); 

    while (1) {
        char c = uart_getc(); // Get user input character

        // Handle backspace and delete key inputs
        if (c == BACKSPACE || c == DELETE) {
            if (index > 0) {
                index--; // Move back one position in the buffer
                uart_puts("\b \b"); // Erase the last character from the display
                cli_buffer[index] = '\0'; // Null-terminate the string
            }
        } 
        // Handle history navigation using the '+' key (previous command)
       else if (c == '+') {
    if (current_index > 0) {
        current_index--;
        clear_line(); // Clear the current line
        uart_puts("*.* Group1_OS *.* > ");
        string_copy(cli_buffer, history[current_index]);
        uart_puts(cli_buffer); // Print the previous command
        index = string_length(cli_buffer);
    }
} else if (c == '-') {
    if (current_index < history_index - 1) {
        current_index++;
        clear_line(); // Clear the current line
        uart_puts("*.* Group1_OS *.* > ");
        string_copy(cli_buffer, history[current_index]);
        uart_puts(cli_buffer); // Print the next command
        index = string_length(cli_buffer);
    }
}
        // Handle tab-completion for both regular commands and "help <command>"
        else if (c == '\t') {
            // Handle completion for "help <command>"
            if (string_starts_with(cli_buffer, "help ") && index >= 5) {
                cli_buffer[index] = '\0'; // Null-terminate the string
                char *match = "";
                int match_count = 0;

                // Search for matching commands after "help "
                for (int i = 0; i < COMMAND_SIZE; i++) {
                    if (string_starts_with(command_list[i], cli_buffer + 5)) {
                        if (match_count == 0) {
                            match = command_list[i]; // Store first match
                        }
                        match_count++;
                    }
                }

                // If exactly one match, autocomplete the command
                if (match_count == 1) {
                    string_copy(cli_buffer + 5, match);
                    uart_puts("\r*.* Group1_OS *.* > ");
                    uart_puts(cli_buffer);
                    index = string_length(cli_buffer);
                } 
                // If multiple matches, display all possible completions
                else if (match_count > 1) {
                    uart_puts("\nPossible matches:\n");
                    for (int i = 0; i < COMMAND_SIZE; i++) {
                        if (string_starts_with(command_list[i], cli_buffer + 5)) {
                            uart_puts(command_list[i]);
                            uart_puts("\t");
                        }
                    }
                    uart_puts("\n*.* Group1_OS *.* > ");
                    uart_puts(cli_buffer);
                }
            } 
            // Handle regular command completion (without "help ")
            else {
                cli_buffer[index] = '\0'; // Null-terminate the command string
                char *match = "";
                int match_count = 0;

                // Search for matching commands
                for (int i = 0; i < COMMAND_SIZE; i++) {
                    if (string_starts_with(command_list[i], cli_buffer)) {
                        if (match_count == 0) {
                            match = command_list[i]; // Store the first match
                        }
                        match_count++;
                    }
                }

                // Autocomplete the command if only one match is found
                if (match_count == 1) {
                    string_copy(cli_buffer, match);
                    uart_puts("\r*.* Group1_OS *.* > ");
                    uart_puts(cli_buffer);
                    index = string_length(cli_buffer); // Move the cursor to the end of the command
                } 
                // If multiple matches, display all possible completions
                else if (match_count > 1) {
                    uart_puts("\nPossible matches:\n");
                    for (int i = 0; i < COMMAND_SIZE; i++) {
                        if (string_starts_with(command_list[i], cli_buffer)) {
                            uart_puts(command_list[i]);
                            uart_puts("\t");
                        }
                    }
                    uart_puts("\n*.* Group1_OS *.* > ");
                    uart_puts(cli_buffer); // Reprint the command buffer after displaying options
                }
            }
        } 
        // Handle Enter key (execution of the current command)
        else if (c == '\r' || c == '\n') {
            cli_buffer[index] = '\0'; // Null-terminate the command string

            // Save the current command to history
            for (int i = 0; i <= index; i++) {
                history[history_index][i] = cli_buffer[i]; // Copy the command to history
            }

            uart_puts("\n"); // New line after the command is entered
            history_index = (history_index + 1) % HISTORY_SIZE; // Update history index
            current_index = history_index; // Reset current history navigation
            index = 0; // Reset command buffer index

            // Exit the loop to process the entered command
            break;
        } 
        // Handle regular character input and echo back to the console
        else if (index < MAX_CMD_SIZE - 1) {
            cli_buffer[index] = c; // Add character to the buffer
            index++; // Move to the next position in the buffer
            uart_sendc(c); // Echo the character back to the terminal
        }
    }

    // Process the entered command after pressing Enter
    if (string_compare(cli_buffer, "exit")) {
        uart_puts("Shutting down...\n");
        uart_puts("Press CTRL + C to return to Terminal\n");
        asm volatile("wfi"); // Wait for interrupt (halt the system)
    }
    if (string_starts_with(cli_buffer, "help ")) {
        char command[MAX_CMD_SIZE];
        string_copy(command, cli_buffer + 5); // Skip the "help " part
        handle_help_command(command); // Handle "help <command>" logic
    } else if (string_compare(cli_buffer, "help")) {
        draw_command_table(); // Display the help menu if just "help" is entered
    } else if (string_starts_with(cli_buffer, "baudrate ")) { 
        handle_baudrate_command_uart1(cli_buffer); // Handle the "baudrate" command
    } else if (string_starts_with(cli_buffer, "stopbits ")) { 
        handle_stopbits_command_uart1(cli_buffer); // Handle the "stopbits" command
    } else if (string_compare(cli_buffer, "showinfo")) {
        display_board_info(); // Display system info (board revision, MAC address)
    } else if (string_compare(cli_buffer, "checkbaudrate")) {
        check_baudrate_uart1(); // Check the current baudrate
    } else if (string_compare(cli_buffer, "checkstopbits")) {
        check_stopbits_uart1(); // Check the current stopbits setting
    } else if (string_compare(cli_buffer, "display image")) {
        draw(); // Display an image
    } else if (string_compare(cli_buffer, "play video")) {
        // playVideo(); // Placeholder for playing video (unimplemented)
    } else if (string_compare(cli_buffer, "clear")) {
        clear_term_Screen(); // Clear the terminal screen
    } else if (string_compare(cli_buffer, "clear QEMU")) {
        clearScreen(); // Clear the screen in QEMU (emulator)
    } else if (string_compare(cli_buffer, "game")) {
        playGame(); // Start the game
    } else if (string_compare(cli_buffer, "exit")) {
        uart_puts("Shutting down...\n");
        asm volatile("wfi"); // Halt the system
    } else {
        uart_puts("Invalid command format. Use 'help' or 'help <command_name>' to get valid commands.\n");
    }
}
    