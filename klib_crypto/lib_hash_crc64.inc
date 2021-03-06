;align 4
;crc64table dq 256 dup 0



align 4
HASH.CRC64: ;( ESI data, ECX size, EBX postgre) ; if postgre==1 => PostgreSQL CRC64 else ECMA-182
	cmp	ebx, 1
	je     .postgre
	call	init_CRC64_ECMA
	call	calc_CRC64_ECMA
	jmp    .ends
      .postgre:
	call	init_CRC64_Postgre
	call	calc_CRC64_Postgre
      .ends:
ret
;   EAX:EDX = CRC64 ������� ������








;-------------------------------------------
; ������� �������� ������� ��� ������� CRC64
;-------------------------------------------
init_CRC64_ECMA:
	pushad
	; ��������� �� ���������� ��� ������� ������
	mov	edi,crc64table
	; ��������� �������� ��� ���� 256 ����
	xor	ebx,ebx
 .calc_crc64table:
	xor	edx,edx
	mov	eax,ebx

	mov	ecx,8
 .do_polynom:
	shrd	eax,edx,1	; ���������� �������� �����
	pushf
	shr	edx,1
	popf
	jnc	@f		; XOR ����������� ������ ���� QWORD ��������
	xor	edx,0C96C5795h	; ���������� ������� = 0xC96C5795D7870F42
	xor	eax,0D7870F42h
   @@:	; ��������� ���
	loop   .do_polynom
	; �������� qword � �������
	stosd
	mov	eax,edx
	stosd
	inc	ebx
	cmp	ebx,256
	; ��������� ����
	jb     .calc_crc64table
	popad
	ret

;-----------------------------------------------------------------------
; ������� ���������� CRC64 - ���������� ��������� �������� (ECMA-182)
;-----------------------------------------------------------------------
; ���������:
;   ESI lData  - ��������� �� ������� ������ ��� ������� CRC64
;   ECX dLen   - ������ ������� � ������
; �� ������:
;   EAX:EDX = CRC64 ������� ������
;-----------------------------------------------------------------------
calc_CRC64_ECMA: ;lData:dword, dLen:dword
	; ��������� ��������
	push	esi ebx ecx

	; ��������� �� ������ ��� �������� CRC64
	;mov	 esi,[lData]
	; ������ ������� ������
	;mov	 ecx,[dLen]
	xor	eax,eax
	; ��������� �������� CRC64 = -1
	mov	ebx,0FFFFFFFFh
	mov	edx,ebx
    @@: ; ��������� ������ ������
	lodsb
	; ��������� �������� QWORD � �������
	xor	al,bl
	; ��������� �������� CRC64
	shrd	ebx,edx,8
	shr	edx,8
	xor	ebx,dword [crc64table+eax*8]
	xor	edx,dword [crc64table+eax*8+4]
	loop	@b
	xchg	eax,ebx
	; ����������� CRC64
	not	eax
	not	edx
	; ������������ ��������
	pop	ecx ebx esi edi
	ret





;-----------------------------------------------------------------------
; ������� ���������� CRC64 - ������ ��������� �������� (PostgreSQL)
;-----------------------------------------------------------------------
; ���������:
;   ESI lData  - ��������� �� ������� ������ ��� ������� CRC64
;   ECX dLen   - ������ ������� � ������
; �� ������:
;   EAX:EDX = CRC64 ������� ������
;-----------------------------------------------------------------------
calc_CRC64_Postgre:; lData:dword, dLen:dword
	push	edi esi ebx ecx 	      ; ��������� ��������
       ; mov	 esi,[lData]		       ; ��������� �� ������ ��� �������� CRC64
       ; mov	 ecx,[dLen]		       ; ������ ������� ������
	xor	eax,eax
	; ��������� �������� CRC64 = -1
	mov	ebx,0FFFFFFFFh
	mov	edx,ebx
   @@:	lodsb				      ; ��������� ������ ������
	; ��������� �������� QWORD � �������
	mov	edi,edx
	shr	edi,24
	xor	eax,edi
	; ��������� �������� CRC64
	shld	edx,ebx,8
	shl	ebx,8
	xor	ebx,dword [crc64table+eax*8]
	xor	edx,dword [crc64table+eax*8+4]
	loop	@b
	xchg	eax,ebx
	; ����������� CRC64
	not	eax
	not	edx
	; ������������ ��������
	pop	ecx ebx esi edi
	ret


;-------------------------------------------
; ������� �������� ������� ��� ������� CRC64
;-------------------------------------------
init_CRC64_Postgre:
	pushad
	; ��������� �� ���������� ��� ������� ������
	mov	edi,crc64table
	; ��������� �������� ��� ���� 256 ����
	xor	ebx,ebx
  .calc_crc64table:
	; EDX:EAX
	xor	eax,eax
	mov	edx,ebx
	shl	edx,24
	mov	ecx,8
  .do_polynom:
	test	edx,edx 	; ���������� ���� �����
	pushf
	shld	edx,eax,1
	shl	eax,1
	popf
	jns	@f		; XOR ����������� ������ ���� QWORD �������������
	xor	edx,0x42F0E1EB	; ������� = 0x42F0E1EBA9EA3693
	xor	eax,0xA9EA3693
   @@:	; ��������� ���
	loop   .do_polynom
	; �������� qword � �������
	stosd
	mov	eax,edx
	stosd
	inc	ebx
	cmp	ebx,256
	; ��������� ����
	jb     .calc_crc64table
	popad
	ret
