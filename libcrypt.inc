use32

include 'mmd_crypt_gost28147.inc'
include 'mmd_crypt_rc4.inc'
include 'mmd_crypt_rc4vmpc.inc'
include 'mmd_crypt_rc6.inc'
include 'lib_hash_gost3411-2012_x86.inc'
; -----------------------------------------------;

BlockSize128   = 16	 ; RC6	128 bit /8

gost34112012: ; esi edi ecx
	mov	ebx, 256			 ; 256 bit version
;	 mov	 esi, [psource]
;	 mov	 edi, [phash512]
;	 mov	 ecx, [psize]
	call	GOST_3411_2012
ret



; CFB
align 4
encrypt_rc6: ; ESI pbuffer, ECX psize, EDX Key256 ; block == 128bit
     call    crypt_RC6_clear
     mov     esi,  EDX
     call    crypt_RC6_Init
     ;mov     ecx,  [psize]
     ;mov     esi,  [pbuffer]
     align 4
 .rc6_block:
     push    esi ecx
     call    crypt_RC6_Encrypt
     pop     ecx esi
     ;
     sub     ecx, BlockSize128
     jecxz  .endx
     ;
     mov     eax, dword[esi + 00]
     xor     dword[esi + BlockSize128 + 00], eax
     mov     eax, dword[esi + 04]
     xor     dword[esi + BlockSize128 + 04], eax
     mov     eax, dword[esi + 08]
     xor     dword[esi + BlockSize128 + 08], eax
     mov     eax, dword[esi + 12]
     xor     dword[esi + BlockSize128 + 12], eax
     ;
     add     esi,  BlockSize128
     jmp    .rc6_block
     ;
 .endx:
     ret

; CFB
align 4
decrypt_rc6: ; ESI pbuffer, ECX psize, EDX Key256 ; block == 128bit
     call    crypt_RC6_clear
     mov     esi,  EDX
     call    crypt_RC6_Init
     ;
     ;mov     ecx,  [psize]
     ;mov     esi,  [pbuffer]
     add     esi,  ecx	       ; what if not aliugned to blk_size?
     sub     esi,  BlockSize128
     align 4
 .rc6_block:
     push    ecx esi
     call    crypt_RC6_Decrypt
     pop     esi ecx
     ;
     sub     ecx,  BlockSize128
     jecxz  .endx
     ;
     mov     eax, dword[esi - BlockSize128 + 00]
     xor     dword[esi + 00], eax
     mov     eax, dword[esi - BlockSize128 + 04]
     xor     dword[esi + 04], eax
     mov     eax, dword[esi - BlockSize128 + 08]
     xor     dword[esi + 08], eax
     mov     eax, dword[esi - BlockSize128 + 12]
     xor     dword[esi + 12], eax
     ;
     sub     esi,  BlockSize128
     jmp    .rc6_block
     ;
 .endx:
     ret




align 4
encrypt_gost: ; ESI pbuffer, ECX psize, EDX pkey32
; ESI: buffer~8[/8=0], ECX: Size, EDI: Key32bytes
; 4) gost_gammaLE() ?? 8byte (gost_syn+0, gost_syn+4)
     ;
     call    gost_clear
     ;
     ;mov     esi, [pbuffer]
     ;mov     ecx, [psize]
     dec     ecx
     mov     edi, EDX
     call    gost_ExpCht
     call    gost_ExpandKey
     mov     edi, esi
     mov     edx, ecx
     xor     ecx, ecx
 @@: push    edx edi ecx
     call    gost_gammaLE
     pop     ecx edi edx
     add     edi, 0x08
     add     ecx, 0x08
     cmp     ecx, edx
     jbe     @b
     ret


align 4
decrypt_gost: ; ESI pbuffer, ECX psize, EDX pkey32
; ESI: buffer~8, ECX: Size, EDI: Key32bytes
     ;
     call    gost_clear
     ;
     ;mov     esi, ESI
     ;mov     ecx, ECX
     dec     ecx
     mov     edi, EDX
     call    gost_ExpCht
     call    gost_ExpandKey
     mov     edi, esi
     mov     edx, ecx
     xor     ecx, ecx
 @@: push    edx edi ecx
     call    gost_gammaLD
     pop     ecx edi edx
     add     edi, 0x08
     add     ecx, 0x08
     cmp     ecx, edx
     jbe     @b
     ret


align 4
crypt_rc4: ;  ESI pbuffer, ECX psize, EDX pkey32    ; unstable
      push   esi
      mov    esi,  EDX
      call   crypt_RC4_Init
      pop    esi
      mov    edi,  ESI
      mov    edx,  ECX
      call   crypt_RC4_CryptEDX
      ret


align 4
crypt_vmpc: ; ESI pbuffer, ECX psize, EDX pkey64    ; test needed
      call   VMPCEraseKey
      pushd  64 			  ; VecLen
      pushd  64 			  ; KeyLen
      pushd  VMPCVector 		  ; Vec
      pushd  EDX			  ; Key
      call   VMPCInitKey
      mov    eax, ESI
;      mov    ecx, [psize]
      call   VMPCEncrypt		  ; [Data]=eax; Len=edx
      ret






