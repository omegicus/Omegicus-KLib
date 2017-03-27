; Copyright (c) 2008, Alex Patterson, Greenville, WI. All rights reserved.

; An AES implementation for x86 processors using the FASM assembler, and is
;  mainly for educational use.	This code provides the standard AES block size
;  of 128 bits (16 bytes), and currently supports 256-bit keys only, due to
;  personal use requirements.
;
;  calling interfaces:
;
;  AES_EncryptKeys    ( byte key[32] )
;  AES_DecryptKeys    ( byte key[32] )
;  AES_EncryptBlock   ( byte in[16], byte out[16] )
;  AES_DecryptBlock   ( byte in[16], byte out[16] )
;  AES_SecureMemory   ( void )
;
; In this implementation the stdcall convention is used, where the parameters
;  are pushed in reverse order onto the stack, and the callee clears the
;  stack frame.  The standard callback registers are preserved across calls
;  to these functions, including ebx,esi,edi,& ebp.
;
; The encryption key schedule is generated in forward order, according to the
;  following diagram.
;
;  lower address   [  encryption round 0   ]   first key addition
;		   [  encryption round 1   ]
;		   [  encryption round 2   ]   main n-1 round table lookups
;		   [  encryption round ..  ]
;		   [  encryption round n-1 ]
;  higher address  [  encryption round n   ]   final subsitution & key addition
;

use32
include 'struct.inc'

include 'aes_const.inc'
include 'aes_macro.inc'


; USING:
;KeyFipsTest	 db 0x60,0x3d,0xeb,0x10
;		 db 0x15,0xca,0x71,0xbe
;		 db 0x2b,0x73,0xae,0xf0
;		 db 0x85,0x7d,0x77,0x81   ; key test only
;		 db 0x1f,0x35,0x2c,0x07
;		 db 0x3b,0x61,0x08,0xd7
;		 db 0x2d,0x98,0x10,0xa3
;		 db 0x09,0x14,0xdf,0xf4

;KeyFipsMain	 db 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07
;		 db 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f  ; main key
;		 db 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17
;		 db 0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f

;FipsEncData	 db 0x00,0x11,0x22,0x33
;		 db 0x44,0x55,0x66,0x77
;		 db 0x88,0x99,0xaa,0xbb
;		 db 0xcc,0xdd,0xee,0xff

;FipsDecData	 db 0x69,0xc4,0xe0,0xd8
;		 db 0x6a,0x7b,0x04,0x30
;		 db 0xd8,0xcd,0xb7,0x80
;		 db 0x70,0xb4,0xc5,0x5a


;	 Outa		 rb 16
;	 Outb		 rb 16
;
;	 call AES_Init
;
;	 push KeyFipsMain		 ; KEY[32]
;	 call AES_EncryptKeys		 ;
;
;	 push Outa			 ; OUT[16]
;	 push FipsEncData		 ; IN [16]
;	 call AES_EncryptBlock		 ;
;
;	 push KeyFipsMain
;	 call AES_DecryptKeys
;
;	 push Outb
;	 push Outa
;	 call AES_DecryptBlock
;
;	 call AES_SecureMemory



align 4
AES_Init:
    pushad
    ; alog and log byte[256] tables
    mov     eax, 1
    mov     byte[alog_table], al
    mov     byte[log_table],   0
    mov     ecx, eax
@@: mov     bh, al
    add     bh, bh
    sbb     bl, bl
    and     bl, gf_modulus
    xor     bh, bl
    xor     al, bh
    mov     byte[ecx+alog_table], al
    mov     byte[eax+ log_table], cl
    add     cl, 1
    jnc     @b
    mov     byte[eax+ log_table], cl
    ; substitution and inverse byte[] tables
    mov     edx, 255
    mov     ecx, gf_magic
    mov     byte[000+ aes_tables.tab_s], cl
    mov     byte[ecx+aes_tables.tab_si], 0
.s :movzx   eax, byte[edx+log_table]
    not     al
    mov     al, byte[eax+alog_table]
    mov     bl, gf_affine
    mov     cl, gf_magic
