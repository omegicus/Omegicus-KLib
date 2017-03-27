; Performance for a PII/PIII Processor at 200MHz reference clock speed:
; 256 Bit:   Key Setup: 1468/1468 cycles
; Encrypt:   1134 cycles = 45.1 mbits/sec
; Decrypt:   1122 cycles = 45.6 mbits/sec


; The MMX register mm7 has all its bits set in the following macros so that
; 'xor mmr,mm7' is a negate operation on mmr.

use32



macro  sb0     a1, a2, a3, a4, a5
{
	pxor	a4,a1
	movq	a5,a2
	pand	a2,a4
	pxor	a5,a3
	pxor	a2,a1
	por	a1,a4
	pxor	a1,a5
	pxor	a5,a4
	pxor	a4,a3
	por	a3,a2
	pxor	a3,a5
	pxor	a5,mm7
	por	a5,a2
	pxor	a2,a4
	pxor	a2,a5
	por	a4,a1
	pxor	a2,a4
	pxor	a5,a4
}

macro  ib0     a1, a2, a3, a4, a5   {
	pxor	a3,mm7
	movq	a5,a2
	por	a2,a1
	pxor	a5,mm7
	pxor	a2,a3
	por	a3,a5
	pxor	a2,a4
	pxor	a1,a5
	pxor	a3,a1
	pand	a1,a4
	pxor	a5,a1
	por	a1,a2
	pxor	a1,a3
	pxor	a4,a5
	pxor	a3,a2
	pxor	a4,a1
	pxor	a4,a2
	pand	a3,a4
	pxor	a5,a3
}

macro  sb1     a1, a2, a3, a4, a5   {
	pxor	a1,mm7
	pxor	a3,mm7
	movq	a5,a1
	pand	a1,a2
	pxor	a3,a1
	por	a1,a4
	pxor	a4,a3
	pxor	a2,a1
	pxor	a1,a5
	por	a5,a2
	pxor	a2,a4
	por	a3,a1
	pand	a3,a5
	pxor	a1,a2
	pand	a2,a3
	pxor	a2,a1
	pand	a1,a3
	pxor	a1,a5
}

macro  ib1     a1, a2, a3, a4, a5    {
	movq	a5,a2
	pxor	a2,a4
	pand	a4,a2
	pxor	a5,a3
	pxor	a4,a1
	por	a1,a2
	pxor	a3,a4
	pxor	a1,a5
	por	a1,a3
	pxor	a2,a4
	pxor	a1,a2
	por	a2,a4
	pxor	a2,a1
	pxor	a5,mm7
	pxor	a5,a2
	por	a2,a1
	pxor	a2,a1
	por	a2,a5
	pxor	a4,a2
}

macro  sb2     a1, a2, a3, a4, a5   {
	movq	a5,a1
	pand	a1,a3
	pxor	a1,a4
	pxor	a3,a2
	pxor	a3,a1
	por	a4,a5
	pxor	a4,a2
	pxor	a5,a3
	movq	a2,a4
	por	a4,a5
	pxor	a4,a1
	pand	a1,a2
	pxor	a5,a1
	pxor	a2,a4
	pxor	a2,a5
	pxor	a5,mm7
}

macro  ib2     a1, a2, a3, a4, a5  {
	pxor	a3,a4
	pxor	a4,a1
	movq	a5,a4
	pand	a4,a3
	pxor	a4,a2
	por	a2,a3
	pxor	a2,a5
	pand	a5,a4
	pxor	a3,a4
	pand	a5,a1
	pxor	a5,a3
	pand	a3,a2
	por	a3,a1
	pxor	a4,mm7
	pxor	a3,a4
	pxor	a1,a4
	pand	a1,a2
	pxor	a4,a5
	pxor	a4,a1
}

macro  sb3     a1, a2, a3, a4, a5   {
	movq	a5,a1
	por	a1,a4
	pxor	a4,a2
	pand	a2,a5
	pxor	a5,a3
	pxor	a3,a4
	pand	a4,a1
	por	a5,a2
	pxor	a4,a5
	pxor	a1,a2
	pand	a5,a1
	pxor	a2,a4
	pxor	a5,a3
	por	a2,a1
	pxor	a2,a3
	pxor	a1,a4
	movq	a3,a2
	por	a2,a4
	pxor	a2,a1
}

