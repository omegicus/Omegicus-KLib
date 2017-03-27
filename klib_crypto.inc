 ; 0 GOST	+  samebuffer
 ; 1 Kuznechik	-  FAIL
 ; 2 Kuznechik	-  FAIL
 ; 3 IDEA	-  FAIL
 ; 4 AES	+  samebuffer
 ; 5 RC4	+  samebuffer
 ; 6 VMPC	+  samebuffer
 ; 7 RC6	+  samebuffer
 ; 8 Serpent	+  samebuffer
 ; 9 Blowfish	+  samebuffer
;
;	 stdcall crypt_create_key,  mykey, 4 ; password, passwordSize	; out in key512
;	 stdcall encrypt,	    7, title,1024,  key512, 0
;
;	 stdcall crypt_create_key,  mykey, 4 ; password, passwordSize	; out in key512
;	 stdcall decrypt,	    7, title,1024,  key512, 0
;



; GOST 28197-89
KeyRepeat	  = 4	    ; Число повторений ключа
BlockSize128	  = 16 ; 128 bit /8
KEY_CREATION_ALGO = 3; 256bit Stribog
		    ; 4  512-bit Stribog
KEY_CREATION_REPT = 0x01FF



include 'klib_crypto/mmd_crypt_gost28147.inc'
if defined KUZNECHIK
  include 'klib_crypto/mmd_crypt_gost28147-14.inc'
end if
include 'klib_crypto/aes/aes.inc'
include 'klib_crypto/mmd_crypt_rc4.inc'
include 'klib_crypto/mmd_crypt_rc4vmpc.inc'
include 'klib_crypto/mmd_crypt_rc6.inc'
include 'klib_crypto/mmd_crypt_idea.inc'
include 'klib_crypto/mmd_crypt_blowfish.inc'
include 'klib_crypto/mmd_crypt_serpent_x86.inc'
;include 'klib_crypto/mmd_crypt_serpent_mmx.inc'

;include 'klib_crypto/klib_crypto_sha256.inc'
include 'klib_crypto/lib_hash_sha1.inc'
include 'klib_crypto/lib_hash_gost3411-94_x86.inc'
include 'klib_crypto/lib_hash_gost3411-2012_x86.inc'
include 'klib_crypto/lib_hash_crc32.inc'
include 'klib_crypto/lib_hash_crc64.inc' ; HASH.CRC64: ;( ESI data, ECX size, EBX postgre) ; if postgre==1 => PostgreSQL CRC64 else ECMA-182
include 'klib_crypto/lib_hash_md5.inc'
include 'klib_crypto/lib_hash_pjw32.inc'

include 'klib_crypto/mmd_crypt_rand.inc'

;include 'klib_crypto/base64.inc'

; sha256_init
;sha256_update:
; in: esi->data, edx=size
;sha256_final:
; out: edi->digest

;base64_fencode:  ; ESI EDI ECX
;base64_decode:  ; ESI: dataZ, EDI: buffer (bufSz <= dataSz)


nullproc: ret

align 4; 04    08    12     16			 20
hash: ; algo, data, size, target (min=512bit), string_target
       push	edx
       mov	edx, esp
       pushad
       ;
       add	edx, 4
       mov	ebp, edx
       ;
       mov	eax, dword[ebp + 04]
       mov	eax, dword[hashalgos + eax*4]
       or	eax, eax
       jz      .err
       xor	edx, edx
       call	eax
.ok:   jmp     .ret
.err:  xor	eax, eax
.ret:
       popad
       pop	edx
ret 4*5
hash_crc32:
       mov	esi, dword[ebp + 08]
       mov	ecx, dword[ebp + 12]
       call	HASH.CRC32
       mov	edi, dword[ebp + 16]
       mov	dword[edi], eax
ret  ; EAX = CRC32
hash_crc64:
       mov	esi, dword[ebp + 08]
       mov	ecx, dword[ebp + 12]
       xor	ebx, ebx		; ECMA-182
       call	HASH.CRC64
       mov	edi, dword[ebp + 16]
       mov	dword[edi +00], edx
       mov	dword[edi +04], eax
ret
hash_gost:
       mov	esi, dword[ebp + 08]
       mov	ecx, dword[ebp + 12]
       mov	ebx, dword[ebp + 16]
       mov	edi, dword[ebp + 20]
       PUSH	EBP
       call	GOST_3411_HASH
       POP	EBP
ret
hash_stribog256:
       mov	esi, dword[ebp + 08]
       mov	edi, dword[ebp + 16]
       mov	ebx, 256
       mov	ecx, dword[ebp + 12]
       PUSH	EBP
       call	GOST_3411_2012
       POP	EBP
