int set_virtual_offset(unsigned int x_offset, unsigned int y_offset);
void draw_char(int x, int y, char c, unsigned int color, int scale);
void draw_string(int x, int y, const char *str, unsigned int color, int scale);
void drawImage(int start_x, int start_y, const unsigned int data[], int width,
               int height);
void draw();
