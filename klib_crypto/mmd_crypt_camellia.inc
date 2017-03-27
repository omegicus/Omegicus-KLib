; Camellia Block Encryption Algorithm
; Copyright Mitsubishi Electric Corp 2000-2001

;use32

align 16
Camellia_SIGMA:
	db 0xa0,0x9e,0x66,0x7f,0x3b,0xcc,0x90,0x8b
	db 0xb6,0x7a,0xe8,0x58,0x4c,0xaa,0x73,0xb2
	db 0xc6,0xef,0x37,0x2f,0xe9,0x4f,0x82,0xbe
	db 0x54,0xff,0x53,0xa5,0xf1,0xd3,0x6f,0x1c
	db 0x10,0xe5,0x27,0xfa,0xde,0x68,0x2d,0x1d
	db 0xb0,0x56,0x88,0xc2,0xb3,0xe6,0xc1,0xfd

align 16
Camellia_SBOX:
db 112,130, 44,236,179, 39,192,229,228,133, 87, 53,234, 12,174, 65
db  35,239,107,147, 69, 25,165, 33,237, 14, 79, 78, 29,101,146,189
db 134,184,175,143,124,235, 31,206, 62, 48,220, 95, 94,197, 11, 26
db 166,225, 57,202,213, 71, 93, 61,217,  1, 90,214, 81, 86,108, 77
db 139, 13,154,102,251,204,176, 45,116, 18, 43, 32,240,177,132,153
db 223, 76,203,194, 52,126,118,  5,109,183,169, 49,209, 23,  4,215
db  20, 88, 58, 97,222, 27, 17, 28, 50, 15,156, 22, 83, 24,242, 34
db 254, 68,207,178,195,181,122,145, 36,  8,232,168, 96,252,105, 80
db 170,208,160,125,161,137, 98,151, 84, 91, 30,149,224,255,100,210
db  16,196,  0, 72,163,247,117,219,138,  3,230,218,  9, 63,221,148
db 135, 92,131,  2,205, 74,144, 51,115,103,246,243,157,127,191,226
db  82,155,216, 38,200, 55,198, 59,129,150,111, 75, 19,190, 99, 46
db 233,121,167,140,159,110,188,142, 41,245,249,182, 47,253,180, 89
db 120,152,  6,106,231, 70,113,186,212, 37,171, 66,136,162,141,250
db 114,  7,185, 85,248,238,172, 10, 54, 73, 42,104, 60, 56,241,164
db  64, 40,211,123,187,201, 67,193, 21,227,173,244,119,199,128,158


KSFT1	dd 0,64,0,64,15,79,15,79,30,94,45,109,45,124,60,124,77,13,94,30,94,30,111,47,111,47
KIDX1	dd 0,0,4,4,0,0,4,4,4,4,0,0,4,0,4,4,0,0,0,0,4,4,0,0,4,4
KSFT2	dd 0,64,0,64,15,79,15,79,30,94,30,94,45,109,45,109,60,124,60,124,60,124,77,13,77,13,94,30,94,30,111,47,111,47
KIDX2	dd 0,0,12,12,8,8,4,4,8,8,12,12,0,0,4,4,0,0,8,8,12,12,0,0,4,4,8,8,4,4,0,0,12,12

;KSFT1   dd 15,79,15,79,30,94,45,109,45,124,60,124,77,13,94,30,94,30,111,47,111,47
;KIDX1   dd 0,0,8,8,8,8,0,0,8,0,8,8,0,0,0,0,8,8,0,0,8,8
;KSFT2   dd 15,79,15,79,30,94,30,94,45,109,45,109,60,124,60,124,60,124,77,13,77,13,94,30,94,30,111,47,111,47
;KIDX2   dd 4,4,8,8,4,4,12,12,0,0,8,8,0,0,4,4,12,12,0,0,8,8,4,4,8,8,0,0,12,12


Camellia_t db 64 dup 0
Camellia_u dd 20 dup 0
Camellia_FLlayer_t dd 4 dup 0
Camellia_FLlayer_u dd 4 dup 0
Camellia_FLlayer_v dd 4 dup 0
Camellia_Feistel_t db 8 dup 0




Camellia_InitSBOX:
;#define SBOX1(n) SBOX[(n)]
;#define SBOX2(n) (Byte)((SBOX[(n)]>>7^SBOX[(n)]<<1)&0xff)
;#define SBOX3(n) (Byte)((SBOX[(n)]>>1^SBOX[(n)]<<7)&0xff)
;#define SBOX4(n) SBOX[((n)<<1^(n)>>7)&0xff]
ret




