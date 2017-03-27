use32

rf9	dq	2.7557319223985890651862166557528e-6
rf7	dq	0.0001984126984126984126984126984127
rf5	dq	0.0083333333333333333333333333333333
rf3	dq	0.16666666666666666666666666666667
rf8	dq	2.4801587301587301587301587301587e-5
rf6	dq	0.0013888888888888888888888888888889
rf4	dq	0.041666666666666666666666666666667
rf2	dq	0.5

one	dq	1.0
mathx	dq	0.0


align 4
cesario_sin: ; x
	fld	[mathx] ; x
	fmul	st0,st0 ; x^2
	fld	[rf9]	; 1/9! x^2
	fmul	st0,st1 ; x^2/9! x^2
	fsub	[rf7]	; (-1/7! + x^2/9!) x^2
	fmul	st0,st1 ; (-x^2/7! + x^4/9!) x^2
	fadd	[rf5]	; (1/5! - x^2/7! + x^4/9!) x^2
	fmul	st0,st1 ; (x^2/5! - x^4/7! + x^6/9!) x^2
	fsub	[rf3]	; (-1/3! + x^2/5! - x^4/7! + x^6/9!) x^2
	fmulp	st1,st0 ; (-x^2/3! + x^4/5! - x^6/7! + x^8/9!) x^2
	fmul	[mathx] ; (-x^3/3! + x^5/5! - x^7/7! + x^9/9!)
	fadd	[mathx] ; (x - x^3/3! + x^5/5! - x^7/7! + x^9/9!)
	fstp	[mathx] ;
	ret	4*1

align 4
cesario_cos: ; x
	fld	[mathx] ; x
	fmul	st0,st0 ; x^2
	fld	[rf8]	; (1/8!) x^2
	fmul	st0,st1 ; (x^2/8!) x^2
	fsub	[rf6]	; (-1/6! + x^2/8!) x^2
	fmul	st0,st1 ; (-x^2/6! + x^4/8!) x^2
	fadd	[rf4]	; (1/4! - x^2/6! + x^4/8!) x^2
	fmul	st0,st1 ; (x^2/4! - x^4/6! + x^6/8!) x^2
	fsub	[rf2]	; (-1/2 + x^2/4! - x^4/6! + x^6/8!) x^2
	fmulp	st1,st0 ; (-x^2/2 + x^4/4! - x^6/6! + x^8/8!)
	fadd	[one]	; (1 - x^2/2 + x^4/4! - x^6/6! + x^8/8!)
	fstp	[mathx] ;
	ret	4*1



; Input - eax, Output - eax
align 4
cesario_abs:
	cdq
	xor eax,edx
	sub eax,edx
	ret

align 4
cesario_rand32: ; <- input seed in EAX (not needed, just for fun)
	push ecx
	push edx
	;
	rdtsc
	;mov eax, NaNRand
	mov edx,0
	mov ecx, 127773 ;q
	div ecx      ; eax == floor( seed / q)
		     ; edx == remainder
	xchg eax, edx
	push edx
	mov ecx, 16807
	mul ecx      ; eax = mul of remainder * a
	pop edx      ; edx == floor of seed/q
  
	xchg eax, edx
	push edx
	mov ecx, 2836
	mul ecx
	pop edx      ; edx == mull of rem * a
		     ; eax == mull of seed/q * r
	sub edx, eax
	mov eax, edx
	;mov NaNRand, eax ; save next seed
	mov ecx, 16357
	mov edx, 0
	div ecx
	mov eax, edx
	pop edx
	pop ecx
ret ; -> rand32 in EAX


align 4
cesarion_fpower:
       ; fld	 [eax]		 ; power
       ; fld	 [ebx]	     ; x
       ; fyl2x
       ; fld1
       ; fld	 st1
       ; fprem
       ; f2xm1
       ; fadd
       ; fscale
       ; fxch	 st1
       ; fstp	 st
       ; ; st0 = X^power
ret

; Integer Square Root:
align 4
cesario_math_isqrt: ; < eax
	;mov eax, num
	xor ebx, ebx
	bsr ecx, eax
	and cl, 0xFE
	mov edx, 1
	shl edx, cl
  .ref: mov esi, ebx
	add esi, edx
	cmp esi, eax
	ja @f
	sub eax, esi
	shr ebx, 1
	add ebx, edx
	jmp .nxt
    @@: shr ebx, 1
  .nxt: shr edx, 2
	jnz .ref
	mov eax, ebx
ret ; > eax









; [ A B C D ]
; [ E F G H ]  X  [ W X Y Z ]  =  [ AW+BX+CY+DZ EW+FX+GY+HZ IW+JX+KY+LZ  ? ]
; [ I J K L ]

; 16 bit numbers are scaled to a fixed point size of:
; 1.111 1111 1111 1111 ; first bit is sign bit
NUMBER_SCALE EQU 15 ; 1 / 2^15
cesario_matrix_mmtransform:
    pMatrix EQU [esp + 08] ; 4x3 transform matrix pointer
    pVector EQU [esp + 12] ; source vectors pointer
    iNumVec EQU [esp + 16] ; number of vectors to transform
    pResult EQU [esp + 20] ; destination for transformed vectors

    mov ecx,iNumVec
    mov eax,pMatrix
    lea edx,[ecx*8] ; size of source/dest vector buffer
    neg ecx

; load entire 3x4 matrix
    movq mm0,[eax +  0]
    movq mm1,[eax +  8]
    movq mm2,[eax + 16]

    mov eax,edx
    add edx,pVector
    add eax,pResult
 .NextVect:
    ; Load vector (4 16-bit elements) into reg
    movq    mm3,[edx + ecx*8]
    inc ecx

    movq    mm4,mm3	;copy to other regs for use by 3 pmadds
    pmaddwd mm3,mm0	;multiply row0 X vector

    movq    mm5,mm4
    pmaddwd mm4,mm1	;multiply row1 X vector

    movq    mm6,mm3	; A1 A2
    pmaddwd mm5,mm2	;multiply row2 X vector

    punpckldq mm3,mm4	; B2 A2
    punpckhdq mm6,mm4	; B1 A1

    movq    mm4,mm5	;add row2 high and low order 32-bit results
    punpckhdq mm5,mm5	;   psrlq   mm5,32

    paddd   mm3,mm6	; B1+B2 A1+A2
    paddd   mm5,mm4

    psrad   mm3,NUMBER_SCALE-2
    psrad   mm5,NUMBER_SCALE-2

    packssdw mm3,mm5	; pack dwords into words
; might need to mask off high word of MMX reg?
    movq    [eax + ecx*8 - 8],mm3 ; store resulting vector

    jnz .NextVect    ;then loop back to do the next one.

    emms	    ;end MMX state (remove fp reg valids)
ret