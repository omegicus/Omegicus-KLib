align 4
GOST_SBOX: ; "A" from GOST
    DB 0x4, 0xA, 0x9, 0x2, 0xD, 0x8, 0x0, 0xE, 0x6, 0xB, 0x1, 0xC, 0x7, 0xF, 0x5, 0x3
    DB 0xE, 0xB, 0x4, 0xC, 0x6, 0xD, 0xF, 0xA, 0x2, 0x3, 0x8, 0x1, 0x0, 0x7, 0x5, 0x9
    DB 0x5, 0x8, 0x1, 0xD, 0xA, 0x3, 0x4, 0x2, 0xE, 0xF, 0xC, 0x7, 0x6, 0x0, 0x9, 0xB
    DB 0x7, 0xD, 0xA, 0x1, 0x0, 0x8, 0x9, 0xF, 0xE, 0x4, 0x6, 0xC, 0xB, 0x2, 0x5, 0x3
    DB 0x6, 0xC, 0x7, 0x1, 0x5, 0xF, 0xD, 0x8, 0x4, 0xA, 0x9, 0xE, 0x0, 0x3, 0xB, 0x2
    DB 0x4, 0xB, 0xA, 0x0, 0x7, 0x2, 0x1, 0xD, 0x3, 0x6, 0x8, 0x5, 0x9, 0xC, 0xF, 0xE
    DB 0xD, 0xB, 0x4, 0x1, 0x3, 0xF, 0x5, 0x9, 0x0, 0xA, 0xE, 0x7, 0x6, 0x8, 0x2, 0xC
    DB 0x1, 0xF, 0xD, 0x0, 0x5, 0x7, 0xA, 0x4, 0x9, 0x2, 0x3, 0xE, 0x6, 0xB, 0x8, 0xC


;align 4
;gostmemvars:
;    gost_ext_simle_table     db  1024 dup 0
;    gost_int_table	      db  0256 dup 0
;    crypt_key		      db  0032 dup 0
;    crypt_vars:
;    crypt_a		      dd  0x00000000
;    crypt_b		      dd  0x00000000
;    crypt_c		      dd  0x00000000
;    crypt_d		      dd  0x00000000
;gostmemvars_sz = $- gostmemvars
;gostmemvars_end:
;
;align 4
;WolframKEYS:
;    WfKey_A	    db 32 dup 0
;    WfKey_B	    db 32 dup 0
;WolframKEYS_Sz     = $ - WolframKEYS
;
;    userkey_buffer db 32 dup 0
;
;
;ALIGN 4
;GOST_3411_vars:
;	 GOST_3411_HASH_ESI DD 0
;	 GOST_3411_HASH_EDI DD 0
;	 GOST_3411_HASH_ECX DD 0
;	 GOST_3411_HASH_EBX DD 0
;	 gost34_sum	    DB 32 DUP 0
;	 gost34_len	    DD 0
;	 gost34_len256	    DD 7  DUP 0
;	 gost34_sys	    DB 148 DUP 0
;	 GOST_3411_FillSboxes:
;		 .x1 dd 0
;		 .x2 dd 0
;		 .x3 dd 0
;		 .x4 dd 0
;	 gost_3411_table    db	4096 dup 0
;	 gost_3411_lastblock db 32 dup 0
;GOST_3411_vars_sz = $ - GOST_3411_vars