ret
hash_stribog512:
       mov	esi, dword[ebp + 08]
       mov	edi, dword[ebp + 16]
       mov	ebx, 512
       mov	ecx, dword[ebp + 12]
       PUSH	EBP
       call	GOST_3411_2012
       POP	EBP
ret
hash_md5:
       mov	esi, dword[ebp + 08]
       mov	ecx, dword[ebp + 12]
       mov	edi, dword[ebp + 16]
       PUSH	EBP
       push	ecx
       push	esi
       push	edi
       call	md5
       POP	EBP
       mov	esi, dword[ebp + 16]
       mov	edi, dword[ebp + 20]
       call	HEX2STR_128BIT
ret
hash_sha1:
       mov	esi, dword[ebp + 08]
       mov	ecx, dword[ebp + 12]
       PUSH	EBP
       call	HASH.SHA1
       POP	EBP
       mov	edi, dword[ebp + 16]
       mov	eax, dword[SHA1_h0]
       stosd
       mov	eax, dword[SHA1_h1]
       stosd
       mov	eax, dword[SHA1_h2]
       stosd
       mov	eax, dword[SHA1_h3]
       stosd
       mov	eax, dword[SHA1_h4]
       stosd
ret
hash_pjw32:
       mov	esi, dword[ebp + 08]
       mov	ecx, dword[ebp + 12]
       call	pjw32
       mov	edi, dword[ebp + 16]
       mov	dword[edi], eax
ret





align 4
crypt_create_key: ; password, passwordSize
       push	edx
       mov	edx, esp
       pushad
       ;
       add	edx, 4
       mov	ebp, edx
       ;
       mov     edi, key512
       mov     ecx, 64/4
       xor     eax, eax
       cld
       rep     stosd
       ;
       mov	esi, dword[ebp + 04]
       mov	ecx, dword[ebp + 08]
       push	dword 0 	; string output
       push	dword key512	; target buffer
       push	dword ECX	; password chars
       push	dword ESI
       push	dword KEY_CREATION_ALGO
       call	hash		; algo, data, size, target (min=512bit), string_target
       ;
       mov	ecx, KEY_CREATION_REPT
       ;
   @@: push	dword 0 	; string output
       push	dword key512	; target buffer
       push	dword 64	; key chars (512 bit)
       push	dword key512	; SRC
       push	dword KEY_CREATION_ALGO
       call	hash		; algo, data, size, target (min=512bit), string_target
       loop	@b
       ;
       popad
       pop	edx
ret 4*2



align 4  ; 04	 08    12    16   20
encrypt: ; algo, data, size, key, outBuffer
       push	edx
       mov	edx, esp
       pushad
       ;
       add	edx, 4
       mov	ebp, edx
       ;
       ; ALIGN DATA:
       add     dword[ebp + 12], 0x0000000F   ; Size
       and     dword[ebp + 12], 0xFFFFFFF0
       ;
       ;
       mov	eax, dword[ebp + 04]
       cmp	eax, 10
       jae     .err
       mov	eax, dword[encryptalgos + eax*4]
       or	eax, eax
       jz      .err
       xor	edx, edx
       call	eax
.ok:   jmp     .ret
.err:  xor	eax, eax
.ret:  popad
       pop	edx
ret 4*5

align 4  ; 04	 08    12    16   20
decrypt: ; algo, data, size, key, outBuffer
       push	edx
       mov	edx, esp
       pushad
       add	edx, 4
       mov	ebp, edx
       ;
       ; ALIGN DATA:
       add     dword[ebp + 12], 0x0000000F   ; Size
       and     dword[ebp + 12], 0xFFFFFFF0

       ;
       ;
       mov	eax, dword[ebp + 04]
       cmp	eax, 10
       jae     .err
       mov	eax, dword[decryptalgos + eax*4]
       or	eax, eax
       jz      .err
       xor	edx, edx
       call	eax
.ok:   jmp     .ret
.err:  xor	eax, eax
.ret:  popad
       pop	edx
ret 4*5


encrypt_gost:
      call    gost_clear
      ;
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      mov     edi, dword[ebp + 16]	; key256 32bytes
      dec     ecx
      call    gost_ExpCht
      call    gost_ExpandKey
      mov     edi, esi;dword[ebp + 20]	    ; output
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
decrypt_gost:
      call    gost_clear
      ;
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      mov     edi, dword[ebp + 16]	; key256 32bytes
      dec     ecx
      call    gost_ExpCht
      call    gost_ExpandKey
      mov     edi, esi;dword[ebp + 20]	    ; output
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
if defined KUZNECHIK
encrypt_kuznechik256:
      mov     eax, dword[ebp + 16]	; key-256
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      mov     edi, dword[ebp + 20]	; output
      call    GOST14_ENCRYPT_CFB
