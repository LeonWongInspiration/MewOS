; Asm functions
; TAB=4

[FORMAT "WCOFF"]				; Mode of file
[INSTRSET "i486p"]
[BITS 32]						; 32-bit system instructions

; File info

[FILE "asm_funcs.nas"]			; Source file

		GLOBAL	_io_hlt			; Function declaration
		GLOBAL	_io_cli
		GLOBAL	_io_sti
		GLOBAL	_io_stihlt
		GLOBAL	_io_in8
		GLOBAL  _io_in16
		GLOBAL  _io_in32
		GLOBAL	_io_out8
		GLOBAL 	_io_out16
		GLOBAL 	_io_out32
		GLOBAL	_io_load_eflags
		GLOBAL	_io_store_eflags
		GLOBAL	_load_gdtr
		GLOBAL	_load_idtr
		GLOBAL	_asm_inthandler0c
		GLOBAL	_asm_inthandler0d
		GLOBAL	_asm_inthandler20
		GLOBAL	_asm_inthandler21
		GLOBAL	_asm_inthandler27
		GLOBAL	_asm_inthandler2c
		GLOBAL	_load_cr0
		GLOBAL  _store_cr0
		GLOBAL	_memtest_sub
		GLOBAL	_load_tr
		GLOBAL	_farjmp
		GLOBAL	_farcall
		GLOBAL	_asm_mew_api
		GLOBAL	_start_app
		GLOBAL	_asm_kill_app

		EXTERN	_inthandler0c 	; Functions from C
		EXTERN	_inthandler0d
		EXTERN	_inthandler20
		EXTERN	_inthandler21
		EXTERN	_inthandler27
		EXTERN	_inthandler2c
		EXTERN	_mew_api

; Function realization

[SECTION .text]		; Write this before the program

_io_hlt:	; void io_hlt();
		HLT
		RET

_io_cli:	; void io_cli();
		CLI
		RET

_io_sti:	; void io_sti();
		STI
		RET

_io_stihlt:	; void io_stihlt();
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags();
		PUSHFD		; PUSH EFLAGS
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS
		RET

_load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_asm_inthandler0c:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0c
		CMP		EAX,0
		JNE		_asm_kill_app
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4
		IRETD

_asm_inthandler0d:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0d
		CMP		EAX,0
		JNE		_asm_kill_app
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4
		IRETD
	
_asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler20
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler27:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_load_cr0:		; int load_cr0();
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; Because we have to use EBX, ESI, EDI
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_load_tr:		; void load_tr(int tr);
		LTR		[ESP+4]			; tr
		RET

_farjmp:		; void farjmp(int eip, int cs);
		JMP		FAR	[ESP+4]				; eip, cs
		RET

_farcall:		; void farcall(int eip, int cs);
		CALL	FAR	[ESP+4]				; eip, cs
		RET

_asm_mew_api:	; MewOS API call, allow third-party binaries to call functions provided by MewOS
		STI
		PUSH	DS
		PUSH	ES
		PUSHAD	; Save the data of registers.
		PUSHAD	; Prepare params to call _mew_api
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_mew_api
		CMP		EAX,0					; Judge if we need to kill the app.
		JNE		_asm_kill_app
		ADD		ESP,32
		POPAD
		POP		ES
		POP		DS
		IRETD
_asm_kill_app:
		MOV		ESP,[EAX]
		MOV		DWORD [EAX+4],0
		POPAD
		RET

_start_app:		; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD
		MOV		EAX,[ESP+36]
		MOV		ECX,[ESP+40]
		MOV		EDX,[ESP+44]
		MOV		EBX,[ESP+48]
		MOV		EBP,[ESP+52]
		MOV		[EBP],ESP
		MOV		[EBP+4],SS
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
		OR		ECX,3
		OR		EBX,3
		PUSH	EBX
		PUSH	EDX
		PUSH	ECX
		PUSH	EAX
		RETF