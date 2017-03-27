use32

; htodw - convert hex string into dword value
align 4
cesario_htodw: ; IN: esi=string, OUT: eax=dword val, ecx=ptr to after
	xor	eax, eax
	xor	edx, edx
	mov	ecx, esi
     .next:
	mov	dl,  byte[ecx]
	cmp	dl, "0"
	jb     .ret
	cmp	dl, "9"
	jg	@f
	sub	dl, "0"
	jmp    .add
    @@: cmp	dl, "F"
	ja	@f
	cmp	dl, "A"
	jb     .ret
	sub	dl, "A"-10
	jmp    .add
    @@: cmp	dl, "f"
	ja     .ret
	cmp	dl, "a"
	jb     .ret
	sub	dl, "a"-10
  .add: shl	eax, 4
	add	eax, edx
	inc	ecx
	jmp    .next
  .ret: ret


; atodw - convert decimal string into dword value
align 4
cesario_atodw: ; IN: esi=string, OUT: eax=dword val
	xor  eax, eax
	xor  edx, edx
	mov  ecx, esi
    @@: mov  dl, [ecx]
	cmp  dl, "0"
	jb  .ret
	cmp  dl, "9"
	ja  .ret
	inc  ecx
	sub  dl, "0"
	lea  eax, [eax+4*eax]
	lea  eax, [edx+2*eax]
	jmp  @b
  .ret: ret


; atodw - converting a Floating-Point number to ASCII
align 4
cesario_ftoa: ; IN: esi=string, OUT: eax=dword val
	xor  eax, eax
	xor  edx, edx
	mov  ecx, esi
    @@: mov  dl, [ecx]
	cmp  dl, "0"
	jb  .ret
	cmp  dl, "9"
	ja  .ret
	inc  ecx
	sub  dl, "0"
	lea  eax, [eax+4*eax]
	lea  eax, [edx+2*eax]
	jmp  @b
  .ret: ret


; dwtoa - convert dword value into string
align 4
cesario_dwtoa: ; IN: eax=value to convert, edi=pointer to buffer; OUT: eax - pointer to end of buff
;uses ebx ecx edx edi
	xor	ecx, ecx
	mov	ebx, 10
    @@: xor	edx, edx
	div	ebx
	push	edx
	inc	ecx
	test	eax, eax
	jnz	@b
    @@: pop	eax
	add	eax, "0"
	stosb
	dec	ecx
	jnz	@b
	mov	eax, edi
	ret



; dwtoah - convert dword value into hex string
align 4
cesario_dwtoh: ; IN: eax=value to convert, edi=pointer to buffer; OUT: eax - pointer to end of buff
; uses ecx edx edi
	mov    esi, edi ; save lpBuffer value
	mov    edx, eax
	test   edx, edx
	jz    .ret
	mov    al,  '0'
	stosb
	dec    edi
	mov    ecx, 9
  .nxt: dec    ecx
	jz    .ret
	rol    edx, 4
	mov    al,  dl
	and    al,  00001111b
	jnz    @f
	cmp    edi, esi
	je    .nxt
    @@: and    dl, 11110000b
	cmp    al, 0x0A
	jae    @f
	add    al, 0x30 	 ; convert digits 0-9 to ascii
	jmp   .stsb
    @@: add    al, 0x57 	 ; convert digits A-F to ascii
 .stsb: stosb
	jmp   .nxt
  .ret: mov    eax, edi
	ret





;; ltrim - strip whitespace from the beginning of a string
;; __in lpString    - pointer to string  ESI
;; __in nStringLen  - len of string	 ECX
;; Return:  0 - failed
;;	   !0 - success, pointer to ltrimed string
;;		ecx - len of ltrimed string
align 4
cesario_ltrim:
;uses edx
	;; Check strings len
	xor	eax,  eax
	cmp	ecx,  eax
	jg	@f
	ret
  .nxt: inc	esi
	dec	ecx
	jz     .ret
    @@: mov	al, byte[esi]
	cmp	al, 32		   ; " " (ASCII 32 (0x20)), an ordinary space.
	je     .nxt
	cmp	al, 09		   ; "\t" (ASCII 9 (0x09)), a tab.
	je     .nxt
	cmp	al, 10		   ; "\n" (ASCII 10 (0x0A)), a new line (line feed).
	je     .nxt
	cmp	al, 13		   ; "\r" (ASCII 13 (0x0D)), a carriage return.
	je     .nxt
	cmp	al, 00		   ; "\0" (ASCII 0 (0x00)), the NUL-byte.
	je     .nxt
	cmp	al, 11		   ; "\x0B" (ASCII 11 (0x0B)), a vertical tab.
	je     .nxt
  .ret: mov	eax, esi	; pointer to ltrimed string
	ret





