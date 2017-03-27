use32
;  include 'lib_blowfish_data.inc'

macro GET_A reg
{
  shr reg,24
  and reg,0xff
  shl reg,2
}
macro GET_B reg
{
  shr reg,16
  and reg,0xff
  shl reg,2
}
macro GET_C reg
{
  shr reg,8
  and reg,0xff
  shl reg,2
}
macro GET_D reg
{
  and reg,0xff
  shl reg,2
}

align 4
BF_Init: ; esi: pointer to 256 bit key
	push	esi esi
	mov	edi, bf_skey
	mov	ecx, 32/4
	cld
	rep	movsd
	pop	esi
	mov	edi, bf_skey
	mov	ecx, 32/4
	cld
	rep	movsd
	pop	esi
	mov	edi, bf_skey
	mov	ecx, 8/4
	cld
	rep	movsd
	;
	mov	esi, bf_initpbox
	mov	edi, bf_pbox
	mov	ecx, 18
	cld
	rep	movsd
	;
	mov	esi, bf_initsbox
	mov	edi, bf_sbox
	mov	ecx, 4*256
	cld
	rep	movsd
	;
	call	BF_GenerateSubKeys
ret

align 4
BF_GenerateSubKeys:
	push	ebx esi edi
	; xor the key bits into the P array:
	mov	esi, bf_skey
	mov	edi, bf_pbox
	xor	ecx, ecx
	align 4
 .XORP: mov	eax, dword[esi]
	xor	dword[edi], eax
	add	esi, 04
	add	edi, 04
	inc	ecx
	cmp	ecx, 18
	jl     .XORP
	; encrypt and replace the P and S-Box arrays
	; start with the all-zero string (in EAX and EDX)
	xor	eax, eax
	xor	edx, edx
	mov	edi, bf_pbox	 ; the P array
	mov	ecx, 09
	call	BF_Replace
	; the S Boxes
	mov	edi, bf_sbox
	mov	ecx, 512
	call	BF_Replace
	;
	pop	edi esi ebx
	ret

align 4
BF_Replace:
    @@: call	BF_Encrypt
	mov	dword [edi+0], eax
	mov	dword [edi+4], edx
	add	edi,8
	dec	ecx
	jecxz	@f
	jmp	@b
    @@: ret


align 4
Blowfish_EncryptCFB:  ; ecx: bytes, esi: buffer
	; input length should be a multiple of 8 bytes
	pushad
    @@:
	mov	eax, dword[esi + 00]
	mov	edx, dword[esi + 04]
	call	BF_Encrypt
	mov	dword[esi + 00], eax
	mov	dword[esi + 04], edx

	sub	ecx, 08
	jecxz  .done

	mov	eax, dword[esi + 00]
	xor	dword[esi + 08 + 00], eax
	mov	eax, dword[esi + 04]
	xor	dword[esi + 08 + 04], eax

	add	esi,  08
	jmp	@b

 .done: popad
ret





align 4
Blowfish_DecryptCFB:  ; ecx: bytes, esi: buffer
	; input length should be a multiple of 8 bytes
	pushad

	add	esi, ecx
	sub	esi, 08

	align 4
    @@: mov	eax, dword[esi + 00]
	mov	edx, dword[esi + 04]
	call	BF_Decrypt
	mov	dword[esi + 00], eax
	mov	dword[esi + 04], edx

	sub	ecx, 08
	jecxz  .done

	mov	eax, dword[esi - 08 + 00]
	xor	dword[esi + 00], eax
	mov	eax, dword[esi - 08 + 04]
	xor	dword[esi + 04], eax

	sub	esi,  08
	jmp	@b

 .done: popad
ret






align 4
BF_Encrypt:
	push	esi ecx ebx
	; xL is passed in EAX
	; xR is passed in EDX
	mov	esi, bf_pbox
	mov	ecx, 16
  .ROUNDS:
	mov	ebx, edx
	xor	eax, dword[esi]
	call	BF_F_xL
	xor	edx, ebx
	add	esi, 04
	dec	ecx
	jecxz  .FINISH
	mov	ebx, edx
	mov	edx, eax
	mov	eax, ebx
	jmp    .ROUNDS
  .FINISH:
	xor	edx, dword [esi]
	add	esi, 04
	xor	eax, dword [esi]
	pop	ebx ecx esi
	ret


align 4
BF_Decrypt:
	push	esi ecx ebx
	; xL is passed in EAX
	; xR is passed in EDX
	mov	esi, bf_pbox
	add	esi, 68
	mov	ecx, 16
  .ROUNDS:
	mov	ebx, edx
	xor	eax, dword[esi]
	call	BF_F_xL
	xor	edx, ebx
	sub	esi, 04
	dec	ecx
	jecxz  .FINISH
	mov	ebx, edx
	mov	edx, eax
	mov	eax, ebx
	jmp    .ROUNDS
  .FINISH:
	xor	edx, dword[esi]
	sub	esi, 04
	xor	eax, dword[esi]
	; xL is now in EAX
	; xR is now in EDX
	pop	ebx ecx esi
	ret






align 4
BF_F_xL:
	push	esi ecx
	mov	ecx, eax
	lea	esi, [bf_sbox]
	GET_A	eax
	mov	edx, [esi+eax]
	lea	esi, [bf_sbox]
	add	esi, 1024
	mov	eax, ecx
	GET_B	eax
	add	edx, [esi+eax]
	lea	esi, [bf_sbox]
	add	esi, 2048
	mov	eax, ecx
	GET_C	eax
	xor	edx, [esi+eax]
	lea	esi, [bf_sbox]
	add	esi, 3072
	mov	eax, ecx
	GET_D	eax
	add	edx, [esi+eax]	  ; return xR in EDX
	mov	eax, ecx	  ; restore EAX
	pop	ecx esi
ret

















; -------------------------------------------------------------------------------------
