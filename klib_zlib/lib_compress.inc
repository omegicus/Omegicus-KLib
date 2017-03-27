align 4
decompress:  ; esi: data, ecx: size  edi=buffer
; << METHODS: 'cLZW', 'cLZO', 'cLZ4', 'cHUF','cLZM'
; >> edi: outData, eax: resultSize
	cmp    dword[esi], 'cLZM'
	je    .lzma
	cmp    dword[esi], 'cLZW'
	je    .lzw
	cmp    dword[esi], 'cLZO'
	je    .lzo
	jmp   .error


     align 4
    .lzw:
       ; mov	 eax, dword[esi + 04]
       ; call	 os_malloc
       ; cmp	 eax, -1
       ; je	.error
       ; mov	 edi, eax
	;
	push	eax ecx edi
	mov	ecx, dword[esi + 04]
	xor	eax, eax
	rep	stosb
	pop	edi ecx eax
	;
	push	edi
	push	dword[esi + 04] ; original size
	add	esi, 64 	; skip header
	;
	push	edi
	push	ecx
	push	esi
	call	lzwDecompress
	add	esp, 3*4
	;
	pop	eax
	pop	edi
	jmp    .ok


     align 4
    .lzma:
      ;  mov	 eax, dword[esi + 04]
      ;  call	 os_malloc
      ;  cmp	 eax, -1
      ;  je	.error
      ;  mov	 edi, eax
	push	edi esi
	push	dword[esi + 04]
	push	edi
	add	esi, 64 	; skip header
	push	esi
	call	lzma_decompress
	pop	esi edi
	mov	eax, dword[esi + 04]
	jmp   .ok


     align 4
    .lzo:
      ;  mov	 eax, dword[esi + 04]		   ; source size
      ;  call	 os_malloc			   ;
      ;  cmp	 eax, -1			   ;
      ;  je	.error				   ;
      ;  mov	 edi, eax			   ;


	push	edi  esi
	add	esi, 64 			  ; skip header
	sub	ecx, 64 			  ; skip header
	call	ARCH_LZO_DECOMPRESS		  ; in:  esi: data, ecx: data size[<=e000], edi: res.data
						  ; out: eax - result length
	pop	esi edi



	mov	eax, dword[esi + 04]
	jmp   .ok



.not_lzma:
.not_lzo:
	jmp   .error
;
.ok:	jmp   .exit
.error:
	mov    eax, -1
	jmp   .exit
.exit:
ret
ret



align 4
compress: ; eax: TYPE, esi: data, ecx: size, edi=outbuffer
; << METHODS: 'cLZW', 'cLZM'
; >> edi: outData, eax: resultSize

	push   ecx edi eax
	mov    edi, header
	mov    ecx, 64/4
	xor    eax, eax
	cld
	rep    stosd
	pop    eax edi ecx

	mov    dword[header + 00], eax
	mov    dword[header + 04], ecx

	cmp    eax, 'cLZW'
	je    .lzw
	cmp    eax, 'cLZM'
	je    .lzma
	cmp    eax, 'cLZO'
	je    .lzo

	jmp   .error


 .lzo:	; esi data
	; ecx size <=57344
	push	eax
	mov	eax, ecx
	call	os_malloc
	mov	edi, eax
	pop	eax
	cmp	edi, -1
	je     .error

	push	ecx esi edi
	mov	esi, header
	mov	ecx, 64/4
	cld
	rep	movsd
	pop	edi esi ecx

	push	edi
	add	edi, 64
	call	ARCH_LZO_COMPRESS    ; in:  esi: data, ecx: data size[<=57344], edi: dest	  ; out: eax - result length
	pop	edi
	add	eax, 64
	; FULL OUT: eax=size, edi=buffer
	jmp    .ok



	align 4
 .lzw:	push	ecx
	call	lzwGetRequiredMemSize
	add	eax, 64
	call	os_malloc
	cmp	eax, -1
	je     .error
	mov	edi, eax
	push	edi
	push	ecx esi edi
	mov	esi, header
	mov	ecx, 64/4
	cld
	rep	movsd
	pop	edi esi ecx
	add	edi, 64
	push	edi
	push	ecx
	push	esi
	call	lzwCompress
	add	esp, 3*4
	pop	edi
	add	eax, 64
	jmp    .ok





	align 4
 .lzma: dec	ecx
	bsr	eax, ecx
	inc	eax
	cmp	eax, 28
	jb	@f
	mov	eax, 28
     @@:inc	ecx
	push	ecx
	push	eax
	push	eax
	call	lzma_set_dict_size
	pop	ecx
	mov	eax, 1
	shl	eax, cl
	pop	ecx
	mov	[lzma_dictsize], eax
	imul	eax, 19
	shr	eax, 1
	add	eax, 509000h
	call	os_malloc
	cmp	eax, -1
	je     .error
	mov	dword[workmem], eax
	mov	eax, ecx
	shr	eax, 3
	add	eax, ecx
	add	eax, 0x0400	  ; should be enough for header
	add	eax, eax
	mov	eax, ecx
	add	eax, 64 	  ; for header
	call	os_malloc
	cmp	eax, -1
	je     .error
	mov	edi, eax
	pushad
	mov	esi, header
	mov	ecx, 64/4
	cld
	rep	movsd
	popad
	add	edi, 64
	push   edi
	push   dword[workmem]
	push   ecx
	push   edi
	push   esi
	call   lzma_compress
	add    eax, 64
	pop    edi
	sub    edi, 64
	jmp   .ok

.not_lzma:
.not_lzo:
	jmp   .error
;
.ok:	jmp   .exit
.error:
	mov    eax, -1
	jmp   .exit
.exit:

ret



os_malloc:
	push	ebx ecx edx esi edi
	;invoke  GlobalAlloc, GPTR , eax
	invoke	VirtualAlloc, 0, EAX, MEM_COMMIT, PAGE_READWRITE
	pop	edi esi edx ecx ebx
ret
os_free:
	push	ebx ecx edx esi edi
	;invoke  GlobalFree, eax
	invoke	VirtualFree,	EAX, 0, MEM_RELEASE
	pop	edi esi edx ecx ebx
ret


include 'lib_lzw.inc'
include 'lib_lzma.inc'
include 'lib_lzo.inc'