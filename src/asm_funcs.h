/** ASM Functions Declaration
 * 
 * @author: Leon Wong
 * @build: 201808250330
 * @brief: This file gives declarations of the functions from asm_funcs.nas.
 * @uasge: Just include this file, there's no need to compile it.
 * 
 */

#if (!defined(ASM_FUNCS_H))
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

#endif
