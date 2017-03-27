;Copyright (C) 1999 Cecchinel Stephan <inter.zone@free.fr>
;RC6 algo mainly consists of 3 functions:
;	setkey in_key, length		->>set crypto key of length bits (256)
;	encrypt in_block,out_block	->>encrypt 16 byte block
;	decrypt in_block,out_block	->>decrypt 16 byte block

use32


align 4
crypt_RC6_clear:
      push  eax edi ecx
      mov   edi, rc6_memvars
      mov   ecx, rc6_memvars_sz/4
      xor   eax, eax
      cld
      rep   stosd
      pop   ecx edi eax
ret

align 4
crypt_RC6_Init:
; ESI: in_key 256 bit
       ; a equ dword[ebp-04]		      ;local vars on the stack
       ; b equ dword[ebp-08]
       ; i equ dword[ebp-12]
       ; j equ dword[ebp-16]
	pushad
	MOV	EDX, 256		; KEY LEN = 256 BITS
	sub	esp, 16;byte 16
	mov	edi,rc6_l_key
	mov	dword[edi],0x0B7E15163
	xor	ecx,ecx
	inc	ecx
       .cpy:
	mov	eax,dword [-4+edi+ecx*4]
	add	eax,0x09E3779B9
	mov	dword [edi+ecx*4],eax
	inc	ecx
	cmp	ecx, 44;byte 44
	jb	.cpy
	xor	ecx,ecx
	shr	edx,5
       .cpy1:
	mov	eax, dword[esi+ecx*4]
	mov	dword[edi+ecx*4],eax
	inc	ecx
	cmp	ecx,edx
	jb     .cpy1
	mov	esi,rc6_ll		    ;esi=ll
	dec	edx			;edx=t=(key_len/32)-1
	xor	ecx,ecx 		;ecx=k=0
	mov	dword[rc6_a],ecx
	mov	dword[rc6_b],ecx
	mov	dword[rc6_i],ecx
	mov	dword[rc6_j],ecx		   ;a=b=i=j=0
       .cpy2:
	push	ecx
	mov	ebx,dword[rc6_i]
	mov	eax,dword[edi+ebx*4]
	add	eax,dword[rc6_a]
	add	eax,dword[rc6_b]
	rol	eax,3
	add	dword[rc6_b],eax		   ;b+=a
	mov	dword[rc6_a],eax		   ;a=rol(l_key[rc6_i]+a+b
	mov	ebx,dword[rc6_j]
	mov	eax,dword [esi+ebx*4]
	add	eax,dword[rc6_b]
	mov	ecx,dword[rc6_b]
	rol	eax,cl
	mov	dword[rc6_b],eax		   ;b=rol(ll[rc6_j]+b,b)
	mov	eax,dword[rc6_a]
	mov	ebx,dword[rc6_i]
	mov	[edi+ebx*4],eax 	;l_key[rc6_i]=a
	mov	eax,dword[rc6_b]
	mov	ebx,dword[rc6_j]
	mov	[esi+ebx*4],eax 	;ll[rc6_j]=b
	mov	eax,dword[rc6_i]
	inc	eax
	cmp	eax, 43;byte 43
	jnz	.s1
	xor	eax,eax
       .s1:
	mov	dword[rc6_i],eax		   ;i=i+1 %43
	mov	eax,dword[rc6_j]
	inc	eax
	cmp	eax,edx
	jnz	.s2
	xor	eax,eax
       .s2:
	mov	dword[rc6_j],eax
	pop	ecx
	inc	ecx
	cmp	ecx,132
	jb	.cpy2
	mov	eax,edi
	add	esp,16;byte 16
	popad
ret


; input:	esi = block
align 4
crypt_RC6_Encrypt:
	pushad
	mov	edi, esi
	push	esi
	mov	esi,rc6_l_key
	mov	eax,[edi+00]		   ;a=in_block[0]
	mov	ebx,[edi+04]
	add	ebx,[esi+00]		   ;b=in_block[1]+l_key[0]
	mov	ecx,[edi+08]		 ;c=in_block[2]
	mov	edx,[edi+12]
	add	edx,[esi+04]		 ;d=in_block[3]+l_key[1]
	lea	ebp,[esi+08]
       .boucle:
	lea	esi,[edx+edx+1]
	imul	esi,edx
	rol	esi,5			;u=rol(d*(d+d+1),5)
	lea	edi,[ebx+ebx+1]
	imul	edi,ebx
	rol	edi,5			;t=rol(b*(b+b+1),5)
	push	ecx
	mov	ecx,esi
	xor	eax,edi
	rol	eax,cl
	add	eax,[ebp]		;a=rol(a^t,u)+l_key[rc6_i]
	pop	ecx
	push	eax
	xchg	ecx,eax
	mov	ecx,edi
	xor	eax,esi
	rol	eax,cl
	add	eax,[ebp+4]		;c=rol(c^u,t)+l_key[i+1]
	xchg	ecx,eax
	pop	eax
	push	eax
	mov	eax,ebx
	mov	ebx,ecx
	mov	ecx,edx
	pop	edx
	add	ebp,8
	cmp	ebp,(rc6_l_key+(42*4))
	jnz	.boucle
	pop	edi
	mov	esi,rc6_l_key
	add	eax,[esi+(42*4)]
	mov	[edi],eax
	mov	[edi+4],ebx
	add	ecx,[esi+(43*4)]
	mov	[edi+8],ecx
	mov	[edi+12],edx
	popad
ret

align 4
crypt_RC6_Decrypt: ; ESI: block
	pusha
	mov	edi, esi
	push	esi
	mov	esi,rc6_l_key
	mov	edx,[edi+12]		;d=in_blk[3]
	mov	ecx,[edi+8]
	sub	ecx,[esi+(43*4)]	;c=in_blk[2]-l_key[43]
	mov	ebx,[edi+4]		;b=in_blk[1]
	mov	eax,[edi]
	sub	eax,[esi+(42*4)]	;a=in_blk[0]-l_key[42]
	lea	ebp,[esi+(40*4)]
       .boucle2:
	push	edx
	mov	edx,ecx
	mov	ecx,ebx
	mov	ebx,eax
	pop	eax
	lea	esi,[edx+edx+1]
	imul	esi,edx
	rol	esi,5			;u=rol(d*(d+d+1),5)
	lea	edi,[ebx+ebx+1]
	imul	edi,ebx
	rol	edi,5			;t=rol(b*(b+b+1),5)
	push	eax
	xchg	ecx,eax
	mov	ecx,edi
	sub	eax,[ebp+4]
	ror	eax,cl
	xor	eax,esi
	xchg	ecx,eax
	pop	eax
	push	ecx
	mov	ecx,esi
	sub	eax,[ebp]
	ror	eax,cl
	xor	eax,edi
	pop	ecx
	sub	ebp,8
	cmp	ebp,rc6_l_key
	jnz	.boucle2
	mov	esi,ebp
	pop	edi
	sub	edx,[esi+4]
	mov	[edi+12],edx		;out_blk[3]=d-l_key[1]
	mov	[edi+08],ecx		 ;out_blk[2]=c
	sub	ebx,[esi]
	mov	[edi+04],ebx		 ;out_blk[1]=b-l_key[0]
	mov	[edi+00],eax		   ;out_blk[0]=a
	popa
ret


; include 'lib_rc_data.inc'
;align 4
;	 rc6_memvars:
;	 rc6_l_key		  dd  0045 dup 0
;	 rc6_ll 		  dd  0009 dup 0
