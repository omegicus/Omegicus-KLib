; RC4 Encryption Algorithm (Stream Cipher)
; RSA Data Security, Inc
use32


; RC4
;          mov  esi, key [32 bytes]
;          call crypt_RC4_Init
;          mov  edi, buffer
;          mov  ebp, size
;          call RC4_Crypt



; Block Size: N/A
; Max Key Size: 2048 bits
align 4
crypt_RC4_Init: ; ESI: 256 bit key
     PUSHAD
     MOV    EDI, ESI
     MOV    ESI, 256		; 256bit key
     XOR    ECX, ECX
     ALIGN  4
 @@: MOV    BYTE[crypt_rc4_table+ECX], CL
     INC    ECX
     CMP    ECX, 255
     JLE    @B
     SHR    ESI, 3
     JE    .rc4_init_end
     CMP    ESI, 256
     JLE    @F
     MOV    ESI, 256
 @@: XOR    ECX, ECX
     XOR    EBX, EBX
     ALIGN  4
 @@: MOV    EAX, ECX
     XOR    EDX, EDX
     DIV     SI
     ADD     BL, BYTE[EDI+EDX]
     MOV     AH, BYTE[crypt_rc4_table+ECX]
     ADD     BL,  AH
     MOV     AL, BYTE[crypt_rc4_table+EBX]
     MOV    BYTE[crypt_rc4_table+ECX], AL
     MOV    BYTE[crypt_rc4_table+EBX], AH
     INC    ECX
     CMP    ECX, 255
     JLE    @B
   .rc4_init_end:
     POPAD
RET

align 4
crypt_RC4_Crypt:
;Data: EDI
;DataSize: EBP
     PUSHAD
     XOR    EAX, EAX
     XOR    ECX, ECX
     XOR    EDX, EDX
 @@: CMP    ECX, EBP
     JNL    @F
     MOV    ESI, ECX
     INC    ESI
     AND    ESI, 255
     MOV    AL, BYTE[crypt_rc4_table+ESI]
     ADD    DL, AL
     MOV    BL, [crypt_rc4_table+EDX]
     MOV    BYTE[crypt_rc4_table+ESI], BL
     MOV    BYTE[crypt_rc4_table+EDX], AL
     ADD    AL, BL
     MOV    AL, [crypt_rc4_table+EAX]
     XOR    BYTE[EDI+ECX], AL
     INC    ECX
     JMP    @B
 @@: POPAD
RETD


align 4
crypt_RC4_CryptEDX:
;Data: EDI
;DataSize: EDX
     PUSHAD
     mov    ebp, edx
     XOR    EAX, EAX
     XOR    ECX, ECX
     XOR    EDX, EDX
 @@: CMP    ECX, EBP
     JNL    @F
     MOV    ESI, ECX
     INC    ESI
     AND    ESI, 255
     MOV    AL, BYTE[crypt_rc4_table+ESI]
     ADD    DL, AL
     MOV    BL, [crypt_rc4_table+EDX]
     MOV    BYTE[crypt_rc4_table+ESI], BL
     MOV    BYTE[crypt_rc4_table+EDX], AL
     ADD    AL, BL
     MOV    AL, [crypt_rc4_table+EAX]
     XOR    BYTE[EDI+ECX], AL
     INC    ECX
     JMP    @B
 @@: POPAD
RETD


align 4
crypt_RC4_Crypt_VAR:
;Data: EDI
;DataSize: dword[rc4ebp]
     PUSHAD
     XOR    EAX, EAX
     XOR    ECX, ECX
     XOR    EDX, EDX
 @@: CMP    ECX, dword[rc4ebp]
     JNL    @F
     MOV    ESI, ECX
     INC    ESI
     AND    ESI, 255
     MOV    AL, BYTE[crypt_rc4_table+ESI]
     ADD    DL, AL
     MOV    BL, [crypt_rc4_table+EDX]
     MOV    BYTE[crypt_rc4_table+ESI], BL
     MOV    BYTE[crypt_rc4_table+EDX], AL
     ADD    AL, BL
     MOV    AL, [crypt_rc4_table+EAX]
     XOR    BYTE[EDI+ECX], AL
     INC    ECX
     JMP    @B
 @@: POPAD
RETD


;IF WIN_APP EQ 0

;END IF

; include 'lib_rc_data.inc'