macro  ib3     a1, a2, a3, a4, a5  {
	movq	a5,a3
	pxor	a3,a2
	pand	a2,a3
	pxor	a2,a1
	pand	a1,a5
	pxor	a5,a4
	por	a4,a2
	pxor	a4,a3
	pxor	a1,a5
	pxor	a3,a1
	por	a1,a4
	pxor	a1,a2
	pxor	a5,a3
	pand	a3,a4
	por	a2,a4
	pxor	a2,a3
	pxor	a5,a1
	pxor	a3,a5
}

macro  sb4     a1, a2, a3, a4, a5   {
	pxor	a2,a4
	pxor	a4,mm7
	pxor	a3,a4
	pxor	a4,a1
	movq	a5,a2
	pand	a2,a4
	pxor	a2,a3
	pxor	a5,a4
	pxor	a1,a5
	pand	a3,a5
	pxor	a3,a1
	pand	a1,a2
	pxor	a4,a1
	por	a5,a2
	pxor	a5,a1
	por	a1,a4
	pxor	a1,a3
	pand	a3,a4
	pxor	a1,mm7
	pxor	a5,a3
}

macro  ib4     a1, a2, a3, a4, a5  {
	movq	a5,a3
	pand	a3,a4
	pxor	a3,a2
	por	a2,a4
	pand	a2,a1
	pxor	a5,a3
	pxor	a5,a2
	pand	a2,a3
	pxor	a1,mm7
	pxor	a4,a5
	pxor	a2,a4
	pand	a4,a1
	pxor	a4,a3
	pxor	a1,a2
	pand	a3,a1
	pxor	a4,a1
	pxor	a3,a5
	por	a3,a4
	pxor	a4,a1
	pxor	a3,a2
}

macro  sb5     a1, a2, a3, a4, a5    {
	pxor	a1,a2
	pxor	a2,a4
	pxor	a4,mm7
	movq	a5,a2
	pand	a2,a1
	pxor	a3,a4
	pxor	a2,a3
	por	a3,a5
	pxor	a5,a4
	pand	a4,a2
	pxor	a4,a1
	pxor	a5,a2
	pxor	a5,a3
	pxor	a3,a1
	pand	a1,a4
	pxor	a3,mm7
	pxor	a1,a5
	por	a5,a4
	pxor	a3,a5
}

macro  ib5     a1, a2, a3, a4, a5  {
	pxor	a2,mm7
	movq	a5,a4
	pxor	a3,a2
	por	a4,a1
	pxor	a4,a3
	por	a3,a2
	pand	a3,a1
	pxor	a5,a4
	pxor	a3,a5
	por	a5,a1
	pxor	a5,a2
	pand	a2,a3
	pxor	a2,a4
	pxor	a5,a3
	pand	a4,a5
	pxor	a5,a2
	pxor	a4,a1
	pxor	a4,a5
	pxor	a5,mm7
}

macro  sb6     a1, a2, a3, a4, a5   {
	pxor	a3,mm7
	movq	a5,a4
	pand	a4,a1
	pxor	a1,a5
	pxor	a4,a3
	por	a3,a5
	pxor	a2,a4
	pxor	a3,a1
	por	a1,a2
	pxor	a3,a2
	pxor	a5,a1
	por	a1,a4
	pxor	a1,a3
	pxor	a5,a4
	pxor	a5,a1
	pxor	a4,mm7
	pand	a3,a5
	pxor	a3,a4
}

macro  ib6     a1, a2, a3, a4, a5   {
	pxor	a1,a3
	movq	a5,a3
	pand	a3,a1
	pxor	a5,a4
	pxor	a3,mm7
	pxor	a4,a2
	pxor	a3,a4
	por	a5,a1
	pxor	a1,a3
	pxor	a4,a5
	pxor	a5,a2
	pand	a2,a4
	pxor	a2,a1
	pxor	a1,a4
	por	a1,a3
	pxor	a4,a2
	pxor	a5,a1
}

macro  sb7     a1, a2, a3, a4, a5   {
	movq	a5,a3
	pand	a3,a2
	pxor	a3,a4
	pand	a4,a2
	pxor	a5,a3
	pxor	a3,a2
	pxor	a2,a1
	por	a1,a5
	pxor	a1,a3
	pxor	a4,a2
	pxor	a3,a4
	pand	a4,a1
	pxor	a4,a5
	pxor	a5,a3
	pand	a3,a1
	pxor	a5,mm7
	pxor	a3,a5
	pand	a5,a1
	pxor	a2,a4
	pxor	a5,a2
}

