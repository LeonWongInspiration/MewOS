; Asm functions
; TAB=4

[FORMAT "WCOFF"]				; Mode of file
[BITS 32]						; 32-bit system instructions


; File info

[FILE "asm_funcs.nas"]			; Source file

		GLOBAL	_io_hlt			; Function declaration


; Function realization

[SECTION .text]		; Write this before the program

_io_hlt:	; void io_hlt(void);
		HLT
		RET
