; LZWLIB 1.12
;#define ERROR_INVALID_LZW_SIGNATURE 0x01
;#define ERROR_INVALID_CRC32	     0x02
;#define ERROR_DATA_IS_CURRUPTED     0x03
;#define ERROR_DATA_IS_CURRUPTED2    0x04
;#define ERROR_HEADER_IS_CORRUPTED   0x05
;     import Lzwlib, \
;	     GetDecompressedSize, 'lzwGetDecompressedSize', \
;	     Decompress,	  'lzwDecompress',\
;	     GetRequiredMemSize,  'lzwGetRequiredMemSize', \
;	     Compress,		  'lzwCompress'
;
;	 invoke lzwGetRequiredMemSize, end_data - begin_data	       ; Получаем размер
;	 mov [size], eax
;	 invoke GlobalAlloc, GMEM_FIXED, eax		 ; Выделяем блок памяти такого же размера
;	 mov [data], eax
;	 invoke lzwCompress, begin_data, end_data - begin_data, [data]
;	 mov [size], eax
;	 invoke lzwGetDoneCompressWork

;	 invoke lzwGetDecompressedSize, begin_data
;	 mov [size], eax
;	 invoke GlobalAlloc, GMEM_FIXED, eax		 ; Выделяем блок памяти такого же размера
;	 mov [data], eax
;	 invoke lzwDecompress, begin_data, end_data - begin_data, [data]
;	 invoke lzwGetDoneDecompressWork
;	 const char *lzwGetErrorCodeString(Dword errCode);




use32







; CODE
; ***************************************************************************
align 4
lzwGetRequiredMemSize:
		mov	eax, dword[esp+4]
		or	eax, eax
		jnz	@f
		mov	eax, 0x0C
		jmp    .exit
	    @@: test	al, 1
		jz	@f
		inc	eax
	    @@: lea	eax, [eax+eax*2]
		shr	eax, 1
		add	eax, 0x0C
	 .exit: ret	4*1


align 4
lzwGetDecompressedSize:
		mov	eax, dword[esp+4]
		cmp	dword[eax], 0x21575A4C
		jz	@f
		xor	eax, eax
		jmp    .exit
	    @@: mov	eax, dword[eax+4]
	 .exit: ret	4

; ***************************************************************************
lzwGetDoneCompressWork:
loc_1000102D:
		mov	eax, [dword_10008900]
		ret

; ***************************************************************************
lzwGetDoneDecompressWork:
loc_10001033:
		mov	eax, [dword_10008904]
		ret

align 4
sub_10001039:
		push	ebp
		mov	ebp, esp
		cmp	dword [ebp+0x0C], 0
		jnz	@f
		xor	eax, eax
		pop	ebp
		retn
	    @@: push	esi
		push	edi
		mov	esi, [ebp+0x08]
		mov	edi, [ebp+0x0C]
		cld
		xor	edx, edx
		dec	edx
	 .next: xor	eax, eax
		lodsb
		xor	al, dl
		xor	ecx, ecx
		mov	cl, 8
		shr	edx, cl
	 .loop: clc
		rcr	eax, 1
		jnb	@f
		xor	eax, 0EDB88320h
	    @@: loop   .loop
		xor	edx, eax
		dec	edi
		jnz    .next
		mov	eax, edx
		not	eax
		pop	edi
		pop	esi
		pop	ebp
		retn


lzwGetErrorCodeString:
loc_10001076:
		mov	eax, [esp+4]
		dec	eax
		jz     .pErrorInvalidLz
		dec	eax
		jz     .pWarningInvalid
		dec	eax
		jz     .pErrorPackedDat
		dec	eax
		jz     .pWarningPossibl
		dec	eax
		jz     .pErrorLzwHeader
		mov	eax, aUnknownError ; "Unknown error"
		retn
	    .pErrorLzwHeader:
		mov	eax, aErrorLzwHeader ; "Error: Lzw header is corrupted"
		retn
	    .pWarningPossibl:
		mov	eax, aWarningPossibl ; "Warning: Possible corrupted data or inv"...
		retn
	    .pErrorPackedDat:
		mov	eax, aErrorPackedDat ; "Error: Packed data is corrupted (invali"...
		retn
	    .pWarningInvalid:
		mov	eax, aWarningInvalid ; "Warning: Invalid Crc32"
		retn
	    .pErrorInvalidLz:
		mov	eax, aErrorInvalidLz ; "Error: Invalid Lzw signature"
		retn