macro  ib7     a1, a2, a3, a4, a5  {
	movq	a5,a3
	pxor	a3,a1
	pand	a1,a4
	pxor	a3,mm7
	por	a5,a4
	pxor	a4,a2
	por	a2,a1
	pxor	a1,a3
	pand	a3,a5
	pxor	a2,a3
	pxor	a3,a1
	por	a1,a3
	pand	a4,a5
	pxor	a1,a4
	pxor	a5,a2
	pxor	a4,a5
	por	a5,a1
	pxor	a4,a3
	pxor	a5,a3
}

macro  f_key   a1, a2, a3, a4	{
	movd	a1,[esi   ]
	movd	a2,[esi+ 4]
	movd	a3,[esi+ 8]
	movd	a4,[esi+12]
	movq	mm6,a1
	psllq	mm6,32	    
	por	a1,mm6
	movq	mm6,a2
	psllq	mm6,32	    
	por	a2,mm6
	movq	mm6,a3
	psllq	mm6,32	    
	por	a3,mm6
	movq	mm6,a4
	psllq	mm6,32	    
	por	a4,mm6
	add	esi,16	    
}

macro  t_key   a1, a2, a3, a4  {
	movq	[edi   ],a1
	movq	[edi+ 8],a2
	movq	[edi+16],a3
	movq	[edi+24],a4
	add	edi,32	    
}

macro  rot     a1, a2, a3, a4, a5  {
	movq	a5,a1
	pslld	a1,13
	psrld	a5,19
	por	a1,a5
	movq	a5,a3
	pslld	a3,3
	psrld	a5,29
	por	a3,a5
	movq	a5,a1
	pslld	a5,3
	pxor	a4,a3
	pxor	a4,a5
	pxor	a2,a1
	pxor	a2,a3
	movq	a5,a2
	pslld	a2,1
	psrld	a5,31
	por	a2,a5
	movq	a5,a4
	pslld	a4,7
	psrld	a5,25
	por	a4,a5
	movq	a5,a2
	pxor	a1,a2
	pxor	a1,a4
	pslld	a5,7
	pxor	a3,a4
	pxor	a3,a5
	movq	a5,a1
	pslld	a1,5
	psrld	a5,27
	por	a1,a5
	movq	a5,a3
	pslld	a3,22
	psrld	a5,10
	por	a3,a5
}

macro  irot    a1, a2, a3, a4, a5  {
	movq	a5,a3
	psrld	a3,22
	pslld	a5,10
	por	a3,a5
	movq	a5,a1
	psrld	a1,5
	pslld	a5,27
	por	a1,a5
	movq	a5,a2
	pslld	a5,7
	pxor	a3,a4
	pxor	a3,a5
	pxor	a1,a2
	pxor	a1,a4
	movq	a5,a4
	psrld	a4,7
	pslld	a5,25
	por	a4,a5
	movq	a5,a2
	psrld	a2,1
	pslld	a5,31
	por	a2,a5
	movq	a5,a1
	pslld	a5,3
	pxor	a4,a3
	pxor	a4,a5
	pxor	a2,a1
	pxor	a2,a3
	movq	a5,a3
	psrld	a3,3
	pslld	a5,29
	por	a3,a5
	movq	a5,a1
	psrld	a1,13
	pslld	a5,19
	por	a1,a5
}























macro  k_loop  a1, a2, a3, a4, a5  {
    f_key   a1,a2,a3,a4
    sb3     a1,a2,a3,a4,a5
    t_key   a2,a3,a4,a5
    f_key   a1,a2,a3,a4
    sb2     a1,a2,a3,a4,a5
    t_key   a3,a4,a2,a5
    f_key   a1,a2,a3,a4
    sb1     a1,a2,a3,a4,a5
    t_key   a3,a1,a4,a2
    f_key   a1,a2,a3,a4
    sb0     a1,a2,a3,a4,a5
    t_key   a2,a5,a3,a1
    f_key   a1,a2,a3,a4
    sb7     a1,a2,a3,a4,a5
    t_key   a3,a5,a4,a1
    f_key   a1,a2,a3,a4
    sb6     a1,a2,a3,a4,a5
    t_key   a1,a2,a5,a3
    f_key   a1,a2,a3,a4
    sb5     a1,a2,a3,a4,a5
    t_key   a2,a4,a1,a3
    f_key   a1,a2,a3,a4
    sb4     a1,a2,a3,a4,a5
    t_key   a2,a5,a1,a4
}

