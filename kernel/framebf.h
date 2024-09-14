#ifndef FRAMEBF_H
#define FRAMEBF_H

void framebf_init();
void drawPixelARGB32(int x, int y, unsigned int attr);
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr,
                    int fill);
void drawLine(int x1, int y1, int x2, int y2, unsigned int attr);
double sqrt(double number);
void drawLCircle(int center_x, int center_y, int radius, unsigned int attr,
                 int fill);
void draw_char(int x, int y, char c, unsigned int color, int scale);
void draw_string(int x, int y, const char *str, unsigned int color, int scale);
void drawImageVideo(int start_x, int start_y, const unsigned int data[], int imageWidth, int imageHeight);

#endif // FRAMEBF_H