; ***************************************************************************
lzwCompress:
loc_100010AD:				; CODE XREF: _lzwCompressj
		push	ebp
		mov	ebp, esp
		mov	eax, 0C02Ch
		call	__alloca_probe
		and	byte[ebp-2], 0
		and	[dword_10008900], 0
		push	ebx
		mov	ebx, [ebp+10h]
		push	esi
		push	edi
		mov	edi, [ebp+0Ch]
		push	edi
		push	dword[ebp+8]
		mov	esi, 100h
		mov	dword[ebp-20h], 9
		mov	[ebp-24h], esi
		mov	dword[ebx], 21575A4Ch
		mov	[ebx+4], edi
		call	sub_10001039
		pop	ecx
		pop	ecx
		push	0Ch
		pop	ecx
		xor	edx, edx
		mov	[ebx+8], eax
		mov	[ebp-10h], ecx
		mov	[ebp-14h], edx
		mov	[ebp-0Ch], esi
		lea	eax, [ebp-0C024h]

loc_10001108:				; CODE XREF: .text:10001117j
		mov	[eax-8], edx
		mov	[eax], edx
		mov	dword[eax-4], 1
		add	eax, ecx
		dec	esi
		jnz	 loc_10001108
		xor	esi, esi
		cmp	edi, edx
		jbe	loc_10001358

loc_10001123:				; CODE XREF: .text:10001346j
		mov	eax, [ebp+8]
		add	eax, esi
		movzx	edi, byte[eax]
		lea	eax, [edi+edi*2]
		mov	ecx, [ebp+eax*4-0C024h]
		test	ecx, ecx
		mov	[ebp-2Ch], edi
		mov	[ebp-1Ch], edi
		jz	loc_100011FA
		mov	dword[ebp-8], 1
		lea	eax, [esi+1]
		jmp	loc_100011EE

loc_10001152:				; CODE XREF: .text:100011F4j
		test	ecx, ecx
		jz	loc_100011FA
		lea	ecx, [ecx+ecx*2]
		shl	ecx, 2
		mov	eax, [ebp+ecx-0C028h]
		cmp	eax, [ebp-8]
		jbe	loc_100011E2
		add	eax, esi
		cmp	eax, [ebp+0Ch]
		ja	loc_100011E2
		mov	edi, [ebp-8]
		mov	edx, [ebp+8]
		mov	eax, [ebp+ecx-0C02Ch]
		add	edx, esi
		mov	dl, [edx+edi]
		lea	ebx, [eax+edi]
		mov	edi, [ebp+8]
		cmp	dl, [ebx+edi]
		jnz	loc_100011DC
		and	dword  [ebp-18h], 0
		mov	byte  [ebp-1], 1
		add	eax, edi

loc_1000119A:				; CODE XREF: .text:100011BFj
		mov	edx, [ebp-18h]
		cmp	edx, [ebp+ecx-0C028h]
		jnb	loc_100011C1
		mov	ebx, [ebp-18h]
		lea	edx, [esi+edi]
		mov	dl, [edx+ebx]
		cmp	dl, [eax]
		jz	loc_100011B7
		and	byte  [ebp-1], 0
loc_100011B7:	inc	dword[ebp-18h]
		inc	eax
		cmp	byte[ebp-1], 0
		jnz	loc_1000119A
loc_100011C1:	cmp	byte[ebp-1], 0
		mov	ebx, [ebp+10h]
		jz	loc_100011DF
		mov	eax, [ebp+ecx-0C028h]
		mov	edi, [ebp-28h]
		mov	[ebp-8], eax
		mov	[ebp-1Ch], edi
		jmp	loc_100011E2
loc_100011DC:	mov	ebx, [ebp+10h]
loc_100011DF:	mov	edi, [ebp-1Ch]
loc_100011E2:	mov	eax, [ebp-8]
		mov	ecx, [ebp+ecx-0C024h]
		add	eax, esi
loc_100011EE:	cmp	eax, [ebp+0Ch]
		mov	[ebp-28h], ecx
		jb	loc_10001152
loc_100011FA:	cmp	dword  [ebp-14h], 0F00h
		jnz	 loc_10001244
		mov	eax, [ebp-0Ch]
		mov	edx, [ebp+8]
		lea	eax, [eax+eax*2]
		shl	eax, 2
		mov	ecx, [ebp+eax-0C02Ch]
		movzx	ecx, byte  [ecx+edx]
		mov	eax, [ebp+eax-0C024h]
		inc	dword  [ebp-0Ch]
		lea	ecx, [ecx+ecx*2]
		mov	[ebp+ecx*4-0C024h], eax
		mov	eax, [ebp-0Ch]
		cdq
		mov	ecx, 1000h
		idiv	ecx
		test	eax, eax
		jz	 loc_10001255
		mov	dword  [ebp-0Ch], 100h
