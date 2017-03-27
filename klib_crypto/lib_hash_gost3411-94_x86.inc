;GOST("This is message, length=32 bytes") = b1c466d3 7519b82e 8319819f f32595e0 47a28cb6 f83eff1c 6916a815 a637fffa
;GOST("Suppose the original message has length = 50 bytes") =
; 471aba57a60a770d3a76130635c1fbea4ef14de51f78b4ae57dd893b62f55208
;GOST("") = ce85b99cc46752fffee35cab9a7b0278abb4c2d2055cff685af4912c49490f8d
;GOST("a") = d42c539e367c66e9c88a801f6649349c21871b4344c6a573f849fdce62f314dd
;GOST( 128 characters of 'U' ) = 53a3a3ed25180cef0c1d85a074273e551c25660a87062a52d926a9e8fe5733a4


; The input message  is split into 256-bit blocks.
; In the case the last block  contains less than 256 bits, it is prepended left
; by zero bits to achieve the desired length.
USE32

;        MOV     ECX, 50
;        MOV     ESI, test50
;        MOV     EDI, outstr
;        MOV     EBX, hash       ; 32 bytes
;        CALL    GOST_3411_HASH


;align 4
;GOST_SBOX: ; "A" from GOST
;    DB 0x4, 0xA, 0x9, 0x2, 0xD, 0x8, 0x0, 0xE, 0x6, 0xB, 0x1, 0xC, 0x7, 0xF, 0x5, 0x3
;    DB 0xE, 0xB, 0x4, 0xC, 0x6, 0xD, 0xF, 0xA, 0x2, 0x3, 0x8, 0x1, 0x0, 0x7, 0x5, 0x9
;    DB 0x5, 0x8, 0x1, 0xD, 0xA, 0x3, 0x4, 0x2, 0xE, 0xF, 0xC, 0x7, 0x6, 0x0, 0x9, 0xB
;    DB 0x7, 0xD, 0xA, 0x1, 0x0, 0x8, 0x9, 0xF, 0xE, 0x4, 0x6, 0xC, 0xB, 0x2, 0x5, 0x3
;    DB 0x6, 0xC, 0x7, 0x1, 0x5, 0xF, 0xD, 0x8, 0x4, 0xA, 0x9, 0xE, 0x0, 0x3, 0xB, 0x2
;    DB 0x4, 0xB, 0xA, 0x0, 0x7, 0x2, 0x1, 0xD, 0x3, 0x6, 0x8, 0x5, 0x9, 0xC, 0xF, 0xE
;    DB 0xD, 0xB, 0x4, 0x1, 0x3, 0xF, 0x5, 0x9, 0x0, 0xA, 0xE, 0x7, 0x6, 0x8, 0x2, 0xC
;    DB 0x1, 0xF, 0xD, 0x0, 0x5, 0x7, 0xA, 0x4, 0x9, 0x2, 0x3, 0xE, 0x6, 0xB, 0x8, 0xC
;ALIGN 4
;GOST_3411_vars:
;        GOST_3411_HASH_ESI DD 0
;        GOST_3411_HASH_EDI DD 0
;        GOST_3411_HASH_ECX DD 0
;        GOST_3411_HASH_EBX DD 0
;        gost34_sum         DB 32 DUP 0
;        gost34_len         DD 0
;        gost34_len256      DD 7  DUP 0
;        gost34_sys         DB 148 DUP 0
;        GOST_3411_FillSboxes:
;                .x1 dd 0
;                .x2 dd 0
;                .x3 dd 0
;                .x4 dd 0
;        gost_3411_table    db  4096 dup 0