@@: shr     al, 1
    sbb     bh, bh
    and     bh, bl
    xor     cl, bh
    rol     bl, 1
    test    al, al
    jnz     @b
    mov     [edx+ aes_tables.tab_s], cl
    mov     [ecx+aes_tables.tab_si], dl
    sub     edx, 1
    jnz     .s
    ; t5, t6, t7, t8, u1, u2, u3 & u4 dword[256] tables
    xor     esi, esi
    xor     edx, edx
    mov     cl,  byte[log_table+0x0e]
    mov     ch,  byte[log_table+0x09]
    mov     bl,  byte[log_table+0x0d]
    mov     bh,  byte[log_table+0x0b]
.t: movzx   eax, byte[esi+aes_tables.tab_si]
    lea     edi, [eax*4]
    test    eax, eax
    movzx   ebp, al
    jz	    .2
    mov     al, byte[eax+ log_table]
    mov     dl, al
    add     dl, cl
    adc     dl, 0
    mov     dl, byte[edx+alog_table]
    shrd    ebp,edx, 8
    mov     dl, al
    add     dl, ch
    adc     dl, 0
    mov     dl, byte[edx+alog_table]
    shrd    ebp,edx, 8
    mov     dl, al
    add     dl, bl
    adc     dl, 0
    mov     dl, byte[edx+alog_table]
    shrd    ebp,edx, 8
    add     al, bh
    adc     al, 0
    mov     al, byte[eax+alog_table]
    shrd    ebp,eax, 8
.2: mov     [esi*4+aes_tables.tab_t5], ebp
    mov     [edi  +aes_tables.tab_u1], ebp
    rol     ebp, 8
    mov     [esi*4+aes_tables.tab_t6], ebp
    mov     [edi  +aes_tables.tab_u2], ebp
    rol     ebp, 8
    mov     [esi*4+aes_tables.tab_t7], ebp
    mov     [edi  +aes_tables.tab_u3], ebp
    rol     ebp, 8
    mov     [esi*4+aes_tables.tab_t8], ebp
    mov     [edi  +aes_tables.tab_u4], ebp
    add     esi, 1
    test    esi, 0xff
    jnz     .t
    ; t1, t2, t3 & t4 dword[256] tables
    xor     edx, edx
@@: movzx   eax, byte[edx+aes_tables.tab_s]
    mov     ah, al
    add     ah, ah
    sbb     bl, bl
    and     bl, gf_modulus
    xor     ah, bl
    mov     ch, al
    xor     ch, ah
    mov     cl, al
    shl     ecx, 16
    mov     ch, al
    mov     cl, ah
    mov     [edx*4+aes_tables.tab_t1], ecx
    rol     ecx, 8
    mov     [edx*4+aes_tables.tab_t2], ecx
    rol     ecx, 8
    mov     [edx*4+aes_tables.tab_t3], ecx
    rol     ecx, 8
    mov     [edx*4+aes_tables.tab_t4], ecx
    add     dl, 1
    jnc     @b
    ; rcon dword[30] table
    xor     edx, edx
    mov     eax, 1
@@: mov     [edx*4+aes_tables.tab_rc], eax
    add     al, al
    sbb     bl, bl
    and     bl, gf_modulus
    xor     al, bl
    add     dl, 1
    cmp     dl, 30
    jb	    @b
    popad
    ret ; 12



; encryption key schedule - user interface
align 4
AES_EncryptKeys:
    ; preserve registers
    preserve_common_registers
    ; obtain input parameter, prepare registers for string instructions
    mov     esi, [esp+aes_stack_space+aes_schedule_param] ; lodsd
    mov     edi, aes_state.keys 			  ; stosd
    mov     ebx, aes_tables.tab_s			  ; xlatb
    ; input the user's key
    cld
    mov     ecx, 8
    rep     movsd
    ; 'kickstart' the algorithm
    mov     eax, [edi-4]
    ; perform the key generation
    mov     ebp, aes_tables.tab_rc
    mov     cl , 32	  ; int i = Nk
.1: mov     edx, ecx
    and     edx, 31	  ;
    cmp     edx, 0	  ; i mod Nk = 0
    jne     .2
    ror     eax, 16	  ; little-endian conversion
    substitute		  ; temp = SubWord(RotWord(temp))
    xor     eax, [ebp]	  ; temp ^= Rcon[i/Nk]
    add     ebp, 4
    jmp     .3