macro  f_xor   a1, a2, a3, a4 {
	pxor	a1, [esi +00]
	pxor	a2, [esi +08]
	pxor	a3, [esi +16]
	pxor	a4, [esi +24]
	add	esi,32	    
}

macro  f_loop  a1, a2, a3, a4, a5 {
    f_xor   a1,a2,a3,a4
    sb0     a1,a2,a3,a4,a5
    rot     a2,a5,a3,a1,a4
    f_xor   a2,a5,a3,a1
    sb1     a2,a5,a3,a1,a4
    rot     a3,a2,a1,a5,a4
    f_xor   a3,a2,a1,a5
    sb2     a3,a2,a1,a5,a4
    rot     a1,a5,a2,a4,a3
    f_xor   a1,a5,a2,a4
    sb3     a1,a5,a2,a4,a3
    rot     a5,a2,a4,a3,a1
    f_xor   a5,a2,a4,a3
    sb4     a5,a2,a4,a3,a1
    rot     a2,a1,a5,a3,a4
    f_xor   a2,a1,a5,a3
    sb5     a2,a1,a5,a3,a4
    rot     a1,a3,a2,a5,a4
    f_xor   a1,a3,a2,a5
    sb6     a1,a3,a2,a5,a4
    rot     a1,a3,a4,a2,a5
    f_xor   a1,a3,a4,a2
    sb7     a1,a3,a4,a2,a5
}


macro  i_xor   a1, a2, a3, a4  {
	sub	esi,32	    
	pxor	a1, [esi+00]
	pxor	a2, [esi+08]
	pxor	a3, [esi+16]
	pxor	a4, [esi+24]
}

macro  i_loop  a1, a2, a3, a4, a5  {
    ib7     a1,a2,a3,a4,a5
    i_xor   a4,a1,a2,a5
    irot    a4,a1,a2,a5,a3
    ib6     a4,a1,a2,a5,a3
    i_xor   a1,a2,a3,a5
    irot    a1,a2,a3,a5,a4
    ib5     a1,a2,a3,a5,a4
    i_xor   a2,a4,a5,a3
    irot    a2,a4,a5,a3,a1
    ib4     a2,a4,a5,a3,a1
    i_xor   a2,a3,a5,a1
    irot    a2,a3,a5,a1,a4
    ib3     a2,a3,a5,a1,a4
    i_xor   a1,a2,a5,a3
    irot    a1,a2,a5,a3,a4
    ib2     a1,a2,a5,a3,a4
    i_xor   a2,a4,a5,a3
    irot    a2,a4,a5,a3,a1
    ib1     a2,a4,a5,a3,a1
    i_xor   a1,a2,a3,a5
    irot    a1,a2,a3,a5,a4
    ib0     a1,a2,a3,a5,a4
    i_xor   a1,a4,a2,a5
}










SERPENTMMX_set_key: ; ley_len = ecx, esi = in_key
	push	ebx
	push	esi
	push	edi
	mov	edx, ecx
	cmp	edx, 256
	ja	near .l3
	mov	ebx, SERPENT_l_key
	;mov     esi, [esp + in_key]
	xor	eax,eax
	mov	ecx,edx
	add	ecx,31
	sar	ecx,5
	je	short .l0
	mov	edi,ebx
	mov	eax,ecx
	rep	movsd
.l0:	cmp	edx,256
	je	short .l1
	mov	ecx,8
	sub	ecx,eax
	xor	eax,eax
	rep	stosd
	mov	ecx,edx
	mov	edx,1
	sal	edx,cl
	sar	ecx,5
	mov	eax,edx
	dec	eax
	and	eax,[ebx+4*ecx]
	or	eax,edx
	mov	[ebx+4*ecx],eax
.l1:	mov	esi,ebx
	mov	eax,0x9e3779b9
	mov	ebx,eax
	xor	eax,[esi+ 8]
	xor	ebx,[esi+12]
	xor	eax,[esi+16]
	xor	ebx,[esi+20]
	xor	eax,[esi+24]
	xor	ebx,[esi+28]
	xor	ecx,ecx
	push	esi
