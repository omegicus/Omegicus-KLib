

align 16
Lmask	   dd 0c0f0e0dh,0c0f0e0dh,0c0f0e0dh,0c0f0e0dh
Lmask192   dd 004070605h, 004070605h, 004070605h, 004070605h
Lmask256   dd 00c0f0e0dh, 00c0f0e0dh, 00c0f0e0dh, 00c0f0e0dh
Lcon1	   dd 1,1,1,1
Lcon2	   dd 1bh,1bh,1bh,1bh


;ctx	  textequ <ecx>
;output   textequ <edx>
;input	  textequ <eax>
;inputLen textequ <edi>


macro aes_rnd i {
    movdqu  xmm7, [i*16 + ctx]
    aesenc  xmm0, xmm7
    aesenc  xmm1, xmm7
    aesenc  xmm2, xmm7
    aesenc  xmm3, xmm7
    aesenc  xmm4, xmm7
    aesenc  xmm5, xmm7
    aesenc  xmm6, xmm7
}

macro aes_last_rnd i {
    movdqu	xmm7, [i*16 + ctx]
    aesenclast	xmm0, xmm7
    aesenclast	xmm1, xmm7
    aesenclast	xmm2, xmm7
    aesenclast	xmm3, xmm7
    aesenclast	xmm4, xmm7
    aesenclast	xmm5, xmm7
    aesenclast	xmm6, xmm7
}

aes_dec_rnd i {
    movdqu  xmm7, [i*16 + ctx]
    aesdec  xmm0, xmm7
    aesdec  xmm1, xmm7
    aesdec  xmm2, xmm7
    aesdec  xmm3, xmm7
    aesdec  xmm4, xmm7
    aesdec  xmm5, xmm7
    aesdec  xmm6, xmm7
}

aes_dec_last_rnd i {
    movdqu	xmm7, [i*16 + ctx]
    aesdeclast	xmm0, xmm7
    aesdeclast	xmm1, xmm7
    aesdeclast	xmm2, xmm7
    aesdeclast	xmm3, xmm7
    aesdeclast	xmm4, xmm7
    aesdeclast	xmm5, xmm7
    aesdeclast	xmm6, xmm7
}




KEY textequ <ecx>
KS  textequ <edx>
ITR textequ <eax>


align 16
intel_aes_encrypt_init_256:
    mov     KEY,    [esp + 1*4 + 0*4]
    mov     KS,     [esp + 1*4 + 1*4]
    movdqu  xmm1, [16*0 + KEY]
    movdqu  xmm3, [16*1 + KEY]
    movdqu  [16*0 + KS], xmm1
    movdqu  [16*1 + KS], xmm3
    lea ITR, Lcon1
    movdqu  xmm0, [ITR]
    lea ITR, Lmask256
    movdqu  xmm5, [ITR]

    pxor    xmm6, xmm6

    mov ITR, 6

 Lenc_256_ks_loop:

	movdqa	xmm2, xmm3
	pshufb	xmm2, xmm5
	aesenclast  xmm2, xmm0
	pslld	xmm0, 1
	movdqa	xmm4, xmm1
	pslldq	xmm4, 4
	pxor	xmm1, xmm4
	pslldq	xmm4, 4
	pxor	xmm1, xmm4
	pslldq	xmm4, 4
	pxor	xmm1, xmm4
	pxor	xmm1, xmm2
	movdqu	[16*2 + KS], xmm1

	pshufd	xmm2, xmm1, 0ffh
	aesenclast  xmm2, xmm6
	movdqa	xmm4, xmm3
	pslldq	xmm4, 4
	pxor	xmm3, xmm4
	pslldq	xmm4, 4
	pxor	xmm3, xmm4
	pslldq	xmm4, 4
	pxor	xmm3, xmm4
	pxor	xmm3, xmm2
	movdqu	[16*3 + KS], xmm3

	lea KS, [32 + KS]
	dec ITR
	jnz Lenc_256_ks_loop

    movdqa  xmm2, xmm3
    pshufb  xmm2, xmm5
    aesenclast	xmm2, xmm0
    movdqa  xmm4, xmm1
    pslldq  xmm4, 4
    pxor    xmm1, xmm4
    pslldq  xmm4, 4
    pxor    xmm1, xmm4
    pslldq  xmm4, 4
    pxor    xmm1, xmm4
    pxor    xmm1, xmm2
    movdqu  [16*2 + KS], xmm1

    ret