Camellia_Ekeygen: ; const Byte *k esi, Byte *e edi
       ; Byte t[64];
       ; Word u[20];
       ; int  i;
       mov    ebp, esp
       ; for( i=0 ; i<32; i++ ) t[i] = k[i]
       xor    ecx,ecx
   @@: mov    al,  byte[esi + ecx]
       mov    byte[Camellia_t + ecx], al
       inc    ecx
       cmp    ecx, 32
       jb     @b

       ;
       ; XorBlock( t+0, t+16, t+32 )
       mov	edx, Camellia_t
       add	edx, 32
       push	edx
       sub	edx, 16
       push	edx
       sub	edx, 16
       push	edx
       call	XorBlock	; ( t+0, t+16, t+32 )

     ;  Camellia_Feistel( t+32, SIGMA+0, t+40 );
	lea	edx, [Camellia_t + 40]
	push	edx
	mov	edx, Camellia_SIGMA
	push	edx
	lea	edx, [Camellia_t + 32]
	push	edx
	call	Camellia_Feistel


     ;  Camellia_Feistel( t+40, SIGMA+8, t+32 );
	lea	edx, [Camellia_t + 32]
	push	edx
	lea	edx, [Camellia_SIGMA + 08]
	push	edx
	lea	edx, [Camellia_t + 40]
	push	edx
	call	Camellia_Feistel

       mov	edx, Camellia_t
       add	edx, 32
       push	edx
       sub	edx, 32
       push	edx
       sub	edx, 32
       push	edx
       call	XorBlock	; ( t+32, t+0, t+32 )

     ;  Camellia_Feistel( t+32, SIGMA+16, t+40 );
	lea	edx, [Camellia_t + 40]
	push	edx
	lea	edx, [Camellia_SIGMA + 16]
	push	edx
	lea	edx, [Camellia_t + 32]
	push	edx
	call	Camellia_Feistel

     ;  Camellia_Feistel( t+40, SIGMA+24, t+32 );
	lea	edx, [Camellia_t + 32]
	push	edx
	lea	edx, [Camellia_SIGMA + 24]
	push	edx
	lea	edx, [Camellia_t + 40]
	push	edx
	call	Camellia_Feistel


       push	Camellia_FLlayer_u	; u
       push	Camellia_FLlayer_t	; t
       call	ByteWord		; t+0,  u+0

       mov	edx, Camellia_FLlayer_u
       add	edx, 4
       push	edx
       mov	edx, Camellia_FLlayer_t
       add	edx, 32
       push	edx
       call	ByteWord		; ( t+32, u+4 );

       mov	edx, Camellia_t
       add	edx, 48
       push	edx
       sub	edx, 32
       push	edx
       sub	edx, 16
       push	edx
       call	XorBlock	; ( t+32, t+16, t+48 )


     ;  Camellia_Feistel( t+48, SIGMA+32, t+56 );
	lea	edx, [Camellia_t + 56]
	push	edx
	lea	edx, [Camellia_SIGMA + 32]
	push	edx
	lea	edx, [Camellia_t + 48]
	push	edx
	call	Camellia_Feistel

     ;  Camellia_Feistel( t+56, SIGMA+40, t+48 );
	lea	edx, [Camellia_t + 48]
	push	edx
	lea	edx, [Camellia_SIGMA + 40]
	push	edx
	lea	edx, [Camellia_t + 56]
	push	edx
	call	Camellia_Feistel


       mov	edx, Camellia_FLlayer_u
       add	edx, 8
       push	edx
       mov	edx, Camellia_FLlayer_t
       add	edx, 16
       push	edx
       call	ByteWord		; ( t+16, u+8 );
       mov	edx, Camellia_FLlayer_u
       add	edx, 12
       push	edx
       mov	edx, Camellia_FLlayer_t
       add	edx, 48
       push	edx
       call	ByteWord		; ( t+48, u+12 );

       ; for( i=0; i<34; i+=2 ){
       xor	ecx, ecx
   @@:

      ; RotBlock u+KIDX2[i+0], KSFT2[i+0], u+16
       lea	edx, [Camellia_FLlayer_u + 16]
       push	edx
       mov	edx, [KSFT2 + ecx*4 + 0]
       push	edx
       mov	edx, [KIDX2 + ecx*4 + 0]
       add	edx, Camellia_FLlayer_u
       push	edx
       call	RotBlock

      ; RotBlock u+KIDX2[i+1], KSFT2[i+1], u+18
       lea	edx, [Camellia_FLlayer_u + 18]
       push	edx
       mov	edx, [KSFT2 + ecx*4 + 4]
       push	edx
       mov	edx, [KIDX2 + ecx*4 + 4]
       add	edx, Camellia_FLlayer_u
       push	edx
       call	RotBlock


	mov	edx, ecx
	shl	edx, 3
	add	edx, dword[ebp + 08]
	push	edx
	mov	edx, Camellia_FLlayer_u
	add	edx, 16
	push	edx
	call	WordByte		; u+16, e+(i<<3)

       inc	ecx
       inc	ecx
       cmp	ecx, 34
       jb	@b
       ; }

