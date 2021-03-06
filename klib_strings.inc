;------------------------------------------------------------
; ������� �������������� ������ � ������������ �����
; by ManHunter / PCL
; http://www.manhunter.ru
;
; ���������:
;   lpStr - ��������� �� �������� ������ � ������� ASCIIZ
;   lpResult - ��������� �� ����������-�������� ��������
; �� ������:
;   EAX = 1 - ������ ������� �������������
;   EAX = 0 - ������ �� ����� ���� ������������� � �����
;
; ����������: ����������-�������� ����� ����� ������ DWORD
; ��� QWORD, � ����������� �� ����� ������ ���������� �
; ������� (��. ����������� � ����� ����). �� ���������
; ���������, ��� ���������� ����� ������ DWORD
;------------------------------------------------------------
;flt	 db '-15.245e+6',0
;result  dq ?

string2float: ;lpStr db '...',lpResult dq
	mov	ebp, esp
	; ��������� ����������
	;locals
	;  dot	  dd ?	 ; ��������� �� ������� �����
	;  exp	  dd ?	 ; ��������� �� ����������
	;  digit  dd ?	 ; �����
	;endl
	pusha
	; �������� ������ �� ����������
	mov	[digit],1
	mov	[exp],0
	mov	[dot],0
	mov	esi,[ebp + 04]
	; ����� ��� ���� ����� ���� ������ � ������
	cmp	byte [esi],'-'
	je	@f
	cmp	byte [esi],'+'
	jne	.loc_chk_loop
    @@: inc	esi
	; ����� ����� �� ����� ���� ����
	cmp	byte [esi],0
	je	.loc_chk_error
   .loc_chk_loop:
	; � ������ ������ ���� ����, ���������� � �� ����� ����� �����
	lodsb
	or	al,al
	jz	.loc_chk_complete
	cmp	al,'e'
	je	.loc_chk_exp
	cmp	al,'E'
	je	.loc_chk_exp
	cmp	al,'.'
	je	.loc_chk_dot
	cmp	al,'0'
	jb	.loc_chk_error
	cmp	al,'9'
	ja	.loc_chk_error
	jmp	.loc_chk_loop
   .loc_chk_dot:
	; ����� � ������ ��� ����?
	cmp	[dot],0
	; ������ ����� ������������ ������
	jne	.loc_chk_error
	; ���������� ��� ����?
	cmp	[exp],0
	; ������ ����� ������������ ������
	jne	.loc_chk_error
	; ��������� �� ������� �����
	mov	[dot],esi
	jmp	.loc_chk_loop
   .loc_chk_exp:
	; ���������� ��� ����?
	cmp	[exp],0
	; ������ ����� ������������ ������
	jne	.loc_chk_error
	; ��������� �� ������ ����������
	mov	[exp],esi
	; ����� ����� ���������� �� ����� ���� ����
	cmp	byte [esi],0
	je	.loc_chk_error
	; ����� ���������� ����� ���� ����
	cmp	byte [esi],'-'
	je	@f
	cmp	byte [esi],'+'
	jne	.loc_chk_loop
   @@:	inc	esi
	; ����� ����� ������ �� ����� ���� ����
	cmp	byte [esi],0
	je	.loc_chk_error
	; ��������� ��������� ������
	jmp	.loc_chk_loop
   .loc_chk_error:
	; ������ �� �������� ������
	mov	[digit],0
	jmp	.loc_ret
   .loc_chk_complete:
	; ������������� ������������
	finit
	; ��������� �������� �����
	fldz
	; ��������� � ��������
	mov	[digit],10
	fild	dword [digit]
	; ������ �������� �� �������
	mov	esi,[ebp + 04]
	; � ������ ������ �����?
	cmp	byte [esi],'-'
	je	@f
	cmp	byte [esi],'+'
	jne	.loc_before_dot
    @@: inc	esi
	; �������������� ����� �� �������
   .loc_before_dot:
	lodsb
	; ����� ������?
	or	al,al
	jz	.loc_complete
	cmp	al,'.'
	je	.loc_complete_before_dot
	cmp	al,'e'
	je	.loc_exp
	cmp	al,'E'
	je	.loc_exp
	; ��������� �����
	sub	al,'0'
	movzx	eax,al
	mov	[digit],eax
	; ��������
	fild	dword [digit]
	fxch	st2
	fmul	st0,st1
	fxch	st2
	fadd	st2,st0
	ffree	st0	; ��������� ����
	fincstp
	jmp	.loc_before_dot
	; �������������� ������� ����� �����
   .loc_complete_before_dot:
	; ������� ����� ����?
	cmp	[dot],0
	je	.loc_complete_after_dot
	; ���������� ����?
	cmp	[exp],0
	je	@f
	; ��������� �� ������ ����������
	mov	esi,[exp]
	jmp	.loc_start_after_dot
    @@: ; ����� ��������� ��������� �� ����� ������
	xor	ecx,ecx
	dec	ecx
	xor	eax,eax
	mov	edi,esi
	repne	scasb
	mov	esi,edi
   .loc_start_after_dot:
	std
	dec	esi
	dec	esi
	; ������� �����
	fldz
	fxch	st1
   .loc_after_dot:
	lodsb
	; ����� ������� �����?
	cmp	al,'.'
	je	.loc_complete_after_dot
	; ��������� �����
	sub	al,'0'
	movzx	eax,al
	mov	[digit],eax
	; ��������
	fild	dword [digit]
	fadd	st2,st0
	fxch	st2
	fdiv	st0,st1
	fxch	st2
	ffree	st0	; ��������� ����
	fincstp
	jmp	.loc_after_dot
   .loc_complete_after_dot:
	; �������� ���� �����������
	cld
	ffree	st0	; ��������� ����
	fincstp
	; ������� ������� � ����� �����
	fadd	st1,st0
   .loc_exp:
	; ���������� ����?
	cmp	[exp],0
	je	.loc_complete
	; �������� �������� ����������
	xor	ecx,ecx
	mov	esi,[exp]
	; � ������ ������ �����?
	cmp	byte [esi],'-'
	je	@f
	cmp	byte [esi],'+'
	jne	.loc_start_exp
    @@: inc	esi
   .loc_start_exp:
	lodsb
	or	al,al
	jz	.loc_end_exp
	sub	al,'0'
	movzx	eax,al
	imul	ecx,10
	add	ecx,eax
	jmp	.loc_start_exp
   .loc_end_exp:
	or	ecx,ecx
	jz	.loc_complete
	ffree	st0	; ��������� ����
	fincstp
	mov	[digit],10
	fild	dword [digit]
	; ������ ��� ��������?
	mov	esi,[exp]
	cmp	byte [esi],'-'
	je	.loc_exp_divide
   .loc_exp_multiple:
	fmul	st1,st0
	loop	.loc_exp_multiple
	jmp	.loc_complete
   .loc_exp_divide:
	fdiv	st1,st0
	loop	.loc_exp_divide
   .loc_complete:
	ffree	st0	; ��������� ����
	fincstp
	; � ������ ������ �����?
	mov	esi,[ebp + 04]
	cmp	byte [esi],'-'
	jne	@f
	; �������� ���� �����
	fchs
    @@: ; �������� �������� � ������ ������
	mov	eax,[ebp + 08]
	; ���� ��������� ���������� ��������, �� ��������
	; ������ ����� ������ QWORD, � ��������� �������
	; ���� �������� �� fstp qword [eax]
	fstp	dword [eax]
	; �������� ��������������
	mov	[digit],1
   .loc_ret:
	popa
	; ��������� ��������������
	mov	eax,[digit]
