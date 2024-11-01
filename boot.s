section .multiboot
    dd 0x1BADB002                ; Magic number
    dd 0x00000007                ; Flags (ALIGN | MEMINFO | VIDEO_MODE)
    dd -(0x1BADB002 + 0x00000007); Checksum
    dd 0                         ; header_addr (kullanılmıyor)
    dd 0                         ; load_addr (kullanılmıyor)
    dd 0                         ; load_end_addr (kullanılmıyor)
    dd 0                         ; bss_end_addr (kullanılmıyor)
    dd 0                         ; entry_addr (kullanılmıyor)
    dd 1                         ; mode_type (1: grafik modu)
    dd 1024                      ; width
    dd 768                       ; height
    dd 32                        ; depth (32 bit renk derinliği)



section .text
	global start
	global strlen
	global strdup
	global port_in
	global port_out
	global keyboard_handler
	global enable_interrupts
	global load_idt
	extern handle_keyboard_interrupt
	extern kmain


strlen:
	push ebp
	mov ebp, esp
	mov edi, [ebp + 8]

	mov eax, 0
	xor ecx, ecx

	loop:
	cmp byte [edi + ecx], 0
	je end
	inc ecx
	inc eax
	jmp loop

	end:
		pop ebp
		ret


load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	ret

enable_interrupts:
	sti
	ret

keyboard_handler:
	pushad
	call handle_keyboard_interrupt
	popad
	iretd


port_in:
	mov edx, [esp + 4]
	in al, dx
	ret

port_out:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, al
	ret

start:
	cli				; Disable interrupts
	push ebx
	call kmain

	infiniteLoop:
		hlt
	jmp infiniteLoop
