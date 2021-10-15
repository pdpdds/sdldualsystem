#ifndef __FONT_EXTRACTOR
#define __FONT_EXTRACTOR

void font_release(void);

void font_extract(char *prefix, char *graphic_file, int nc, char *characters);
void font_preview(char *prefix);

void font_print(char *font, char *text);
void font_print(char *font, char *text, int x_offset,float alpha);
void font_print(int x, int y, int z, float angle, float zoom, char *font, char *text);
void font_print(int x, int y, int z, float angle, float zoom, char *font, char *text, int x_offset);
void font_print(int x, int y, int z, float angle, float zoom, float alpha, char *font, char *text, int x_offset);

void font_print_c(char *font, char *text);
void font_print_c(char *font, char *text, int x_offset,float alpha);
void font_print_c(int x, int y, int z, float angle, float zoom, char *font, char *text);
void font_print_c(int x, int y, int z, float angle, float zoom, char *font, char *text, int x_offset);
void font_print_c(int x, int y, int z, float angle, float zoom, float alpha, char *font, char *text, int x_offset);

void font_box_c(char *font, char *text, int *x, int *y, int *dx, int *dy);
void font_box_c(char *font, char *text, int *x, int *y, int *dx, int *dy, int x_offset);

void font_print(char *prefix, char *text, int x_offset, bool y_center);
void font_print(int x, int y, int z, float angle, float scale, char *font, char *text, int x_offset, bool y_center);

#endif

