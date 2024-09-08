#ifndef FRAMEBF_H
#define FRAMEBF_H

#define MAX_CMD_SIZE 100
#define BACKSPACE 8
#define DELETE 127

void framebf_init();
void drawPixelARGB32(int x, int y, unsigned int attr);
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr,
                    int fill);
void drawLine(int x1, int y1, int x2, int y2, unsigned int attr);
double sqrt(double number);
void drawLCircle(int center_x, int center_y, int radius, unsigned int attr,
                 int fill);

void cli();

#endif // FRAMEBF_H
