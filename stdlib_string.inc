align 4
str_replace: 
; IN: eax: whatZ, ebx: replacementZ, edx: stringZ, edi=0 for self malloc or buffer
; OUT: eax = 0 | destignation string
	mov	esi, edx	; ESI: String
	mov	edx, ebx	; EDX: REaplcement
	mov	ebx, eax	; EBX: PatternZ
    cmp     edi, 0
    jne    .buffered
	;
	mov	eax, esi
	call	strlen
	mov	ecx, eax
	mov	edi, ecx
	;
	mov	eax, ebx
	call	strlen
	sub	ecx, eax
	;
	mov	eax, edx
	call	strlen
	add	ecx, eax
	cmp	ecx, edi     ; is new string bigger then source?
	ja     .getmem	     ; yes...
	mov	edi, esi     ; no...   set dest == source
	jmp	   .algo
	;
.getmem:
	mov	eax, ecx
	call	os_malloc
	or		eax, eax
	jz	   .mem_error
	mov	edi, eax
	jmp	   .algo
	;
.buffered: ; buffer in EDI already prepared:
.algo:
    pushad
    ; lpSrc:DWORD, lpPattern:DWORD, lpReplace:DWORD, lpDst:DWORD, dNum:DWORD 
	push	dword 1 	; just 1 replacement
	push	dword edi	; dest
	push	dword edx   ; replacement
	push	dword ebx   ; pattern
	push	dword esi   ; stringZ
	call   _replace
	;
    popad
    mov     eax,  edi
    jmp    .ok

.ok:jmp    .exit
.mem_error: 
    jmp    .error
.error:
    xor     eax, eax
.exit:
ret





align 4
strlen: ; eax: string; out: eax: char count
	push	ecx esi
	mov	esi, eax
	xor	ecx, ecx
	align	4
@@: lodsb
	or	al,  al
	jz	@f
	inc	ecx
	jmp	@b
@@: mov 	eax, ecx
	pop	esi ecx
ret



;----------------------------------------------------- 
; Функция замены подстроки в строке 
;----------------------------------------------------- 
; lpSrc - указатель на исходную строку 
; lpDst - указатель на буфер для полученной строки 
; lpPattern - указатель на заменяемую подстроку 
; lpReplace - указатель на строку для замены 
; dNum - количество замен (0 - заменить все) 
;----------------------------------------------------- 
align 4
_replace: 
; lpSrc:DWORD, lpPattern:DWORD, lpReplace:DWORD, lpDst:DWORD, dNum:DWORD 
	; Указатель на буфер-приемник 
	mov	edx, [esp + 16] 			 ; lpDst
	; Счетчик замен 
	xor	ebx, ebx 
	; Исходная строка не пустая? 
	mov	ecx, [esp + 04] 				 ; lpSrc
	cmp	byte [ecx],0 
	jz	.loc_ret 
	; Заменяемая строка не пустая? 
	mov	eax, [esp + 08] 				 ; lpPattern
	cmp	byte [eax],0 
	jz	.loc_copy_all 
	.loc_scan: 
	mov	esi, ecx 
	mov	edi, [esp + 08] 				 ; lpPattern
	; Исходная строка закончилась? 
	cmp	byte [esi],0 
	je	.loc_end_replace 
	@@: ; Строки совпали с паттерном? 
	cmp	byte [edi],0 
	je	.loc_move_replace 
	; Символ совпадает с 
	lodsb 
	; Заменять все вхождения? 
	cmp	dword[esp + 20],0 
	je	.loc_skip_counter 
	; Уже заменили нужное количество? 
	cmp	ebx, dword[esp + 20] 
	je	.loc_move_one_char 
.loc_skip_counter: 
	cmp	al,byte [edi] 
	jne	.loc_move_one_char 
	inc	edi 
	jmp	@b 
.loc_move_replace: 
	; Увеличить счетчик замен 
	inc	ebx 
	mov	ecx,esi 
	; Записать заменяющую строку 
	mov	esi, [esp + 12] 				 ; lpReplace
	mov	edi,edx 
@@:	lodsb 
	or	al,al 
	jz	.loc_scan 
	stosb 
	inc	edx 
	jmp	@b 
.loc_move_one_char: 
	; Скопировать один символ 
	mov	al, byte [ecx] 
	mov	byte [edx],al 
	inc	edx 
	inc	ecx 
	jmp	.loc_scan 
.loc_end_replace: 
	; Записать финальный 0 в строку 
	mov	byte [edx],0 
  
	jmp	.loc_ret 
.loc_copy_all: 
	; Просто скопировать исходную строку 
	mov	esi, [esp + 04] 				 ; lpSrc
	mov	edi, [esp + 16] 				 ; lpDst
@@:	lodsb 
	stosb 
	or	al,  al 
	jnz	@b 
.loc_ret: 
	ret


align 4
toString: ; IN=eax ; OUT=qword[esi] binary converted to ASCII HEX - Madis
		push ebx
	mov	 edi,0F0F0F0Fh
	mov	 ecx,eax
	mov	 ebx,eax
	shr	 eax,4
	and	 ebx,edi
	and	 eax,edi
	mov	 ecx,eax
	mov	 edx,ebx
	add	 ebx,06060606h
	add	 eax,06060606h
	shr	 ebx,4
	shr	 eax,4
	and	 ebx,edi
	and	 eax,edi
	lea	 edx,[ebx*8+edx+30303030h]
	lea	 ecx,[eax*8+ecx+30303030h]
	sub	 edx,ebx
	sub	 ecx,eax
	xchg	 ch,dl
	rol	 ecx,16
	rol	 edx,16
	xchg	 ch,dl
	rol	 ecx,16
	xchg	 cx,dx
	rol	 edx,16
	mov	 [esi],ecx
	mov	 [esi+4],edx
	pop ebx
	ret