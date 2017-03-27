; IDEA
use32


; multiply ax with bx modulo (2**16)+1 the result goes to ax
; NOTE: the MSBs of eax must be zeroed!
align 4
mod_mul:
	or	eax,eax
	jz	short _a0
	or	bx,bx
	jz	short _b0
	push	edx
	push	ebx
	xor	edx,edx
	mul	bx
	shl	edx,16
	xchg	dx,ax
	xchg	edx,eax
	mov	ebx,65537
	div	ebx
	xchg	ax,dx
	pop	ebx
	pop	edx
	ret
 _a0:	mov	ax,bx
 _b0:	neg	ax
	inc	ax
ret


; ax's multiplicative inversion (brutal method)
;      NOTE: the MSBs of eax are zeroed!
align 4
inv:
	and	eax,0xffff
	or	eax,eax
	jz	short .inv_ret
	push	ebx
	push	ecx
	push	edx
	mov	ecx,65535
	lea	ebx,[ecx+2]
    @@: xor	edx,edx
	push	eax
	mul	ecx
	div	ebx
	pop	eax
	dec	edx
	jz	short .inv_done
	loop	@B
 .inv_done:
	xchg	ecx,eax
	pop	edx
	pop	ecx
	pop	ebx
 .inv_ret:
ret


; encode key generator
; in:  esi - user_key[16]
align 4
gen_en_key:
	mov	edi, idea_enkey
	pusha
	push	edi
	mov    ecx,8
	rep	movsw			; memcpy(en_key,user_key,2*8)
	pop	esi
	inc	ecx			; i = 1, b = 0..
	mov    ebx,0
    @@: push	ecx
	and	ecx,7			; ax = en_key[b+(i&7)]
	lea	edx,[ebx+ecx]
	mov	ax,[esi+edx*2]
	inc	ecx			; dx = en_key[b+((i+1)&7)]
	and	ecx,7
	lea	edx,[ebx+ecx]
	mov	dx,[esi+edx*2]
	shl	ax,9			; en_key[j++] = (ax << 9)|(dx >> 7)
	shr	dx,7
	or	ax,dx
	stosw
	pop	ecx
	mov	eax,ecx 		; b += (!((i++)&7))*8
	and	eax,7
	sub	al,1
	setc	al
	shl	eax,3
	add	ebx,eax
	inc	ecx
	cmp	ecx,44
	jle	short @B
	popa
ret


; decode key generator
; in:  esi - enkey[52]
align 4
gen_de_key:
	mov	esi, idea_enkey
	mov	edi, idea_dekey
	pusha
	xor	eax,eax
	mov	ax,[esi+2*48]		; de_key[0]=inv(en_key[48])
	call	inv
	stosw
	mov	ax,[esi+2*49]		; de_key[1]=-en_key[49]
	neg	ax
	stosw
	mov	ax,[esi+2*50]		; de_key[2]=-en_key[50]
	neg	ax
	stosw
	mov	ax,[esi+2*51]		; de_key[3]=inv(en_key[51])
	call	inv
	stosw
	mov	ecx,42			; k = 42
   @@:	mov	eax,[esi+ecx*2+2*4]	; de_key[i++]=en_key[k+4]
	stosd				; de_key[i++]=en_key[k+5]
	mov	ax,[esi+ecx*2]		; de_key[i++]=inv(en_key[k])
	call	inv
	stosw
	mov	ax,[esi+ecx*2+2*2]	; de_key[i++]=-en_key[k+2]
	neg	ax
	stosw
	mov	ax,[esi+ecx*2+2*1]	; de_key[i++]=-en_key[k+1]
	neg	ax
	stosw
	mov	ax,[esi+ecx*2+2*3]	; de_key[i++]=inv(en_key[k+3])
	call	inv
	stosw
	sub	ecx,6
	jnz	short @B
	mov	eax,[esi+2*4]		; de_key[46]=en_key[4]
	stosd				; de_key[47]=en_key[5]
	lodsw				; de_key[48]=inv(en_key[0])
	call	inv
	stosw
	lodsw				; de_key[49]=-en_key[1]
	neg	ax
	stosw
	lodsw				; de_key[50]=-en_key[2]
	neg	ax
	stosw
	lodsw				; de_key[51]=inv(en_key[3])
	call	inv
	stosw
	popa
ret


; encode / decode routine
; in:  esi - en/de_key[52], edi - input[4]
; out: edi - output[4]
align 4
do_idea:
	pusha
	push	edi
	mov	cx,[edi+2*0]		; x1
	mov	dx,[edi+2*1]		; x2
	mov	bp,[edi+2*2]		; x3
	mov	di,[edi+2*3]		; x4
	mov    eax,8
    @@: push	eax
	lodsw				; x1 = x1 %* key[i++]
	mov	bx,cx
	call	mod_mul
	xchg	cx,ax
	lodsw				; x2 += key[i++]
	add	dx,ax
	lodsw				; x3 += key[i++]
	add	bp,ax
	lodsw				; x4 = x4 %* key[i++]
	mov	bx,di
	call	mod_mul
	xchg	di,ax
	mov	bx,bp			; tmp1 = (x1^x3) %* key[i++]
	xor	bx,cx
	lodsw
	call	mod_mul
	push	eax
	mov	bx,di			; tmp2 = (tmp1 + (x2^x4)) %* key[i++]
	xor	bx,dx
	add	bx,ax
	lodsw
	call	mod_mul
	pop	ebx			; tmp1 += tmp2
	add	bx,ax
	xor	cx,ax			; x1 ^= tmp2
	xor	bp,ax			; x3 ^= tmp2
	xor	dx,bx			; x2 ^= tmp1
	xor	di,bx			; x4 ^= tmp1
	xchg	bp,dx			; swap(x2,x3)
	pop	eax
	dec	eax
	jnz	short @B
	mov	bx,cx			; x1 = x1 %* key[i++]
	lodsw
	call	mod_mul
	xchg	cx,ax
	lodsw				; x2 += key[i++]
	add	bp,ax
	lodsw				; x3 += key[i++]
	add	dx,ax
	mov	bx,di			; x4 = x4 %* key[i++]
	lodsw
	call	mod_mul
	pop	edi			; store..
	mov	[edi],cx		; x1
	mov	[edi+2*1],bp		; x2
	mov	[edi+2*2],dx		; x3
	mov	[edi+2*3],ax		; x4
	popa
ret


; include 'lib_idea_data.inc'
;align 32
;	 idea_enkey		  db  64 dup 0
;	 idea_dekey		  db  64 dup 0
