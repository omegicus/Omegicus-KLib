; Реализация алгоритма "Кузнечик"
; Copyright InfoTeCS. All rights reserved. 2015.
; Assembler code by Vladislav Kabak (Omegicus)

include 'arch.inc'



align 8
gost14_funcX:
;(unsigned char* a, unsigned char* b, unsigned char* outdata)
;     for(i = 0; i < 16; ++i) { outdata[i] = a[i] ^ b[i]; }
     mov      rax, rsp
     pushad
     mov      rbp, rax
     ;
     xor     rcx, rcx			    ; i
     mov     rbx, opsz[rbp + opsa*1]	    ; *a
     mov     rdx, opsz[rbp + opsa*2]	    ; *b
     mov     rdi, opsz[rbp + opsa*3]	    ; *outdata
     ;					    ;
     align   4
 @@: mov     rab, byte[rbx + rcx]	    ;
     xor     rab, byte[rdx + rcx]	    ;
     mov     byte[rdi + rcx], rab	    ;
     inc     rcx
     cmp     rcx, 16
     jne     opsz @b
     ;
     popad
ret


align 8
gost14_funcS:
;(unsigned char* indata, unsigned char* outdata)
     mov      rax, rsp
     pushad
     mov      rbp, rax
     ;
;     for(i = 0; i < 16; ++i)
;     {
;	   outdata[i] = kPi[indata[i]];
;     }
     xor     rcx, rcx			    ; i
     mov     rsi, opsz[rbp + opsa*1]	    ; *indata
     mov     rdi, opsz[rbp + opsa*2]	    ; *outdata
     ;					    ;
     align 4
 @@: movzx   rdx, byte[rsi + rcx]	    ; indata[i]
     mov     rab, byte[gost14_kPi + rdx]    ; gost14_kPi[indata[i]]
     mov     byte[rdi + rcx], rab	    ; outdata[i] = gost14_kPi[indata[i]]
     inc     rcx
     cmp     rcx, 16
     jne     opsz @b
     ;
     popad
ret

align 8
gost14_funcReverseS:
; unsigned char* indata, unsigned char*  outdata
     mov      rax, rsp
     pushad
     mov      rbp, rax
     ;
     xor     rcx, rcx			    ; i
     mov     rsi, opsz[rbp + opsa*1]	    ; *indata
     mov     rdi, opsz[rbp + opsa*2]	    ; *outdata
     ;
 @@: movzx   rdx, byte[rsi + rcx]	    ; indata[i]
     mov     rab, byte[gost14_kRevPi + rdx] ; gost14_kRevPi[indata[i]]
     mov     byte[rdi + rcx], rab	    ; outdata[i] = gost14_kRevPi[indata[i]]
     ;
     inc     rcx
     cmp     rcx, 16
     jne     opsz @b
     ;
     popad
ret





macro g14_R_loop i {
     xor     rdx, rdx
     mov      dl, byte[rsi + i] 	    ; indata[i]
     shl     rdx, 8			    ; indata[i]*256
     xor     rbx, rbx
     mov      bl, byte[gost14_kB + i]	    ; kB[i]
     add     rdx, rbx			    ; indata[i]*256 + kB[i]
     mov      bl, byte[gost14_mT + rdx]     ; multTable[indata[i]*256 + kB[i]]
     xor     rax, rbx			    ; sum ^= multTable[indata[i]*256 + kB[i]]
}
macro gost14_funcR {
     mov     rcx, rbx
     xor     rax, rax			    ;  sum
     ;
     g14_R_loop 0
     g14_R_loop 1
     g14_R_loop 2
     g14_R_loop 3

     g14_R_loop 4
     g14_R_loop 5
     g14_R_loop 6
     g14_R_loop 7

     g14_R_loop 8
     g14_R_loop 9
     g14_R_loop 10
     g14_R_loop 11

     g14_R_loop 12
     g14_R_loop 13
     g14_R_loop 14
     g14_R_loop 15

;     ; memcpy(outdata+1, indata, 15):

;     emms
;     ; outdata[0] = sum:
;     mov     byte[rdi + 00], rab

     ; outdata[0] = sum:
     mov     byte[rdi + 00], rab
     ; memcpy(outdata+1, indata, 15):
     ;inc     rdi
     ;movsd
     ;movsd
     ;movsd
     ;movsw
     ;movsb
     movq    mm0, qword[esi + 00]
     movq    qword[edi+01], mm0
     mov     rax, dword[esi + 08]
     mov     dword[edi+09], rax
     mov      ax,  word[esi + 12]
     mov      word[edi+13],  ax
     mov      al,  byte[esi + 14]
     mov      byte[edi+15],  al
     ;
     mov     rbx, rcx
}