.2: cmp     edx, 16	  ; i mod Nk = 4
    jne     .3
    substitute		  ; temp = SubWord(temp)
    ror     eax, 8
.3: xor     eax, [edi-32] ; temp ^= w[i-Nk]
    stosd		  ; w[i] = temp
    add     ecx, 4	  ; i++
    cmp     cl , 240	  ; while (i < Nb*(Nr+1))
    jnz     .1
    ; restore registers and return
    restore_common_registers 4


; decryption key schedule - user interface
align 4
AES_DecryptKeys:
    preserve_common_registers
    ; perform a normal encryption key scheduling
    push    dword[esp+aes_stack_space+aes_schedule_param]
    call    AES_EncryptKeys
    ; set up schedule pointers
    mov     esi, aes_state.keys
    mov     edi, aes_state.keys_d
    ; move first round into decryption schedule
    mov     ecx, 4
    rep     movsd
    ; perform the invmixcolumns operation on all but first and last round keys
    mov     ebp, 52
@@: table_lookup_keys_d esi
    add     esi, 4
    dec     ebp
    jnz     @b
    ; finish by moving last round into decryption schedule
    mov     cl, 4
    rep     movsd
    ; restore registers and return
    restore_common_registers 4


; aes encryption - user interface
align 4
AES_EncryptBlock:
    preserve_common_registers
    ; obtain input parameter, prepare registers for instructions
    cld
    mov     esi, [esp+aes_stack_space+aes_in_blk] ; lodsd
    mov     edi, aes_state.state		  ; stosd
    ; import user data, perform first key addition
  rept 4 column {
    lodsd
    xor     eax, dword[aes_state.keys+(column-1)*4]
    stosd
  }
    ; prepare registers for main algorithm
    mov     esi, aes_state.state     ; *state
    mov     ebp, aes_state.keys+16   ; *round key
    mov     edx, aes_max_rounds-1    ; rounds-1
    ; perform the nr-1 normal rounds

@@: table_lookup_key_add esi,edi,ebp
    add     ebp, 16
    xchg    edi, esi
    dec     edx
    jnz     @b
    ; perform final round, export data to out[]
    mov     edi, [esp+aes_stack_space+aes_out_blk]
    table_lookup_final esi,edi,aes_state.keys+224
    ; restore registers and return
    restore_common_registers 8


; aes decryption - user interface
align 4
AES_DecryptBlock:
    ; preserve registers
    preserve_common_registers
    ; obtain input parameter, prepare registers for instructions
    mov     esi, [esp+aes_stack_space+aes_in_blk] ; lodsd
    mov     edi, aes_state.state		  ; stosd
    ; import user data, perform first key addition
  rept 4 column {
    lodsd
    xor     eax, dword[aes_state.keys_d+224+(column-1)*4]
    stosd	}
    ; prepare pointer to state
    mov     esi, aes_state.state ; *state
    ; perform the nr-1 normal rounds
    ; prepare pointer to decryption schedule, loop counter
    mov     ebp, aes_state.keys_d+208 ; *round key
    mov     edx, aes_max_rounds-1     ; rounds-1
@@: table_lookup_key_add_d esi,edi,ebp
    sub     ebp, 16
    xchg    edi, esi
    dec     edx
    jnz     @b
    ; perform final round, export data to out[]
    mov     edi, [esp+aes_stack_space+aes_out_blk]
    table_lookup_final_d esi,edi,aes_state.keys_d
    ; restore registers and return
    restore_common_registers 8


; aes secure zero memory
align 4
AES_SecureMemory:
     ; fill all bytes with 0b.11110000
     cld     ; forward
     mov     eax, 0xF0F0F0F0
     mov     ecx, 140
     mov     edi, aes_state.keys
     rep     stosd
     sub     edi, 4
     ; overlap all bytes with 0b.00001111
     std     ; backward
     not     eax ; 0x0F0F0F0F
     mov     ecx, 140
     rep     stosd
     ret


;include 'aes_data.inc'