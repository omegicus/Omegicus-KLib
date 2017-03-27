ALIGN 4
HEX2STR_64BIT:	  ; ^ESI->, ^EDI<-
		PUSHAD
		XOR	ECX, ECX
		;
	    @@: INC	ECX
		CMP	ECX, 8
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
ALIGN 4
HEX2STR_64BIT_REVERSE:	  ; ^ESI->, ^EDI<-
		PUSHAD
		MOV	ECX, 8
		DEC	ECX
		;
	    @@: MOV	AL,  BYTE[ESI + ECX]
		;
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
		;
		OR	ECX, ECX
		JZ     .END
		DEC	ECX
		JMP	@B
	       .END:
		XOR	EAX, EAX
		STOSB
		POPAD
RET






; Unicode UTF-8:
; 0x00000000 — 0x0000007F: 0xxxxxxx
; 0x00000080 — 0x000007FF: 110xxxxx 10xxxxxx
; 0x00000800 — 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
; 0x00010000 — 0x001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
; UTF-8     EF BB BF
; UTF-16BE  FE FF
; UTF-16LE  FF FE
; UTF-32BE  00 00 FE FF
; UTF-32LE  FF FE 00 00
USE32
cp1251_to_866_char: ; al
       @@: cmp	al,0xc0
	   jb  .1
	   cmp	al,0xef
	   ja  .3
	   xor	al,0x40
	   jmp .2
       .1: cmp	al,0xa8
	   jnz .3
	   mov	al,0xf0
	   jmp .2
       .3: cmp	al,0xb8
	   jnz .4
	   mov	al,0xf1
	   jmp .2
       .4: cmp	al,0xf0
	   jb  .2
	   xor	al,0x10
       .2: ret

align 4
cp1251utf8_array:
	DB 208, 160
	DB 192, 144
	DB 193, 145
	DB 194, 146
	DB 195, 147
	DB 196, 148
	DB 197, 149
	DB 168, 129
	DB 198, 150
	DB 199, 151
	DB 200, 152
	DB 201, 153
	DB 202, 154
	DB 203, 155
	DB 204, 156
	DB 205, 157
	DB 206, 158
	DB 207, 159
	DB 209, 161
	DB 210, 162
	DB 211, 163
	DB 212, 164
	DB 213, 165
	DB 214, 166
	DB 215, 167
	DB 216, 168
	DB 217, 169
	DB 218, 170
	DB 219, 171
	DB 220, 172
	DB 221, 173
	DB 222, 174
	DB 223, 175
	DB 224, 176
	DB 225, 177
	DB 226, 178
	DB 227, 179
	DB 228, 180
	DB 229, 181
	DB 184, 145
	DB 230, 182
	DB 231, 183
	DB 232, 184
	DB 233, 185
	DB 234, 186
	DB 235, 187
	DB 236, 188
	DB 237, 189
	DB 238, 190
	DB 239, 191
	DB 240, 128
	DB 241, 129
	DB 242, 130
	DB 243, 131
	DB 244, 132
	DB 245, 133
	DB 246, 134
	DB 247, 135
	DB 248, 136
	DB 249, 137
	DB 250, 138
	DB 251, 139
	DB 252, 140
	DB 253, 141
	DB 254, 142
	DB 255, 143
	DB 0,	0


align 4
utf8_to_cp1251:
; in:  esi: string-Z, edi: out buffer

; out: -
ret


