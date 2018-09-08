/** ASM Functions Declaration
 * 
 * @author: Leon Wong
 * @build: 201808310608
 * @brief: This file gives declarations of the functions from asm_funcs.nas.
 * @uasge: Just include this file, there's no need to compile it.
 * 
 */

#ifndef ASM_FUNCS_H
#define ASM_FUNCS_H 1

/**
 * @brief: HLT instruct.
 */ 
void io_hlt();

/**
 * @brief: CLI instruct.
 */ 
void io_cli();

/**
 * @brief: STI instruct.
 */
void io_sti();

/**
 * @brief: do STI HLT RET.
 */ 
void io_stihlt();

/**
 * @brief: Read in 8bits from a port.
 * @param: int port: The port to send data.
 * @warning: Only the loew 8 bits of the data will be used!
 */ 
int io_in8(int port);

/**
 * @brief: Send out 8bits to a port.
 * @param: int port: The port to send data.
 * @param: int data: The data to send.
 * @warning: Only the low 16 bits of the int port will be used!
 * @warning: Only the loew 8 bits of the data will be used!
 */ 
void io_out8(int port, int data);

/**
 * @breif: Get the eflags
 * @return: int: a 32-bit int indicating eflags.
 */ 
int io_load_eflags();

/**
 * @brief: Save the eflags.
 * @param: int eflags: the int including the eflags.
 */ 
void io_store_eflags(int eflags);

/**
 * @brief: Load GDTR.
 * @param: int limit: Limitation (see GDT)
 * @param: int addr: The address of the gdt.
 */ 
void load_gdtr(int limit, int addr);

/**
 * @brief: Load IDTR.
 * @param: int limit: Limitation (see IDT)
 * @param: int addr: The address of the idt.
 */ 
void load_idtr(int limit, int addr);

/**
 * @brief: Interrupt handler for Timer
 * @seealso: inthandler20
 */ 
void asm_inthandler20();

/**
 * @brief: Interrupt handler for Keyboard PS/2
 * @seealso: inthandler21
 */ 
void asm_inthandler21();

/**
 * @brief: Interrupt handler for IRQ7
 * @seealso: inthandler27
 */ 
void asm_inthandler27();

/**
 * @brief: Interrupt handler for Mouse PS/2
 * @seealso: inthandler2c
 */ 
void asm_inthandler2c();

/**
 * @brief: Read CR0
 * @return: (int) CR0
 */ 
int load_cr0();

/**
 * @brief: Load CR0
 * @param: (int) cr0: CR0 value to be loaded
 */ 
void store_cr0(int cr0);

/**
 * @brief: The actual part when exec memtest.
 * @seealso: memtest.
 */ 
unsigned int memtest_sub(unsigned int start, unsigned int end);

/**
 * @brief: Load TR register; TR: task register indicating which segment is currently executing \
 * Intel defined this and we have to time 8 to the number of segment.
 * @param: (int)tr: The data for TR register to load.
 */ 
void load_tr(int tr);

/**
 * @brief: Far JMP, change current segment and exec another task.
 * @param: (int)eip: New EIP register.
 * @param: (int)cs: New segment. (CR: Code Segment)
 */ 
void farjmp(int eip, int cs);

/**
 * @brief: Far CALL, call and run another segment of codes.
 * @param: (int)eip: New EIP register.
 * @param: (int)cs: New segment.
 */ 
void farcall(int eip, int cs);

/**
 * @brief: MewOS System API function.
 * @param: (int)registers.
 */ 
void asm_mew_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);

#endif