ret
decrypt_kuznechik256:
      mov     eax, dword[ebp + 16]	; key-256
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      mov     edi, dword[ebp + 20]	; output
      call    GOST14_DECRYPT_CFB
ret
end if
encrypt_idea:
      mov     edi, idea_enkey
      xor     eax, eax
      mov     ecx, (64+64)/4
      cld
      rep     stosd
      mov     esi, dword[ebp + 16]	; key-128
      call    gen_en_key
      ;
      mov     edi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      mov     esi, idea_enkey
  @@: push    edi ecx esi
      call    do_idea ; ESI Key, EDI pBlockOut64bit; EDI outs.
      pop     esi ecx edi
      add     edi,  4
      sub     ecx,  4
      cmp     ecx,  0
      jg      @b
ret
decrypt_idea:
      mov     edi, idea_enkey
      xor     eax, eax
      mov     ecx, (64+64)/4
      cld
      rep     stosd
      mov     edi, idea_dekey
      xor     eax, eax
      mov     ecx, (64+64)/4
      cld
      rep     stosd
      ;
      mov     esi, dword[ebp + 16]	; key-128
      call    gen_de_key
      ;
      mov     edi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      mov     esi, idea_dekey
  @@: push    edi ecx esi
      call    do_idea ; ESI Key, EDI pBlockOut64bit; EDI outs.
      pop     esi ecx edi
      add     edi,  4
      sub     ecx,  4
      cmp     ecx,  0
      jg      @b
ret



;	 push Outa			 ; OUT[16]
;	 push FipsEncData		 ; IN [16]
;	 call AES_EncryptBlock		 ;


encrypt_aes:
      call    AES_Init
      push    dword[ebp + 16]		 ; KEY[32]     256bit
      call    AES_EncryptKeys		 ;
      ;
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      ;
     align 4
 .rc6_blocke:
     push    esi ecx
     push    dword aes_block		; OUT[16]
     push    dword esi			; IN [16]
     call    AES_EncryptBlock
     pop     ecx esi

     push    esi ecx edi
     mov     edi, esi
     mov     esi, aes_block
     mov     ecx, 16/4
     cld
     rep     movsd
     pop     edi ecx esi

     mov     eax, dword[esi + 00]
     xor     dword[esi + BlockSize128 + 00], eax
     mov     eax, dword[esi + 04]
     xor     dword[esi + BlockSize128 + 04], eax
     mov     eax, dword[esi + 08]
     xor     dword[esi + BlockSize128 + 08], eax
     mov     eax, dword[esi + 12]
     xor     dword[esi + BlockSize128 + 12], eax

     add     esi, BlockSize128

     sub     ecx, BlockSize128
     cmp     ecx, 0
     jle    .ends

     jmp    .rc6_blocke
 .ends:


ret

decrypt_aes:
      call    AES_Init
      push    dword[ebp + 16]		 ; KEY[32]     256bit
      call    AES_DecryptKeys		 ;
      ;
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      add     esi, ecx
      sub     esi, BlockSize128
      ;
 @@:

     push    esi ecx
     push    dword aes_block		; OUT[16]
     push    dword esi			; IN [16]
     call    AES_DecryptBlock
     pop     ecx esi

     push    esi ecx edi
     mov     edi, esi
     mov     esi, aes_block
     mov     ecx, 16/4
     cld
     rep     movsd
     pop     edi ecx esi


     sub     ecx,  BlockSize128
     cmp     ecx, 0
     jle     @f

     mov     eax, dword[esi - BlockSize128 + 00]
     xor     dword[esi + 00], eax
     mov     eax, dword[esi - BlockSize128 + 04]
     xor     dword[esi + 04], eax
     mov     eax, dword[esi - BlockSize128 + 08]
     xor     dword[esi + 08], eax
     mov     eax, dword[esi - BlockSize128 + 12]
     xor     dword[esi + 12], eax

     sub     esi,  BlockSize128


     jmp     @b


 @@:
ret




crypt_rc4:
      mov     esi, dword[ebp + 16]	; key-256
      call    crypt_RC4_Init
      mov     edi, dword[ebp + 08]	; data
      mov     edx, dword[ebp + 12]	; size
      call    crypt_RC4_CryptEDX
