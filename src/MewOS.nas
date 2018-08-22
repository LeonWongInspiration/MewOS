; MewOS main system file
; TAB=4

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

fin:
		HLT
		JMP		fin