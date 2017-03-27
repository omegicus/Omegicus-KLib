; Spritz
; Realization: Vladislav Kabak, 2015

int spritz_hash(unsigned char *out, size_t outlen,
		const unsigned char *msg, size_t msglen);

int spritz_stream(unsigned char *out, size_t outlen,
		  const unsigned char *key, size_t keylen);

int spritz_encrypt(unsigned char *out, const unsigned char *msg, size_t msglen,
		   const unsigned char *nonce, size_t noncelen,
		   const unsigned char *key, size_t keylen);

int spritz_decrypt(unsigned char *out, const unsigned char *c, size_t clen,
		   const unsigned char *nonce, size_t noncelen,
		   const unsigned char *key, size_t keylen);

int spritz_auth(unsigned char *out, size_t outlen,
		const unsigned char *msg, size_t msglen,
		const unsigned char *key, size_t keylen);

main(void)
{
    unsigned char	out[32];
    const unsigned char msg[] = { 'a', 'r', 'c', 'f', 'o', 'u', 'r' };
    size_t		i;

    spritz_hash(out, sizeof out, msg, 7);
    spritz_stream(out, sizeof out, msg, 7);
}




SpritzN = 256



align 64
typedef struct State_ {
    unsigned char s[N];
    unsigned char a;
    unsigned char i;
    unsigned char j;
    unsigned char k;
    unsigned char w;
    unsigned char z;
} State;

#define LOW(B)	((B) & 0xF)
#define HIGH(B) ((B) >> 0x4)

static void memzero(void *pnt, size_t len)
{
    volatile unsigned char *pnt_ = (volatile unsigned char *) pnt;
    size_t		       i = (size_t) 0U;
    while (i < len) {
	pnt_[i++] = 0U;
    }
}



align 4
spritz_initialize_state: ; state in EAX
	xor	ecx, ecx
    @@: mov	byte[eax + ecx], al
	inc	ecx
	cmp	ecx, SpritzN
	jne	@b
	;
	mov	 byte[eax + SpritzN + 00], 0		   ; state->a = 0
	mov	 byte[eax + SpritzN + 01], 0		   ; state->i = 0
	mov	 byte[eax + SpritzN + 02], 0		   ; state->j = 0
	mov	 byte[eax + SpritzN + 03], 0		   ; state->k = 0
	mov	 byte[eax + SpritzN + 04], 1		   ; state->w = 1
	mov	 byte[eax + SpritzN + 05], 0		   ; state->z = 0
ret

align 4
spritz_update: ; state in EAX
	mov	 ebx, eax
	mov	 al, byte[ebx + SpritzN + 04]
	add	 byte[ebx + SpritzN + 01], al		   ; state->i += state->w
	; y = state->j + state->s[state->i]:
	xor	 ecx, ecx
	xor	 edx, edx
	mov	 dl,  byte[ebx + SpritzN + 01]		   ; state->i
	mov	 cl,  byte[ebx + SpritzN + 02]		   ; state->j
	add	 cl,  byte[ebx + edx]			   ; y = state->j + state->s[state->i]
	; state->j = state->k + state->s[y]:
	mov	 al,  byte[ebx + SpritzN + 03]		   ; state->k
	add	 al,  byte[ebx + ecx]
	add	 byte[ebx + SpritzN + 02], al		   ; state->i += state->w
	; state->k = state->i + state->k + state->s[state->j]:
	mov	 al,  byte[ebx + SpritzN + 01]
	add	 al,  byte[ebx + SpritzN + 03]
	xor	 ecx, ecx
	mov	 cl,  byte[eax + SpritzN + 02]
	add	 al,  byte[ebx + ecx]
	mov	 byte[ebx + SpritzN + 03], al
	; t = state->s[state->i]:
	xor	 edx, edx
	mov	 dl,  byte[ebx + SpritzN + 01]
	mov	 dl,  byte[ebx + edx]
	; state->s[state->i] = state->s[state->j]:
	xor	 ecx, ecx
	mov	 cl,  byte[ebx + SpritzN + 02]
	mov	 al,  byte[ebx + ecx]
	mov	 cl,  byte[ebx + SpritzN + 01]
	mov	 byte[ebx + ecx], al
	; state->s[state->j] = t:
	xor	 ecx, ecx
	mov	 cl,  byte[ebx + SpritzN + 02]
	mov	 byte[ebx + ecx], dl
