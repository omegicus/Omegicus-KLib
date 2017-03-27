;-----------------------------------------------------------------------
; Функция расчета хэша SHA1
; Copyright (C) ManHunter / PCL
; http://www.manhunter.ru
;-----------------------------------------------------------------------
; Параметры:
;      ESI lpData - указатель на блок данных
;      ECX dSize  - размер блока
; На выходе: заполненные переменные SHA1_h0 - SHA1_h0
;-----------------------------------------------------------------------
HASH.SHA1:; lpData:DWORD, dSize:DWORD	  ; -> [SHA1_h0],[SHA1_h1],[SHA1_h2],[SHA1_h3],[SHA1_h4]
	pushad
	mov	dword[sha1_dSize],  ecx
	mov	dword[sha1_lpData], esi
	; Инициализация
	mov	[SHA1_h0],0x67452301
	mov	[SHA1_h1],0xEFCDAB89
	mov	[SHA1_h2],0x98BADCFE
	mov	[SHA1_h3],0x10325476
	mov	[SHA1_h4],0xC3D2E1F0
	; Получить количество 64-байтных блоков
	mov	eax,[sha1_dSize]
	push	eax
	shr	eax,6
	mov	[sha1_len],eax
	pop	eax
	; Получить остаток от деления на 64
	and	eax,3Fh
	; Расчет количества добавляемых байт
	mov	[sha1_padding],eax
	; Подсчет SHA1 основных данных
	mov	[sha1_index],0
   .main_loop:
	mov	eax,[sha1_index]
	cmp	eax,[sha1_len]
	je     .main_done
	; Указатель * 64
	shl	eax,6
	mov	esi,[sha1_lpData]
	add	esi,eax
	mov	edi,SHA1_Buff
	mov	ecx,16
	rep	movsd
	; Привести строку к Big-endian виду
	call	SHA1_BE
	call	SHA1_Calc
	inc	[sha1_index]
	jmp	.main_loop
   .main_done:
	; Подсчет SHA1 остаточных данных
	xor	eax,eax
	mov	edi,SHA1_Buff
	mov	ecx,16
	rep	stosd
	; Скопировать остаточные данные в буфер
	mov	eax,[sha1_index]
	shl	eax,6
	mov	esi,[sha1_lpData]
	add	esi,eax
	mov	edi,SHA1_Buff
	mov	ecx,[sha1_padding]
	rep	movsb
	; Дописать финальный бит
	mov	al,80h
	stosb
	; Привести строку к Big-endian виду
	call	SHA1_BE
	; Проверка на пограничные случаи, когда остаток
	; строки меньше, чем требуется для записи финального
	; бита и длины строки
	cmp	[sha1_padding],56
	jae	@f
	; В конец буфера записать длину строки
	mov	eax,[sha1_dSize]
	shl	eax,3
	mov	dword [SHA1_Buff+60],eax
	call	SHA1_Calc
	jmp    .sha1_done
    @@: call	SHA1_Calc
	; Очистка буфера
	mov	edi,SHA1_Buff
	xor	eax,eax
	mov	ecx,15
	rep	stosd
	; В конец буфера записать длину строки
	mov	eax,[sha1_dSize]
	shl	eax,3
	stosd
	call	SHA1_Calc
   .sha1_done:
	popad
	ret


;-----------------------------------------------------------------------
; Вспомогательная функция
; Приведение строки к Big-endian виду
;-----------------------------------------------------------------------
SHA1_BE:
	pusha
	mov	ecx,16
	mov	esi,SHA1_Buff
	mov	edi,esi
    @@: lodsd
	bswap	eax
	stosd
	loop	@b
	popa
	ret