align 8
gost14_funcReverseR:
     mov      rax, rsp
     pushad
     mov      rbp, rax
; unsigned char* indata, unsigned char* outdata
     ; unsigned char tmp[16] = {0};
     mov     rdi, gost14_tmp
     xor     rax, rax
     mov     rcx, 16/4
     cld
     rep     stosd
     ;
     xor     rcx, rcx			    ; i
     mov     rsi, opsz[rbp + opsa*1]	    ; *indata
     mov     rdi, opsz[rbp + opsa*2]	    ; *outdata
     xor     rax, rax			    ; sum

     ; memcpy(tmp, indata+1, 15)
     inc     rsi
     mov     rcx, 15
     mov     rdi, gost14_tmp
     cld
     rep     movsb
     ; tmp[15] = indata[0]
     mov     rsi, opsz[rbp + opsa*1]	    ; *indata
     mov     rab, byte[rsi + 00]
     mov     byte[gost14_tmp + 15], rab

     ; for(i = 0; i < 16; ++i)	sum ^= multTable[tmp[i]*256 + kB[i]];
     xor     rcx, rcx			    ; i
     xor     rax, rax			    ; sum
 @@: movzx   rdx, byte[gost14_tmp + rcx]    ; tmp[i]
     shl     rdx, 8			    ; tmp[i]*256
     movzx   rbx, byte[gost14_kB + rcx]     ; kB[i]
     add     rdx, rbx			    ; tmp[i]*256 + kB[i]
     movzx   rbx, byte[gost14_mT + rdx]     ; multTable[tmp[i]*256 + kB[i]]
     xor     rax, rbx			    ; sum ^= multTable[indata[i]*256 + kB[i]]
     ;
     inc     rcx
     cmp     rcx, 16
     jne     opsz @b
     ; memcpy(outdata, tmp, 15);
     mov     rdi, opsz[rbp + opsa*2]	    ; *outdata
     mov     rsi, gost14_tmp
     mov     rcx, 15
     cld
     rep     movsb
     ; outdata[15] = sum;
     mov     rdi, opsz[rbp + opsa*2]	    ; *outdata
     mov     byte[rdi + 15], rab
     ;
     popad
ret


macro gost14_funcL_loop {
     ; funcR(tmp, outdata):
     mov     rsi, gost14_Ltmp
     mov     rdi, rbx
     gost14_funcR ; esi edi
     ; memcpy(tmp, outdata, 16):
     mov     rsi, rbx
     mov     rdi, gost14_Ltmp
     ;
     movq    mm0, qword [esi+00]
     movq    mm1, qword [esi+08]
     movq    qword[edi+00], mm0
     movq    qword[edi+08], mm1
     ;movsd
     ;movsd
     ;movsd
     ;movsd
}

macro gost14_funcL { ; esi indata,  edi outdata
     ; memcpy(tmp, indata, 16)
     mov     rbx, rdi		; outdata
     mov     rdi, gost14_Ltmp
     movq    mm0, qword [esi+00]
     movq    mm1, qword [esi+08]
     movq    qword[edi+00], mm0
     movq    qword[edi+08], mm1
     ;movsd
     ;movsd
     ;movsd
     ;movsd
     ;
     gost14_funcL_loop
     gost14_funcL_loop
     gost14_funcL_loop
     gost14_funcL_loop
     ;
     gost14_funcL_loop
     gost14_funcL_loop
     gost14_funcL_loop
     gost14_funcL_loop
     ;
     gost14_funcL_loop
     gost14_funcL_loop
     gost14_funcL_loop
     gost14_funcL_loop
     ;
     gost14_funcL_loop
     gost14_funcL_loop
     gost14_funcL_loop
     gost14_funcL_loop
     ;
     emms
}


