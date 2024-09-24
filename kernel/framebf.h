#ifndef FRAMEBF_H
#define FRAMEBF_H

// Constants
#define COLOR_DEPTH 32
#define PIXEL_ORDER 0
#define CLEAR_COLOR 0x00000000  // Black color in ARGB format

// External global variables for screen info
extern unsigned int width, height, pitch;
extern unsigned char *fb;

// Function declarations

/**
 * Initialize the framebuffer with the screen resolution.
 * Sets the framebuffer and screen dimensions.
 */
void framebf_init();

/**
 * Draw a pixel at the specified (x, y) position with the specified ARGB32 color.
 * @param x The X position.
 * @param y The Y position.
 * @param attr The ARGB32 color value.
 */
void drawPixelARGB32(int x, int y, unsigned int attr);

/**
 * Get the ARGB32 pixel color at the specified (x, y) position.
 * @param x The X position.
 * @param y The Y position.
 * @return The ARGB32 color value of the pixel.
 */
unsigned int getPixelARGB32(int x, int y);

/**
 * Draw a rectangle between (x1, y1) and (x2, y2) with the specified ARGB32 color.
 * @param x1 The starting X position.
 * @param y1 The starting Y position.
 * @param x2 The ending X position.
 * @param y2 The ending Y position.
 * @param attr The ARGB32 color value.
 * @param fill Set to 1 to fill the rectangle, or 0 for an outline.
 */
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill);

/**
 * Draw a line from (x1, y1) to (x2, y2) with the specified ARGB32 color.
 * @param x1 The starting X position.
 * @param y1 The starting Y position.
 * @param x2 The ending X position.
 * @param y2 The ending Y position.
 * @param attr The ARGB32 color value.
 */
void drawLine(int x1, int y1, int x2, int y2, unsigned int attr);

/**
 * Draw a vertical line from (x1, y1) to (x2, y2) with the specified ARGB32 color.
 * @param x1 The starting X position.
 * @param y1 The starting Y position.
 * @param x2 The ending X position.
 * @param y2 The ending Y position.
 * @param attr The ARGB32 color value.
 */
void drawLineVetical(int x1, int y1, int x2, int y2, unsigned int attr);

/**
 * Draw a circle with the given center, radius, and ARGB32 color.
 * @param center_x The X position of the center.
 * @param center_y The Y position of the center.
 * @param radius The radius of the circle.
 * @param attr The ARGB32 color value.
 * @param fill Set to 1 to fill the circle, or 0 for an outline.
 */
void drawLCircle(int center_x, int center_y, int radius, unsigned int attr, int fill);

/**
 * Erase a circle with the given center and radius by replacing it with the background.
 * @param center_x The X position of the center.
 * @param center_y The Y position of the center.
 * @param radius The radius of the circle.
 */
void eraseLCircle(int center_x, int center_y, int radius);

/**
 * Draw a character at the specified (x, y) position with the specified color and scale.
 * @param x The X position.
 * @param y The Y position.
 * @param c The character to draw.
 * @param color The ARGB32 color value.
 * @param scale The scaling factor for the character.
 */
void draw_char(int x, int y, char c, unsigned int color, int scale);

/**
 * Draw a string starting at the specified (x, y) position with the specified color and scale.
 * @param x The X position.
 * @param y The Y position.
 * @param str The string to draw.
 * @param color The ARGB32 color value.
 * @param scale The scaling factor for the string.
 */
void draw_string(int x, int y, const char *str, unsigned int color, int scale);

/**
 * Draw an image or video frame at the specified (start_x, start_y) position.
 * @param start_x The X position to start drawing.
 * @param start_y The Y position to start drawing.
 * @param data The array containing the image pixel data.
 * @param imageWidth The width of the image.
 * @param imageHeight The height of the image.
 */
void drawImageVideo(int start_x, int start_y, const unsigned int data[], int imageWidth, int imageHeight);

/**
 * Get the background pixel color at the specified (x, y) position.
 * @param x The X position.
 * @param y The Y position.
 * @return The ARGB32 color value of the pixel.
 */
unsigned int getBackgroundPixelColor(int x, int y);

/**
 * Draw a pixel from the background image at the specified (x, y) position.
 * @param x The X position.
 * @param y The Y position.
 */
void drawBackgroundPixel(int x, int y);

/**
 * Clear the terminal screen by sending ANSI escape codes.
 */
void clear_term_Screen();

#endif // FRAMEBF_H