ALIGN 4
GOST_3411_EXPANDSBOX:
	PUSHAD
	;
	XOR	EDX, EDX			 ; n := 0
	XOR	ECX, ECX			 ; i := 0
	; for i := 0 to 15 do begin
	.fori15:
	   ; x1 := sbox[1][i] shl 15
	   MOVZX    EAX, BYTE[GOST_SBOX + 1*16 + ECX]
	   SHL	    EAX, 15
	   MOV	    DWORD[GOST_3411_FillSboxes.x1], EAX
	   ; x2 := sbox[3][i] shl 23
	   MOVZX    EAX, BYTE[GOST_SBOX + 3*16 + ECX]
	   SHL	    EAX, 23
	   MOV	    DWORD[GOST_3411_FillSboxes.x2], EAX
	   ; x3 := (sbox[5][i] shl 31) or (sbox[5][i] shr 1)
	   MOVZX    EAX, BYTE[GOST_SBOX + 5*16 + ECX]
	   SHL	    EAX, 31
	   MOVZX    EBX, BYTE[GOST_SBOX + 5*16 + ECX]
	   SHR	    EBX, 1
	   OR	    EAX, EBX
	   MOV	    DWORD[GOST_3411_FillSboxes.x3], EAX
	   ; x4 := sbox[7][i] shl 7
	   MOVZX    EAX, BYTE[GOST_SBOX + 7*16 + ECX]
	   SHL	    EAX, 7
	   MOV	    DWORD[GOST_3411_FillSboxes.x4], EAX

	   ; for k := 0 to 15 do begin
	   XOR	   EBX, EBX			    ; k := 0
	  .fork15:
	       ; GOSTSBOX[n + 0*256] := x1 or (sbox[0][k] shl 11)
	       MOVZX	EAX, BYTE[GOST_SBOX + 0*16 + EBX]
	       SHL	EAX, 11
	       MOV	ESI, DWORD[GOST_3411_FillSboxes.x1]
	       OR	EAX, ESI
	       MOV	DWORD[gost_3411_table + EDX + 0*1024], EAX
	       ; GOSTSBOX[n + 1*256] := x2 or (sbox[2][k] shl 19)
	       MOVZX	EAX, BYTE[GOST_SBOX + 2*16 + EBX]
	       SHL	EAX, 19
	       MOV	ESI, DWORD[GOST_3411_FillSboxes.x2]
	       OR	EAX, ESI
	       MOV	DWORD[gost_3411_table + EDX + 1*1024], EAX
	       ; GOSTSBOX[n + 2*256] := x3 or (sbox[4][k] shl 27)
	       MOVZX	EAX, BYTE[GOST_SBOX + 4*16 + EBX]
	       SHL	EAX, 27
	       MOV	ESI, DWORD[GOST_3411_FillSboxes.x3]
	       OR	EAX, ESI
	       MOV	DWORD[gost_3411_table + EDX + 2*1024], EAX
	       ; GOSTSBOX[n + 3*256] := x4 or (sbox[6][k] shl 3)
	       MOVZX	EAX, BYTE[GOST_SBOX + 6*16 + EBX]
	       SHL	EAX, 3
	       MOV	ESI, DWORD[GOST_3411_FillSboxes.x4]
	       OR	EAX, ESI
	       MOV	DWORD[gost_3411_table + EDX + 3*1024], EAX
	       ; inc(n)
	       ADD	EDX, 4
	       INC	EBX
	       CMP	EBX, 16
	       JB      .fork15
	    ; end;
	    INC      ECX
	    CMP      ECX, 16
	    JB	    .fori15
	; end
	POPAD
RET





ALIGN 4
GOST_3411_HASH: ; ESI: data, ECX: size, EDI: 64 byte buffer for string output hash OR 0 if not needed, ebx: 32byte hash buffer
; !!! Hash('') - return 0                                                                            !!!
; !!! is data size / 32 != 0 => buffer must have at least 32 bytes additional space right after data !!!
		PUSHAD
		CALL	GOST_3411_EXPANDSBOX
		MOV	DWORD[GOST_3411_HASH_ESI], ESI		       ;
		MOV	DWORD[GOST_3411_HASH_EDI], EDI		       ;
		MOV	DWORD[GOST_3411_HASH_ECX], ECX		       ;
		SHL	DWORD[GOST_3411_HASH_ECX], 3		       ; * 8 [bit]
		MOV	DWORD[GOST_3411_HASH_EBX], EBX		       ;
		;
		CMP	ECX, 0
		JA	@F
		PUSH	EBX
		MOV	ebx, DWORD[GOST_3411_HASH_EBX]
		mov	dword[ebx + 00], 0x9CB985CE
		mov	dword[ebx + 04], 0xFF5267C4
		mov	dword[ebx + 08], 0xAB5CE3FE
		mov	dword[ebx + 12], 0x78027B9A
		mov	dword[ebx + 16], 0xD2C2B4AB
		mov	dword[ebx + 20], 0x68FF5C05
		mov	dword[ebx + 24], 0x2C91F45A
		mov	dword[ebx + 28], 0x8D0F4949
		POP	EBX
		JMP    .END
		;
	      @@:
		; Init::  h := 0;
		MOV	EDI, DWORD[GOST_3411_HASH_EBX]
		MOV	ECX, 32/4
		XOR	EAX, EAX
		CLD
		REP	STOSD
		; Init::  E := 0; L := 0
		MOV	EDI, gost34_sum
		MOV	ECX, (16+32+32+148)/4
		XOR	EAX, EAX
		CLD
		REP	STOSD
		;
	       .NEXT:
		CMP	DWORD[GOST_3411_HASH_ECX], 256
		JBE    .LASTBLOCK
		;
	      ; Each block is processed by the step hash function
		MOV	ESI, DWORD[GOST_3411_HASH_ESI]		       ; apply step hash function
		MOV	EDI, DWORD[GOST_3411_HASH_EBX]		       ; ^h
		CALL	GOST_R_341194				       ; h := f(h,m[i])
		ADD	DWORD[gost34_len], 256			       ; L := L + 256
		MOV	ESI, gost34_sum 			       ; calculate control sum
		MOV	EDX, DWORD[GOST_3411_HASH_ESI]		       ; m[i]
		CALL	GOST_R_341194_CS			       ; E := E + m[i]
		ADD	DWORD[GOST_3411_HASH_ESI], 32
		SUB	DWORD[GOST_3411_HASH_ECX], 256
		JMP    .NEXT
		;
	      ; Compression function of final iteration
	       .LASTBLOCK:
		; L := L + | m[n] |        ; calculate the full message lentgh in bits
		MOV	ECX, DWORD[GOST_3411_HASH_ECX]
		ADD	DWORD[gost34_len], ECX
		; pad the last message with zeroes if it's size < 256bit
