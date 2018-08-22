; MewOS main system file
; TAB=4

		ORG		0xc200			; The address of this executable

		MOV		AL,0x13			; Use VGA graphic with 320*200*8bit
		MOV		AH,0x00
		INT		0x10

fin:
		HLT
		JMP		fin