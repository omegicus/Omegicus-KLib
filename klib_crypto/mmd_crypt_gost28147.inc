; GOST 28197-89
use32

KeyRepeat = 4	    ; Число повторений ключа

; GOST
; gost_encrypt: ; ESI: buffer[/8=0], ECX: Size, EDI: Key32bytes
; gost_decrypt: ; ESI: buffer[/8=0], ECX: Size, EDI: Key32bytes


; GOST variables
    align 32
;    GOST_SBOX: ; "A" from GOST
;    db 0x4, 0xA, 0x9, 0x2, 0xD, 0x8, 0x0, 0xE, 0x6, 0xB, 0x1, 0xC, 0x7, 0xF, 0x5, 0x3
;    db 0xE, 0xB, 0x4, 0xC, 0x6, 0xD, 0xF, 0xA, 0x2, 0x3, 0x8, 0x1, 0x0, 0x7, 0x5, 0x9
;    db 0x5, 0x8, 0x1, 0xD, 0xA, 0x3, 0x4, 0x2, 0xE, 0xF, 0xC, 0x7, 0x6, 0x0, 0x9, 0xB
;    db 0x7, 0xD, 0xA, 0x1, 0x0, 0x8, 0x9, 0xF, 0xE, 0x4, 0x6, 0xC, 0xB, 0x2, 0x5, 0x3
;    db 0x6, 0xC, 0x7, 0x1, 0x5, 0xF, 0xD, 0x8, 0x4, 0xA, 0x9, 0xE, 0x0, 0x3, 0xB, 0x2
;    db 0x4, 0xB, 0xA, 0x0, 0x7, 0x2, 0x1, 0xD, 0x3, 0x6, 0x8, 0x5, 0x9, 0xC, 0xF, 0xE
;;    db 0xD, 0xB, 0x4, 0x1, 0x3, 0xF, 0x5, 0x9, 0x0, 0xA, 0xE, 0x7, 0x6, 0x8, 0x2, 0xC
;    db 0x1, 0xF, 0xD, 0x0, 0x5, 0x7, 0xA, 0x4, 0x9, 0x2, 0x3, 0xE, 0x6, 0xB, 0x8, 0xC


;align 32
;gostmemvars:
;    gost_ext_simle_table     db  1024 dup 0
;    gost_int_table	      db  0256 dup 0
;    crypt_key		      db  0032 dup 0
;    crypt_vars:
;    crypt_a		      dd  0x00000000
;    crypt_b		      dd  0x00000000
;    crypt_c		      dd  0x00000000
;    crypt_d		      dd  0x00000000
;gostmemvars_end:

align 4
gost_clear:
     pushad
     ;
     xor     eax, eax
     mov     ecx, G12_VARS_SZ / 4
     mov     edi, G12_VARS
     cld
     rep     stosd
     mov     byte[hash_X_v512+62], 0x02
     ;
     mov   edi, gostmemvars
     mov   ecx, gostmemvars_sz/4
     xor   eax, eax
     cld
     rep   stosd
     ;
     popad
ret


; 4) gost_gammaLE() ?? 8byte (gost_syn+0, gost_syn+4)
align 4
gost_encrypt: ; ESI: buffer~8[/8=0], ECX: Size, EDI: Key32bytes
	pushad
	call	gost_ExpCht
	call	gost_ExpandKey
	mov	edi, esi
	mov	edx, ecx
	xor	ecx, ecx
    @@: push	edx
	call	gost_gammaLE
	pop	edx
	add	edi, 0x08
	add	ecx, 0x08
	cmp	ecx, edx
	jbe	@b
	popad
ret


align 4
gost_decrypt: ; ESI: buffer~8, ECX: Size, EDI: Key32bytes
	pushad
	call	gost_ExpCht
	call	gost_ExpandKey
	mov	edi, esi
	mov	edx, ecx
	xor	ecx, ecx
    @@: push	edx
	call	gost_gammaLD
	pop	edx
	add	edi, 0x08
	add	ecx, 0x08
	cmp	ecx, edx
	jbe	@b
	popad
ret


; -----------------------------------------------;


align 4
gost_gost32:
	mov	ebx, gost_ext_simle_table
	align	4
    @@: mov	ebp, eax
	add	eax, dword[esi]
	add	esi, 0x04
	xlatb
	ror	eax, 0x08
	add	ebx, 0x0100
	xlatb
	ror	eax, 0x08
	add	ebx, 0x0100
	xlatb
	ror	eax, 0x08
	add	ebx, 0x100
	xlatb
	sub	ebx, 0x300
	rol	eax, 0x03
	xor	eax, edx
	mov	edx, ebp
	dec	ecx
	jnz	@b
ret


align 4
gost_gammaLD:  ; in EDI - buffer
       pusha
       mov	eax, dword[crypt_vars+0]
       mov	edx, dword[crypt_vars+4]
       mov	ecx, 32
       mov	esi, gost_int_table
       call	gost_gost32
       xchg	eax, edx
       xor	eax, dword[edi + 00]
       xor	edx, dword[edi + 04]
       xchg	dword[edi + 00], eax
       xchg	dword[edi + 04], edx
       mov	dword[crypt_vars+0], eax
       mov	dword[crypt_vars+4], edx
       popa
ret

align 4
gost_gammaLE:  ; in EDI - buffer
       pusha
       mov	eax, dword[crypt_vars+0]
       mov	edx, dword[crypt_vars+4]
       mov	ecx, 32
       mov	esi, gost_int_table
       call	gost_gost32
       xchg	eax, edx
       xor	eax, dword[edi + 00]
       xor	edx, dword[edi + 04]
       mov	dword[edi + 00], eax
       mov	dword[edi + 04], edx
       mov	dword[crypt_vars+0], eax
       mov	dword[crypt_vars+4], edx
       popa
ret


align 4
gost_ExpandKey: ; EDI points to Key
	pushad
	mov	esi, edi
	mov	edi, gost_int_table
	lea	ebx, [esi + 28]
	xor	eax, eax
	cld
	align	4
  .b34: cmp	eax, 0x04
	jge    .g2e
	mov	ecx, 0x08
	push	edx
	mov	edx, 0x08
	bt	edx, eax
	pop	edx
	jc     .rev
	mov	esi, crypt_key
	rep	movsd
	jmp    .inc
  .rev: mov	esi, ebx
    @@: movsd
	sub	esi, 0x08
	loop	@b
  .inc: inc	eax
	jmp    .b34
  .g2e: popad
ret


align 4
gost_ExpCht:
	pushad
	mov	esi, GOST_SBOX
	mov	edi, gost_ext_simle_table
	cld
	mov	ecx, 0x04
 .blks: lea	ebx, [esi + 0x10]
	push	ecx
	mov	 cl, 16
 .lins: push	esi
	mov	 ah, byte[ebx + 0x00]
	shl	 ah, 0x04
	push	ecx
	mov	 cl, 16
    @@: lodsb
	or	 al, ah
	stosb
	loop	 @b
	pop	 ecx
	pop	 esi
	inc	 ebx
	loop	.lins
	pop	 ecx
	add	 esi, 0x20
	loop	.blks
	popad
ret
