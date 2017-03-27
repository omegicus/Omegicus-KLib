 ;  Created on: May 5, 2012
 ;	Author: Oleksandr Kazymyrov
 ;		Acknowledgments: Oleksii Shevchuk
 ;
 ;    FASM x86: Vladislav Kabak


;include 'lib_gostcrypt_a_data.inc'

align 4
KeySchedule: ; unsigned char *K EAX, int i ECX	 ; +OK
      push	eax ebx ecx esi
      ; K = K xor C[i]
      push	eax
      mov	ebx, eax
      mov	esi, eax
      mov	eax, ecx
      shl	eax, 0x06			 ; *64	(get block # by index)
      add	eax, const_C			 ; point to C-table
      call	AddXor512			 ; ebx eax esi ; AddXor512(K,C[i],K)
      pop	esi
      ;
      call	fn_S				 ; K = S(K)
      call	fn_P				 ; K = P(K)
      call	fn_L				 ; K = L(K)
      ;
      pop	esi ecx ebx eax
ret

align 4
fn_S: ; SubBytes S function. unsigned char *state [esi] 	      ; +OK
      push	 eax ebx ecx edx
      xor	 ecx, ecx			 ; int i = 0
      mov	 ebx, G12_Sbox			 ; S-Box
      xor	 edx, edx
      align 4					 ; for(i=0;i<64;i++)  state[i] = Sbox[state[i]]
  @@: mov	 dl,  byte[esi	+ ecx]		 ; edx = state[i]
      mov	 al,  byte[G12_Sbox + edx]	 ; al = Sbox[state[i]]
      mov	 byte[esi + ecx], al		 ; state[i] = Sbox[state[i]]
      inc	 ecx
      cmp	 ecx, 64
      jne	 @b
      pop	 edx ecx ebx eax
ret

align 4
fn_P: ; Transposition P function. unsigned char *state in ESI		   ; +OK
      pushad
      xor	ecx, ecx			 ; int i = 0
      xor	edx, edx
      align 4
  @@: mov	dl,  byte [Tau	+ ecx]		 ; edx = Tau[i]
      mov	al,  byte [esi	+ edx]		 ; eax = state[Tau[i]]
      mov	byte[fn_P_t + ecx], al		 ; t[i] = state[Tau[i]]
      inc	ecx				 ; for(i=0;i<64;i++)
      cmp	ecx, 64
      jne	@b
      ; memcpy(state,t,64)
      mov	ecx, 64/4
      mov	edi, esi
      mov	esi, fn_P_t
      cld
      rep	movsd
      popad
ret


align 4
fn_E: ; eax: ^K, ebx: *m, esi: *state		 ; +OK
; unsigned char *K,  const unsigned char *m, unsigned char *state
      push	ebx ecx
      ; AddXor512(m,K,state)
      call	AddXor512			 ; m=ebx, K=eax, state=esi, safe needed
      xor	ecx, ecx			 ; int i = 0
      align 4					 ; for(i=0;i<12;i++) {
  @@: call	fn_S				 ; state = S(state)
      call	fn_P				 ; state = P(state)
      call	fn_L				 ; state = L(state)
      ; K = KeySchedule(K, i)
      call	KeySchedule			 ; eax=K, i=ecx, safe needed
      ; AddXor512(state,K,state)
      mov	ebx, esi
      call	AddXor512			 ; (state, K, state)
      inc	ecx
      cmp	ecx, 12
      jne	@b				 ; }
      pop	ecx ebx
ret

align 4
AddXor512: ; AddRoundKey X-function. XOR 512-bytes	    ; +OK  EBX EAX ESI
	push	ecx edx
	xor	ecx, ecx
	align 4
    @@: mov	edx, dword[ebx + ecx]
	xor	edx, dword[eax + ecx]
	mov	dword[esi + ecx], edx
	add	ecx, 4
	cmp	ecx, 64
	jne	@b
	pop	edx ecx
ret

; g (N,m,H)
align 4
fn_g_N:  ; EDI *N, ESI *h, EDX *m		  ; +OK
; const unsigned char *N,unsigned char *h,const unsigned char *m
      pushad
      push	esi
      ; AddXor512(N,h,K)
      mov	ebx, edi			 ; N
      mov	eax, esi			 ; h
      mov	esi, fn_g_N_K
      call	AddXor512			 ; ebx eax esi
      mov	esi, fn_g_N_K			 ;
      call	fn_S				 ; K = S(K)
      call	fn_P				 ; K = P(K)
      call	fn_L				 ; K = L(K)
      ; E(K,m,t)				 ; t = E(K, m)
      mov	eax, fn_g_N_K			 ; K
      mov	ebx, edx			 ; m
      mov	esi, fn_g_N_t			 ; =t
      call	fn_E				 ; eax, ebx, esi
      ;
      pop	esi				 ; h
      push	esi				 ; h
      ; AddXor512(t,h,t)			 ; t = h xor t
      mov	ebx, fn_g_N_t			 ; t
      mov	eax, esi			 ; h
      mov	esi, fn_g_N_t			 ; t
      call	AddXor512			 ; ebx eax esi
      pop	esi				 ; h
      ; AddXor512(t,m,h)			 ; G = t xor m
      mov	ebx, fn_g_N_t			 ; t
      mov	eax, edx			 ; m
      call	AddXor512			 ; ebx eax esi
      popad
ret ; result = G


align 4
AddModulo512: ; eax *a	esi *b	edi *c		 ; +OK
; const unsigned char *a,const unsigned char *b,unsigned char *c
      push	ebx ecx edx
      ;
      mov	ecx, 64 			 ; int i = 63
      xor	ebx, ebx			 ; int t = 0
      ; for(i=63;i>=0;i--)
      ; {
      align 4
  @@: sar	ebx, 0x00000008 		 ; t := t >> 8
      movzx	edx, byte[eax + ecx -1] 	 ; edx = a[i]
      add	ebx, edx			 ; t := (t>>8) + a[i]
      movzx	edx, byte[esi + ecx -1] 	 ; edx = b[i]
      add	ebx, edx			 ; t := (t>>8) + +a[i] + b[i]
      mov	byte[edi + ecx -1], bl		 ; c[i] = t & 0xFF
      ;
      dec	ecx
      or	ecx, ecx
      jnz	@b
      ; }
      pop	edx ecx ebx
ret
























; L: umnozhenie 64bit vectora vhodnogo na matritsu A(64x64)
      ; unsigned long long v = 0;
      ; int i = 0, j = 0, k = 0;
      ; for(i=0;i<8;i++) {
      ; 	 v=0;
      ; 	 for(k=0;k<8;k++) {
      ; 		 for(j=0;j<8;j++) {
      ; 			if ((state[i*8+k] & (1<<(7-j))) != 0)
      ; 			       v ^= A[k*8+j]
      ; 		 }
      ; 	 }
      ; 	 for(k=0;k<8;k++)
      ; 	 {
      ; 		 state[i*8+k] = (v & ((unsigned long long)0xFF << (7-k)*8)) >> (7-k)*8;
      ; 	 }
      ;}

align 4
fn_L:  ; unsigned char *state in ESI		   ;
      pushad
      ;
      xor      ecx, ecx 			 ; int i = 0
      xor      ebx, ebx 			 ; int j = 0
      xor      edx, edx 			 ; int k = 0
      pxor     mm0, mm0 			 ;
      ; for(i=0;i<8;i++) {
      align 4
 .next_i:
      pxor     mm0, mm0 			 ; v = 0
      ;
      ; for(k=0;k<8;k++) {
      xor      edx, edx 			 ; k = 0
      align 4
 .next_k:
      ; for(j=0;j<8;j++) {
      ; 	 if ((state[i*8+k] & (1<<(7-j))) != 0) v ^= A[k*8+j]
      ; }
      xor      ebx, ebx 			 ; j = 0
      align 4
 .next_j:
      mov      eax, 0x00000007			 ; 7
      sub      eax, ebx 			 ; 7-j
      mov      edi, 0x00000001			 ; 1
      ;
      push     ecx
      mov      ecx, eax
      shl      edi, cl				 ; (1<<(7-j))	== EDI
      pop      ecx
      ;
      mov      eax, ecx 			 ; i
      shl      eax, 0x00000003			 ; i*8
      add      eax, edx 			 ; (i*8+k)
      movzx    eax, byte[esi + eax]		 ; state[i*8+k]
      and      eax, edi 			 ; (  state[i*8+k] & (1<<(7-j)	)
      cmp      eax, 0				 ; if ((state[i*8+k] & (1<<(7-j))) != 0) v ^= A[k*8+j] ???
      jz      .next_ji				 ; == 0
      ; v ^= A[k*8+j]				 ; != 0
      mov      eax, edx 			 ; k
      shl      eax, 0x00000003			 ; k*8
      add      eax, ebx 			 ; k*8+j
      shl      eax, 0x00000003			 ; *8 (point from index to 64bit value (8bytes per value))
      add      eax, matrix_A			 ; ^A[k*8+j]
      movq     mm1, qword[eax]			 ; A[k*8+j]
      pxor     mm0, mm1 			 ; v ^= A[k*8+j]
      jmp     .next_ji
      ; }
      jmp     .next_ki
      ; }

 .next_k2start:
      xor      edx, edx
      align 4
 .next_k2:
      mov      eax, 0x00000007			 ; 7
      sub      eax, edx 			 ; (7-k)
      shl      eax, 0x00000003			 ; (7-k)*8
      mov      edi, 0x000000FF
      pxor     mm2, mm2
      movd     mm2, edi 			 ; (unsigned long long)0xFF
      pxor     mm4, mm4
      movd     mm4, eax
      psllq    mm2, mm4 			 ; (unsigned long long)0xFF << (7-k)*8
      movq     mm3, mm0 			 ; v
      pand     mm3, mm2 			 ; (v & ((unsigned long long)0xFF << (7-k)*8))
      psrlq    mm3, mm4 			 ; (v & ((unsigned long long)0xFF << (7-k)*8)) >> (7-k)*8
      movd     eax, mm3
      mov      edi, ecx 			 ; i
      shl      edi, 0x00000003			 ; i*8
      add      edi, edx 			 ; i*8+k
      add      edi, esi 			 ; ^state[i*8+k]
      mov      byte[edi], al			 ; state[i*8+k] = (v & ((unsigned long long)0xFF << (7-k)*8)) >> (7-k)*8;
      jmp     .next_k2i

      align 4
 .next_ji:
      inc      ebx
      cmp      ebx, 8
      jb      .next_j
      jmp     .next_ki
      align 4
 .next_ki:
      inc      edx
      cmp      edx, 8
      jb      .next_k
      jmp     .next_k2start
      align 4
 .next_k2i:
      inc      edx
      cmp      edx, 8
      jb      .next_k2
      jmp     .next_ii
      align 4
 .next_ii:
      inc      ecx
      cmp      ecx, 8
      jb      .next_i
      jmp     .endx
 .endx:
      ;
      emms
      popad
ret













align 4
GOST_3411_2012: ;( EBX unsigned char *IV, ESI const unsigned char *message, ECX length, EDI unsigned char *out)
	; *hash = IV
	shl	ecx, 0x00000003 	    ; convert bytes to bits
	pushad
	mov	edi, G12_VARS
	mov	ecx, G12_VARS_SZ / 4
	xor	eax, eax
	cld
	rep	stosd
	;
	mov	eax, 0x00000000
	cmp	ebx, 256
	jne    .h512
	mov	eax, 0x01010101
      .h512:
	mov	edi, gosth_hash
	mov	ecx, 64/4
	cld
	rep	stosd
	;
	mov	byte[hash_X_v512 + 62], 0x02	   ; init hash_X_v512
	popad
	;
	; Stage 2
       ;; while (len >= 512)
       ;; {
       ;;	  memcpy(m, message + len/8 - 63 - ( (len & 0x7) == 0 ), 64);
       ;;	  g_N(N,hash,m);
       ;;	  AddModulo512(N,v512,N);
       ;;	  AddModulo512(hash_X_Sigma,m,hash_X_Sigma);
       ;;	  len -= 512;
       ;; }
	align 4
  .stage_2:
	cmp	ecx, 512
	jb     .stage_25
	;
	push	ecx esi edi
	mov	edi, g12_X_m
	push	ecx
	shr	ecx, 0x00000003
	add	esi, ecx
	sub	esi, 64
	pop	ecx
	mov	ecx, 64/4
	cld
	rep	movsd
	pop	edi esi ecx
	; g_N(N,hash,m)
	push	edx esi edi
	mov	edi, hash_X_N
	mov	esi, gosth_hash
	mov	edx, g12_X_m
	call	fn_g_N	; EDI *N, ESI *h, EDX *m  ; +++++
	pop	edi esi edx
	; AddModulo512(N,v512,N)
	push	eax esi edi
	mov	eax, hash_X_N
	mov	esi, hash_X_v512
	mov	edi, hash_X_N
	call	AddModulo512 ; eax *a  esi *b  edi *c
	pop	edi esi eax
	; AddModulo512(hash_X_Sigma,m,hash_X_Sigma)
	push	eax esi edi
	mov	eax, hash_X_Sigma
	mov	esi, g12_X_m
	mov	edi, hash_X_Sigma
	call	AddModulo512 ; eax *a  esi *b  edi *c
	pop	edi esi eax
	; len -= 512;
	sub	ecx, 512
	cmp	ecx, 512
	jae    .stage_2
	; }
  .stage_25:
	; +OK memset(m,0,64)
	push	edi ecx eax
	mov	edi, g12_X_m
	xor	eax, eax
	mov	ecx, 64/4
	cld
	rep	stosd
	pop	eax ecx edi
	; +OK memcpy(m + 63 - len/8 + ( (len & 0x7) == 0 ), message, len/8 + 1 - ( (len & 0x7) == 0 ))
	push	eax ecx esi edi
	mov	eax, 64
	shr	ecx, 0x00000003
	sub	eax, ecx
	mov	edi, g12_X_m
	add	edi, eax
	cld
	rep	movsb
	pop	edi esi ecx eax
	;
	;
	; Stage 3
	; m[ 63 - len/8 ] |= (1 << (len & 0x7))
	push	ebx ecx edx
	push	ecx
	and	ecx, 0x00000007
	mov	ebx, 0x00000001
	shl	ebx, cl
	pop	ecx
	push	ecx
	shr	ecx, 0x00000003
	mov	edx, 63
	sub	edx, ecx
	pop	ecx
	add	edx, g12_X_m
	or	byte[edx], bl
	pop	edx ecx ebx






	; g_N(N,hash,m);
	push	edx esi edi
	mov	edi, hash_X_N
	mov	esi, gosth_hash
	mov	edx, g12_X_m
	call	fn_g_N	; EDI *N, ESI *h, EDX *m
	pop	edi esi edx
	; v512[63] = len & 0xFF;
       ; push	 ecx
       ; and	 ecx, 0x000000FF
	mov	byte[hash_X_v512 + 63], cl
       ; pop	 ecx
	; v512[62] = len >> 8;
	push	ecx
	shr	ecx, 0x00000008
	mov	byte[hash_X_v512 + 62], cl
	pop	ecx
	; AddModulo512(N,v512,N);
	push	esi edi
	mov	eax, hash_X_N
	mov	esi, hash_X_v512
	mov	edi, hash_X_N
	call	AddModulo512 ; eax *a  esi *b  edi *c
	pop	edi esi
	; AddModulo512(hash_X_Sigma, m, hash_X_Sigma)
	push	esi edi
	mov	eax, hash_X_Sigma
	mov	esi, g12_X_m
	mov	edi, hash_X_Sigma
	call	AddModulo512 ; eax *a  esi *b  edi *c
	pop	edi esi
	; g_N(v0,hash,N);
	push	esi edi
	mov	edi, hash_X_v0
	mov	esi, gosth_hash
	mov	edx, hash_X_N
	call	fn_g_N	; EDI *N, ESI *h, EDX *m
	; g_N(v0,hash,hash_X_Sigma);
	mov	edi, hash_X_v0
	mov	esi, gosth_hash
	mov	edx, hash_X_Sigma
	call	fn_g_N	; EDI *N, ESI *h, EDX *m
	pop	edi esi
	; memcpy(out, hash, 64)
	push	ecx esi edi
	mov	esi, gosth_hash

	mov	ecx, 64/4
	cmp	ebx, 256
	jne    .h512c
	mov	ecx, 32/4
      .h512c:
	cld
	rep	movsd
	pop	edi esi ecx
ret





ALIGN 4
HEX2STR_256_BIT:    ; ^ESI->, ^EDI<-
		PUSHAD
		XOR	ECX, ECX
		;
	    @@: INC	ECX
		CMP	ECX, 32
		JA     .END
		LODSB
		PUSH	ECX
		MOV	DL,  AL
		MOV	ECX, 0x02
	   .L1: ROL	DL,  0x04
		MOV	EAX, 0x300F
		AND	AL,  DL
		AAA
		AAD	0x11
		STOSB
		LOOP   .L1
		POP	ECX
		JMP	@B
	       .END:
		XOR	EAX, EAX
		STOSB
		POPAD
RETD

ALIGN 4
HEX2STR_512_BIT:    ; ^ESI->, ^EDI<-
		PUSHAD
		XOR	ECX, ECX
		;
	    @@: INC	ECX
		CMP	ECX, 64
		JA     .END
		LODSB
		PUSH	ECX
		MOV	DL,  AL
		MOV	ECX, 0x02
	   .L1: ROL	DL,  0x04
		MOV	EAX, 0x300F
		AND	AL,  DL
		AAA
		AAD	0x11
		STOSB
		LOOP   .L1
		POP	ECX
		JMP	@B
	       .END:
		XOR	EAX, EAX
		STOSB
		POPAD
RETD

