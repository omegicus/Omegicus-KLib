align 4

sha1_len	rd 1
sha1_padding	rd 1
sha1_index	rd 1
sha1_dSize	rd 1
sha1_lpData	rd 1

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



SHA1_W		rd 80 ;dup 0
SHA1_Buff	rb 64 ;dup 0