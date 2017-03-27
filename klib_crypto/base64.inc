; calculates size of base64 stream after encoding
; in:  eax
; out: eax
base64_get_size:
	push	ebx ecx edx
	mov	ebx,eax
	add	eax,2
	xor	edx,edx
	mov	ecx,3
	div	ecx
	add	ebx,3
	sub	ebx,edx
	mov	eax,ebx
	xor	edx,edx
	div	ecx
	shl	eax,2
	pop	edx ecx ebx
ret


; encodes given buffer into base64 stream
base64_encode:
	push	ecx edx esi edi
	mov	esi,[b64_source]
	mov	ecx,[b64_src_size]
	mov	edi,[b64_out_buf]
     .loop:
	mov	edx,[esi]
	cmp	ecx,3
	jae	.remainder_ok
	and	edx,0xffff
	cmp	ecx,2
	jae	.remainder_ok
	and	edx,0xff
     .remainder_ok:
	bswap	edx

	mov	eax,edx
	shr	eax,26
	and	eax,111111b
	mov	al,[base64_table+eax]
	mov	[edi],al
	inc	edi

	mov	eax,edx
	shr	eax,20
	and	eax,111111b
	mov	al,[base64_table+eax]
	mov	[edi],al
	inc	edi
	dec	ecx
	jz	.r2

	mov	eax,edx
	shr	eax,14
	and	eax,111111b
	mov	al,[base64_table+eax]
	mov	[edi],al
	inc	edi
	dec	ecx
	jz	.r1

	mov	eax,edx
	shr	eax,8
	and	eax,111111b
	mov	al,[base64_table+eax]
	mov	[edi],al
	inc	edi

	add	esi,3
	dec	ecx
	jnz	.loop
	jmp	.finish
     .r2:
	mov	byte[edi],'='
	inc	edi
     .r1:
	mov	byte[edi],'='
	inc	edi
     .finish:
	mov	byte[edi],0
	sub	edi,[b64_out_buf]
	mov	eax,edi 	; return output size in eax
	pop	edi esi edx ecx
ret


; decodes base64 stream into given buffer
base64_decode:
;.source,.src_size,.out_buf
	push	ebx ecx edx esi edi
	xor	eax,eax
	mov	ebx,63
    .make_table:
	mov	al,[base64_table+ebx]
	mov	[decode_table+eax],bl
	dec	ebx
	jns	.make_table
	mov	[decode_table+'='],0

	mov	esi,[b64_source]
	mov	ecx,[b64_src_size]
	mov	edi,[b64_out_buf]
	xor	ebx,ebx
;	 xor	 eax,eax	; not needed - eax was xored before
    .load_dword:
	mov	edx,[esi]
	bswap	edx
	add	esi,4
	mov	al,dl
	mov	al,byte[decode_table+eax]
	shrd	ebx,eax,6
	shr	edx,8
	mov	al,dl
	mov	al,byte[decode_table+eax]
	shrd	ebx,eax,6
	shr	edx,8
	mov	al,dl
	mov	al,byte[decode_table+eax]
	shrd	ebx,eax,6
	shr	edx,8
	mov	al,dl
	mov	al,byte[decode_table+eax]
	shrd	ebx,eax,6
	bswap	ebx

	mov	[edi],ebx
	add	edi,3
	sub	ecx,4
	js	.exit_loop	; this jump should be never taken
	jnz	.load_dword	; (src_size should be a multiple of four)
     .exit_loop:
	sub	edi,2
	cmp	byte[edi],0
	je	.size_ok
	inc	edi
	cmp	byte[edi],0
	je	.size_ok
	inc	edi
	mov	byte[edi],0
	inc	edi
     .size_ok:
	sub	edi,[b64_out_buf]
	mov	eax,edi 	; return output size in eax
	pop	edi esi edx ecx ebx
ret