align 8
gost14_funcReverseL:
; unsigned char* indata, unsigned char* outdata
;     memcpy(tmp, indata, 16);
;     for(i = 0; i < 16; ++i)
;     {
;	   funcReverseR(tmp, outdata, print);
;	   memcpy(tmp, outdata, 16);
;     }
     mov      rax, rsp
     pushad
     mov      rbp, rax
     ; memcpy(tmp, indata, 16);
     mov     rsi, opsz[rbp + opsa*1]	    ; *indata
     mov     rdi, gost14_Ltmp
     mov     rcx, 16/4
     cld
     rep     movsd
     ;
     xor     rcx, rcx
 @@: mov     rax, opsz[rbp + opsa*2]
     push    rax
     push    opsz gost14_Ltmp
     call    gost14_funcReverseR	    ; gost14_funcR(tmp, outdata)
     add     rsp, 2*opsa

     push    rcx
     mov     rsi, opsz[rbp + opsa*2]
     mov     rdi, gost14_Ltmp
     mov     rcx, 16/4
     cld
     rep     movsd
     pop     rcx
     ;
     inc     rcx
     cmp     rcx, 16
     jne     opsz @b
     popad
ret








align 8
gost14_funcLSX:
;(unsigned char* a, unsigned char* b, unsigned char* outdata)
     mov      rax, rsp
     pushad
     mov      rbp, rax
     ; funcX(a, b, gost14_funcLSX_data1);
     mov     rax, opsz[rbp + opsa*1]	    ; *a
     mov     rbx, opsz[rbp + opsa*2]	    ; *b
     mov     rcx, opsz[rbp + opsa*3]	    ; *outdata
     push    gost14_funcLSX_data1
     push    rbx
     push    rax
     call    gost14_funcX
     add     rsp, 3*opsa
     ; funcS(gost14_funcLSX_data1, gost14_funcLSX_data2);
     push    gost14_funcLSX_data2
     push    gost14_funcLSX_data1
     call    gost14_funcS
     add     rsp, 2*opsa
     ; funcL(gost14_funcLSX_data2, outdata);
     mov     rdi, opsz[rbp + opsa*3]
     mov     rsi, gost14_funcLSX_data2
     gost14_funcL
     popad
ret


align 8
gost14_funcReverseLSX:
;(unsigned char* a, unsigned char* b, unsigned char* outdata)
     mov      rax, rsp
     pushad
     mov      rbp, rax
     ;
     ; funcX(a, b, temp1);
     mov     rax, opsz[rbp + opsa*1]	    ; *a
     mov     rbx, opsz[rbp + opsa*2]	    ; *b
     mov     rcx, opsz[rbp + opsa*3]	    ; *outdata
     push    gost14_funcLSX_data1
     push    rbx
     push    rax
     call    gost14_funcX
     add     rsp, 3*opsa

     ;
     ; funcReverseL(temp1, temp2);
     push    gost14_funcLSX_data2
     push    gost14_funcLSX_data1
     call    gost14_funcReverseL
     add     rsp, 2*opsa
     ;
     ; funcReverseS(temp2, outdata);
     mov     rax, opsz[rbp + opsa*3]	    ; *outdata
     push    rax
     push    gost14_funcLSX_data2
     call    gost14_funcReverseS
     add     rsp, 2*opsa
     ;
     popad
ret

align 8
gost14_funcF:
;(*inputKey, *inputKeySecond, *iterationConst, *outputKey, *outputKeySecond)
     mov      rax, rsp
     pushad
     mov      rbp, rax
     ; funcLSX(inputKey, iterationConst, gost14_funcF_data1):
     push    gost14_funcF_data1
     mov     rax, opsz[rbp + opsa*3]	; iterationConst
     push    rax
     mov     rax, opsz[rbp + opsa*1]	; inputKey
     push    rax
     call    gost14_funcLSX
     add     rsp, 3*opsa

     ; funcX(gost14_funcF_data1, inputKeySecond, gost14_funcF_data2):
     push    gost14_funcF_data2
     mov     rax, opsz[rbp + opsa*2]	; inputKeySecond
     push    rax
     push    gost14_funcF_data1
     call    gost14_funcX
     add     rsp, 3*opsa
     ;
     ; memcpy(outputKeySecond, inputKey, 16):
     mov     rsi, opsz[rbp + opsa*1]
     mov     rdi, opsz[rbp + opsa*5]
     mov     rcx, 16/4
     cld
     rep     movsd
     ; memcpy(outputKey, gost14_funcF_data2, 16):
     mov     rsi, gost14_funcF_data2
     mov     rdi, opsz[rbp + opsa*4]
     mov     rcx, 16/4
     cld
     rep     movsd
     ;
     popad
ret