intel_aes_encrypt_init_256  ENDP

ALIGN 16
intel_aes_decrypt_init_256  PROC
    mov     KEY,	[esp + 1*4 + 0*4]
    mov     KS, 	[esp + 1*4 + 1*4]

    push    KS
    push    KEY

    call    intel_aes_encrypt_init_256

    pop     KEY
    pop     KS

    movdqu  xmm0, [0*16 + KS]
    movdqu  xmm1, [14*16 + KS]
    movdqu  [14*16 + KS], xmm0
    movdqu  [0*16 + KS], xmm1

    i = 1
    WHILE i LT 7
	movdqu	xmm0, [i*16 + KS]
	movdqu	xmm1, [(14-i)*16 + KS]

	aesimc	xmm0, xmm0
	aesimc	xmm1, xmm1

	movdqu	[(14-i)*16 + KS], xmm0
	movdqu	[i*16 + KS], xmm1

	i = i+1
    ENDM

    movdqu  xmm0, [7*16 + KS]
    aesimc  xmm0, xmm0
    movdqu  [7*16 + KS], xmm0
    ret
intel_aes_decrypt_init_256  ENDP



gen_aes_cbc_enc_func MACRO rnds

LOCAL	loop1
LOCAL	bail

	push	inputLen

	mov	ctx,	[esp + 2*4 + 0*4]
	mov	output,     [esp + 2*4 + 1*4]
	mov	input,	    [esp + 2*4 + 4*4]
	mov	inputLen,   [esp + 2*4 + 5*4]

	lea	ctx,  [44+ctx]

	movdqu	xmm0, [-32+ctx]

	movdqu	xmm2, [0*16 + ctx]
	movdqu	xmm3, [1*16 + ctx]
	movdqu	xmm4, [2*16 + ctx]
	movdqu	xmm5, [3*16 + ctx]
	movdqu	xmm6, [4*16 + ctx]

loop1:
	cmp	inputLen, 1*16
	jb	bail

	movdqu	xmm1, [input]
	pxor	xmm1, xmm2
	pxor	xmm0, xmm1

	aesenc	xmm0, xmm3
	aesenc	xmm0, xmm4
	aesenc	xmm0, xmm5
	aesenc	xmm0, xmm6

	i = 5
    WHILE i LT rnds
	    movdqu  xmm7, [i*16 + ctx]
	    aesenc  xmm0, xmm7
	    i = i+1
	ENDM
	movdqu	xmm7, [rnds*16 + ctx]
	aesenclast xmm0, xmm7

	movdqu	[output], xmm0

	lea input, [1*16 + input]
	lea output, [1*16 + output]
	sub inputLen, 1*16
	jmp loop1

bail:
	movdqu	[-32+ctx], xmm0

	xor eax, eax
	pop inputLen
	ret

ENDM

gen_aes_cbc_dec_func MACRO rnds

LOCAL	loop7
LOCAL	loop1
LOCAL	dec1
LOCAL	bail

	push	inputLen

	mov	ctx,	[esp + 2*4 + 0*4]
	mov	output,     [esp + 2*4 + 1*4]
	mov	input,	    [esp + 2*4 + 4*4]
	mov	inputLen,   [esp + 2*4 + 5*4]

	lea	ctx, [44+ctx]

