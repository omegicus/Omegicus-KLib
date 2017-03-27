use32


align 8
AlphaBlend: ; 13056 ticks for 1000 quads on Athlon.
	mov	eax, [esp + 04] ;Source
	mov	edx, [esp + 08] ;Destination
	mov	ecx, [esp + 12] ;Count ; number of pixels/2
	dec	ecx
	pxor	mm7,mm7
	movq	mm0,[eax+ecx*8]
	movq	mm1,[edx+ecx*8]
@@:	movq	mm4,mm0
	movq	mm2,mm0
	psrlw	mm4,1
	movq	mm3,mm1
	movq	mm5,mm4
      punpcklbw mm0,mm7
      punpcklbw mm1,mm7
      punpckhbw mm2,mm7
      punpckhbw mm3,mm7
      psubsw	mm0,mm1
      psubsw	mm2,mm3
      punpcklwd mm4,mm4
      punpckhwd mm5,mm5
      punpckhdq mm4,mm4
      punpckhdq mm5,mm5
      psllw	mm0,1
      psllw	mm2,1
      pmulhw	mm0,mm4
      pmulhw	mm2,mm5
      paddsw	mm0,mm1
      paddsw	mm2,mm3
      packuswb	mm0,mm2
	movq	[edx+ecx*8],mm0
	dec	ecx
	movq	mm0,[eax+ecx*8]
	movq	mm1,[edx+ecx*8]
	jns	@b
ret 4*3






Resample proc uses edi esi ebx lpSrc:DWORD, lpDest:DWORD, dwWidth:DWORD, dwHeight:DWORD
    mov     esi, lpSrc
    mov     edi, lpDest
    mov     edx, dwWidth
    shr     edx, 1

    mov     ecx, dwWidth

    pxor    MM4, MM4

    mov     ebx, dwHeight
    shr     ebx, 1
    ALIGN 16
    @nextline:

	mov	edx, ecx
	shr	edx, 2
	ALIGN 16
	@nextpixel:
	    movq	MM0, [esi]	    ; MM0: X2 R2 G2 B2-X1 R1 G1 B1
	    movq	MM1, [esi+8]	    ; MM1: X4 R4 G4 B4-X3 R3 G3 B3
	    movq	MM2, [esi+4*ecx]	; MM2: X6 R6 G6 B6-X5 R5 G5 B5
	    movq	MM3, [esi+4*ecx+8]  ; MM3: X8 R8 G8 B8-X7 R7 G7 B7

	    movq	MM5, MM0
	    movq	MM6, MM2

	    punpckhbw	MM0, MM4	    ; MM0: 00 X2 00 R2-00 G2 00 B2
	    punpckhbw	MM2, MM4	    ; MM2: 00 X6 00 R6-00 B6 00 B6
	    punpcklbw	MM5, MM4	    ; MM5: 00 X1 00 R1-00 G1 00 B1
	    punpcklbw	MM6, MM4	    ; MM6: 00 X5 00 R5-00 G5 00 B5

	    paddw	MM0, MM2
	    paddw	MM0, MM5
	    paddw	MM0, MM6
	    psrlw	MM0, 2

	    ; second pixel:

	    movq	MM5, MM1
	    movq	MM6, MM3

	    punpckhbw	MM1, MM4	    ; MM1: 00 X4 00 R4-00 G4 00 B4
	    punpckhbw	MM3, MM4	    ; MM3: 00 X8 00 R8-00 B8 00 B8
	    punpcklbw	MM5, MM4	    ; MM5: 00 X3 00 R3-00 G3 00 B3
	    punpcklbw	MM6, MM4	    ; MM6: 00 X7 00 R7-00 G7 00 B7

	    paddw	MM1, MM3
	    paddw	MM1, MM5
	    paddw	MM1, MM6
	    psrlw	MM1, 2

	    ; now:  MM0: 00 XQ 00 RQ-00 GQ 00 BQ ;where Q is the first mixed pixel
	    ; now:  MM1: 00 XP 00 RP-00 GP 00 BP ;where P is the second mixed pixel
	    ; output should be: BQ GQ RQ XQ-BP GP RP XP (in mem)
	    ;			XP RP GP BP XQ RQ GQ BQ (in reg)

	    packuswb	MM0, MM1
	    movq	[edi], MM0


	    add     esi, 16
	    add     edi, 8
	    dec     edx
	    jnz     @nextpixel

	lea	esi, [esi+4*ecx]
	dec	ebx
	jnz	@nextline
    emms
ret
Resample endp