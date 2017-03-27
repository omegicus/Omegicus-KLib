;-----------------------------------------------------------------------
; Функция расчета энтропии блока памяти
; Параметры:
; ESI szStr - указатель на блок памяти для расчета
; ECX dLen  - размер блока в байтах
; EDI lpRes - указатель на приемник результата (DWORD)
;-----------------------------------------------------------------------
align 4
string_entropy:
	pushad
	mov	dword[entropy_dLen],  ecx
	mov	dword[entropy_szStr], esi
	;mov	 dword[entropy_lpRes], entropy;edi
	; Инициализация сопроцессора
	finit
	; Обнулить счетчики символов
	lea	edi, [entropy_cnt]
	mov	ecx, 256
	xor	eax, eax
	rep	stosd



	; Подсчет количества символов
	mov	esi, [entropy_szStr]
	mov	ecx, [entropy_dLen]
	align	4
    .loc_count_chars:
	xor	eax, eax
	lodsb
	shl	eax,2
	inc	dword[entropy_cnt+eax]
	loop   .loc_count_chars


	; Начальное значение энтропии
	fldz
	; Расчет энтропии для каждого символа
	mov	ecx,256
    .loc_calc_entr:
	dec	ecx
	; Получить количества символов
	mov	eax,ecx
	shl	eax,2
	mov	eax,[entropy_cnt+eax]
	; Нулевые количества пропускаем
	or	eax,eax
	jz	@f
	mov	[entropy_count],eax
	fild	[entropy_dLen]			; Длина строки
	fild	[entropy_count] 	; Количество символов
	fdiv	st0,st1 		; P(i) = SUM(i)/total
	fst	st1			; Скопировать st0 в st1
	fchs				; Изменить знак
	fxch	st1			; Поменять местами регистры
	fyl2x				; H(i) = -P(i)*log2(P(i))
	fadd	st1,st0 		; H = H+H(i)
	ffree	st0
	fincstp 			; Почистить стек
    @@: or	ecx,ecx
	jnz    .loc_calc_entr
	mov	eax, entropy;[entropy_lpRes]
	fstp	dword [eax]
	ffree	st0
	fincstp 	; Почистить стек
	popad

	;
	; Округлить до второго знака после запятой
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
