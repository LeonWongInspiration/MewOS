; MewOS main system file
; TAB=4

[INSTRSET "i486p"]

VBEMODE	EQU		0x101			; VESA BIOS Ext (VBE) video mode setting
;	0x100: 640*400*8bit
;	0x101: 640*480*8bit
;	0x103: 800*600*8bit
;	0x105: 1024*768*8bit
;	0x107: 1280*1024*8bit 

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

; Check if the machine supports VBE
		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00
		INT		0x10			; First set registers like those above, and then use INT 0x10
		CMP		AX,0x004f		; if the machine supports VBE, we'll get AX == 0x004f
		JNE		scrn320			; Unfortunately the machine doesnt support VBE

; Check the version of VBE
		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		scrn320			; We need VBE version >= 2.0 to support high resolution.

; Try to set the VBE mode once, and check if this mode works.
		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AX,0x004f		; After setting the VBE mode, if AX != 0x004f, this VBE mode is not supprted.
		JNE		scrn320

; Check if the VBE mode works
		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080		; The 7th bit of VBE mode can indicate if the VBE mode can actually work.
		JZ		scrn320

; Actually set the VBE mode
		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8	; Take down the VBE mode (for BootInfo)
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

scrn320:						; Unfortunately we have to use low resolution :(
		MOV		AL,0x13			; Use VGA graphic with 320*200*8bit
		MOV		AH,0x00
		INT		0x10

		MOV		BYTE [VMODE],8	; Save the info to memory
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; Read the led of the keyboard from BIOS
keystatus:
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