;                CMP     DWORD[GOST_3411_HASH_ECX], 256
;                JE      @F
		;
;                PUSH    EAX ECX EDI
;                SHR     ECX, 3                  ; /8
;                MOV     EAX, 32
;                SUB     EAX, ECX
;                MOV     EDI, DWORD[GOST_3411_HASH_ESI]
;                ADD     EDI, ECX
;                MOV     ECX, EAX
;                XOR     EAX, EAX
;                CLD
;                REP     STOSB
;                POP     EDI ECX EAX


		push	esi edi eax ecx
		mov	edi, gost_3411_lastblock
		xor	eax, eax
		mov	ecx, 32/4
		cld
		rep	stosd
		;
		mov	esi, DWORD[GOST_3411_HASH_ESI]
		mov	edi, gost_3411_lastblock
		mov	ecx, DWORD[GOST_3411_HASH_ECX]
		shr	ecx, 3
		cld
		rep	movsb
		;
		mov	esi, gost_3411_lastblock
		mov	DWORD[GOST_3411_HASH_ESI], esi
		pop	ecx eax edi esi


	      @@:
		MOV	ESI, gost34_sum 			       ; update control sum
		MOV	EDX, DWORD[GOST_3411_HASH_ESI]		       ;
		CALL	GOST_R_341194_CS			       ; E := E + m[n]
		MOV	ESI, DWORD[GOST_3411_HASH_ESI]		       ; m[n]
		MOV	EDI, DWORD[GOST_3411_HASH_EBX]		       ; ^h
		CALL	GOST_R_341194				       ; h := f(h,m[n])
		MOV	ESI, gost34_len 			       ;
		MOV	EDI, DWORD[GOST_3411_HASH_EBX]		       ; MD - strengthen up by hashing message length
		CALL	GOST_R_341194				       ; h := f(h, L)
		MOV	ESI, gost34_sum 			       ;
		MOV	EDI, DWORD[GOST_3411_HASH_EBX]		       ; hash control sum
		CALL	GOST_R_341194				       ; h := f(h, E)
	      .END:
		CMP	DWORD[GOST_3411_HASH_EDI], 0
		JE     .EXIT
		MOV	ESI, DWORD[GOST_3411_HASH_EBX]
		MOV	EDI, DWORD[GOST_3411_HASH_EDI]
		CALL	HEX2STR_256BIT
	      .EXIT:
		POPAD
RET


