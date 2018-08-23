; MewOS main system file
; TAB=4

CENTRY	EQU		0x00280000		; C system file entry
DSKCAC	EQU		0x00100000		; Address of disk cache
DSKCAC0 EQU		0x00008000		; Address of disk cache under real mode

; Boot info
CYLS	EQU		0x0ff0			; The boot sector
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; Visual UI mode
SCRNX	EQU		0x0ff4			; X value of resolution
SCRNY	EQU		0x0ff6			; Y value of resolution
VRAM	EQU		0x0ff8			; The address of VRAM

		ORG		0xc200			; The address of this executable

		MOV		AL,0x13			; Use VGA graphic with 320*200*8bit
		MOV		AH,0x00
		INT		0x10

		MOV		BYTE [VMODE],8	; Save the info to memory
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; Read the led of the keyboard from BIOS
		MOV		AH,0x02
		INT		0x16			; Keyboard BIOS
		MOV		[LEDS],AL

; Disallow PIC to receive any interrupts
; for AT-compatible PCs, if PIC inits before CLI
; the system may go down

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; OUT after OUT may not work
		OUT		0xa1,AL

		CLI						; Disallow interrupts for CPU

; Set A20 gate for CPU to reach memory more than 1MB
		
		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; Enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; Switch to protected mode

[INSTRSET "i486p"]				; Disclaim support for 486 instructions

		LGDT	[GDTR0]			; Set temp GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; Set the 31st bit to 0 to disallow pages
		OR		EAX,0x00000001	; Set the 0th bit to 1 to allow mode switch
		MOV		CR0,EAX
		JMP		pipelineflush

pipelineflush:
		MOV		AX,1*8			; Seg set to 32bit R/W
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; Load C System Entry

		MOV		ESI,clangentry		; Source
		MOV		EDI,CENTRY
		MOV		ECX,512*1024/4
		CALL	memcpy

; Also move disk data to the original address

; Boot sector
		MOV		ESI,0x7c00
		MOV		EDI,DSKCAC
		MOV		ECX,512/4
		CALL	memcpy

; Remain
		MOV		ESI,DSKCAC0+512
		MOV		EDI,DSKCAC+512
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; Change cylinder index to byte index/4
		SUB		ECX,512/4		; Subtract the data of IPL
		CALL	memcpy

; Now the MewOS asm head finishes its work
; give permit to c_entry

; Start C Language Entry
		MOV		EBX,CENTRY
		MOV		ECX,[EBX+16]
		ADD		ECX,3
		SHR		ECX,2
		JZ		skip			; if nothing to pass
		MOV		ESI,[EBX+20]
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]
		CALL	memcpy

skip:
		MOV		ESP,[EBX+12]	; Init val of stack
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		AL,0x64
		AND		AL,0x02
		JNZ		waitkbdout
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy
		RET

		ALIGNB	16
GDT0:
		RESB	8				; NULL selector
		DW		0xffff,0x0000,0x9200,0x00cf	; segment = 32bit R/W
		DW		0xffff,0x0000,0x9a28,0x0047	; segment = 32bit R/W for c lang entry

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16

clangentry:
