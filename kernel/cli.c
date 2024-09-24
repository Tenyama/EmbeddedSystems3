// ----------------------------------- cli.c
#include "../game/game.h"
#include "../uart/uart1.h"
#include "./draw.h"
#include "./framebf.h"
#include "./mbox.h"
#include "./menu.h"
#include "./video.h"
#define MAX_CMD_SIZE 100
#define HISTORY_SIZE 10
#define BACKSPACE 8
#define DELETE 127
#define COMMAND_SIZE 12

// Command list
static char *command_list[] = {
    "help",  "showinfo",   "baudrate",      "stopbits",
    "clear", "clear QEMU", "display image", "game",
    "exit",  "play video", "checkbaudrate", "checkstopbits"};

// CLI function with command history, backspace, and tab-completion
void cli() {
  static char history[HISTORY_SIZE][MAX_CMD_SIZE]; // Command history
  static char cli_buffer[MAX_CMD_SIZE]; // CLI buffer for current command
  static int index = 0;                 // CLI buffer index
  static int history_index = 0;         // History index
  static int current_index = 0;         // Current history index

  uart_puts("*.* Group1_OS *.* > "); // Prompt message

  while (1) {
    char c = uart_getc(); // Get user input

    // Handle backspace and delete
    if (c == BACKSPACE || c == DELETE) {
      if (index > 0) {
        index--;
        uart_puts("\b \b");
        cli_buffer[index] = '\0';
      }
    }
    // Handle history navigation with + and -
    else if (c == '+') {
      if (current_index > 0) {
        current_index--;
        uart_puts("\n*.* Group1_OS *.* > ");
        string_copy(cli_buffer, history[current_index]);
        uart_puts(cli_buffer);
        index = string_length(cli_buffer);
      }
    } else if (c == '-') {
      if (current_index < history_index - 1) {
        current_index++;
        uart_puts("\n*.* Group1_OS *.* > ");
        string_copy(cli_buffer, history[current_index]);
        uart_puts(cli_buffer);
        index = string_length(cli_buffer);
      }
    }
    // Handle tab-completion for both normal commands and "help <command>"
    else if (c == '\t') {
      // Handle completion for "help <command>"
      if (string_starts_with(cli_buffer, "help ") && index >= 5) {
        cli_buffer[index] = '\0';
        char *match = "";
        int match_count = 0;

        // Find matching commands after "help "
        for (int i = 0; i < COMMAND_SIZE; i++) {
          if (string_starts_with(command_list[i], cli_buffer + 5)) {
            if (match_count == 0) {
              match = command_list[i];
            }
            match_count++;
          }
        }

        // If one match, autocomplete
        if (match_count == 1) {
          string_copy(cli_buffer + 5, match);
          uart_puts("\r*.* Group1_OS *.* > ");
          uart_puts(cli_buffer);
          index = string_length(cli_buffer);
        }
        // If multiple matches, display all possibilities
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
      // Handle normal command completion
      else {
        cli_buffer[index] = '\0';
        char *match = "";
        int match_count = 0;

        // Find matching commands
        for (int i = 0; i < COMMAND_SIZE; i++) {
          if (string_starts_with(command_list[i], cli_buffer)) {
            if (match_count == 0) {
              match = command_list[i];
            }
            match_count++;
          }
        }

        // If one match, autocomplete
        if (match_count == 1) {
          string_copy(cli_buffer, match);
          uart_puts("\r*.* Group1_OS *.* > ");
          uart_puts(cli_buffer);
          index = string_length(cli_buffer);
        }
        // If multiple matches, display all possibilities
        else if (match_count > 1) {
          uart_puts("\nPossible matches:\n");
          for (int i = 0; i < COMMAND_SIZE; i++) {
            if (string_starts_with(command_list[i], cli_buffer)) {
              uart_puts(command_list[i]);
              uart_puts("\t");
            }
          }
          uart_puts("\n*.* Group1_OS *.* > ");
          uart_puts(cli_buffer);
        }
      }
    }
    // Handle Enter key
    else if (c == '\r' || c == '\n') {
      cli_buffer[index] = '\0';

      // Store the current command into history
      for (int i = 0; i <= index; i++) {
        history[history_index][i] = cli_buffer[i]; // Copy each character
      }

      uart_puts("\n");
      history_index = (history_index + 1) % HISTORY_SIZE;
      current_index = history_index;
      index = 0;

      // Break out of the loop after Enter key
      break;
    }
    // Handle normal character input and echo back
    else if (index < MAX_CMD_SIZE - 1) {
      cli_buffer[index] = c;
      index++;
      uart_sendc(c); // Echo the character back
    }
  }

  // Process the entered command
  if (string_compare(cli_buffer, "exit")) {
    uart_puts("Shutting down...\n");
    uart_puts("Press CTRL + C to return to Terminal\n");
    asm volatile("wfi");
  }
  if (string_starts_with(cli_buffer, "help ")) {
    char command[MAX_CMD_SIZE];
    string_copy(command, cli_buffer + 5); // Skip "help " and copy the rest
    handle_help_command(command);
  } else if (string_compare(cli_buffer, "help")) {
    draw_command_table(); // Show the command menu if just "help" is entered
  } else if (string_starts_with(cli_buffer, "baudrate ")) {
    handle_baudrate_command_uart1(cli_buffer);
  } else if (string_starts_with(cli_buffer, "stopbits ")) {
    handle_stopbits_command_uart1(cli_buffer);
  } else if (string_compare(cli_buffer, "showinfo")) {
    display_board_info();
  } else if (string_compare(cli_buffer, "checkbaudrate")) {
    check_baudrate_uart1();
  } else if (string_compare(cli_buffer, "checkstopbits")) {
    check_stopbits_uart1();
  } else if (string_compare(cli_buffer, "display image")) {
    clearScreen();
    draw();
  } else if (string_compare(cli_buffer, "play video")) {
    clearScreen();
    playVideo();
  } else if (string_compare(cli_buffer, "clear")) {
    clear_term_Screen();
  } else if (string_compare(cli_buffer, "clear QEMU")) {
    clearScreen();
  } else if (string_compare(cli_buffer, "game")) {
    playGame();
  } else if (string_compare(cli_buffer, "exit")) {
    uart_puts("Shutting down...\n");
    asm volatile("wfi");
  } else {
    uart_puts("Invalid command format. Use 'help' or 'help <command_name>' to "
              "get valid commands.\n");
  }
}