loc_10001244:	mov	eax, [ebp-14h]
		cmp	eax, 0F00h
		jge	 loc_10001255
		mov	ecx, [ebp-0Ch]
		add	ecx, eax
		jmp	 loc_10001266
loc_10001255:	mov	ecx, [ebp-0Ch]
		dec	ecx
		cmp	ecx, 0FFh
		jnz	 loc_10001266
		mov	ecx, 0FFFh
loc_10001266:	mov	eax, [ebp-2Ch]
		jmp	 loc_1000126D
loc_1000126B:	mov	eax, [edx]
loc_1000126D:	lea	edx, [eax+eax*2]
		lea	edx, [ebp+edx*4-0C024h]
		cmp	dword  [edx], 0
		jnz	 loc_1000126B
		lea	eax, [eax+eax*2]
		mov	[ebp+eax*4-0C024h], ecx
		lea	eax, [ecx+ecx*2]
		shl	eax, 2
		lea	edx, [edi+edi*2]
		mov	[ebp+eax-0C02Ch], esi
		mov	edx, [ebp+edx*4-0C028h]
		and	dword  [ebp+eax-0C024h], 0
		inc	edx
		cmp	dword  [ebp-14h], 0F00h
		lea	ecx, [ebp+eax-0C028h]
		mov	[ecx], edx
		jge	 loc_100012BB
		inc	dword  [ebp-14h]
loc_100012BB:	mov	eax, [ecx]
		mov	ecx, [ebp-20h]
		lea	esi, [esi+eax-1]
		xor	eax, eax
		inc	eax
		shl	eax, cl
		mov	[dword_10008900], esi
		cmp	eax, [ebp-24h]
		jnz	 loc_100012DD
		cmp	ecx, 0Ch
		jge	 loc_100012DD
		inc	ecx
		mov	[ebp-20h], ecx
loc_100012DD:	and	dword  [ebp-8], 0
		test	ecx, ecx
		jle	 loc_10001340
loc_100012E5:	mov	eax, [ebp-10h]
		mov	al, [eax+ebx]
		mov	ecx, [ebp-8]
		mov	[ebp-1], al
		xor	eax, eax
		inc	eax
		shl	eax, cl
		push	7
		mov	dl, 1
		pop	ecx
		test	edi, eax
		movzx	eax, byte  [ebp-2]
		jz	 loc_1000130C
		sub	ecx, eax
		shl	dl, cl
		or	[ebp-1], dl
		jmp	 loc_10001315
loc_1000130C:	sub	ecx, eax
		shl	dl, cl
		not	dl
		and	[ebp-1], dl
loc_10001315:	mov	ecx, [ebp-10h]
		mov	dl, [ebp-1]
		inc	eax
		mov	[ecx+ebx], dl
		push	8
		cdq
		pop	ecx
		idiv	ecx
		test	dl, dl
		mov	[ebp-2], dl
		jnz	 loc_10001335
		inc	dword  [ebp-10h]
		mov	eax, [ebp-10h]
		and	[eax+ebx], dl
loc_10001335:	inc	dword  [ebp-8]
		mov	eax, [ebp-8]
		cmp	eax, [ebp-20h]
		jl	 loc_100012E5
loc_10001340:	inc	dword  [ebp-24h]
		cmp	esi, [ebp+0Ch]
		jb	loc_10001123
		cmp	byte  [ebp-2], 0
		jz	 loc_10001358
		mov	eax, [ebp-10h]
		inc	eax
		jmp	 loc_1000135B
loc_10001358:	mov	eax, [ebp-10h]
loc_1000135B:	pop	edi
		pop	esi
		pop	ebx
		leave
		retn

; ***************************************************************************
lzwDecompress:
loc_10001360:				; CODE XREF: _lzwDecompressj
		push	ebp
		mov	ebp, esp
		mov	eax, 0xC024
		call	__alloca_probe
		mov	ecx, [ebp+0x0C]
		pushd	esi
		pushd	0xC
		pop	esi
		cmp	ecx, esi
		ja	loc_10001380
		pushd	5
		pop	eax
		jmp	loc_1000142E

loc_10001380:	mov	eax, [ebp+8]
		cmp	dword[eax], 0x21575A4C
		jz	loc_10001393
		xor	eax, eax
		inc	eax
		jmp	loc_1000142E