loop7:
	cmp	inputLen, 7*16
	jb	dec1

	movdqu	xmm0, [0*16 + input]
	movdqu	xmm1, [1*16 + input]
	movdqu	xmm2, [2*16 + input]
	movdqu	xmm3, [3*16 + input]
	movdqu	xmm4, [4*16 + input]
	movdqu	xmm5, [5*16 + input]
	movdqu	xmm6, [6*16 + input]

	movdqu	xmm7, [0*16 + ctx]
	pxor	xmm0, xmm7
	pxor	xmm1, xmm7
	pxor	xmm2, xmm7
	pxor	xmm3, xmm7
	pxor	xmm4, xmm7
	pxor	xmm5, xmm7
	pxor	xmm6, xmm7

	i = 1
	WHILE i LT rnds
	    aes_dec_rnd i
	    i = i+1
	    ENDM
	aes_dec_last_rnd rnds

	movdqu	xmm7, [-32 + ctx]
	pxor	xmm0, xmm7
	movdqu	xmm7, [0*16 + input]
	pxor	xmm1, xmm7
	movdqu	xmm7, [1*16 + input]
	pxor	xmm2, xmm7
	movdqu	xmm7, [2*16 + input]
	pxor	xmm3, xmm7
	movdqu	xmm7, [3*16 + input]
	pxor	xmm4, xmm7
	movdqu	xmm7, [4*16 + input]
	pxor	xmm5, xmm7
	movdqu	xmm7, [5*16 + input]
	pxor	xmm6, xmm7
	movdqu	xmm7, [6*16 + input]

	movdqu	[0*16 + output], xmm0
	movdqu	[1*16 + output], xmm1
	movdqu	[2*16 + output], xmm2
	movdqu	[3*16 + output], xmm3
	movdqu	[4*16 + output], xmm4
	movdqu	[5*16 + output], xmm5
	movdqu	[6*16 + output], xmm6
	movdqu	[-32 + ctx], xmm7

	lea input, [7*16 + input]
	lea output, [7*16 + output]
	sub inputLen, 7*16
	jmp loop7
dec1:

	movdqu	xmm3, [-32 + ctx]

loop1:
	cmp	inputLen, 1*16
	jb	bail

	movdqu	xmm0, [input]
	movdqa	xmm4, xmm0
	movdqu	xmm7, [0*16 + ctx]
	pxor	xmm0, xmm7

	i = 1
    WHILE i LT rnds
	    movdqu  xmm7, [i*16 + ctx]
	    aesdec  xmm0, xmm7
	    i = i+1
	ENDM
	movdqu	xmm7, [rnds*16 + ctx]
	aesdeclast xmm0, xmm7
	pxor	xmm3, xmm0

	movdqu	[output], xmm3
	movdqa	xmm3, xmm4

	lea input, [1*16 + input]
	lea output, [1*16 + output]
	sub inputLen, 1*16
	jmp loop1

bail:
	movdqu	[-32 + ctx], xmm3
	xor eax, eax
	pop	inputLen
	ret
ENDM




align 16
intel_aes_encrypt_cbc_256:
	gen_aes_cbc_enc_func  14
	ret

align 16
intel_aes_decrypt_cbc_256:
	gen_aes_cbc_dec_func  14
	ret





gen_aes_ctr_func MACRO rnds

LOCAL	loop7
LOCAL	loop1
LOCAL	enc1
LOCAL	bail

	push	inputLen
	push	ctrCtx
	push	CTR
	push	ebp

	mov	ctrCtx, [esp + 4*5 + 0*4]
	mov	output, [esp + 4*5 + 1*4]
	mov	input,	[esp + 4*5 + 4*4]
	mov	inputLen, [esp + 4*5 + 5*4]

	mov	ctx, [4+ctrCtx]
	lea	ctx, [44+ctx]

	mov	ebp, esp
	sub	esp, 7*16
	and	esp, -16

	movdqu	xmm0, [8+ctrCtx]
	mov	ctrCtx, [ctrCtx + 8 + 3*4]
	bswap	ctrCtx
	movdqu	xmm1, [ctx + 0*16]

	pxor	xmm0, xmm1

	movdqa	[esp + 0*16], xmm0
	movdqa	[esp + 1*16], xmm0
	movdqa	[esp + 2*16], xmm0
	movdqa	[esp + 3*16], xmm0
	movdqa	[esp + 4*16], xmm0
	movdqa	[esp + 5*16], xmm0
	movdqa	[esp + 6*16], xmm0

	inc	ctrCtx
	mov	CTR, ctrCtx
	bswap	CTR
	xor	CTR, [ctx + 3*4]
	mov	[esp + 1*16 + 3*4], CTR

	inc	ctrCtx
	mov	CTR, ctrCtx
	bswap	CTR
	xor	CTR, [ctx + 3*4]
	mov	[esp + 2*16 + 3*4], CTR

	inc	ctrCtx
	mov	CTR, ctrCtx
	bswap	CTR
	xor	CTR, [ctx + 3*4]
	mov	[esp + 3*16 + 3*4], CTR

	inc	ctrCtx
	mov	CTR, ctrCtx
	bswap	CTR
	xor	CTR, [ctx + 3*4]
	mov	[esp + 4*16 + 3*4], CTR

	inc	ctrCtx
	mov	CTR, ctrCtx
	bswap	CTR
	xor	CTR, [ctx + 3*4]
	mov	[esp + 5*16 + 3*4], CTR

	inc	ctrCtx
	mov	CTR, ctrCtx
	bswap	CTR
	xor	CTR, [ctx + 3*4]
	mov	[esp + 6*16 + 3*4], CTR


