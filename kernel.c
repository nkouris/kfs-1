#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

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

static inline uint8_t	vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return (fg | bg << 4);
}

static inline uint16_t	vga_entry(uint16_t uc, uint16_t color)
{
	return (uc | color << 8);
}


struct terminal_state {
	size_t		row;
	size_t		column;
	uint8_t		color;
	uint16_t	*buffer;
};

struct terminal_state	terminal_cond = {0};

void					terminal_init(void)
{
	size_t	index = 0;

	terminal_cond.row = 0;
	terminal_cond.column = 0;
	terminal_cond.color = vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
	terminal_cond.buffer = (uint16_t *)0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			index = y * VGA_WIDTH + x;
			terminal_cond.buffer[index] = vga_entry(' ', terminal_cond.color);
		}
	}
}

void					terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_cond.buffer[index] = vga_entry(c, color);
}

void					terminal_putchar(char c)
{
	if (c != '\n')
		terminal_putentryat(c, terminal_cond.color, terminal_cond.column, terminal_cond.row);
	if (++terminal_cond.column == VGA_WIDTH
		|| c == '\n')
	{
		terminal_cond.column = 0;
		if (++terminal_cond.row == VGA_HEIGHT)
			terminal_cond.row = 0;
		terminal_putentryat('1', terminal_cond.color, terminal_cond.column, terminal_cond.row);
	}
}

void					terminal_write(const char *data, size_t len)
{
	for (size_t i = 0; i < len; i++)
		terminal_putchar(data[i]);
}

void					kernel_main(void)
{
	char	*str = "CrappyKernel v0.01\n";

	terminal_init();
	terminal_write(str, strlen(str));
}