align 8
gost14_funcC:
;(unsigned char number, unsigned char* output)
     mov      rax, rsp
     pushad
     mov      rbp, rax
     ; gost14_funcC_data[15] = number:
     mov     rax, opsz[rbp + opsa*1]	    ; number
     mov     byte[gost14_funcC_data + 15], al
     ;
     ; funcL(gost14_funcC_data, output):
     mov     rsi, gost14_funcC_data
     mov     rdi, opsz[rbp + opsa*2]
     gost14_funcL

     ;
     popad
ret




align 8
gost14_ExpandKey:
;(unsigned char* masterKey, unsigned char* keys)

     mov      rax, rsp
     pushad
     mov      rbp, rax

     mov      rsi, opsz[rbp + opsa*1] ; masterKey
     mov      rdi, opsz[rbp + opsa*2]
     mov      rcx, 32/4
     cld
     rep      movsd
     ;

     xor      rdx, rdx

    ; for(j = 0; j < 4; ++j)
    ; {
  .for1:
     ; memcpy(gost14_ExpandKey_t1, keys + j * 2 * 16, 16):
     mov      rsi, rdx
     shl      rsi, 5 ; *2*16
     add      rsi, opsz[rbp + opsa*2]
     mov      rdi, gost14_ExpandKey_t1
     mov      rcx, 16/4
     cld
     rep      movsd
     ; memcpy(gost14_ExpandKey_t2, keys + (j * 2 + 1) * 16, 16):
     mov      rsi, rdx
     shl      rsi, 1 ; *2
     inc      rsi
     shl      rsi, 4 ;
     add      rsi, opsz[rbp + opsa*2]
     mov      rdi, gost14_ExpandKey_t2
     mov      rcx, 16/4
     cld
     rep      movsd
     ;
;	   for( i = 1; i < 8; ++i )
;	   {
;		funcC(j*8+i, C);
;		funcF(temp1, temp2, C, temp1, temp2);
;	   }
     ;
     ; for( i = 1; i < 8; ++i ) {
     xor      rbx, rbx
     inc      rbx
    .for2:
     ; funcC(j*8+i, gost14_ExpandKey_c):
     mov      rax, rdx
     shl      rax, 3
     add      rax, rbx
     push     gost14_ExpandKey_c
     push     rax
     call     gost14_funcC
     add      rsp, 2*opsa



     ; funcF(gost14_ExpandKey_t1, gost14_ExpandKey_t2, gost14_ExpandKey_c, gost14_ExpandKey_t1, gost14_ExpandKey_t2)
     push     gost14_ExpandKey_t2
     push     gost14_ExpandKey_t1
     push     gost14_ExpandKey_c
     push     gost14_ExpandKey_t2
     push     gost14_ExpandKey_t1
     call     gost14_funcF
     add      rsp, 5*opsa
     ;
     inc      rbx
     cmp      rbx, 8
     jne     .for2
     ; }
     ;
;	   funcC(j*8+8, C);
;	   funcF(temp1, temp2, C, temp1, temp2);

     ; funcC(j*8+8, C):
     push     gost14_ExpandKey_c
     mov      rax, rdx
     shl      rax, 3
     add      rax, 8
     push     rax
     call     gost14_funcC
     add      rsp, 2*opsa


     ; funcF(gost14_ExpandKey_t1, gost14_ExpandKey_t2, gost14_ExpandKey_c, gost14_ExpandKey_t1, gost14_ExpandKey_t2);
     push     gost14_ExpandKey_t2
     push     gost14_ExpandKey_t1
     push     gost14_ExpandKey_c
     push     gost14_ExpandKey_t2
     push     gost14_ExpandKey_t1
     call     gost14_funcF
     add      rsp, 5*opsa
     ;

;	   memcpy(keys + (j * 2 + 2) * 16, temp1, 16);
;	   memcpy(keys + (j * 2 + 3) * 16, temp2, 16);

     ; memcpy(keys + (j * 2 + 2) * 16, gost14_ExpandKey_t1, 16);
     mov      rsi, gost14_ExpandKey_t1
     mov      rdi, rdx
     shl      rdi, 1
     inc      rdi
     inc      rdi
     shl      rdi, 4
     add      rdi, opsz[rbp + opsa*2]
     mov      rcx, 16/4
     cld
     rep      movsd
     ;
     ; memcpy(keys + (j * 2 + 3) * 16, gost14_ExpandKey_t2, 16):
     mov      rsi, gost14_ExpandKey_t2
     mov      rdi, rdx
     shl      rdi, 1
     inc      rdi
     inc      rdi
     inc      rdi
     shl      rdi, 4
     add      rdi, opsz[rbp + opsa*2]
     mov      rcx, 16/4
     cld
     rep      movsd
     ;}
     inc      rdx
     cmp      rdx, 4
     jne     .for1
     ;
   .end:
     popad
