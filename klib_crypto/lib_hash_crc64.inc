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
;   EAX:EDX = CRC64 участка памяти








;-------------------------------------------
; Функция создания таблицы для расчета CRC64
;-------------------------------------------
init_CRC64_ECMA:
	pushad
	; Указатель на выделенную под таблицу память
	mov	edi,crc64table
	; Расчитать значения для всех 256 байт
	xor	ebx,ebx
 .calc_crc64table:
	xor	edx,edx
	mov	eax,ebx

	mov	ecx,8
 .do_polynom:
	shrd	eax,edx,1	; Определить четность числа
	pushf
	shr	edx,1
	popf
	jnc	@f		; XOR выполняется только если QWORD нечетный
	xor	edx,0C96C5795h	; Зеркальный полином = 0xC96C5795D7870F42
	xor	eax,0D7870F42h
   @@:	; Следующий бит
	loop   .do_polynom
	; Записать qword в таблицу
	stosd
	mov	eax,edx
	stosd
	inc	ebx
	cmp	ebx,256
	; Следующий байт
	jb     .calc_crc64table
	popad
	ret

;-----------------------------------------------------------------------
; Функция вычисления CRC64 - Зеркальный табличный алгоритм (ECMA-182)
;-----------------------------------------------------------------------
; Параметры:
;   ESI lData  - указатель на участок памяти для расчета CRC64
;   ECX dLen   - размер участка в байтах
; На выходе:
;   EAX:EDX = CRC64 участка памяти
;-----------------------------------------------------------------------
calc_CRC64_ECMA: ;lData:dword, dLen:dword
	; Сохранить регистры
	push	esi ebx ecx

	; Указатель на данные для подсчета CRC64
	;mov	 esi,[lData]
	; Размер участка данных
	;mov	 ecx,[dLen]
	xor	eax,eax
	; Начальное значение CRC64 = -1
	mov	ebx,0FFFFFFFFh
	mov	edx,ebx
    @@: ; Следующий символ данных
	lodsb
	; Вычислить смещение QWORD в таблице
	xor	al,bl
	; Вычислить значение CRC64
	shrd	ebx,edx,8
	shr	edx,8
	xor	ebx,dword [crc64table+eax*8]
	xor	edx,dword [crc64table+eax*8+4]
	loop	@b
	xchg	eax,ebx
	; Финализация CRC64
	not	eax
	not	edx
	; Восстановить регистры
	pop	ecx ebx esi edi
	ret





;-----------------------------------------------------------------------
; Функция вычисления CRC64 - Прямой табличный алгоритм (PostgreSQL)
;-----------------------------------------------------------------------
; Параметры:
;   ESI lData  - указатель на участок памяти для расчета CRC64
;   ECX dLen   - размер участка в байтах
; На выходе:
;   EAX:EDX = CRC64 участка памяти
;-----------------------------------------------------------------------
calc_CRC64_Postgre:; lData:dword, dLen:dword
	push	edi esi ebx ecx 	      ; Сохранить регистры
       ; mov	 esi,[lData]		       ; Указатель на данные для подсчета CRC64
       ; mov	 ecx,[dLen]		       ; Размер участка данных
	xor	eax,eax
	; Начальное значение CRC64 = -1
	mov	ebx,0FFFFFFFFh
	mov	edx,ebx
   @@:	lodsb				      ; Следующий символ данных
	; Вычислить смещение QWORD в таблице
	mov	edi,edx
	shr	edi,24
	xor	eax,edi
	; Вычислить значение CRC64
	shld	edx,ebx,8
	shl	ebx,8
	xor	ebx,dword [crc64table+eax*8]
	xor	edx,dword [crc64table+eax*8+4]
	loop	@b
	xchg	eax,ebx
	; Финализация CRC64
	not	eax
	not	edx
	; Восстановить регистры
	pop	ecx ebx esi edi
	ret


;-------------------------------------------
; Функция создания таблицы для расчета CRC64
;-------------------------------------------
init_CRC64_Postgre:
	pushad
	; Указатель на выделенную под таблицу память
	mov	edi,crc64table
	; Расчитать значения для всех 256 байт
	xor	ebx,ebx
  .calc_crc64table:
	; EDX:EAX
	xor	eax,eax
	mov	edx,ebx
	shl	edx,24
	mov	ecx,8
  .do_polynom:
	test	edx,edx 	; Определить знак числа
	pushf
	shld	edx,eax,1
	shl	eax,1
	popf
	jns	@f		; XOR выполняется только если QWORD отрицательный
	xor	edx,0x42F0E1EB	; Полином = 0x42F0E1EBA9EA3693
	xor	eax,0xA9EA3693
   @@:	; Следующий бит
	loop   .do_polynom
	; Записать qword в таблицу
	stosd
	mov	eax,edx
	stosd
	inc	ebx
	cmp	ebx,256
	; Следующий байт
	jb     .calc_crc64table
	popad
	ret