ALIGN 4
HEX2STR_256BIT:    ; ^ESI->, ^EDI<-
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
GOST_R_341194:
		PUSHAD
		;
		mov	ebp, gost_3411_table
		xor	ebx, ebx	     ; xor  B, B                 ; prepare 0 for loop counter

		mov	dword[gost34_sys+0x80], esi	 ; preserve data pointer in the local var
		mov	dword[gost34_sys+0x84], edi	 ; preserve digest pointer in the local var
		mov	dword[gost34_sys+0x88], ebp	 ; preserve GOSTBOX pointer in the local var
		mov	dword[gost34_sys+0x8C], ebx	 ; zeroize loop counter
		mov	eax, [edi+0x00]      ; move original Digest to local vars
		mov	ebx, [edi+0x04]
		mov	ecx, [edi+0x08]      ; move data buffer to local vars
		mov	edx, [edi+0x0C]
		mov	dword[gost34_sys+20h], eax
		mov	dword[gost34_sys+24h], ebx
		mov	dword[gost34_sys+28h], ecx
		mov	dword[gost34_sys+2Ch], edx
		mov	eax, [edi+10h]
		mov	ebx, [edi+14h]
		mov	ecx, [edi+18h]
		mov	edx, [edi+1Ch]
		mov	dword[gost34_sys+30h], eax
		mov	dword[gost34_sys+34h], ebx
		mov	dword[gost34_sys+38h], ecx
		mov	dword[gost34_sys+3Ch], edx
		mov	eax, [esi+0x00]
		mov	ebx, [esi+0x04]
		mov	ecx, [esi+0x08]
		mov	edx, [esi+0x0C]
		mov	dword[gost34_sys+40h], eax
		mov	dword[gost34_sys+44h], ebx
		mov	dword[gost34_sys+48h], ecx
		mov	dword[gost34_sys+4Ch], edx
		mov	eax, [esi+10h]
		mov	ebx, [esi+14h]
		mov	ecx, [esi+18h]
		mov	edx, [esi+1Ch]
		mov	dword[gost34_sys+50h], eax
		mov	dword[gost34_sys+54h], ebx
		mov	dword[gost34_sys+58h], ecx
		mov	dword[gost34_sys+5Ch], edx
	@@loc_0_A1:
		mov	eax, dword[gost34_sys+20h]
		mov	ebx, dword[gost34_sys+28h]
		mov	ecx, dword[gost34_sys+30h]
		mov	edx, dword[gost34_sys+38h]
		mov	edi, dword[gost34_sys+40h]
		mov	esi, dword[gost34_sys+48h]
		xor	eax, edi
		xor	ebx, esi
		mov	edi, dword[gost34_sys+50h]
		mov	esi, dword[gost34_sys+58h]
		xor	ecx, edi
		xor	edx, esi
		mov	edi, ebx
		mov	ebp, edx
		mov	esi, eax
		and	edi, 0xFF
		shl	ebp, 0x18
		and	esi, 0xFF
		shl	edi, 8
		or	ebp, esi
		mov	esi, ecx
		shr	eax, 8
		and	esi, 0xFF
		or	ebp, edi
		shl	esi, 0x10
		mov	edi, eax
		or	ebp, esi
		mov	esi, ebx
		mov	dword[gost34_sys], ebp
		mov	ebp, ecx
		and	edi, 0xFF
		and	ebp, 0xFF00
		shr	eax, 8
		and	esi, 0xFF00
		shl	ebp, 8
		or	edi, esi
		shr	ebx, 8
		mov	esi, edx
		or	ebp, edi
		and	esi, 0xFF00
		mov	edi, eax
		shl	esi, 0x10
		or	ebp, esi
		mov	esi, ebx
		mov	dword[gost34_sys+4], ebp
		and	edi, 0xFF
		mov	ebp, ecx
		and	esi, 0xFF00
		shr	eax, 8
		and	ebp, 0xFF0000
		or	edi, esi
		mov	esi, edx
		shr	ebx, 8
		and	esi, 0xFF0000
		or	ebp, edi
		shl	esi, 8
		and	ebx, 0xFF00
		shr	ecx, 8
		or	ebp, esi
		and	ecx, 0xFF0000
		mov	dword[gost34_sys+8], ebp
		or	eax, ebx
		and	edx, 0xFF000000
		or	eax, ecx
		or	eax, edx
		mov	dword[gost34_sys+0x0C], eax
		mov	eax, dword[gost34_sys+24h]
		mov	ebx, dword[gost34_sys+2Ch]
		mov	ecx, dword[gost34_sys+34h]
		mov	edx, dword[gost34_sys+3Ch]
		mov	edi, dword[gost34_sys+44h]
		mov	esi, dword[gost34_sys+4Ch]
		xor	eax, edi
		xor	ebx, esi
		mov	edi, dword[gost34_sys+54h]
		mov	esi, dword[gost34_sys+5Ch]
		xor	ecx, edi
		xor	edx, esi
		mov	edi, ebx
		mov	ebp, edx
		mov	esi, eax
		and	edi, 0xFF
		shl	ebp, 0x18
		and	esi, 0xFF
		shl	edi, 8
		or	ebp, esi
		mov	esi, ecx
		shr	eax, 8
		and	esi, 0xFF
		or	ebp, edi
		shl	esi, 0x10
		mov	edi, eax
		or	ebp, esi
		mov	esi, ebx
		mov	dword[gost34_sys+0x10], ebp
		mov	ebp, ecx
		and	edi, 0xFF
		and	ebp, 0xFF00
		shr	eax, 8
		and	esi, 0xFF00
		shl	ebp, 8
		or	edi, esi
		shr	ebx, 8
		mov	esi, edx
		or	ebp, edi
		and	esi, 0xFF00
		mov	edi, eax
		shl	esi, 0x10
		or	ebp, esi
		mov	esi, ebx
		mov	dword[gost34_sys+0x14], ebp
		and	edi, 0xFF
		mov	ebp, ecx
		and	esi, 0xFF00
		shr	eax, 8
		and	ebp, 0xFF0000
		or	edi, esi
		mov	esi, edx
		shr	ebx, 8
		and	esi, 0xFF0000
		or	ebp, edi
		shl	esi, 8
		and	ebx, 0xFF00
		shr	ecx, 8
		or	ebp, esi
		and	ecx, 0xFF0000
		mov	dword[gost34_sys+18h], ebp
		or	eax, ebx
		and	edx, 0xFF000000
		or	eax, ecx
		or	eax, edx
		mov	dword[gost34_sys+1Ch], eax
		mov	edi, dword[gost34_sys+8Ch]
		mov	esi, dword[gost34_sys+84h]
		mov	edx, dword[gost34_sys+88h]
		mov	ecx, dword[gost34_sys]
		mov	ebx, dword[esi+edi*4]
		mov	eax, dword[esi+edi*4+4]
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0xFF
		shr	ebp, 0x0E
		and	esi, 0x03FC
		mov	ecx, dword[edx+ecx*4]
		and	ebp, 0x03FC
		mov	esi, dword[edx+esi+0x0400]
		shr	edi, 0x18
		xor	eax, ecx
		mov	ebp, dword[edx+ebp+0x0800]
		xor	eax, esi
		mov	edi, dword[edx+edi*4+0x0C00]
		xor	eax, ebp
		mov	ecx, dword[gost34_sys+4]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0xFF
		shr	ebp, 0x0E
		and	esi, 0x03FC
		mov	ecx, dword[edx+ecx*4]
		and	ebp, 0x03FC
		mov	esi, dword[edx+esi+400h]
		shr	edi, 0x18
		xor	ebx, ecx
		mov	ebp, dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi, dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx, dword[gost34_sys+8]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx, dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi, dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp, dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi, dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx, dword[gost34_sys+0Ch]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx, dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi, dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp, dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi, dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx, dword[gost34_sys+10h]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx, dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi, dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp, dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi, dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx, dword[gost34_sys+14h]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx, [edx+ecx*4]
		and	ebp, 3FCh
		mov	esi, [edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp, [edx+ebp+800h]
		xor	ebx, esi
		mov	edi, [edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+18h]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+1Ch]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+4]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+8]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+0Ch]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+10h]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+14h]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+18h]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+1Ch]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+4]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+8]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+0Ch]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+10h]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+14h]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+18h]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+1Ch]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+1Ch]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+18h]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+14h]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+10h]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+0Ch]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys+8]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys+4]
		xor	ebx, edi
		add	ecx, ebx
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	eax, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	eax, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	eax, ebp
		mov	ecx,dword[gost34_sys]
		xor	eax, edi
		add	ecx, eax
		mov	edi, ecx
		mov	esi, ecx
		mov	ebp, ecx
		shr	esi, 6
		and	ecx, 0FFh
		shr	ebp, 0Eh
		and	esi, 3FCh
		mov	ecx,dword[edx+ecx*4]
		and	ebp, 3FCh
		mov	esi,dword[edx+esi+400h]
		shr	edi, 18h
		xor	ebx, ecx
		mov	ebp,dword[edx+ebp+800h]
		xor	ebx, esi
		mov	edi,dword[edx+edi*4+0C00h]
		xor	ebx, ebp
		mov	ecx,dword[gost34_sys]
		xor	ebx, edi
		mov	ebp,dword[gost34_sys+8Ch]
		mov    dword[gost34_sys+ebp*4+60h], eax
		mov    dword[gost34_sys+ebp*4+64h], ebx
		cmp	ebp, 6
		jz	@@loc_0_C5C
		add	ebp, 2
		mov    dword[gost34_sys+8Ch], ebp
		mov	eax,dword[gost34_sys+20h]
		mov	ebx,dword[gost34_sys+24h]
		mov	ecx,dword[gost34_sys+28h]
		mov	edx,dword[gost34_sys+2Ch]
		xor	eax, ecx
		xor	ebx, edx
		cmp	ebp, 4
		jz	@@loc_0_BB0
		mov    dword[gost34_sys+20h], ecx
		mov    dword[gost34_sys+24h], edx
		mov	edi,dword[gost34_sys+30h]
		mov	esi,dword[gost34_sys+34h]
		mov	ecx,dword[gost34_sys+38h]
		mov	edx,dword[gost34_sys+3Ch]
		jmp	@@loc_0_BF7

	@@loc_0_BB0:
		xor	ecx, 0FF00FF00h
		xor	edx, 0FF00FF00h
		xor	eax, 0FFh
		xor	ebx, 0FF00FFFFh
		mov    dword[gost34_sys+20h], ecx
		mov    dword[gost34_sys+24h], edx
		mov	edi,dword[gost34_sys+30h]
		mov	esi,dword[gost34_sys+34h]
		mov	ecx,dword[gost34_sys+38h]
		mov	edx,dword[gost34_sys+3Ch]
		xor	edi, 0FF00FFh
		xor	esi, 0FF00FFh
		xor	ecx, 0FFFF00h
		xor	edx, 0FF0000FFh

	@@loc_0_BF7:
		mov    dword[gost34_sys+28h], edi
		mov    dword[gost34_sys+2Ch], esi
		mov    dword[gost34_sys+30h], ecx
		mov    dword[gost34_sys+34h], edx
		mov    dword[gost34_sys+38h], eax
		mov    dword[gost34_sys+3Ch], ebx
		mov	eax,dword[gost34_sys+40h]
		mov	ebx,dword[gost34_sys+44h]
		mov	ecx,dword[gost34_sys+48h]
		mov	edx,dword[gost34_sys+4Ch]
		mov	edi,dword[gost34_sys+50h]
		mov	esi,dword[gost34_sys+54h]
		mov    dword[gost34_sys+40h], edi
		mov    dword[gost34_sys+44h], esi
		xor	edi, ecx
		xor	esi, edx
		xor	ecx, eax
		xor	edx, ebx
		mov	eax,dword[gost34_sys+58h]
		mov	ebx,dword[gost34_sys+5Ch]
		mov    dword[gost34_sys+48h], eax
		mov    dword[gost34_sys+4Ch], ebx
		mov    dword[gost34_sys+50h], ecx
		mov    dword[gost34_sys+54h], edx
		mov    dword[gost34_sys+58h], edi
		mov    dword[gost34_sys+5Ch], esi
		jmp	@@loc_0_A1

	@@loc_0_C5C:
		mov	edi,dword[gost34_sys+80h]
		mov	esi,dword[edi]
		mov	ebp,dword[edi+4]
		mov	ecx,dword[edi+8]
		mov	edx,dword[edi+0Ch]
		xor	esi, eax
		xor	ebp, ebx
		mov    dword[gost34_sys+20h], esi
		mov    dword[gost34_sys+24h], ebp
		mov	edi, ebx
		xor	ecx, eax
		mov	esi, ebx
		ror	ebx, 10h
		and	edi, 0FFFF0000h
		xor	edx, eax
		ror	eax, 10h
		mov	ebp, ebx
		xor	edx, ebx
		and	esi, 0FFFFh
		mov	ebx,dword[gost34_sys+60h]
		xor	ecx, edi
		xor	edx, eax
		and	ebp, 0FFFFh
		mov	edi, eax
		xor	edx, esi
		mov	esi, ebx
		and	edi, 0FFFF0000h
		mov	eax,dword[gost34_sys+64h]
		xor	ecx, ebp
		and	ebx, 0FFFFh
		mov	ebp, eax
		xor	ecx, edi
		xor	edx, ebx
		shr	ebp, 10h
		mov	edi, eax
		xor	ecx, ebx
		mov	ebx, esi
		ror	eax, 10h
		ror	esi, 10h
		xor	ecx, ebp
		mov	ebp,dword[gost34_sys+68h]
		and	edi, 0FFFFh
		xor	edx, eax
		mov	eax, ebp
		ror	ebp, 10h
		shl	ebx, 10h
		xor	ecx, esi
		xor	edx, edi
		mov	esi,dword[gost34_sys+6Ch]
		shl	eax, 10h
		xor	edx, ebp
		xor	ecx, edi
		shl	esi, 10h
		xor	edx, ebx
		mov	ebp,dword[gost34_sys+60h]
		mov	edi,dword[gost34_sys+80h]
		xor	ecx, eax
		xor	edx, esi
		mov    dword[gost34_sys+28h], ecx
		mov    dword[gost34_sys+2Ch], edx
		mov	eax,dword[edi+10h]
		mov	ebx,dword[edi+14h]
		mov	ecx,dword[edi+18h]
		mov	edx,dword[edi+1Ch]
		mov	edi, ebp
		mov	esi, ebp
		xor	ecx, ebp
		shl	esi, 10h
		and	ebp, 0FFFF0000h
		shr	edi, 10h
		xor	ebp, esi
		mov	esi,dword[gost34_sys+64h]
		xor	edx, ebp
		xor	ebp, edi
		mov	edi, esi
		shr	esi, 10h
		xor	eax, ebp
		xor	ebx, ebp
		xor	ecx, esi
		xor	eax, esi
		mov	esi, edi
		mov	ebp, edi
		and	edi, 0FFFFh
		shl	esi, 10h
		xor	edx, edi
		and	ebp, 0FFFF0000h
		xor	edx, esi
		mov	esi,dword[gost34_sys+68h]
		xor	ebx, edi
		xor	eax, ebp
		mov	edi, esi
		mov	ebp,dword[gost34_sys+6Ch]
		xor	ebx, esi
		shr	esi, 10h
		xor	ecx, ebp
		shl	edi, 10h
		xor	eax, esi
		xor	ebx, esi
		xor	edx, esi
		mov	esi, ebp
		shr	ebp, 10h
		xor	eax, edi
		shl	esi, 10h
		xor	ecx, edi
		mov	edi,dword[gost34_sys+70h]
		xor	eax, ebp
		xor	ebx, ebp
		xor	ecx, ebp
		xor	edx, esi
		xor	eax, esi
		xor	ebx, esi
		mov	esi, edi
		xor	edx, edi
		shl	edi, 10h
		mov	ebp,dword[gost34_sys+74h]
		shr	esi, 10h
		xor	eax, edi
		xor	edi, esi
		xor	edx, esi
		mov	esi, ebp
		shl	ebp, 10h
		xor	ebx, edi
		shr	esi, 10h
		xor	ebx, ebp
		xor	esi, ebp
		xor	ecx, edi
		mov	ebp,dword[gost34_sys+78h]
		xor	edx, esi
		xor	ecx, esi
		mov	edi, ebp
		xor	ecx, ebp
		ror	ebp, 10h
		shr	edi, 10h
		mov	esi,dword[gost34_sys+7Ch]
		xor	eax, ebp
		xor	edx, edi
		mov	edi, esi
		xor	ebx, ebp
		xor	ecx, ebp
		mov	ebp, esi
		ror	esi, 10h
		shl	edi, 10h
		xor	eax, esi
		xor	ebx, esi
		xor	ecx, edi
		mov	edi, ebp
		and	ebp, 0FFFF0000h
		xor	edx, esi
		and	edi, 0FFFFh
		xor	ebx, ebp
		xor	eax, edi
		xor	edx, edi
		mov    dword[gost34_sys+30h], eax
		mov    dword[gost34_sys+34h], ebx
		mov    dword[gost34_sys+38h], ecx
		mov    dword[gost34_sys+3Ch], edx
		mov	edi,dword[gost34_sys+84h]
		mov	esi,dword[gost34_sys+20h]
		mov	ebp,dword[gost34_sys+24h]
		mov	eax,dword[edi]
		mov	ebx,dword[edi+4]
		shr	esi, 10h
		mov	ecx, ebp
		shl	ebp, 10h
		mov	edx,dword[gost34_sys+28h]
		xor	eax, esi
		mov	esi, edx
		shl	edx, 10h
		xor	eax, ebp
		shr	ecx, 10h
		xor	ebx, edx
		mov	ebp,dword[gost34_sys+2Ch]
		xor	ebx, ecx
		mov    dword[gost34_sys+40h], eax
		mov    dword[gost34_sys+44h], ebx
		mov	eax,dword[edi+8]
		mov	ebx,dword[edi+0Ch]
		shr	esi, 10h
		mov	ecx, ebp
		shl	ebp, 10h
		mov	edx,dword[gost34_sys+30h]
		xor	eax, esi
		mov	esi, edx
		shl	edx, 10h
		xor	eax, ebp
		shr	ecx, 10h
		xor	ebx, edx
		mov	ebp,dword[gost34_sys+34h]
		xor	ebx, ecx
		mov    dword[gost34_sys+48h], eax
		mov    dword[gost34_sys+4Ch], ebx
		mov	eax,dword[edi+10h]
		mov	ebx,dword[edi+14h]
		shr	esi, 10h
		mov	ecx, ebp
		shl	ebp, 10h
		mov	edx,dword[gost34_sys+38h]
		xor	eax, esi
		mov	esi, edx
		shl	edx, 10h
		xor	eax, ebp
		shr	ecx, 10h
		xor	ebx, edx
		mov	ebp,dword[gost34_sys+3Ch]
		xor	ebx, ecx
		mov    dword[gost34_sys+50h], eax
		mov    dword[gost34_sys+54h], ebx
		mov	ecx, esi
		mov	edx, ebp
		mov	eax,dword[edi+18h]
		shr	ecx, 10h
		mov	ebx,dword[edi+1Ch]
		shl	edx, 10h
		xor	eax, ecx
		shl	esi, 10h
		mov	ecx, ebp
		xor	eax, edx
		shr	ebp, 10h
		xor	ebx, esi
		and	ecx, 0FFFF0000h
		xor	ebx, ebp
		mov	esi,dword[gost34_sys+20h]
		xor	ebx, ecx
		mov	ebp,dword[gost34_sys+24h]
		mov	ecx, esi
		mov	edx, ebp
		shl	esi, 10h
		and	ecx, 0FFFF0000h
		xor	ebx, esi
		shl	ebp, 10h
		xor	ebx, ecx
		and	edx, 0FFFF0000h
		xor	ebx, ebp
		xor	ebx, edx
		mov    dword[gost34_sys+58h], eax
		mov    dword[gost34_sys+5Ch], ebx
		mov	edi, eax
		mov	esi, ebx
		mov	ebp, ebx
		and	ebx, 0FFFF0000h
		mov	ecx, eax
		shl	edi, 10h
		xor	ebx, eax
		shr	eax, 10h
		mov	edx, edi
		shr	ebp, 10h
		xor	ecx, eax
		xor	ebx, edi
		mov	edi, esi
		and	esi, 0FFFFh
		xor	ebx, ebp
		shl	edi, 10h
		xor	ebp, esi
		mov	esi,dword[gost34_sys+40h]
		xor	eax, edi
		xor	ecx, edi
		mov	edi, esi
		xor	edx, ebp
		xor	eax, ebp
		xor	ecx, ebp
		and	edi, 0FFFFh
		mov	ebp, esi
		shl	esi, 10h
		xor	ecx, edi
		mov	edi, ebp
		shr	ebp, 10h
		xor	ecx, esi
		and	edi, 0FFFF0000h
		xor	esi, ebp
		mov	ebp,dword[gost34_sys+44h]
		xor	esi, edi
		mov	edi, ebp
		and	ebp, 0FFFFh
		xor	ebx, esi
		xor	eax, esi
		xor	edx, esi
		mov	esi, edi
		shr	edi, 10h
		xor	ebx, ebp
		shl	ebp, 10h
		and	esi, 0FFFF0000h
		xor	ecx, ebp
		mov	ebp,dword[gost34_sys+48h]
		xor	edi, esi
		mov	esi,dword[gost34_sys+4Ch]
		xor	eax, edi
		xor	edx, edi
		xor	ecx, edi
		mov	edi, ebp
		xor	ebx, ebp
		xor	edx, ebp
		shl	ebp, 10h
		xor	ecx, esi
		shr	edi, 10h
		xor	eax, ebp
		xor	ecx, ebp
		xor	edx, ebp
		mov	ebp, esi
		shr	esi, 10h
		xor	ebx, edi
		xor	edx, edi
		mov	edi,dword[gost34_sys+50h]
		shl	ebp, 10h
		xor	eax, esi
		xor	ecx, esi
		mov	esi,dword[gost34_sys+54h]
		xor	ebx, ebp
		xor	edx, ebp
		mov	ebp, esi
		xor	eax, esi
		shl	esi, 10h
		xor	edx, edi
		shr	ebp, 10h
		xor	ebx, esi
		xor	edx, esi
		mov	esi, edi
		shl	edi, 10h
		xor	eax, ebp
		shr	esi, 10h
		xor	ecx, ebp
		mov	ebp,dword[gost34_sys+84h]
		xor	eax, edi
		xor	ebx, esi
		xor	edx, esi
		mov	esi,dword[gost34_sys+40h]
		xor	ecx, edi
		mov    dword[ebp+0], eax
		mov    dword[ebp+4], ebx
		mov    dword[ebp+8], ecx
		mov    dword[ebp+0Ch], edx
		mov	eax, esi
		mov	edi,dword[gost34_sys+44h]
		mov	ebx, esi
		mov	ecx, esi
		mov	ebp, edi
		and	edi, 0FFFF0000h
		and	esi, 0FFFF0000h
		shr	eax, 10h
		mov	edx, ebx
		shl	ebx, 10h
		xor	edx, eax
		xor	ebx, edi
		shr	edi, 10h
		xor	eax, ebp
		shl	ebp, 10h
		xor	ebx, esi
		mov	esi,dword[gost34_sys+48h]
		xor	edi, ebp
		xor	eax, ebp
		mov	ebp,dword[gost34_sys+4Ch]
		xor	edx, edi
		xor	ebx, edi
		mov	edi, esi
		xor	edx, ebp
		xor	eax, esi
		xor	ebx, esi
		xor	ecx, ebp
		xor	eax, ebp
		xor	ebx, ebp
		shl	edi, 10h
		xor	ecx, esi
		shr	esi, 10h
		xor	ebx, edi
		xor	edx, edi
		mov	edi, ebp
		shl	ebp, 10h
		xor	eax, esi
		shr	edi, 10h
		xor	ecx, esi
		mov	esi,dword[gost34_sys+50h]
		xor	eax, ebp
		xor	ecx, ebp
		mov	ebp,dword[gost34_sys+54h]
		xor	edx, edi
		xor	ebx, edi
		xor	eax, edi
		mov	edi, ebp
		xor	edx, ebp
		xor	ecx, ebp
		shl	edi, 10h
		xor	eax, ebp
		shr	ebp, 10h
		xor	ebx, esi
		xor	ecx, edi
		xor	edx, esi
		xor	eax, ebp
		xor	ecx, esi
		xor	ebx, edi
		mov	edi, esi
		shl	esi, 10h
		xor	edx, ebp
		shr	edi, 10h
		xor	eax, esi
		xor	ecx, ebp
		mov	ebp,dword[gost34_sys+58h]
		xor	ebx, esi
		xor	edx, esi
		mov	esi,dword[gost34_sys+5Ch]
		xor	ecx, edi
		xor	ebx, edi
		mov	edi, esi
		xor	ecx, ebp
		xor	ebx, ebp
		ror	ebp, 10h
		and	edi, 0FFFF0000h
		xor	ecx, esi
		xor	edx, esi
		shl	esi, 10h
		xor	ebx, edi
		shr	edi, 10h
		xor	eax, ebp
		xor	ecx, ebp
		xor	ebx, ebp
		xor	edx, esi
		xor	eax, esi
		xor	ebx, edi
		mov	edi,dword[gost34_sys+84h]
		xor	ecx, esi
		xor	ebx, esi
		xor	edx, ebp
		mov    dword[edi+10h], eax
		mov    dword[edi+14h], ebx
		mov    dword[edi+18h], ecx
		mov    dword[edi+1Ch], edx
		;
		POPAD