ret


align 8
xor128bitBlock:  ; RSI: data pointer, RDI: result pointer
;, RBX: Syn block
     push	rax rbx rcx rdx rsi rdi
     xor	rcx, rcx
     mov	rbx, gost14_ciphblock
     align 4
 @@: mov	eax, dword[rsi + rcx]
     xor	dword[rbx + rcx], eax
     mov	eax, dword[rbx + rcx]
     mov	dword[rdi + rcx], eax
     add	rcx, 4
     cmp	rcx, 16
     jb 	@b
     pop	rdi rsi rdx rcx rbx rax
ret

ALIGN 8
GOST14_DECRYPT_CFB: ; RAX: KEY, RBX: KEY_SZ, RCX: LEN, RSI: DATA, RDI: BUFFER
     ;
     push	rax rbx rcx rdx rsi rdi
     mov	rdi, gost14_clear
     mov	rcx, gost14_clear_sz / 4
     xor	rax, rax
     cld
     rep	stosd
     pop	rdi rsi rdx rcx rbx rax
     ;
     push	rax rbx rcx rdx rsi rdi
     MOV	rcx, rbx			; masterKeySize
     MOV	rbx, 256			; 256-bit variant
     MOV	rsi, rax			; MastrKey data
     MOV	rdi, gost14_keysmaster		; HashBuffer
     CALL	GOST_3411_2012
     pop	rdi rsi rdx rcx rbx rax
     ;
     ;
     push	rax rbx rcx rdx rsi rdi
     ;
     push	gost14_keysblock
     push	gost14_keysmaster
     call	gost14_ExpandKey
     add	esp, 4*2

     ;
     push	gost14_keysblock	       ; keys
     push	16				;
     push	gost14_ciphblock		; cryptotext
     push	gost14_SP			; IV
     call	gost14_simple_encrypt		; Simple Encrypt
     add	esp, 4*4
     ;
     pop	rdi rsi rdx rcx rbx rax
     ;
     ;
     mov	rdx, rcx			; DataLength
     xor	rcx, rcx

     ; decrypt first data block:
     call	xor128bitBlock			; ESI, EDI
     add	rsi, 16
     add	rdi, 16
     add	rcx, 16
     cmp	rcx, rdx
     jae       .end


  .nextblock:
     ; decrypt next data block:

     mov	rax, rsi
     sub	rax, 16
     push	gost14_keysblock	       ; keys
     push	16				;
     push	gost14_ciphblock		; cryptotext
     push	rax				; IV ( prev data crypted block)
     call	gost14_simple_encrypt		; Simple Encrypt
     add	esp, 4*4			; *chipherText, *plainText, int lenPlainText, *keys



     call	xor128bitBlock			; ESI, EDI
     add	rsi, 16
     add	rdi, 16
     add	rcx, 16
     cmp	rcx, rdx
     jae       .end
     ;

     jmp       .nextblock
     ;

  .end:
RET

ALIGN 8
GOST14_ENCRYPT_CFB: ; RAX: KEY, RBX: KEY_SZ, RCX: LEN, RSI: DATA, RDI: BUFFER
     ;
     push	rax rbx rcx rdx rsi rdi
     mov	rdi, gost14_clear
     mov	rcx, gost14_clear_sz / 4
     xor	rax, rax
     cld
     rep	stosd
     pop	rdi rsi rdx rcx rbx rax
     ;
     push	rax rbx rcx rdx rsi rdi
     MOV	rcx, rbx			; masterKeySize
     MOV	rbx, 256			; 256-bit variant
     MOV	rsi, rax			; MastrKey data
     MOV	rdi, gost14_keysmaster		; HashBuffer
     CALL	GOST_3411_2012
     pop	rdi rsi rdx rcx rbx rax
     ;
     ;
     push	rax rbx rcx rdx rsi rdi
     ;
     push	gost14_keysblock
     push	gost14_keysmaster
     call	gost14_ExpandKey
     add	esp, 4*2
     ;
     push	gost14_keysblock		; keys
     push	16				;
     push	gost14_ciphblock		; cryptotext
     push	gost14_SP			; IV
     call	gost14_simple_encrypt		; Simple Encrypt
     add	esp, 4*4
     ;
     pop	rdi rsi rdx rcx rbx rax
     ;
     ;
     mov	rdx, rcx			; DataLength
     xor	rcx, rcx
  .nextblock:
     ; encrypt first data block:
     call	xor128bitBlock			; ESI, EDI
     add	rsi, 16
     add	rdi, 16
     add	rcx, 16
     cmp	rcx, rdx
     jae       .end
     ;
     mov	rax, rdi
     sub	rax, 16
     push	gost14_keysblock		; keys
     push	16				;
     push	gost14_ciphblock		; cryptotext
     push	rax				; IV ( prev data crypted block)
     call	gost14_simple_encrypt		; Simple Encrypt
     add	esp, 4*4
     jmp       .nextblock
     ;

  .end:

