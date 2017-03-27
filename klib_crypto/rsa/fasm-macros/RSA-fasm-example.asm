max_modulus_words fix 16	;for this example we use 512 bits, that means 16 dwords
include 'RSA-fasm.inc'

;************************************************************
;Our publicised information. Tell everybody
;************************************************************

public_modulus	equ	05E92B22DFF503C7Bh,05B2F062DD8C13F22h,0FA8FDAF9DF5C1B05h,088ABD46CADB707A7h,\
			0CE4B56BBB19C50EEh,0CC107EA49A80727Ch,0FE3DB9CB1DD18BBAh,0E948B30F5E4C7434h
public_key	equ	7

;************************************************************
;Our secret decoding/encoding key. Tell no one
;************************************************************

private_key	equ	06D2CD042BB8B3567h,038ECEFD59AA56CA5h,0979F587848E13AE6h,0091C6A663EEF7D23h,\
			08415867EC508171Fh,05EE0242F079269DAh,048A3EBF0E3F2BA35h,042A70E96AD3A6A58h

;************************************************************
;The data we want to sign/encrypt
;************************************************************

special_text:	db	"Signature or key message goes here"
		times	((-$) and 63) db 0

;************************************************************
;Include our public information so others know how to decode
;************************************************************

exposed_modulus dq	public_modulus
exposed_key	dd	public_key

modulus_words equ 16	;set the size of our data for the macros

RSA_encrypt special_text,special_text,exposed_modulus,private_key