loc_10001393:				; CODE XREF: .text:10001389j
		push	ebx
		push	edi

		push	eax
		call	lzwGetDecompressedSize

		and	byte [ebp+0Fh], 0
		add	esp, 4
		push	9
		pop	edi
		mov	[ebp-10h], eax
		lea	eax, dword [0FFFFFFA0h+ecx*8]
		mov	edx, 100h
		xor	ebx, ebx
		cmp	eax, edi
		mov	[ebp-20h], esi
		mov	[ebp-0Ch], edi
		mov	[ebp-14h], edx
		mov	[dword_10008904], ebx
		mov	[ebp-18h], edx
		mov	ecx, edi
		jl	short loc_100013E8

loc_100013CE:				; CODE XREF: .text:100013E4j
		xor	edi, edi
		inc	edi
		shl	edi, cl
		cmp	[ebp-18h], edi
		jnz	short loc_100013DD
		cmp	ecx, esi
		jz	short loc_100013DD
		inc	ecx

loc_100013DD:				; CODE XREF: .text:100013D6j
					; .text:100013DAj
		inc	dword [ebp-18h]
		sub	eax, ecx
		cmp	eax, ecx
		jge	short loc_100013CE
		xor	ebx, ebx

loc_100013E8:				; CODE XREF: .text:100013CCj
		mov	[ebp-1Ch], ebx
		mov	[ebp-8], edx
		lea	eax, [ebp-0C01Ch]
		mov	ecx, edx

loc_100013F6:				; CODE XREF: .text:10001405j
		mov	[eax-8], ebx
		mov	[eax], ebx
		mov	dword [eax-4], 1
		add	eax, esi
		dec	ecx
		jnz	short loc_100013F6
		xor	esi, esi
		cmp	[ebp-10h], ebx
		ja	short loc_10001433

loc_1000140E:				; CODE XREF: .text:100015A7j
		push	dword [ebp-10h]
		push	dword [ebp+10h]
		call	sub_10001039
		pop	ecx
		pop	ecx
		mov	ecx, eax
		mov	eax, [ebp+8]
		mov	eax, [eax+8]
		sub	eax, ecx
		neg	eax
		sbb	eax, eax
		and	eax, 2

loc_1000142C:				; CODE XREF: .text:100015B4j
		pop	edi
		pop	ebx

loc_1000142E:				; CODE XREF: .text:1000137Bj
					; .text:1000138Ej
		pop	esi
		leave
		retn

loc_10001431:				; CODE XREF: .text:1000159Ej
		xor	ebx, ebx

loc_10001433:				; CODE XREF: .text:1000140Cj
		mov	eax, [ebp-14h]
		cmp	eax, [ebp-18h]
		jnb	loc_100015A4
		cmp	[ebp-0Ch], ebx
		mov	[ebp-4], ebx
		jle	short loc_1000149B

loc_10001447:				; CODE XREF: .text:10001491j
		movzx	edi, byte [ebp+0Fh]
		mov	eax, [ebp-20h]
		lea	eax, [edi+eax*8]
		push	8
		cdq
		pop	ecx
		idiv	ecx
		push	7
		pop	ecx
		sub	ecx, edx
		xor	edx, edx
		inc	edx
		shl	edx, cl
		mov	ecx, [ebp+8]
		mov	al, [eax+ecx]
		test	al, dl
		jz	short loc_10001475
		mov	ecx, [ebp-4]
		xor	eax, eax
		inc	eax
		shl	eax, cl
		or	ebx, eax

loc_10001475:				; CODE XREF: .text:10001469j
		lea	eax, [edi+1]
		push	8
		cdq
		pop	ecx
		idiv	ecx
		test	dl, dl
		mov	[ebp+0Fh], dl
		jnz	short loc_10001488
		inc	dword [ebp-20h]

loc_10001488:				; CODE XREF: .text:10001483j
		inc	dword [ebp-4]
		mov	eax, [ebp-4]
		cmp	eax, [ebp-0Ch]
		jl	short loc_10001447
		cmp	ebx, 0FFh
		ja	short loc_100014A3

loc_1000149B:				; CODE XREF: .text:10001445j
		mov	eax, [ebp+10h]
		mov	[esi+eax], bl
		jmp	short loc_10001503

loc_100014A3:				; CODE XREF: .text:10001499j
		mov	eax, [ebp-1Ch]
		cmp	eax, 0F00h
		jge	short loc_100014BA
		add	eax, 0FFh
		cmp	ebx, eax
		ja	loc_100015B1