ret
crypt_vmpc:
      call    VMPCEraseKey
      mov     eax, dword[ebp + 16]	; key-256
      push    ebp

      pushd   64			; VecLen
      pushd   64			; KeyLen
      pushd   VMPCVector		; Vec
      pushd   eax
      call    VMPCInitKey

      pop     ebp
      mov     eax, dword[ebp + 08]	; data
      mov     edx, dword[ebp + 12]	; size
      call    VMPCEncrypt		; [Data]=eax; Len=edx
ret
encrypt_rc6: ; CFB
      call    crypt_RC6_clear
      mov     esi, dword[ebp + 16]	; key-256
      call    crypt_RC6_Init
      ;
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      ;
     align 4
 .rc6_blocke:
     push    esi ecx
     call    crypt_RC6_Encrypt
     pop     ecx esi

     sub     ecx, BlockSize128
     cmp     ecx, 0
     jle    .ends

     mov     eax, dword[esi + 00]
     xor     dword[esi + BlockSize128 + 00], eax
     mov     eax, dword[esi + 04]
     xor     dword[esi + BlockSize128 + 04], eax
     mov     eax, dword[esi + 08]
     xor     dword[esi + BlockSize128 + 08], eax
     mov     eax, dword[esi + 12]
     xor     dword[esi + BlockSize128 + 12], eax

     add     esi,  BlockSize128
     jmp    .rc6_blocke
 .ends:
ret
decrypt_rc6: ; CFB
      call    crypt_RC6_clear
      mov     esi, dword[ebp + 16]	; key-256
      call    crypt_RC6_Init
      ;
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      add     esi, ecx
      sub     esi, BlockSize128
      ;
     align 4
 .rc6_blockd:
     push    ecx esi
     call    crypt_RC6_Decrypt
     pop     esi ecx

     sub     ecx,  BlockSize128
     cmp     ecx, 0
     jle    .ends

     mov     eax, dword[esi - BlockSize128 + 00]
     xor     dword[esi + 00], eax
     mov     eax, dword[esi - BlockSize128 + 04]
     xor     dword[esi + 04], eax
     mov     eax, dword[esi - BlockSize128 + 08]
     xor     dword[esi + 08], eax
     mov     eax, dword[esi - BlockSize128 + 12]
     xor     dword[esi + 12], eax

     sub     esi,  BlockSize128
     jmp    .rc6_blockd


 .ends:
ret
encrypt_serpent: ; CFB256
      mov     edi, SERPENT_data
      mov     ecx, SERPENT_data_sz/4
      xor     eax, eax
      cld
      rep     stosd
      ;
      mov     ecx, 256
      mov     esi, dword[ebp + 16]	; key-256
      call    SERPENT_set_key ; ecx = key_len(256,...); esi: key^
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
     align 4
 .serp_block:
     push    esi ecx
     push    esi
     mov     edi,  SERPENT_block
     mov     ecx,  BlockSize128/4
     cld
     rep     movsd
     pop     esi
     mov     edi,  esi
     mov     esi,  SERPENT_block
     call    SERPENT_encrypt ; ESI: 16byte block, EDI: out_16byteBlock
     pop     ecx esi
     ;
     sub     ecx, BlockSize128
     jecxz  .ends
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
     jmp    .serp_block
  .ends:
ret
decrypt_serpent: ; CFB256
      mov     edi, SERPENT_data
      mov     ecx, SERPENT_data_sz/4
      xor     eax, eax
      cld
      rep     stosd
      ;
      mov     ecx, 256
      mov     esi, dword[ebp + 16]	; key-256
      call    SERPENT_set_key ; ecx = key_len(256,...); esi: key^
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      add     esi,  ecx 	; what if not aliugned to blk_size?
      sub     esi,  BlockSize128
     align 4
 .serp_block:
      push    esi ecx
      push    esi
      mov     edi,  SERPENT_block
      mov     ecx,  BlockSize128/4
      cld
      rep     movsd
      pop     esi
      mov     edi,  esi
      mov     esi,  SERPENT_block
      call    SERPENT_decrypt ; ESI: 16byte block, EDI: out_16byteBlock
      pop     ecx esi
      sub     ecx,  BlockSize128
      jecxz  .ends

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
      jmp    .serp_block
  .ends:
ret
encrypt_blowfish:
      mov     esi, dword[ebp + 16]	; key-256
      call    BF_Init
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      call    Blowfish_EncryptCFB  ; ecx: bytes, esi: buffer
ret
decrypt_blowfish:
      mov     esi, dword[ebp + 16]	; key-256
      call    BF_Init
      mov     esi, dword[ebp + 08]	; data
      mov     ecx, dword[ebp + 12]	; size
      call    Blowfish_DecryptCFB  ; ecx: bytes, esi: buffer
ret