align 4
cp1251_to_utf8: ;--------------------------------;
; in:  esi: string-Z, edi: out buffer		 ;
     push   eax ebx esi edi			 ;
 .n:						 ;
     lodsb					 ;
     or     al,  al				 ;
     jz     short .exit 			 ;
     cmp    al,  0x7F				 ;
     ja     short @f				 ;
     and    al, 01111111b			 ; clear hight bit
     stosb					 ;
     jmp    short .n				 ;
 @@: mov    bl,  al				 ;
     push   esi 				 ;
     mov    esi, cp1251utf8_array		 ;
 @@: lodsw					 ;
     or     ax, ax				 ;
     jz     short .k				 ;
     cmp    al, bl				 ;
     jne    short @b				 ;
     jmp    short @f				 ;
 .k: pop    esi 				 ;
     jmp    short .n				 ;
 @@: pop    esi 				 ;
     cmp    al, 184				 ;
     je     short @f				 ;
     cmp    al, 240				 ;
     jae    short @f				 ;
     mov    al, 208				 ;
     jmp    short .s				 ;
 @@: mov    al, 209				 ;
 .s: stosw					 ; store utf special byte
     jmp    short .n				 ;
 .exit: 					 ;
     xor    al, al				 ;
     stosb					 ;
     pop    edi esi ebx eax			 ;
; out: -					 ;
ret ;--------------------------------------------;






;--------------------------------------------------------
; Проверка строки на соответствие формату
; кириллического UTF-8
;--------------------------------------------------------
; Символы [0x00-0x7F] или двухсимвольные конструкции
; вида 0xD0[0x81|0x90-0xBF] или 0xD1[0x91|0x80-0x8F]
;--------------------------------------------------------
; На выходе:
;    EAX = 1 - строка соответствует UTF-8
;    EAX = 0 - строка не соответствует формату
;--------------------------------------------------------
string_is_utf8: ; ESI: tstr:DWORD
	push	esi ebx
	;mov	 esi,[tstr]
	; По умолчанию строка соответствует формату
	mov	ebx,1
    .loc_scan:
	lodsb
	; Окончание строки?
	or	al,al
	jz	.loc_ret
	; Проверка символов [0x00-0x7F]
	cmp	al,07Fh
	jbe	.loc_scan
	; Проверка двухсимвольной конструкции
	; 0xD0[0x81|0x90-0xBF]
	cmp	al,0D0h
	jne	@f
	lodsb
	cmp	al,81h
	je	.loc_scan
	cmp	al,90h
	jb	.loc_fail
	cmp	al,0BFh
	ja	.loc_fail
	jmp	.loc_scan
    @@: ; Проверка двухсимвольной конструкции
	; 0xD1[0x91|0x80-0x8F]
	cmp	al,0D1h
	jne	.loc_fail
	lodsb
	cmp	al,91h
	je	.loc_scan
	cmp	al,80h
	jb	.loc_fail
	cmp	al,8Fh
	jbe	.loc_scan
    .loc_fail:
	; Строка не соответствует формату
	xor	ebx,ebx
    .loc_ret:
	mov	eax,ebx
	pop	ebx esi
	ret



;-----------------------------------------------------------------------
; Функция URL-кодирования
; by ManHunter / PCL
; http://www.manhunter.ru
;-----------------------------------------------------------------------
; Параметры:
;	lpData - указатель на строку
;	dSize  - длина строки
;	lpBuff - указатель на буфер-приемник
;	dRaw   - использовать RAW-кодирование (TRUE/FALSE)
;-----------------------------------------------------------------------
align 4
url_encode: ; ESI lpData:DWORD, ECX dSize:DWORD, EDI lpBuff:DWORD, EBX dRaw:DWORD
	push	eax ecx edi esi
	;mov	 ecx,[dSize]
	;mov	 esi,[lpData]
	;mov	 edi,[lpBuff]
	or	ecx,ecx
	jz     .loc_done
    .loc_encode:
	lodsb
	; Принудительно конвертировать все подряд
	cmp	ebx, TRUE
	je     .raw_encode
	; Пробел заменяется на '+'
	cmp	al,' '
	jne	@f
	mov	al,'+'
	jmp    .loc_store
    @@: ; Следующие символы без изменения
	cmp	al,'_'
	jz     .loc_store
	cmp	al,'.'
	jz     .loc_store
	cmp	al,'-'
	jz     .loc_store
	; Буквенно-цифровые символы без изменения
	cmp	al,'0'
	jb	.raw_encode
	cmp	al,'9'
	jbe	.loc_store
	cmp	al,'A'
	jb	.raw_encode
	cmp	al,'Z'
	jbe	.loc_store
	cmp	al,'a'
	jb	.raw_encode
	cmp	al,'z'
	jbe	.loc_store
	; Все остальное кодируется
    .raw_encode:
	push	eax
	mov	al,'%'
	stosb
	pop	eax
	; Перевести байт в HEX-строку
	movzx	eax,al
	ror	eax,4
	and	al,0Fh
	daa
	add	al,0F0h
	adc	al,40h
	stosb
	shr	eax,28
	and	al,0Fh
	daa
	add	al,0F0h
	adc	al,40h
    .loc_store:
	; Записать закодированный символ в строку
	stosb
	loop	.loc_encode
    .loc_done:
	; Символ конца строки
	mov	al,0
	stosb
	pop	esi edi ecx eax
	ret