ret 4*2


; strings lib
; for asciiz strings
; ds:esi - source string
; es:edi - destination string

; string length
;  IN:	esi - string
;  OUT: ecx and eax - string length
strlen:
	push	edi esi ecx

	mov	edi, esi

	xor	eax, eax
	mov	ecx, eax
	not	ecx
	cld
	repnz	scasb
	not	ecx	; neg ecx :)
	dec	ecx	;
	mov	eax, ecx

	pop	ecx esi edi
	ret

; copy one string to another
;  IN:	ds:esi - source
;	es:edi - destination
;  OUT: -
strcopy:
	push	ecx
	push	esi
	push	edi

	call	strlen
	inc	ecx	; copy 0
	cld
	rep	movsb

	pop	edi
	pop	esi
	pop	ecx
	ret


; to uppercase - for ascii table only!
;  IN:	ds:esi - source string
;	es:edi - destination (may be = source)
;  OUT: -
strtoupper:
	push	eax
	push	ecx
	push	edi
	push	esi

	xor	eax, eax
	mov	ecx, eax
	not	ecx
	cld
   .l1: lodsb
	or	al, al
	jz	.exit
	cmp	al, 'a'
	jb	.l2
	cmp	al, 'z'
	ja	.l2
	sub	al, ('a'-'A')
   .l2: stosb
	loop	.l1
   .exit:
	pop	esi
	pop	edi
	pop	ecx
	pop	eax
	ret

; to lowercase - for ascii table only!
;  IN:	ds:esi - source string
;	es:edi - destination (may be = source)
;  OUT: -
strtolower:
	push	eax
	push	ecx
	push	edi
	push	esi

	xor	eax, eax
	mov	ecx, eax
	not	ecx
	cld
   .l1: lodsb
	or	al, al
	jz	.exit
	cmp	al, 'A'
	jb	.l2
	cmp	al, 'Z'
	ja	.l2
	add	al, ('a'-'A')
   .l2: stosb
	loop	.l1
   .exit:
	pop	esi
	pop	edi
	pop	ecx
	pop	eax
	ret

; compares two strings (case sensitive)
;  IN:	ds:esi - string1
;	es:edi - string2
;  OUT: flags for j-condition (ja,je,...)
_strcmp:
	push	eax
	push	edi
	push	esi

	cld
   .l1: lodsb
	scasb
	jne	.exit
	or	al, al
	jnz	.l1
   .exit:
	pop	esi
	pop	edi
	pop	eax
	ret