RET


; GOST R 34.11-94 sum calculating modulo 256 bits procedure for i386 compatible processors.
; It can be used in any GOST R 34.11-94 x86 implementation.
; ESI register points to previous sum (256 bits - 32 bytes),
; EDX register points to the DATA block (256 bits - 32 bytes).
; Result placed over previous sum.
align 8
GOST_R_341194_CS:
    pushad
    ;
    mov  eax,	dword[esi + 0*0]
    mov  edi,	dword[esi + 1*4]
    mov  ecx,	dword[edx + 0*0]
    mov  ebp,	dword[edx + 1*4]
    add  eax,	ecx
    mov  ecx,	dword[edx + 2*4]
    adc  edi,	ebp
    mov dword[esi], eax
    mov  ebp,	dword[edx + 3*4]
    mov dword[esi + 1*4], edi
    mov  eax,	dword[esi + 2*4]
    mov  edi,	dword[esi + 3*4]
    adc  eax,	ecx
    mov  ecx,	dword[edx + 4*4]
    adc  edi,	ebp
    mov dword[esi + 2*4], eax
    mov  ebp,	dword[edx + 5*4]
    mov dword[esi + 3*4], edi
    mov  eax,	dword[esi + 4*4]
    mov  edi,	dword[esi + 5*4]
    adc  eax,	ecx
    mov  ecx,	dword[edx + 6*4]
    adc  edi,	ebp
    mov dword[esi + 4*4], eax
    mov  ebp,	dword[edx + 7*4]
    mov dword[esi + 5*4], edi
    mov  eax,	dword[esi + 6*4]
    mov  edi,	dword[esi + 7*4]
    adc  eax,	ecx
    adc  edi,	ebp
    mov dword[esi + 6*4], eax
    mov dword[esi + 7*4], edi
    ; end of code
    popad
ret