;-----------------------------------------------------------------------
; Функция URL-декодирования
; by ManHunter / PCL
; http://www.manhunter.ru
;-----------------------------------------------------------------------
; Параметры:
;     ESI  lpData - указатель на строку
;     ECX  dSize  - длина строки
;     EDI  lpBuff - указатель на буфер-приемник
;     EBX  dRaw   - использовать RAW-декодирование (TRUE/FALSE)
;-----------------------------------------------------------------------
urldecode: ; lpData:DWORD, dSize:DWORD, lpBuff:DWORD, dRaw:DWORD
	push	eax ebx ecx edx edi esi
	;mov	 ecx,[dSize]
	;mov	 esi,[lpData]
	;mov	 edi,[lpBuff]
	or	ecx, ecx
	jz     .loc_done
    .loc_decode:
	xor	eax, eax
	lodsb
	; Принудительно конвертировать все подряд
	cmp	ebx, TRUE
	je     .raw_decode
	; '+' заменяется на пробел
	cmp	al,  '+'
	jne    .raw_decode
	mov	al,  ' '
	jmp    .loc_store
    .raw_decode:
	; Возможно, это закодированный символ
	cmp	al,  '%'
	jne    .loc_store
	; Строка заканчивается?
	cmp	ecx, 3
	jb     .loc_store
	cmp	byte [esi],'u'
	jne    .decode_ascii
	; Строка заканчивается?
	cmp	ecx, 5
	jb     .loc_store
	; Двухбайтные юникод-скаляры %uNNNN (UTF-16)
 .decode_unicode:
	push	eax
	xor	ebx, ebx
	xor	edx, edx
	inc	edx
	mov	ah,  5
	jmp    .decode_digit
	; Однобайтовые юникод-скаляры %NN
 .decode_ascii:
	push	eax
	xor	ebx, ebx
	xor	edx, edx
	mov	ah,  2
	; Декодировать нужное количество цифр
 .decode_digit:
	shl	ebx,4
	mov	al,byte [esi+edx]
	cmp	al,'0'
	jb	.loc_skip
	cmp	al,'9'
	ja	@f
	sub	al,'0'
	jmp	.decode_digit_next
@@:
	; Символ в нижний регистр
	or	al,20h
	cmp	al,'a'
	jb	.loc_skip
	cmp	al,'f'
	ja	.loc_skip
	sub	al,'a'-10
.decode_digit_next:
	add	bl,al
	inc	edx
	cmp	dl,ah
	jb	.decode_digit

	xchg	eax,ebx
	shr	ebx,8

	; Пропустить следующие символы
	sub	ecx,ebx
	add	esi,ebx

	pop	ebx
	jmp	.loc_store
.loc_skip:
	pop	eax

.loc_store:
	cmp	eax,0FFh
	jbe	.loc_store_byte
.loc_store_word:
	; Записать раскодированный юникодный символ в строку
	stosw
	jmp	@f
.loc_store_byte:
	; Записать раскодированный символ в строку
	stosb
@@:
	loop	.loc_decode
.loc_done:
	; Символ конца строки
	mov	al,0
	stosb

	pop	esi edi edx ecx ebx eax
	ret

