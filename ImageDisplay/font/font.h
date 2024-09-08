#ifndef FONT_H
#define FONT_H
#include "../kernel/gpio.h"

// Define the number of characters and the size of each character bitmap
#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define FONT_CHAR_COUNT 95

// Extern declaration of the font data array
extern const uint8_t font_data[FONT_CHAR_COUNT][FONT_HEIGHT];

#endif // FONT_H
