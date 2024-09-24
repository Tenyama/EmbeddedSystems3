#ifndef MENU_H
#define MENU_H

#define MAX_CMD_SIZE 100
#define BACKSPACE 8
#define DELETE 127

// methods for displaying
void print_logo();
void draw_command_table();

// methods for handling commands
void handle_help_command(char command[]);
void handle_stopbits_command(char *command);
void handle_stopbits_command(char *command);

// methods handling the font and format of string
int string_compare(const char *str1, const char *str2);
int string_starts_with(const char *str, const char *prefix);
void string_copy(char *dest, const char *src);
int string_length(char x[MAX_CMD_SIZE]);
int string_to_int(const char *str);

void display_board_info();
void handle_baudrate_command_uart1(char *command);
void handle_stopbits_command_uart1(char *command);
void int_to_str(int value, char *str);
void clearScreen();
void clear_line();
#endif // MENU_H
