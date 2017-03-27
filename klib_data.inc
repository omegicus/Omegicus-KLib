align 4
key512	 db  64 dup 0	   ; Key storage




align 4
hashalgos:
dd hash_crc32		   ; 0
dd hash_crc64		   ; 1
dd hash_gost		   ; 2
dd hash_stribog256	   ; 3
dd hash_stribog512	   ; 4
dd hash_md5		   ; 5
dd hash_sha1		   ; 6
dd hash_pjw32		   ; 7
dd 0

align 4
encryptalgos:
dd encrypt_gost 	   ; 0
if defined KUZNECHIK
  dd encrypt_kuznechik256  ; 1
else
  dd nullproc
end if
dd nullproc
dd encrypt_idea 	   ; 3
dd encrypt_aes		   ; 4
dd crypt_rc4		   ; 5
dd crypt_vmpc		   ; 6
dd encrypt_rc6		   ; 7
dd encrypt_serpent	   ; 8
dd encrypt_blowfish	   ; 9
dd 0

align 4
decryptalgos:
dd decrypt_gost
if defined KUZNECHIK
  dd decrypt_kuznechik256
else
  dd nullproc
end if
dd nullproc
dd decrypt_idea
dd decrypt_aes
dd crypt_rc4
dd crypt_vmpc
dd decrypt_rc6
dd decrypt_serpent
dd decrypt_blowfish
dd 0



virtual at 0
streamInfo:
.fullSize	dq	?
.fillBuf	dd	?	; procedure which fills the buffer
.bufSize	dd	?
.bufDataLen	dd	?
.bufPtr 	dd	?
.size = $
end virtual





; CHARSETS:
unicode_utf8_magic DB 0xEF, 0xBB, 0xBF
align 4
cp1251utf8_array:
	DB 208, 160
	DB 192, 144
	DB 193, 145
	DB 194, 146
	DB 195, 147
	DB 196, 148
	DB 197, 149
	DB 168, 129
	DB 198, 150
	DB 199, 151
	DB 200, 152
	DB 201, 153
	DB 202, 154
	DB 203, 155
	DB 204, 156
	DB 205, 157
	DB 206, 158
	DB 207, 159
	DB 209, 161
	DB 210, 162
	DB 211, 163
	DB 212, 164
	DB 213, 165
	DB 214, 166
	DB 215, 167
	DB 216, 168
	DB 217, 169
	DB 218, 170
	DB 219, 171
	DB 220, 172
	DB 221, 173
	DB 222, 174
	DB 223, 175
	DB 224, 176
	DB 225, 177
	DB 226, 178
	DB 227, 179
	DB 228, 180
	DB 229, 181
	DB 184, 145
	DB 230, 182
	DB 231, 183
	DB 232, 184
	DB 233, 185
	DB 234, 186
	DB 235, 187
	DB 236, 188
	DB 237, 189
	DB 238, 190
	DB 239, 191
	DB 240, 128
	DB 241, 129
	DB 242, 130
	DB 243, 131
	DB 244, 132
	DB 245, 133
	DB 246, 134
	DB 247, 135
	DB 248, 136
	DB 249, 137
	DB 250, 138
	DB 251, 139
	DB 252, 140
	DB 253, 141
	DB 254, 142
	DB 255, 143
	DB 0,	0





; CRYPTO:
include 'klib_crypto/data/lib_idea_data.inc'
if defined KUZNECHIK
      include 'klib_crypto/data/gost28147-14_data.inc'
end if
include 'klib_crypto/data/lib_gost89_data.inc'

include 'klib_crypto/data/lib_gost89_hash12_data.inc'
include 'klib_crypto/data/lib_rc_data.inc'


;include 'klib_crypto/data/lib_hash_crc64_data.inc'
include 'klib_crypto/data/lib_blowfish_data.inc'
;include 'klib_crypto/data/lib_serpent_data.inc'
;include 'klib_crypto/data/base64_data.inc'


align 4 ; BASE64:
base64_table db "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	     db "abcdefghijklmnopqrstuvwxyz"
	     db "0123456789+/"





; CRYPTO RDATA:

  align 4  ; RC DATA:
  rc6_memvars:
	rc6_l_key		 rd 0045 ;dup 0
	rc6_ll			 rd 0009 ;dup 0
	crypt_rc4_table 	 rb 256 ;dup 0
	rc4ebp			 rd 1;0x00000000
	rc6_a			 rd 1
	rc6_b			 rd 1
	rc6_i			 rd 1
	rc6_j			 rd 1
	;----------- VMPC Stream Cipher variables:
  align 4
  vmpc_data:
	P			 rb 256 ;dup 0
	s			 rb 1
	n			 rb 1
	rb			 2;0, 0 ; just for align 4 stub only
	VMPCVector		 rb 64; dup 0
	vmpc_data_sz		 = 256+2+2;$ - vmpc_data
  rc6_memvars_sz = vmpc_data_sz + 4*(45+9+5) + 256;$- rc6_memvars