;; ==================================================================================================== ;;
;; StrIMaskMatch - compare string with mask								;;
;;		   - "#" - any 1 digit, 								;;
;;		   - "?" - any 1 symbol,								;;
;;		   - "*" - any group of symbols or no symbols at all					;;
;; __in lpString    - pointer to string 								;;
;; __in nStringLen  - len of string									;;
;; __in lpStrMask   - pointer to mask									;;
;; __in nStrMaskLen - len of mask									;;
;; Return:  0 - failed											;;
;;	   !0 - success 										;;
;; ==================================================================================================== ;;
align 4
cesario_strimaskmatch:
; uses ecx ebx edx esi edi
; lpString, nStringLen, lpStrMask, nStrMaskLen : dword

	;; Check strings len
	xor	eax, eax
       ; cmp	 nStringLen, eax
	jle    .ret
       ; cmp	 nStrMaskLen, eax
	jle    .ret
	;; Init string vars
       ; mov	 esi, lpString
       ; mov	 ebx, nStringLen
	add	ebx, esi
	dec	ebx			   ;; lpSrting last symbol ptr

       ; mov	 edi, lpStrMask
       ; mov	 ecx, nStrMaskLen
	add	ecx, edi
	dec	ecx			   ;; lpStrMask last symbol ptr

	xor	edx, edx	   ;; Group start ptr

	jmp	@f			   ;; Start compare

  .nxt:
	cmp	ebx, esi
	je     .finish		  ;; If last symbol in lpSrting
	cmp	ecx, edi
	je     .finish		  ;; If last symbol in lpStrMask

	inc	esi
	inc	edi


	;; Compare
   @@:	mov	al, [edi]	   ;; Symbol from mask



	;; Any single symbol (just skip it)
	cmp	al, "?"
	je     .nxt

	;; Any single digit or actual # symbol
	cmp	al, "#"
	jne	@f
	cmp	al, [esi]
	je     .nxt
	mov	al, [esi]
	cmp	al, "0"
	jb     .notmatch
	cmp	al, "9"
	ja     .notmatch

	jmp    .nxt


	;; Any group of symbols
    @@: cmp	al, "*"
	jne	@f
	cmp	ecx, edi
	je     .ret		  ;; if end of lpStrMask

	mov	edx, edi   ;; save start of group
	dec	esi
	jmp    .nxt


	;; Case insensitive symbol compare
@@:	cmp	al, [esi]
	je     .nxt

	cmp	al, "z"
	ja     .notmatch
	cmp	al, "a"
	jb     .cmpupper

	sub	al, 32
	cmp	al, [esi]
	je     .nxt
	jmp    .notmatch

   .cmpupper:
	cmp	al, "Z"
	ja     .notmatch
	cmp	al, "A"
	jb     .notmatch

	add	al, 32
	cmp	al, [esi]
	je     .nxt


	;; Symbol not match, check if group state
  .notmatch:
	test	edx, edx
	jz     .finish

	add	esi, edx
	sub	esi, edi
	inc	esi

	mov	edi, edx
	jmp    .nxt


;; ================================================== ;;
	;; Finish
.finish:
	xor	eax, eax
	cmp	ebx, esi
	jne    .ret		  ;; if not end of lpSrting


	mov	al, "*"
@@:	cmp	ecx, edi
	je     .ret		  ;; if end of lpStrMask
	 ;; Last * in lpStrMask
	inc	edi
	cmp	al, [edi]
	je	@b
	xor	eax, eax
  .ret: ret



