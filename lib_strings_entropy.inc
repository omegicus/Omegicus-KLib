;-----------------------------------------------------------------------
; ������� ������� �������� ����� ������
; ���������:
; ESI szStr - ��������� �� ���� ������ ��� �������
; ECX dLen  - ������ ����� � ������
; EDI lpRes - ��������� �� �������� ���������� (DWORD)
;-----------------------------------------------------------------------
align 4
string_entropy:
	pushad
	mov	dword[entropy_dLen],  ecx
	mov	dword[entropy_szStr], esi
	;mov	 dword[entropy_lpRes], entropy;edi
	; ������������� ������������
	finit
	; �������� �������� ��������
	lea	edi, [entropy_cnt]
	mov	ecx, 256
	xor	eax, eax
	rep	stosd



	; ������� ���������� ��������
	mov	esi, [entropy_szStr]
	mov	ecx, [entropy_dLen]
	align	4
    .loc_count_chars:
	xor	eax, eax
	lodsb
	shl	eax,2
	inc	dword[entropy_cnt+eax]
	loop   .loc_count_chars


	; ��������� �������� ��������
	fldz
	; ������ �������� ��� ������� �������
	mov	ecx,256
    .loc_calc_entr:
	dec	ecx
	; �������� ���������� ��������
	mov	eax,ecx
	shl	eax,2
	mov	eax,[entropy_cnt+eax]
	; ������� ���������� ����������
	or	eax,eax
	jz	@f
	mov	[entropy_count],eax
	fild	[entropy_dLen]			; ����� ������
	fild	[entropy_count] 	; ���������� ��������
	fdiv	st0,st1 		; P(i) = SUM(i)/total
	fst	st1			; ����������� st0 � st1
	fchs				; �������� ����
	fxch	st1			; �������� ������� ��������
	fyl2x				; H(i) = -P(i)*log2(P(i))
	fadd	st1,st0 		; H = H+H(i)
	ffree	st0
	fincstp 			; ��������� ����
    @@: or	ecx,ecx
	jnz    .loc_calc_entr
	mov	eax, entropy;[entropy_lpRes]
	fstp	dword [eax]
	ffree	st0
	fincstp 	; ��������� ����
	popad

	;
	; ��������� �� ������� ����� ����� �������
	push	ecx
	fld	dword [entropy]
	fild	dword [entropy_coeff]
	fmul	st0,  st1
	fistp	dword [entropy]
	ffree	st0
	mov	eax,  [entropy]
	xor	edx,  edx
	mov	ecx,  [entropy_coeff]
	div	ecx
	pop	ecx
ret  ;=> Entropy ==  eax.edx