;-----------------------------------------------------------------------
; Вспомогательная функция для расчета SHA1
;-----------------------------------------------------------------------
SHA1_Calc:
	pusha

	mov	eax,[SHA1_h0]
	mov	[SHA1_a],eax
	mov	eax,[SHA1_h1]
	mov	[SHA1_b],eax
	mov	eax,[SHA1_h2]
	mov	[SHA1_c],eax
	mov	eax,[SHA1_h3]
	mov	[SHA1_d],eax
	mov	eax,[SHA1_h4]
	mov	[SHA1_e],eax

	xor	ecx,ecx ; i
.cycle_loop:
	mov	eax,ecx
	shl	eax,2

	cmp	ecx,16
	jae	@f

	mov	ebx,dword [SHA1_Buff+eax]
	mov	dword [SHA1_W+eax],ebx
	jmp	.@1
@@:
	; rol(w[j-3] ^ w[j-8] ^ w[j-14] ^ w[j-16], 1)
	mov	eax,ecx
	sub	eax,3
	shl	eax,2
	mov	ebx,dword [SHA1_W+eax]

	mov	eax,ecx
	sub	eax,8
	shl	eax,2
	xor	ebx,dword [SHA1_W+eax]

	mov	eax,ecx
	sub	eax,14
	shl	eax,2
	xor	ebx,dword [SHA1_W+eax]

	mov	eax,ecx
	sub	eax,16
	shl	eax,2
	xor	ebx,dword [SHA1_W+eax]

	rol	ebx,1

	mov	eax,ecx
	shl	eax,2
	mov	dword [SHA1_W+eax],ebx
.@1:
	mov	edx,[SHA1_a]
	rol	edx,5

	; Расчет t и k
	cmp	ecx,20
	jae	@f
	; (b & c) | ((~b) & d)
	mov	eax,[SHA1_b]
	and	eax,[SHA1_c]
	mov	ebx,[SHA1_b]
	not	ebx
	and	ebx,[SHA1_d]
	or	eax,ebx
	add	edx,0x5A827999
	jmp	.@2
@@:
	cmp	ecx,40
	jae	@f
	; b ^ c ^ d
	mov	eax,[SHA1_b]
	xor	eax,[SHA1_c]
	xor	eax,[SHA1_d]
	add	edx,0x6ED9EBA1
	jmp	.@2
@@:
	cmp	ecx,60
	jae	@f
	; (b & c) | (b & d) | (c & d)
	mov	eax,[SHA1_b]
	and	eax,[SHA1_c]
	mov	ebx,[SHA1_b]
	and	ebx,[SHA1_d]
	or	eax,ebx
	mov	ebx,[SHA1_c]
	and	ebx,[SHA1_d]
	or	eax,ebx
	add	edx,0x8F1BBCDC
	jmp	.@2
@@:
	mov	eax,[SHA1_b]
	xor	eax,[SHA1_c]
	xor	eax,[SHA1_d]
	add	edx,0xCA62C1D6
.@2:
	add	edx,eax
	add	edx,[SHA1_e]

	mov	eax,ecx
	shl	eax,2
	add	edx,[SHA1_W+eax]

	mov	eax,[SHA1_d]	     ; e = d
	mov	[SHA1_e],eax

	mov	eax,[SHA1_c]	     ; d = c
	mov	[SHA1_d],eax

	mov	eax,[SHA1_b]	     ; c = rol(b,30)
	rol	eax,30
	mov	[SHA1_c],eax

	mov	eax,[SHA1_a]	     ; b = a
	mov	[SHA1_b],eax

	mov	[SHA1_a],edx	     ; a = t

	inc	ecx
	cmp	ecx,80
	jne	.cycle_loop

	mov	eax,[SHA1_a]
	add	[SHA1_h0],eax
	mov	eax,[SHA1_b]
	add	[SHA1_h1],eax
	mov	eax,[SHA1_c]
	add	[SHA1_h2],eax
	mov	eax,[SHA1_d]
	add	[SHA1_h3],eax
	mov	eax,[SHA1_e]
	add	[SHA1_h4],eax

	popa
	ret