;; ==================================================================================================== ;;
;; StrIPos - case insensitive search first entry of char in string					;;
;; __in lpString    - pointer to string 								;;
;; __in nStringLen  - len of string									;;
;; __in lpSubString - pointer to substring								;;
;; __in nSubStrLen  - len of substring									;;
;; Return:  0 - failed											;;
;;	   !0 - success, pointer to start of founded substring						;;
;; ==================================================================================================== ;;
StrIPos:

;proc uses ebx ecx edx edi esi
;lpString, nStringLen, lpSubString, nSubStrLen : dword

	;; Check strings len
	xor  eax, eax
      ;  cmp  nStringLen, eax
	jle  .ret
      ;  cmp  nSubStrLen, eax
	jle  .ret


	;; Init string vars
      ;  mov  esi, lpString		 ;; esi = first symbol pointer
	dec  esi
      ;  mov  ebx, nStringLen
	add  ebx, esi			;; ebx = last symbol pointer


.nextcycle:
	xor  eax, eax
	cmp  esi, ebx			;; check if last symbol in string
	je  .ret

	inc  esi				;; next symbol ptr search from
	mov  ecx, esi			;; copy symbol ptr search from, for use in compare

      ;  mov  edi, lpSubString	 ;; init substring var
      ;  mov  edx, nSubStrLen	 ;; init substring var
      ;

	;; Case insensitive symbol compare
.nextchar:
	mov  al, [ecx]
	cmp  al, [edi]
	je   .charmatch

	cmp  al, "z"
	ja   .nextcycle
	cmp  al, "a"
	jb   .cmpupper

	sub  al, 32
	cmp  al, [edi]
	je   .charmatch
	jmp  .nextcycle

.cmpupper:
	cmp  al, "Z"
	ja   .nextcycle
	cmp  al, "A"
	jb   .nextcycle

	add  al, 32
	cmp  al, [edi]
	jne  .nextcycle


.charmatch:
	inc  ecx				;; next symbol in string
	inc  edi				;; next symbol in substring
	dec  edx				;; substring len
	jnz  .nextchar


	;; Substring found
	mov  eax, esi


.ret:
	ret









;; ==================================================================================================== ;;
;; StrIPosEx - case insensitive search first entry of substring in string				;;
;;	       - whitespaces insensitive								;;
;;	       - <'> equals <"> 									;;
;; __in lpString    - pointer to string 								;;
;; __in nStringLen  - len of string									;;
;; __in lpSubString - pointer to substring								;;
;; __in nSubStrLen  - len of substring									;;
;; Return:  0 - failed											;;
;;	   !0 - success 										;;
;;		eax - pointer to start of founded substring						;;
;;		ecx - pointer to end of founded substring						;;
;; ==================================================================================================== ;;
StrIPosEx:
;proc uses ebx edx edi esi
;lpString, nStringLen, lpSubString, nSubStrLen : dword

	;; Check strings len
	xor  eax, eax
      ;  cmp  nStringLen, eax
	jle .ret
      ;  cmp  nSubStrLen, eax
	jle  .ret


	;; Init string vars
      ;  mov  esi, lpString		 ;; esi = first symbol pointer
	dec  esi
      ;  mov  ebx, nStringLen
	add  ebx, esi			;; ebx = last symbol pointer


.nextcycle:
	xor  eax, eax
	cmp  esi, ebx			;; check if last symbol in string
	je   .ret

	inc  esi				;; next symbol ptr search from
	mov  ecx, esi			;; copy symbol ptr search from, for use in compare

      ;  mov  edi, lpSubString	 ;; init substring var
      ;  mov  edx, nSubStrLen	 ;; init substring var


	;; Case insensitive symbol compare
.nextchar:
	mov  al, [ecx]
	cmp  al, [edi]
	je  .charmatch

	cmp  al, "z"
	ja  .nextcycle
	cmp  al, "a"
	jb  .cmpupper

	sub  al, 32
	cmp  al, [edi]
	je  .charmatch
	jmp .nextcycle

.cmpupper:
	cmp  al, "Z"
	ja  .nextcycle
	cmp  al, "A"
	jb  .nextcycle

	add  al, 32
	cmp  al, [edi]
	jne .nextcycle


