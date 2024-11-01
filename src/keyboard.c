#include "inc/kern.h"
#include "inc/keyboard.h"
#include "inc/fdf.h"

extern t_fdf env;

struct IDT_entry IDT[256];

void	init_idt()
{
	unsigned int offset = (unsigned int)keyboard_handler; // get addr in boot.s

	IDT[0x20].offset_lowerbits = offset & 0x0000FFFF; // lower 16 bits
	IDT[0x20].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x20].zero = 0;
	IDT[0x20].type_attr = IDT_INTERRUPT_GATE_32BIT;
	IDT[0x20].offset_upperbits = (offset & 0xFFFF0000) >> 16;

	// PICs = Programmable Interrupt Controllers

	// ICW1: Initialization command
	// Send a fixed value of 0x11 to each PIC to tell it to expect ICW2-4
	// Restart PIC1
	port_out(PIC1_COMMAND_PORT, 0x13);
	//port_out(PIC2_COMMAND_PORT, 0x11);
	// ICW2: Vector Offset (this is what we are fixing)
	// Start PIC1 at 32 (0x20 in hex) (IRQ0=0x20, ..., IRQ7=0x27)
	// Start PIC2 right after, at 40 (0x28 in hex)
	port_out(PIC1_DATA_PORT, 0x20);
	//port_out(PIC2_DATA_PORT, 0x28);
	// ICW3: Cascading (how master/slave PICs are wired/daisy chained)
	// Tell PIC1 there is a slave PIC at IRQ2 (why 4? don't ask me - https://wiki.osdev.org/8259_PIC)
	// Tell PIC2 "its cascade identity" - again, I'm shaky on this concept. More resources in notes
	port_out(PIC1_DATA_PORT, 0x0);
	//port_out(PIC2_DATA_PORT, 0x0);
	// ICW4: "Gives additional information about the environemnt"
	// See notes for some potential values
	// We are using 8086/8088 (MCS-80/85) mode
	// Not sure if that's relevant, but there it is.
	// Other modes appear to be special slave/master configurations (see wiki)
	port_out(PIC1_DATA_PORT, 0x1);
	//port_out(PIC2_DATA_PORT, 0x1);
	// Voila! PICs are initialized

	// Mask all interrupts (why? not entirely sure)
	// 0xff is 16 bits that are all 1.
	// This masks each of the 16 interrupts for that PIC.
	port_out(PIC1_DATA_PORT, 0xff);
	//port_out(PIC2_DATA_PORT, 0xff);

	// Last but not least, we fill out the IDT descriptor
	// and load it into the IDTR register of the CPU,
	// which is all we need to do to make it active.
	struct IDT_pointer idt_ptr;
	idt_ptr.limit = (sizeof(struct IDT_entry) * IDT_SIZE) - 1;
	idt_ptr.base = (unsigned int) &IDT;
	// Now load this IDT
	load_idt((unsigned int*)&idt_ptr);
}

void	kb_init()
{
	port_out(PIC1_DATA_PORT, 254); // 11111110 in bin
}

void handle_keyboard_interrupt()
{
    port_out(PIC1_COMMAND_PORT, 0x20); // End of interrupt (EOI)
    unsigned char status = port_in(KEYBOARD_STATUS_PORT);

    if (status & 0x1) {
        char keycode = port_in(KEYBOARD_DATA_PORT);
        if (keycode < 0 || keycode >= 128)
            return;

        // Ok tuşlarına göre kamera pozisyonu güncelleniyor
        if (keycode == 72) { // Up arrow
            env.camera->y_offset -= 10;
        }
        else if (keycode == 80) { // Down arrow
            env.camera->y_offset += 10;
        }
        else if (keycode == 75) { // Left arrow
            env.camera->x_offset -= 10;
        }
        else if (keycode == 77) { // Right arrow
            env.camera->x_offset += 10;
        }

        // Haritayı yeni pozisyona göre tekrar çizdir
        ft_draw(env.map, &env);
    }
}