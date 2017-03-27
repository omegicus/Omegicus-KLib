align 4
base64_table db "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	     db "abcdefghijklmnopqrstuvwxyz"
	     db "0123456789+/"
align 4
decode_table db 0x0100 dup 0
base64_reg   dd 0
base64_res   db 0x0100 dup 0
base64_rds   db 0x0100 dup 0
