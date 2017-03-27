; -----------------------------------------------------------------------------
; table generation (galois field polynomials / memory)
 gf_modulus	     = 0x1b
 gf_affine	     = 0x1f
 gf_magic	     = 0x63
 alog_table	     = aes_tables.tab_t2
 log_table	     = aes_tables.tab_t2+256
; stack frame
 aes_stack_space     = 16 ; stack space for preserved registers
 aes_in_blk	     = 4  ; offset to in[] parameter
 aes_out_blk	     = 8  ; offset to out[] parameter
 aes_schedule_param  = 4  ; schedule's offset to parameter
; aes constants
 aes_max_rounds      = 14 ; aes-256 rounds
; size control options
 aes_encrypt_unroll  = 0  ; 0 for small & slow looped, 1 for big & fast unrolled
 aes_decrypt_unroll  = 0  ; //
 aes_schedule_unroll = 0  ; //

; -----------------------------------------------------------------------------
; structures
 struct aes_state_
     align 16
     keys	rd 4*(aes_max_rounds+1)
     align 16
     keys_d	rd 4*(aes_max_rounds+1)
     align 16
     state	rb 16
		rb 16
 ends
 struct aes_tables_
     tab_s	rb 256
     tab_si	rb 256
     tab_t1	rd 256
     tab_t2	rd 256
     tab_t3	rd 256
     tab_t4	rd 256
     tab_t5	rd 256
     tab_t6	rd 256
     tab_t7	rd 256
     tab_t8	rd 256
     tab_u1	rd 256
     tab_u2	rd 256
     tab_u3	rd 256
     tab_u4	rd 256
     tab_rc	rd 30
 ends