loc_100014BA:				; CODE XREF: .text:100014ABj
		lea	eax, [ebx+ebx*2]
		shl	eax, 2
		lea	ecx, [ebp+eax-0C020h]
		mov	edx, [ecx]
		lea	edi, [edx+esi]
		cmp	edi, [ebp-10h]
		ja	loc_100015B1
		and	dword [ebp-4], 0
		test	edx, edx
		jbe	short loc_10001503
		mov	eax, [ebp+eax-0C024h]
		mov	ecx, edx
		mov	[ebp-24h], ecx
		mov	ecx, [ebp+10h]
		add	eax, ecx
		lea	edi, [esi+ecx]

loc_100014F1:				; CODE XREF: .text:10001501j
		mov	ecx, [ebp-4]
		mov	dl, [eax]
		mov	[edi+ecx], dl
		inc	ecx
		inc	eax
		cmp	ecx, [ebp-24h]
		mov	[ebp-4], ecx
		jb	short loc_100014F1

loc_10001503:				; CODE XREF: .text:100014A1j
					; .text:100014DBj
		mov	edi, [ebp-1Ch]
		cmp	edi, 0F00h
		jnz	short loc_10001529
		inc	dword [ebp-8]
		mov	eax, [ebp-8]
		cdq
		mov	ecx, 1000h
		idiv	ecx
		test	eax, eax
		jz	short loc_10001532
		mov	dword [ebp-8], 100h
		cmp	edi, edi

loc_10001529:				; CODE XREF: .text:1000150Cj
		jge	short loc_10001532
		mov	eax, [ebp-8]
		add	eax, edi
		jmp	short loc_10001542

loc_10001532:				; CODE XREF: .text:1000151Ej
					; .text:10001529j
		mov	eax, [ebp-8]
		dec	eax
		cmp	eax, 0FFh
		jnz	short loc_10001542
		mov	eax, 0FFFh

loc_10001542:				; CODE XREF: .text:10001530j
					; .text:1000153Bj
		lea	eax, [eax+eax*2]
		shl	eax, 2
		lea	edx, [ebx+ebx*2]
		mov	[ebp+eax-0C024h], esi
		mov	edx, [ebp+edx*4-0C020h]
		and	dword [ebp+eax-0C01Ch], 0
		inc	edx
		cmp	edi, 0F00h
		lea	ecx, [ebp+eax-0C020h]
		mov	[ecx], edx
		jge	short loc_10001577
		inc	edi
		mov	[ebp-1Ch], edi

loc_10001577:				; CODE XREF: .text:10001571j
		mov	eax, [ecx]
		mov	ecx, [ebp-0Ch]
		inc	dword [ebp-14h]
		lea	esi, [esi+eax-1]
		xor	eax, eax
		inc	eax
		shl	eax, cl
		mov	[dword_10008904], esi
		cmp	eax, [ebp-14h]
		jnz	short loc_1000159B
		cmp	ecx, 0Ch
		jge	short loc_1000159B
		inc	dword [ebp-0Ch]

loc_1000159B:				; CODE XREF: .text:10001591j
					; .text:10001596j
		cmp	esi, [ebp-10h]
		jb	loc_10001431

loc_100015A4:				; CODE XREF: .text:10001439j
		cmp	esi, [ebp-10h]
		jnb	loc_1000140E
		push	4
		jmp	short loc_100015B3

loc_100015B1:				; CODE XREF: .text:100014B4j
					; .text:100014CFj
		push	3

loc_100015B3:				; CODE XREF: .text:100015AFj
		pop	eax
		jmp	loc_1000142C

_DllMain@12:

		xor	eax, eax
		inc	eax
		retn	0Ch

__alloca_probe:
					; .text:10001368p ...
arg_0		= byte	4

		cmp	eax, 1000h
		jnb	short loc_10001605 ; probesetup
		neg	eax
		add	eax, esp
		add	eax, 4
		test	[eax], eax
		xchg	eax, esp
		mov	eax, [eax]
		push	eax
		retn

loc_10001605:				; CODE XREF: __alloca_probe+5j
		push	ecx		; probesetup
		lea	ecx, [esp+4+arg_0]

loc_1000160A:				; CODE XREF: __alloca_probe+2Cj
		sub	ecx, 1000h	; probepages
		sub	eax, 1000h
		test	[ecx], eax
		cmp	eax, 1000h
		jnb	short loc_1000160A ; probepages
		sub	ecx, eax	; lastpage
		mov	eax, esp
		test	[ecx], eax
		mov	esp, ecx
		mov	ecx, [eax]
		mov	eax, [eax+4]
		push	eax
		retn
