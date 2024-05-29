#ifndef TERMINAL_H
# define TERMINAL_H

#define VGAWIDTH 80
#define VGAHEIGHT 25

# define LINES 25 // 2 bytes each
# define COLUMNS_IN_LINE 80

extern unsigned short *termBuff;
extern const unsigned int	vgaWidth;
extern const unsigned int	vgaHeight;
extern unsigned int	prompt[8];
extern const unsigned int	promptMax;
extern unsigned short termLines[8][VGAHEIGHT * 2][VGAWIDTH];
extern unsigned int	lineCount;
extern unsigned char colorSet;
extern unsigned short (*termLinesBegin)[VGAWIDTH];
extern unsigned int arrowBuff;
extern unsigned char termNumber;


enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};  

void    update_cursor(int x, int y);
void    termReset(void);
void	flushTermOld();
void    leftRightArrowHandler(unsigned char mode);
void    initTerminal();
void	ArrowHandler(unsigned char mode);
void	flushLastRow(unsigned char *lastRow, unsigned int len);
void	write(int fd, unsigned char *str, unsigned int len);

#endif