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
is_utf8: ; string
	mov	ebp, esp
	push	esi  ebx
	mov	esi, [ebp + 04]
	mov	ebx,1				; По умолчанию строка соответствует формату
   .loc_scan:
	lodsb
	or	al,al				; Окончание строки?
	jz     .loc_ret
	cmp	al,07Fh 			; Проверка символов [0x00-0x7F]
	jbe    .loc_scan
	cmp	al,0D0h 			; Проверка двухсимвольной конструкции 0xD0[0x81|0x90-0xBF]
	jne	@f
	lodsb
	cmp	al,81h
	je     .loc_scan
	cmp	al,90h
	jb     .loc_fail
	cmp	al,0BFh
	ja     .loc_fail
	jmp    .loc_scan
    @@: ; Проверка двухсимвольной конструкции 0xD1[0x91|0x80-0x8F]
	cmp	al,0D1h
	jne    .loc_fail
	lodsb
	cmp	al,91h
	je     .loc_scan
	cmp	al,80h
	jb     .loc_fail
	cmp	al,8Fh
	jbe    .loc_scan
   .loc_fail:
	; Строка не соответствует формату
	xor	ebx,ebx
   .loc_ret:
	mov	eax,ebx
	pop	ebx esi
ret 4*1



align 4
utf8_to_cp1251:
; in:  esi: string-Z, edi: out buffer

; out: -
ret