align 4
crc_table    rd      256

align 4 ; BASE64:
decode_table rb 0x0100 ;dup 0
base64_reg   rd 1
base64_res   rb 0x0100 ;dup 0
base64_rds   rb 0x0100 ;dup 0


align 4 ; BLOWFISH:
bf_pbox 	rd 18	 ;dup 0
bf_sbox 	rd 4*256 ;dup 0
bf_skey 	rb 72	 ;dup 0


align 4  ; GOST-14:
gost14_clear:
gost14_tmp	     rb 16 ; dup 0
gost14_Ltmp	     rb 16 ; dup 0
gost14_funcLSX_data1 rb 16 ; dup 0
gost14_funcLSX_data2 rb 16 ; dup 0
gost14_funcF_data1   rb 16 ; dup 0
gost14_funcF_data2   rb 16 ; dup 0
gost14_funcC_data    rb 16 ; dup 0
gost14_ExpandKey_c   rb 16 ; dup 0
gost14_ExpandKey_t1  rb 16 ; dup 0
gost14_ExpandKey_t2  rb 16 ; dup 0
gost14_xTemp	     rb 16 ; dup 0
gost14_yTemp	     rb 16 ; dup 0
gost14_ciphblock     rb 16 ; dup 0
gost14_keysmaster    rb 32 ; dup 0
gost14_keysblock     rb 160; dup 0
gost14_ivblock	     rb 16 ; dup 0
gost14_clear_sz = 16*14 + 21 + 160 ;$ - gost14_clear


align 4  ; GOST-89:
gostmemvars:
    gost_ext_simle_table     rb  1024 ;dup 0
    gost_int_table	     rb  0256 ;dup 0
    crypt_key		     rb  0032 ;dup 0
    crypt_vars:
    crypt_a		     rd  1;0x00000000
    crypt_b		     rd  1;0x00000000
    crypt_c		     rd  1;0x00000000
    crypt_d		     rd  1;0x00000000
gostmemvars_sz = 1024+256+32 + 4*4;$- gostmemvars
gostmemvars_end:
WolframKEYS:
    WfKey_A	   rb 32 ;dup 0
    WfKey_B	   rb 32 ;dup 0
WolframKEYS_Sz	   = 64;$ - WolframKEYS
userkey_buffer	   rb 32 ;dup 0
GOST_3411_vars:
	GOST_3411_HASH_ESI rd 1
	GOST_3411_HASH_EDI rd 1
	GOST_3411_HASH_ECX rd 1
	GOST_3411_HASH_EBX rd 1
	gost34_sum	   rb 32  ; DUP 0
	gost34_len	   rd 1
	gost34_len256	   rd 7   ; DUP 0
	gost34_sys	   rb 148 ; DUP 0
	GOST_3411_FillSboxes:
		.x1 rd 1
		.x2 rd 1
		.x3 rd 1
		.x4 rd 1
	gost_3411_table     rb	4096 ;dup 0
	gost_3411_lastblock rb	32   ;dup 0
GOST_3411_vars_sz = 4*4 + 32 + 4*8 + 148 + 4*4 + 4096 + 32; $ - GOST_3411_vars


align 4  ; GOST-64:
crc64table rq 256; dup 0

align 4  ; SERPENT:
SERPENT_data:
      SERPENT_l_key   rd 140; 140 DUP 0
      SERPENT_m_key   rd 280; 280 DUP 0
      SERPENT_block   rb 32;32 dup 0
SERPENT_data_sz = 140*4 + 280*4 + 32;$ - SERPENT_data


; CRYPTO SHA-1
SHA1_h0 	rd 1
SHA1_h1 	rd 1
SHA1_h2 	rd 1
SHA1_h3 	rd 1
SHA1_h4 	rd 1
SHA1_a		rd 1
SHA1_b		rd 1
SHA1_c		rd 1
SHA1_d		rd 1
SHA1_e		rd 1
SHA1_W		rd 80
SHA1_Buff	rb 64
sha1_dSize	rd 1
sha1_lpData	rd 1
sha1_len	rd 1
sha1_padding	rd 1
sha1_index	rd 1


; STRINGS:
  dot	 rd 1	; ”казатель на дробную часть
  exp	 rd 1	; ”казатель на экспоненту
  digit  rd 1	; ÷ифра


include 'klib_crypto/aes/aes_data.inc'