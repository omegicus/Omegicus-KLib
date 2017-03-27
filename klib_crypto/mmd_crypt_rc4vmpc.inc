use32


;-----------------------------------------------------------------------------;
;									      ;
;		  Implementation of the VMPC Stream Cipher		      ;
;									      ;
;		 Author of the algorithms: Bartosz Zoltak		      ;
;	       Author of the implementation: Bartosz Zoltak		      ;
;      Author of the flat assembler implementation: Vladislav Kabak	      ;
;			  www.vmpcfunction.com				      ;
;									      ;
;-----------------------------------------------------------------------------;

;var
;   Key, Vector : array[0..63] of byte;
;   Message	: array[0..999] of byte;

;Encryption:
;   VMPCInitKey_ASM(Klucz, Wektor, 16, 16);
;   VMPCEncrypt_ASM(Wiadomosc, 1000);

;Decryption:
;   VMPCInitKey_ASM(Klucz, Wektor, 16, 16);
;   VMPCEncrypt_ASM(Wiadomosc, 1000);
;
;   (the VMPCEncrypt_ASM procedure is used for both encryption and decryption).


;----------------------------------------------------------------------------------------------------
;The VMPCInitKey_ASM / VMPCInitKey16_ASM functions (employing the VMPC-KSA3 key initialization algorithm)
;provide higher security level but about 1/3 lower efficiency.
;than the basic VMPCInitKeyBASIC_ASM / VMPCInitKey16BASIC_ASM functions.
;
;If only the system efficiency allows, the author recommends to use the VMPCInitKey_ASM / VMPCInitKey16_ASM functions.
;At the same time the VMPCInitKeyBASIC_ASM / VMPCInitKey16BASIC_ASM functions also remain secure.
;----------------------------------------------------------------------------------------------------
;CAUTION!
;A DIFFERENT value of the initialization vector ("Vector")
;should be used for each encryption with the same key ("Key").
;
;Encrypting two messages with THE SAME key and THE SAME initialization vector
;drastically reduces security level!
;
;The key is a secret value.
;The initialization vector is not secret - it can be passed in plain form along with the encrypted message.
;-----------------------------------------------------------------------------------------------------------*)







align 4
VMPCInitKeyRound:
; [Data]=eax; Len=edx; Src=ecx
; The procedure works only for Len=2,4,8,16,32,64
    push	edi esi ebp ebx
    mov 	ebp, eax
    or		ecx, ecx
    jnz        .Start
    mov 	esi, Permut123
    mov 	edi, P
    mov 	ecx, 64
    cld
    rep 	movsd
    mov 	byte[s], 0
  .Start:
    mov 	eax, 768     ;ah=3, al=0
    mov 	al, byte[s]
    dec 	edx
    xor 	ebx, ebx
  .Step:
    xor 	esi, esi
  .Loop:
    mov 	ebx, esi
    and 	ebx, edx     ;equivalent to "ebx modulo Len" for Len=2^X
    mov 	bl, byte [ebp+ebx]
    add 	bl, byte [P+esi]
    add 	bl, al
    mov 	al, byte [P+ebx]
    ;
    mov 	bl, al
    mov 	ch, byte [P+ebx]
    mov 	cl, byte [P+esi]
    mov 	byte  [P+ebx], cl
    mov 	byte  [P+esi], ch
    ;
    inc 	esi
    cmp 	esi, 256
    jb	       .Loop
    dec 	ah
    jnz        .Step
    mov 	byte[s], al
    mov 	byte[n], 0
    pop 	ebx ebp esi edi
ret


align 4
VMPCInitKey:
;  Key, Vec; KeyLen, VecLen
; KeyLen, VecLen = 2,4,8,16,32,64
	mov	ebp, esp
	pushad
	;
	mov	eax, dword[ebp + 04] ; Key
	mov	edx, dword[ebp + 12] ; KeyLen
	mov	ecx, 0
	call	VMPCInitKeyRound
	;
	mov	eax, dword[ebp + 08] ; Vec
	mov	edx, dword[ebp + 16] ; VecLen
	mov	ecx, 1
	call	VMPCInitKeyRound
	;
	mov	eax, dword[ebp + 04] ; Key
	mov	edx, dword[ebp + 12] ; KeyLen
	mov	ecx, 1
	call	VMPCInitKeyRound
	;
	popad
ret 4*4





align 4
VMPCEncrypt:
; [Data]=eax; Len=edx
    or		edx, edx
    jz	       .Finish;
    push	edi  esi ebp ebx
    mov 	edi, eax
    mov 	ebp, edx
    add 	ebp, edi
    xor 	eax, eax
    xor 	ebx, ebx
    mov 	al,  byte[s]
    mov 	bl,  byte[n]
    mov 	esi, ebx
    align 4
  .Loop:
    add 	al, byte [P+esi]
    mov 	al, byte [P+eax]
    ;
    mov 	bl, byte [P+eax]
    mov 	bl, byte [P+ebx]
    add 	bl, 1
    mov 	bl, byte [P+ebx]
    ;
    xor 	byte [edi], bl
    ;
    mov 	bl, byte [P+eax]
    mov 	cl, byte [P+esi]
    mov 	byte [P+eax], cl
    mov 	byte [P+esi], bl
    ;
    inc 	esi
    and 	esi, 255
    inc 	edi
    cmp 	edi, ebp
    jb	       .Loop
    mov 	byte[s], al
    mov 	ebx, esi
    mov 	byte[n], bl
    pop 	ebx ebp esi edi
  .Finish:
ret



align 4
VMPCEraseKey:
    push	eax ecx edi
    xor 	eax, eax
    mov 	edi, vmpc_data
    mov 	ecx, vmpc_data_sz / 4
    cld
    rep 	stosd
    pop 	edi ecx eax
ret