ret


align 4
Camellia_Encrypt: ; ( const Byte *p, const Byte *e,  Byte *c )

	mov	ebp, esp
	push	dword[ebp + 12] ; c
	push	dword[ebp + 08] ; e+0
	push	dword[ebp + 04] ; p
	call	XorBlock	; ( p, e+0, c );

	xor	ecx, ecx	; for( i=0; i<3; i++ ){
    @@: mov	edx, dword[ebp + 12]
	add	edx, 8
	push	edx
	mov	edx, ecx
	shl	edx, 4
	add	edx, dword[ebp + 08]
	add	edx, 16
	push	edx
	mov	edx, dword[ebp + 12]
	push	edx
	call	Camellia_Feistel ;( c+0, e+16+(i<<4), c+8 );
	mov	edx, dword[ebp + 12]
	push	edx
	mov	edx, ecx
	shl	edx, 4
	add	edx, dword[ebp + 08]
	add	edx, 24
	push	edx
	mov	edx, dword[ebp + 12]
	add	edx, 8
	push	edx
	call	Camellia_Feistel ;( c+8, e+24+(i<<4), c+0 )
	inc	ecx
	cmp	ecx, 3
	jne	@b

	; }
	mov	edx, dword[ebp + 08] ; e
	add	edx, 72
	push	edx
	mov	edx, dword[ebp + 08] ; e
	add	edx, 64
	push	edx
	mov	edx, dword[ebp + 12]
	push	edx
	call	Camellia_FLlayer ; ( c, e+64, e+72 );


	;for( i=0; i<3; i++ ){
	;        Camellia_Feistel( c+0, e+80+(i<<4), c+8 );
	;        Camellia_Feistel( c+8, e+88+(i<<4), c+0 );
	;}
	xor	ecx, ecx	; for( i=0; i<3; i++ ){
    @@: mov	edx, dword[ebp + 12]
	add	edx, 8
	push	edx
	mov	edx, ecx
	shl	edx, 4
	add	edx, dword[ebp + 08]
	add	edx, 80
	push	edx
	mov	edx, dword[ebp + 12]
	push	edx
	call	Camellia_Feistel ;( c+0, e+80+(i<<4), c+8 );
	mov	edx, dword[ebp + 12]
	push	edx
	mov	edx, ecx
	shl	edx, 4
	add	edx, dword[ebp + 08]
	add	edx, 88
	push	edx
	mov	edx, dword[ebp + 12]
	add	edx, 8
	push	edx
	call	Camellia_Feistel ;( c+8, e+88+(i<<4), c+0 )
	inc	ecx
	cmp	ecx, 3
	jne	@b



	mov	edx, dword[ebp + 08] ; e
	add	edx, 136
	push	edx
	mov	edx, dword[ebp + 08] ; e
	add	edx, 128
	push	edx
	mov	edx, dword[ebp + 12]
	push	edx
	call	Camellia_FLlayer ; ( c, e+128, e+136 )


	;for( i=0; i<3; i++ ){
	;        Camellia_Feistel( c+0, e+144+(i<<4), c+8 );
	;        Camellia_Feistel( c+8, e+152+(i<<4), c+0 );
	;}
	xor	ecx, ecx	; for( i=0; i<3; i++ ){
    @@: mov	edx, dword[ebp + 12]
	add	edx, 8
	push	edx
	mov	edx, ecx
	shl	edx, 4
	add	edx, dword[ebp + 08]
	add	edx, 144
	push	edx
	mov	edx, dword[ebp + 12]
	push	edx
	call	Camellia_Feistel ;( c+0, e+144+(i<<4), c+8 );
	mov	edx, dword[ebp + 12]
	push	edx
	mov	edx, ecx
	shl	edx, 4
	add	edx, dword[ebp + 08]
	add	edx, 152
	push	edx
	mov	edx, dword[ebp + 12]
	add	edx, 8
	push	edx
	call	Camellia_Feistel ;( c+8, e+152+(i<<4), c+0 )
	inc	ecx
	cmp	ecx, 3
	jne	@b




	mov	edx, dword[ebp + 08] ; e
	add	edx, 200
	push	edx
	mov	edx, dword[ebp + 08] ; e
	add	edx, 192
	push	edx
	mov	edx, dword[ebp + 12]
	push	edx
	call	Camellia_FLlayer ; ( c, e+192, e+200 )


	;for( i=0; i<3; i++ ){
	;        Camellia_Feistel( c+0, e+208+(i<<4), c+8 );
	;        Camellia_Feistel( c+8, e+216+(i<<4), c+0 );
	;}
	xor	ecx, ecx	; for( i=0; i<3; i++ ){
    @@: mov	edx, dword[ebp + 12]
	add	edx, 8
	push	edx
	mov	edx, ecx
	shl	edx, 4
	add	edx, dword[ebp + 08]
	add	edx, 208
	push	edx
	mov	edx, dword[ebp + 12]
	push	edx
	call	Camellia_Feistel ;( c+0, e+208+(i<<4), c+8 );
	mov	edx, dword[ebp + 12]
	push	edx
	mov	edx, ecx
	shl	edx, 4
	add	edx, dword[ebp + 08]
	add	edx, 216
	push	edx
	mov	edx, dword[ebp + 12]
	add	edx, 8
	push	edx
	call	Camellia_Feistel ;( c+8, e+216+(i<<4), c+0 )
	inc	ecx
	cmp	ecx, 3
	jne	@b


	push	dword[ebp + 12]
	call	SwapHalf ; ( c );

	push	dword[ebp + 12] ; c
	mov	edx, dword[ebp + 08] ; e+256
	add	edx, 256
	push	edx
	push	dword[ebp + 04] ; p
	call	XorBlock	; ( p, e+256, c );

ret 4*3





macro Camellia_Feistel_to_T i {
	xor	eax, eax
	mov	ah,  [esi + i]
	mov	al,  [ebx + i]
	xor	al,  ah
	xor	ah,  ah
	mov	al,  byte[Camellia_SBOX + eax]
	mov	byte[Camellia_Feistel_t + i], al
}

macro Camellia_y_from_t_6 i0, i1, i2, i3, i4, i5, i6 {
	mov	al, byte[Camellia_Feistel_t + i1]
	xor	al, byte[Camellia_Feistel_t + i2]
	xor	al, byte[Camellia_Feistel_t + i3]
	xor	al, byte[Camellia_Feistel_t + i4]
	xor	al, byte[Camellia_Feistel_t + i5]
	xor	al, byte[Camellia_Feistel_t + i6]
	mov	byte[edi + i0], al
}
macro Camellia_y_from_t_5 i0,  i1, i2, i3, i4, i5  {
	mov	al, byte[Camellia_Feistel_t + i1]
	xor	al, byte[Camellia_Feistel_t + i2]
	xor	al, byte[Camellia_Feistel_t + i3]
	xor	al, byte[Camellia_Feistel_t + i4]
	xor	al, byte[Camellia_Feistel_t + i5]
	mov	byte[edi + i0], al
}


align 4
Camellia_Feistel: ;  const Byte *x, const Byte *k, Byte *y
	mov	ebp, esp
	pushad

	mov	esi, dword[ebp + 04]	; x
	mov	ebx, dword[ebp + 08]	; k
	mov	edi, dword[ebp + 12]	; y

	Camellia_Feistel_to_T 0
	Camellia_Feistel_to_T 1
	Camellia_Feistel_to_T 2
	Camellia_Feistel_to_T 3
	Camellia_Feistel_to_T 4
	Camellia_Feistel_to_T 5
	Camellia_Feistel_to_T 6
	Camellia_Feistel_to_T 7

	Camellia_y_from_t_6 0, 0,2,3,5,6,7
	Camellia_y_from_t_6 1, 0,1,3,4,6,7
	Camellia_y_from_t_6 2, 0,1,2,4,5,7
	Camellia_y_from_t_6 3, 1,2,3,4,5,6

	Camellia_y_from_t_5 4, 0,1,5,6,7
	Camellia_y_from_t_5 5, 1,2,4,6,7
	Camellia_y_from_t_5 6, 2,3,4,5,7
	Camellia_y_from_t_5 7, 0,3,4,5,6

	popad
ret 4*3





Camellia_FLlayer: ; Byte *x, const Byte *kl, const Byte *kr
;        Word t[4],u[4],v[4];
	mov	ebp, esp

	push	Camellia_FLlayer_t	; t
	push	dword[ebp + 04] 	; x
	call	ByteWord		; ByteWord x, t
	;
	push	Camellia_FLlayer_u	; u
	push	dword[ebp + 08] 	; k1
	call	ByteWord		; ByteWord kl, u
	;
	push	Camellia_FLlayer_v	; v
	push	dword[ebp + 12] 	; kr
	call	ByteWord		; ByteWord kr, v

	; t[1] ^= (t[0]&u[0])<<1 ^ (t[0]&u[0])>>31;
	mov	eax, dword[Camellia_FLlayer_t + 0*4]
	and	eax, dword[Camellia_FLlayer_u + 0*4]
	shl	eax, 1
	mov	ebx, dword[Camellia_FLlayer_t + 0*4]
	and	ebx, dword[Camellia_FLlayer_u + 0*4]
	shr	ebx, 31
	xor	eax, ebx
	xor	dword[Camellia_FLlayer_t + 1*4], eax
	; t[0] ^= t[1]|u[1];
	mov	eax, dword[Camellia_FLlayer_t + 1*4]
	or	eax, dword[Camellia_FLlayer_u + 1*4]
	xor	dword[Camellia_FLlayer_t + 0*4], eax
	; t[2] ^= t[3]|v[1];
	mov	eax, dword[Camellia_FLlayer_t + 3*4]
	or	eax, dword[Camellia_FLlayer_v + 1*4]
	xor	dword[Camellia_FLlayer_t + 2*4], eax
	; t[3] ^= (t[2]&v[0])<<1 ^ (t[2]&v[0])>>31;
	mov	eax, dword[Camellia_FLlayer_t + 2*4]
	and	eax, dword[Camellia_FLlayer_v + 0*4]
	shl	eax, 1
	mov	ebx, dword[Camellia_FLlayer_t + 2*4]
	and	ebx, dword[Camellia_FLlayer_v + 0*4]
	shr	ebx, 31
	xor	eax, ebx
	xor	dword[Camellia_FLlayer_t + 3*4], eax


	push	dword[ebp + 04] 	; x
	push	Camellia_FLlayer_t	; t
	call	WordByte		; WordByte  t, x
ret 4*3





ByteWord:; x, y ;( const Byte *x, DWord *y ) USES EAX ; ?????? Correct??
	mov	ebp, esp
	push	eax esi edi
       ; int i;
       ; for( i=0; i<4; i++ ){
       ;         y[i] = ((Word)x[(i<<2)+0]<<24) + ((Word)x[(i<<2)+1]<<16)
       ;              + ((Word)x[(i<<2)+2]<<8 ) + ((Word)x[(i<<2)+3]<<0 );
       ; }
	mov	esi, dword[ebp + 04]
	mov	edi, dword[ebp + 08]
	mov	eax, [esi + 00]
	mov	dword[edi + 00], eax
	mov	eax, [esi + 04]
	mov	dword[edi + 04], eax
	mov	eax, [esi + 08]
	mov	dword[edi + 08], eax
	mov	eax, [esi + 12]
	mov	dword[edi + 12], eax
	pop	edi esi eax
ret 4*2

WordByte: ; x, y ;( const DWord *x, Byte *y ) USES EAX ; ?????? Correct??
	mov	ebp, esp
	push	eax esi edi
	;int i;
	;for( i=0; i<4; i++ ){
	;        y[(i<<2)+0] = (Byte)(x[i]>>24&0xff);
	;        y[(i<<2)+1] = (Byte)(x[i]>>16&0xff);
	;        y[(i<<2)+2] = (Byte)(x[i]>> 8&0xff);
	;        y[(i<<2)+3] = (Byte)(x[i]>> 0&0xff);
	;}
	mov	esi, dword[ebp + 04]
	mov	edi, dword[ebp + 08]
	mov	eax, [esi + 00]
	mov	dword[edi + 00], eax
	mov	eax, [esi + 04]
	mov	dword[edi + 04], eax
	mov	eax, [esi + 08]
	mov	dword[edi + 08], eax
	mov	eax, [esi + 12]
	mov	dword[edi + 12], eax
	pop	edi esi eax
ret 4*2





align 4
RotBlock: ; x, n, y ; ( const Word *x, const int n, Word *y )
	; Returns 64 bits of rotated 128 bit block
	mov	esi, [esp + 04]
	mov	edi, [esp + 12]
	mov	ecx, [esp + 08]
	;
	pushad
	; r := n and 31;
	; n := n shr 5;
	mov	edx, ecx
	and	edx, 31
	shr	ecx, 05
	;
	; y[0] := (x[n and 3] shl r) xor (x[(n+1) and 3] shr (32-r));
	mov	ebx, ecx	; n
	and	ebx, 03 	; n and 3
	mov	ebx, [esi + ebx]; x[n and 3]

	push	ecx
	mov	ecx, edx
	shl	ebx, cl 	; dd (x[n and 3] shl r)
	pop	ecx

	push	ebx
	mov	eax, ecx	; n
	inc	eax		; n+1
	and	eax, 03 	; (n+1) and 3
	mov	eax, [esi + eax]; x[(n+1) and 3
	mov	ebx, 32 	;
	sub	ebx, edx	; (32-r)

	push	ecx
	mov	ecx, edx
	shr	eax, cl        ; x[(n+1) and 3] shr (32-r)
	pop	ecx

	pop	ebx
	xor	ebx, eax	; (x[n and 3] shl r) xor (x[(n+1) and 3] shr (32-r))
	mov	dword[edi], ebx ; y[0]
	;
	; y[1] := (x[(n+1) and 3 ] shl r) xor (x[(n+2) and 3] shr (32-r));
	mov	ebx, ecx	; n
	inc	ebx		; n+1
	and	ebx, 03 	; (n+1) and 3
	mov	ebx, [esi + ebx]; x[(n+1) and 3]

	push	ecx
	mov	ecx, edx
	shl	ebx, cl 	; (x[(n+1) and 3] shl r)
	pop	ecx

	push	ebx
	mov	eax, ecx	; n
	inc	eax		; n+1
	inc	eax		; n+2
	and	eax, 03 	; (n+2) and 3
	mov	eax, [esi + eax]; x[(n+1) and 3
	mov	ebx, 32 	;
	sub	ebx, edx	; (32-r)

	push	ecx
	mov	ecx, edx
	shr	eax, cl 	; x[(n+1) and 3] shr (32-r)
	pop	ecx

	pop	ebx
	xor	ebx, eax	; (x[n and 3] shl r) xor (x[(n+1) and 3] shr (32-r))
	mov	dword[edi + 04], ebx ; y[1]
	popad
ret 4*3






align 4
SwapHalf: ; x ; ( Byte *x )
      mov	ebp, esp
      ;  Byte t;  int  i;
      ;  for( i=0; i<8; i++ ) {
      ;          t = x[i];
      ;          x[i] = x[8+i];
      ;          x[8+i] = t;
      ;  }
      push	eax ecx esi
      mov	esi, [ebp + 04]
      xor	ecx, ecx
  .1: mov	al,  byte[esi + ecx]
      mov	ah,  byte[esi + ecx + 8]
      mov	byte[esi + ecx], ah
      mov	byte[esi + ecx + 8], al
      inc	ecx
      cmp	ecx, 8
      jb       .1
      pop	esi ecx eax
ret 4*1

XorBlock: ; x, y, z ; ( const Byte *x, const Byte *y, Byte *z )   USES EAX ECX
      mov	ebp, esp
      push	eax ecx esi edi ebx
      mov	esi, dword[ebp +04]  ; x
      mov	edi, dword[ebp +08]  ; y
      mov	ebx, dword[ebp +12]  ; z

	xor	ecx, ecx
	; for( i=0; i<16; i++ ) z[i] = x[i] ^ y[i];
    @@: mov	eax, dword[esi + ecx]
	xor	eax, dword[edi + ecx]
	mov	dword[ebx + ecx], eax
	add	ecx, 4
	cmp	ecx, 16
	jb	@b

      pop	ebx edi esi ecx eax
ret   4*3

