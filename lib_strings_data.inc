; entropy
entropy 	dd 0
entropy_dLen	dd 0
entropy_szStr	dd 0
entropy_lpRes	dd 0
entropy_coeff	dd 100
entropy_count	dd 0		; Количество символов
entropy_cnt	dd 256 dup 0	; Счетчики символов
mask_entropy	db '%.01i.%.02i', 0
;padding_entr	 db 256 dup 0
; //entropy