#ifndef KERN_H

# define KERN_H

# define IDT_SIZE 256 // Specific to x86 architecture
# define KERNEL_CODE_SEGMENT_OFFSET 0x10
# define IDT_INTERRUPT_GATE_32BIT 0x8e

# define PIC1_COMMAND_PORT 0x20
# define PIC1_DATA_PORT 0x21
# define PIC2_COMMAND_PORT 0xA0
# define PIC2_DATA_PORT 0xA1

# define KEYBOARD_DATA_PORT 0x60
# define KEYBOARD_STATUS_PORT 0x64


# pragma GCC diagnostic ignored "-Waddress-of-packed-member"

struct IDT_pointer {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

struct IDT_entry {
	unsigned short offset_lowerbits; // 16 bits
	unsigned short selector; // 16 bits
	unsigned char zero; // 8 bits
	unsigned char type_attr; // 8 bits
	unsigned short offset_upperbits; // 16 bits
} __attribute__((packed));

extern struct IDT_entry IDT[IDT_SIZE];

//boot.s funcs
extern void				keyboard_handler();
extern	char			port_in(unsigned short nbr);
extern	void			port_out(unsigned short portNo, unsigned char val);
extern void				load_idt(unsigned int* idt_address);
extern void				enable_interrupts();
extern	unsigned int				strlen(unsigned char *str);

//keyboard.c funcs
void	kb_init();
void	init_idt();
void	handle_keyboard_interrupt();

//extern	unsigned char *strdup(unsigned char *str);
void	termReset(void);
void	write(int fd, unsigned char *str, unsigned int len);
void	print(unsigned char *str);
void	putnbr(int nbr);
void	ArrowHandler(unsigned char mode);
void leftRightArrowHandler(unsigned char mode);





#endif