; searches one string for another
;  IN:	ds:esi - string to look in
;	es:edi - string to search for
;  OUT: edx - position (-1 if not found)
strpos:
	push	esi
	push	edi
	push	eax
	push	ebx
	push	ecx

	call	strlen
	mov	eax, ecx	; eax = str1.Length;
	xchg	esi, edi
	call	strlen
	cmp	eax, ecx
	jb	.notfound	; exit if str2.Length > str1.Length;

	mov	ebx, ecx	; ebx = str2.Length
	sub	eax, ebx	; last possible index
	xchg	esi, edi
	xor	edx, edx

   .l1: mov	cl, [ebx+esi]		; save byte
	mov	byte[ebx+esi], 0	; end of string
	call   _strcmp			; compare
	mov	[ebx+esi], cl		; restore byte
	je	.exit
	inc	esi
	inc	edx
	cmp	eax, edx	; end of str1
	jne	.l1

   .notfound:
	xor	edx, edx
	dec	edx		; -1 - not found

   .exit:
	pop	ecx
	pop	ebx
	pop	eax
	pop	edi
	pop	esi
	ret



;-----------------------------------------------------
; ������� ������ ��������� � ������
;-----------------------------------------------------
; lpSrc - ��������� �� �������� ������
; lpDst - ��������� �� ����� ��� ���������� ������
; lpPattern - ��������� �� ���������� ���������
; lpReplace - ��������� �� ������ ��� ������
; dNum - ���������� ����� (0 - �������� ���)
;-----------------------------------------------------
;	       4	    8		     12 	      16	   20
str_replace: ; lpSrc:DWORD, lpPattern:DWORD, lpReplace:DWORD, lpDst:DWORD, dNum:DWORD
	mov	ebp, esp
	pusha
	mov	edx, dword[ebp + 16]			 ; ��������� �� �����-��������
	xor	ebx, ebx			 ; ������� �����
	; �������� ������ �� ������?
	mov	ecx, dword[ebp + 04]
	cmp	byte [ecx],0
	jz     .loc_ret
	; ���������� ������ �� ������?
	mov	eax, dword[ebp + 08]
	cmp	byte [eax],0
	jz     .loc_copy_all
   .loc_scan:
	mov	esi, ecx
	mov	edi, dword[ebp + 08]
	; �������� ������ �����������?
	cmp	byte [esi],0
	je     .loc_end_replace
    @@: ; ������ ������� � ���������?
	cmp	byte [edi],0
	je     .loc_move_replace
	; ������ ��������� �
	lodsb
	; �������� ��� ���������?
	cmp	dword[ebp + 20],0
	je     .loc_skip_counter
	; ��� �������� ������ ����������?
	cmp	ebx, dword[ebp + 20]
	je     .loc_move_one_char
   .loc_skip_counter:
	cmp	al, byte [edi]
	jne    .loc_move_one_char
	inc	edi
	jmp	@b
   .loc_move_replace:
	; ��������� ������� �����
	inc	ebx
	mov	ecx,esi
	; �������� ���������� ������
	mov	esi, dword[ebp + 12]
	mov	edi,edx
    @@: lodsb
	or	al,al
	jz     .loc_scan
	stosb
	inc	edx
	jmp	@b
   .loc_move_one_char:
	; ����������� ���� ������
	mov	al,byte [ecx]
	mov	byte [edx],al
	inc	edx
	inc	ecx
	jmp    .loc_scan
   .loc_end_replace:
	; �������� ��������� 0 � ������
	mov	byte [edx],0
	jmp    .loc_ret
   .loc_copy_all:
	; ������ ����������� �������� ������
	mov	esi, dword[ebp + 04]
	mov	edi, dword[ebp + 16]
    @@: lodsb
	stosb
	or	al,al
	jnz	@b
   .loc_ret:
	popa
ret 4*5












ALIGN 4
HEX2STR_32BIT:	  ; EAX->, ^EDI<-
		mov	ecx,7
 .1:		mov	edx,eax
		shr	eax,4
		and	edx,0xF
		cmp	edx,0xA
		jae    .2
		add	edx,'0'
		jmp    .3
 .2:		add	edx,'A' - 0xA
 .3:		mov	byte[edi + ecx],dl
		dec	ecx
		jns    .1
		mov	byte[edi + 8],0
ret


ALIGN 4
HEX2STR_128BIT:    ; ^ESI->, ^EDI<-
		PUSHAD
		XOR	ECX, ECX
		;
	    @@: INC	ECX
		CMP	ECX, 16
		JA     .END
		LODSB
		PUSH	ECX
		MOV	DL,  AL
		MOV	ECX, 0x02
	   .L1: ROL	DL,  0x04
		MOV	EAX, 0x300F
		AND	AL,  DL
		AAA
		AAD	0x11
		STOSB
		LOOP   .L1
		POP	ECX
		JMP	@B
	       .END:
		XOR	EAX, EAX
		STOSB
		POPAD
RET