ret

align 4
spritz_output: ; state in EAX
	mov	 ebx, eax
	xor	 eax, eax
	mov	 al,  byte[ebx + SpritzN + 05]
	add	 al,  byte[ebx + SpritzN + 03]		   ; y1
	mov	 al,  byte[ebx + eax]
	add	 al,  byte[ebx + SpritzN + 01]		   ; x1
	mov	 al,  byte[ebx + eax]
	add	 al,  byte[ebx + SpritzN + 02]		   ; y2
	; state->z = state->s[y2]:
	mov	 al,  byte[ebx + eax]
	mov	 byte[ebx + SpritzN + 05], al
	; return state->z
ret

align 4
spritz_crush: ; state in EAX
	mov	 ebx, eax
	xor	 ecx, ecx    ; unsigned char v, v = 0
	xor	 eax, eax
	;
;    for (v = 0; v < N / 2; v++) {
  .nxt:
	; y = (N - 1) - v
	mov	 al, (N-1)
	sub	 al, cl
;	 x1 = state->s[v]
;	 x2 = state->s[y]
	mov	 dl, [ebx + ecx]	; x1
	mov	 dh, [ebx + eax]	; x2
	cmp	 dl, dh
	jbe	.sub2			; x1 <= x2
	; if (x1 > x2):
;	     state->s[v] = x2;
	mov	 byte[ebx + ecx], dh
;	     state->s[y] = x1;
	mov	 byte[ebx + eax], dl
	jmp	.esub
	; else:
;	     state->s[v] = x1;
	mov	 byte[ebx + ecx], dl
;	     state->s[y] = x2;
	mov	 byte[ebx + eax], dh
	jmp	.esub
	 ;
   .esub:
	inc	 ecx
	cmp	 ecx, (SpritzN/2)
	je	.end
	inc	 ecx
	jmp	.nxt
  .end:
;    }

ret




align 4
spritz_whip: ; state in EAX
	mov	 ebx, eax
	mov	 eax, N*2    ; r
	xor	 ecx, ecx    ; v
	; for (v = 0; v < r; v++) update(state)
    @@: push	 eax ebx ecx
	mov	 eax, ebx
	call	 spritz_update
	pop	 ecx ebx eax
	inc	 ecx
	cmp	 ecx, eax
	jne	 @b
	; state->w += 2;
	add	 byte[ebx + SpritzN + 04], 2
ret


align 4
spritz_shuffle: ; state in EAX
	mov	 ebx, eax

	push	 ebx
	mov	 eax, ebx
	call	 spritz_whip
	pop	 ebx
	;
	push	 ebx
	mov	 eax, ebx
	call	 spritz_crush
	pop	 ebx
	;
	push	 ebx
	mov	 eax, ebx
	call	 spritz_whip
	pop	 ebx
	;
	push	 ebx
	mov	 eax, ebx
	call	 spritz_crush
	pop	 ebx
	;
	push	 ebx
	mov	 eax, ebx
	call	 spritz_whip
	pop	 ebx
	;
	mov	 byte[ebx + SpritzN + 00], 0
ret

align 4
spritz_absorb_stop: ; state in EAX
	mov	 ebx, eax
	cmp	 byte[ebx + SpritzN + 00], (SpritzN/2)
	je	.shuffle
    @@: inc	 byte[ebx + SpritzN + 00]
	ret
   .shuffle:
	push	 ebx
	mov	 eax, ebx
	call	 spritz_shuffle
	pop	 ebx
	jmp	 @b
ret