loop7:
	cmp	inputLen, 7*16
	jb	loop1

	movdqu	xmm0, [0*16 + esp]
	movdqu	xmm1, [1*16 + esp]
	movdqu	xmm2, [2*16 + esp]
	movdqu	xmm3, [3*16 + esp]
	movdqu	xmm4, [4*16 + esp]
	movdqu	xmm5, [5*16 + esp]
	movdqu	xmm6, [6*16 + esp]

	i = 1
	WHILE i LE 7
	    aes_rnd i

	    inc     ctrCtx
	    mov     CTR, ctrCtx
	    bswap   CTR
	    xor     CTR, [ctx + 3*4]
	    mov     [esp + (i-1)*16 + 3*4], CTR

	    i = i+1
	ENDM
	WHILE i LT rnds
	    aes_rnd i
	    i = i+1
	    ENDM
	aes_last_rnd rnds

	movdqu	xmm7, [0*16 + input]
	pxor	xmm0, xmm7
	movdqu	xmm7, [1*16 + input]
	pxor	xmm1, xmm7
	movdqu	xmm7, [2*16 + input]
	pxor	xmm2, xmm7
	movdqu	xmm7, [3*16 + input]
	pxor	xmm3, xmm7
	movdqu	xmm7, [4*16 + input]
	pxor	xmm4, xmm7
	movdqu	xmm7, [5*16 + input]
	pxor	xmm5, xmm7
	movdqu	xmm7, [6*16 + input]
	pxor	xmm6, xmm7

	movdqu	[0*16 + output], xmm0
	movdqu	[1*16 + output], xmm1
	movdqu	[2*16 + output], xmm2
	movdqu	[3*16 + output], xmm3
	movdqu	[4*16 + output], xmm4
	movdqu	[5*16 + output], xmm5
	movdqu	[6*16 + output], xmm6

	lea input, [7*16 + input]
	lea output, [7*16 + output]
	sub inputLen, 7*16
	jmp loop7


loop1:
	cmp	inputLen, 1*16
	jb	bail

	movdqu	xmm0, [esp]
	add	esp, 16

	i = 1
    WHILE i LT rnds
	    movdqu  xmm7, [i*16 + ctx]
	    aesenc  xmm0, xmm7
	    i = i+1
	ENDM
	movdqu	xmm7, [rnds*16 + ctx]
	aesenclast xmm0, xmm7

	movdqu	xmm7, [input]
	pxor	xmm0, xmm7
	movdqu	[output], xmm0

	lea input, [1*16 + input]
	lea output, [1*16 + output]
	sub inputLen, 1*16
	jmp loop1

bail:

	mov	ctrCtx, [ebp + 4*5 + 0*4]
	movdqu	xmm0, [esp]
	movdqu	xmm1, [ctx + 0*16]
	pxor	xmm0, xmm1
	movdqu	[8+ctrCtx], xmm0


	xor	eax, eax
	mov	esp, ebp
	pop	ebp
	pop	CTR
	pop	ctrCtx
	pop	inputLen
	ret
ENDM



align 16
intel_aes_encrypt_ctr_256:
    gen_aes_ctr_func  14
    ret


