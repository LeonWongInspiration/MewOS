; Asm functions
; TAB=4

[FORMAT "WCOFF"]				; Mode of file
[INSTRSET "i486p"]
[BITS 32]						; 32-bit system instructions

; File info

[FILE "asm_funcs.nas"]			; Source file

		GLOBAL	_io_hlt			; Function declaration
		GLOBAL	_write_mem8

; Function realization

[SECTION .text]		; Write this before the program

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_write_mem8:
		MOV		ECX,[ESP+4]		; Address
		MOV		AL,[ESP+8]		; Data
		MOV		[ECX],AL		; Save
		RET