align 4
spritz_absorb_nibble:
; EAX State *state, EDX(DL) const unsigned char x
	and	 edx, 0x000000FF
	mov	 ebx, eax
	cmp	 byte[ebx + SpritzN + 00], (SpritzN/2)
	jne	 @f
	push	 ebx
	mov	 eax, ebx
	call	 spritz_shuffle
	pop	 ebx
    @@: add	 edx, (SpritzN / 2)	 ; y
	xor	 eax, eax
	mov	  al, byte[ebx + SpritzN + 00]
	mov	  al, byte[ebx + eax]	 ; t = state->s[state->a]


    state->s[state->a] = state->s[y];
    state->s[y] = t;
    state->a++;
ret


align 4
spritz_absorb_byte(State *state, const unsigned char b)
    absorb_nibble(state, LOW(b));
    absorb_nibble(state, HIGH(b));
ret

align 4
spritz_absorb(State *state, const unsigned char *msg, size_t length)
    size_t v;
    for (v = 0; v < length; v++) {
	absorb_byte(state, msg[v]);
    }
ret

align 4
spritz_drip(State *state)
    if (state->a > 0) {
	shuffle(state);
    }
    update(state);
    return output(state);
ret

align 4
spritz_squeeze(State *state, unsigned char *out, size_t outlen)
    size_t v;
    if (state->a > 0) {
	shuffle(state);
    }
    for (v = 0; v < outlen; v++) {
	out[v] = drip(state);
    }
ret
















static void key_setup(State *state, const unsigned char *key, size_t keylen)
{
    initialize_state(state);
    absorb(state, key, keylen);
    if (state->a > 0) {
	shuffle(state);
    }
}

int spritz_hash(unsigned char *out, size_t outlen,
	    const unsigned char *msg, size_t msglen)
{
    State	  state;
    unsigned char r;

    if (outlen > 255) {
	return -1;
    }
    r = (unsigned char) outlen;
    initialize_state(&state);
    absorb(&state, msg, msglen);
    absorb_stop(&state);
    absorb(&state, &r, 1U);
    squeeze(&state, out, outlen);
    memzero(&state, sizeof state);

    return 0;
}

int
spritz_stream(unsigned char *out, size_t outlen,
	      const unsigned char *key, size_t keylen)
{
    State state;

    initialize_state(&state);
    absorb(&state, key, keylen);
    squeeze(&state, out, outlen);
    memzero(&state, sizeof state);

    return 0;
}

int
spritz_encrypt(unsigned char *out, const unsigned char *msg, size_t msglen,
	       const unsigned char *nonce, size_t noncelen,
	       const unsigned char *key, size_t keylen)
{
    State  state;
    size_t v;

    key_setup(&state, key, keylen);
    absorb_stop(&state);
    absorb(&state, nonce, noncelen);
    for (v = 0; v < msglen; v++) {
	out[v] = msg[v] + drip(&state);
    }
    memzero(&state, sizeof state);

    return 0;
}

int
spritz_decrypt(unsigned char *out, const unsigned char *c, size_t clen,
	       const unsigned char *nonce, size_t noncelen,
	       const unsigned char *key, size_t keylen)
{
    State  state;
    size_t v;

    key_setup(&state, key, keylen);
    absorb_stop(&state);
    absorb(&state, nonce, noncelen);
    for (v = 0; v < clen; v++) {
	out[v] = c[v] - drip(&state);
    }
    memzero(&state, sizeof state);

    return 0;
}




;int spritz_auth(unsigned char *out, size_t outlen,
;	     const unsigned char *msg, size_t msglen,
;	     const unsigned char *key, size_t keylen)
align 4
spritz_auth:

;    State	   state;
;    unsigned char r;

	cmp	ecx, 256
	jae    .error
;-----------------------;
    r = (unsigned char) outlen;
    key_setup(&state, key, keylen);
    absorb_stop(&state);
    absorb(&state, msg, msglen);
    absorb_stop(&state);
    absorb(&state, &r, 1U);
    squeeze(&state, out, outlen);
    memzero(&state, sizeof state);

	jmp    .ok
;-----------------------;
  .err: mov	eax, -1
	jmp    .end
  .ok:	xor	eax, eax
  .end:
ret









