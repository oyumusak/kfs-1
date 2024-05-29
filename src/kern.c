#include "inc/kern.h"
#include "inc/terminal.h"

int	kmain()
{
	initTerminal();

	init_idt();
	kb_init();
	enable_interrupts();
	
	return (0);
}