.l2:	mov	edx,[esi]
	xor	edx,ecx
	xor	edx,ebx
	ror	edx,21
	mov	[esi+32],edx
	xor	eax, [esi+8]
	xor	eax, edx
	add	esi, 4
	inc	ecx
	mov	edx, [esi]
	xor	edx, ecx
	xor	edx, eax
	ror	edx, 21
	mov	[esi+32],edx
	xor	ebx, [esi+8]
	xor	ebx, edx
	add	esi, 4
	inc	ecx
	cmp	ecx, 132
	jne    .l2
	pop	esi
	add	esi, 4*8
	lea	edi, [SERPENT_m_key + 8*8]
	xor	eax, eax
	dec	eax
	movd	mm7, eax
	movd	mm6, eax
	psllq	mm6, 32
	por	mm7, mm6
    
	k_loop	mm0,mm1,mm2,mm3,mm4
	k_loop	mm0,mm1,mm2,mm3,mm4 
	k_loop	mm0,mm1,mm2,mm3,mm4 
	k_loop	mm0,mm1,mm2,mm3,mm4 
	f_key	mm0,mm1,mm2,mm3
	sb3	mm0,mm1,mm2,mm3,mm4 
	t_key	mm1,mm2,mm3,mm4
	emms
.l3:	pop	edi
	pop	esi
	pop	ebx
ret

SERPENTMMX_encrypt: ; esi=in_blk, edi=out_blk
	push	    esi
	;
	movq	    mm0, [esi+00]
	movq	    mm2, [esi+08]
	movq	    mm4, [esi+16]
	movq	    mm5, [esi+24]
	movq	    mm1, mm0
	movq	    mm3, mm2
	punpckldq   mm0, mm4
	punpckhdq   mm1, mm4
	punpckldq   mm2, mm5
	punpckhdq   mm3, mm5
	;
	lea	    esi, [SERPENT_m_key + 8*8]
	pxor	    mm7, mm7
	pcmpeqd     mm7, mm7
	;
	push	    edi
	;
	f_loop	    mm0, mm1, mm2, mm3, mm4
	rot	    mm3, mm4, mm1, mm0, mm2
	f_loop	    mm3, mm4, mm1, mm0, mm2
	rot	    mm0, mm2, mm4, mm3, mm1
	f_loop	    mm0, mm2, mm4, mm3, mm1
	rot	    mm3, mm1, mm2, mm0, mm4
	f_loop	    mm3, mm1, mm2, mm0, mm4
	f_xor	    mm0, mm4, mm1, mm3
	;
	pop	    esi
	;mov         esi, [esp + out_blk]
	movq	    mm2, mm0
	movq	    mm5, mm1
	punpckldq   mm0, mm4
	punpckldq   mm1, mm3
	punpckhdq   mm4, mm2
	punpckhdq   mm3, mm5
	pshufw	    mm4, mm4, 0x4E
	pshufw	    mm3, mm3, 0x4E
	movq	    [esi+00], mm0
	movq	    [esi+08], mm1
	movq	    [esi+16], mm4
	movq	    [esi+24], mm3
	emms
	pop	    esi
ret

SERPENTMMX_decrypt:  ; esi=in_blk, edi=out_blk
	push	    esi
	;
	movq	    mm0, [esi+00]
	movq	    mm2, [esi+08]
	movq	    mm4, [esi+16]
	movq	    mm5, [esi+24]
	movq	    mm1, mm0
	movq	    mm3, mm2
	punpckldq   mm0, mm4
	punpckhdq   mm1, mm4
	punpckldq   mm2, mm5
	punpckhdq   mm3, mm5
	;
	lea	    esi, [SERPENT_m_key + 8*140]
	pxor	    mm7, mm7
	pcmpeqd     mm7, mm7
	;
	push	    edi
	;
	i_xor	    mm0, mm1, mm2, mm3
	i_loop	    mm0, mm1, mm2, mm3, mm4
	irot	    mm0, mm3, mm1, mm4, mm2
	i_loop	    mm0, mm3, mm1, mm4, mm2
	irot	    mm0, mm4, mm3, mm2, mm1
	i_loop	    mm0, mm4, mm3, mm2, mm1
	irot	    mm0, mm2, mm4, mm1, mm3
	i_loop	    mm0, mm2, mm4, mm1, mm3
	;
	pop	    esi
	;mov         esi,[esp + out_blk]
	movq	    mm4,mm0
	movq	    mm5,mm2
	punpckldq   mm0,mm1
	punpckldq   mm2,mm3
	punpckhdq   mm1,mm4
	punpckhdq   mm3,mm5
	pshufw	    mm1,mm1,0x4e
	pshufw	    mm3,mm3,0x4e
	movq	    [esi+00], mm0
	movq	    [esi+08], mm1
	movq	    [esi+16], mm4
	movq	    [esi+24], mm3
	emms
	pop	    esi
ret





;include 'lib_serpent_data.inc'

