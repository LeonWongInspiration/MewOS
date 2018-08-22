; MewOS Boot Image Source Code
; TAB=4

		ORG		0x7c00

; First sector for FAT12

		JMP		entry
		DB		0x90
		DB		"MewOSIPL"
		DW		512
		DB		1
		DW		1
		DB		2
		DW		224
		DW		2880
		DB		0xf0
		DW		9
		DW		18
		DW		2
		DD		0
		DD		2880
		DB		0,0,0x29
		DD		0xffffffff
		DB		"MewMewOS   "
		DB		"FAT12   "
		RESB	18

;  Program starts here

entry:
		MOV		AX,0			; Initialize AX Register
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		MOV		ES,AX

		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; Show a character
		MOV		BX,15			; Character color
		INT		0x10			; Call GPU BIOS to refresh
		JMP		putloop
fin:
		HLT						; Halt
		JMP		fin

msg:
		DB		0x0a, 0x0a		; 0x0a = \n
		DB		"hello, world"
		DB		0x0a
        DB      "MewOS Version 0.0.0.1"
        DB      0x0a
		DB		0

		RESB	0x7dfe-$

		DB		0x55, 0xaa