RET

align 8
gost14_simple_encrypt:
; *plainText, *chipherText, int lenPlainText,  *keys
     mov      rax, rsp
     pushad
     mov      rbp, rax

     ; memcpy(gost14_xTemp, plainText, 16):
     mov     rsi, opsz[rbp + opsa*1]
     mov     rdi, gost14_xTemp
     mov     rcx, 16/4
     cld
     rep     movsd
     ; for(i = 0; i < 9; ++i) {
     xor     rdx, rdx
 @@: ; funcLSX(gost14_xTemp, keys + 16*i, yTemp):
     push    gost14_yTemp
     mov     rax, rdx
     shl     rax, 4
     add     rax, opsz[rbp + opsa*4]
     push    rax
     push    gost14_xTemp
     call    gost14_funcLSX
     add      rsp, 3*opsa
     ; memcpy(gost14_xTemp, gost14_yTemp, 16):
     mov     rsi, gost14_yTemp
     mov     rdi, gost14_xTemp
     mov     rcx, 16/4
     cld
     rep     movsd
     ; }
     inc     rdx
     cmp     rdx, 9
     jne     @b
     ;
     ; gost14_funcX(gost14_yTemp, keys+9*16, chipherText)
     mov     rax, opsz[rbp + opsa*2]
     push    rax
     mov     rax, opsz[rbp + opsa*4]
     add     rax, 9*16
     push    rax
     push    gost14_yTemp
     call    gost14_funcX
     add     rsp, 3*opsa
     ;
     popad
ret

align 8
gost14_simple_decrypt:
; *chipherText, *plainText, int lenPlainText, *keys
;     memcpy(xTemp, chipherText, 16);
;     for(i = 0; i < 9; ++i)
;     {
;	   funcReverseLSX(xTemp, keys+(9-i)*16, yTemp, print);
;	   memcpy(xTemp, yTemp, 16);
;     }
;     funcX(yTemp, keys, plainText, print);

     mov      rax, rsp
     pushad
     mov      rbp, rax

     ;	   memcpy(xTemp, chipherText, 16);
     mov     rsi, opsz[rbp + opsa*1]
     mov     rdi, gost14_xTemp
     mov     rcx, 16/4
     cld
     rep     movsd
     ;


     ; for(i = 0; i < 9; ++i) {
     xor     rdx, rdx
 @@: ; funcReverseLSX(gost14_xTemp, keys + (9-i)*16, yTemp)
     push    gost14_yTemp
     mov     rax, 9
     sub     rax, rdx
     shl     rax, 4
     add     rax, opsz[rbp + opsa*4]
     push    rax
     push    gost14_xTemp
     call    gost14_funcReverseLSX
     add     rsp, 3*opsa
     ; memcpy(gost14_xTemp, gost14_yTemp, 16):
     mov     rsi, gost14_yTemp
     mov     rdi, gost14_xTemp
     mov     rcx, 16/4
     cld
     rep     movsd
     ; }
     inc     rdx
     cmp     rdx, 9
     jne     @b
     ;
     ; gost14_funcX(gost14_yTemp, keys, plainText)
     mov     rax, opsz[rbp + opsa*2]
     push    rax
     mov     rax, opsz[rbp + opsa*4]
     push    rax
     push    gost14_yTemp
     call    gost14_funcX
     add     rsp, 3*opsa
     ;
     popad
ret



;include 'gost28147-14_data.inc'