.charmatch:
	inc  ecx				;; next symbol in string
	inc  edi				;; next symbol in substring
	dec  edx				;; substring len
	jnz .nextchar


	;; Substring found
	mov  eax, esi


.ret:
	ret










;[ebp+...] args
;[ebp+C] lpformat
;[ebp+8] lpDst
cesario_wsprintf:
	push	ebp
	mov	ebp,esp
  lpFormat_len equ ebp-4  ;.lpFormat_len
	sub	esp, 4
	pushad
	xor	edx,edx
	mov	edi, [ebp+0x08]
	mov	esi, [ebp+0x0C]
	; scan for null terminator and calculate length of input string
	xor	ecx,ecx
@@:	mov	al, [esi+ecx]
	or	al,al
	jz	.lok
	inc	ecx
	jmp	@B
.lok:	; ecx = length of [lpDst] (incl null terminator)
	mov	[lpFormat_len],ecx
	xor	ebx,ebx
@@:	mov	al, [esi]
	cmp	al, 0FFh	; test for string insert indicator (0FFh)
	jz	.insertString
	mov	[edi+ebx],al
	inc	ebx
	inc	esi
.k:	cmp	al,0
	jnz	@B
	mov	byte [edi+ebx],0
	popad
	add	esp,4 ; remove arg
	pop	ebp
	ret
.insertString:
	inc	esi
	push	esi
	push	ecx
	push	ebp
	mov	esi, [ebp+edx*4+10h]	; get ptr to arg
	; get len of arg
	xor	ecx,ecx
@@:	mov	al, [esi+ecx]
	or	al,al
	jz	.lok2
	inc	ecx
	jmp	@B
.lok2:	 ; ecx = length of [lpDst] (incl null terminator)
	dec    ecx
	push   ecx
	; copy arg to [plDst], last char first
.l:	mov	al, [esi+ecx]
	lea	ebp, [edi+ebx]	   ;|since below is illegal
	mov	[ds:ebp+ecx],al    ;|mov     [edi+ebx+ecx],al
	sub	ecx,1
	jns	.l	; if ecx=-1 then  we've copied all chars
	pop	ecx
	add	ebx,ecx ; adjust the dst offset.
	inc	edx  ; increase argument counter
	pop	ebp
	pop	ecx
	pop	esi
	jmp	.k
ret 4*3





align 4
cesario_insstr: ; lpString1,lpString2,len1,len2,index
	mov	ebp,  esp
	push	ebx esi edi
	;
	mov	esi,  dword[ebp + 04]	    ; esi = lpString1
	mov	ebx,  dword[ebp + 12]	    ; ebx = len1
	mov	eax,  dword[ebp + 20]	    ; eax = index
	mov	ecx,  dword[ebp + 16]	    ; ecx = len2
	add	esi,  eax
	mov	edi,  ecx
	mov	edx,  esi
	and	esi,  not 7
	and	edx,  7
	lea	edi,  [esi+ecx]
	neg	eax
	push	ecx
	lea	ecx,  [eax+ebx+7]
	neg	eax
	sub	ecx,  edx
	shr	ecx,  3
    @@: movq	mm0,  qword[esi + ecx*8]
	movq	qword[edi + ecx*8],mm0
	dec	ecx
	jns	@b
	emms
	mov	edi,  dword[ebp + 04]
	pop	ecx
	mov	edx,  ecx
	mov	esi,  dword[ebp + 08]
	add	edi,  eax ;edi = String1+index
	and	ecx,  not 3
	je     .bytes
	add	edi,  ecx
	add	esi,  ecx
	neg	ecx
    @@: mov	eax,  dword[esi+ecx]
	mov	dword[edi+ecx],eax
	add	ecx,  4
	jne	@b
    .bytes:
	and	edx,  3
	je     .exit
	add	esi,  edx
	add	edi,  edx
	neg	edx
    @@: mov	al,   byte[esi+edx]
	mov	byte[edi+edx],al
	inc	edx
	jne	@b
   .exit:
	pop	edi esi ebx
	ret	4*5
