; Unicode UTF-8:
; 0x00000000 — 0x0000007F: 0xxxxxxx
; 0x00000080 — 0x000007FF: 110xxxxx 10xxxxxx
; 0x00000800 — 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
; 0x00010000 — 0x001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
; UTF-8     EF BB BF
; UTF-16BE  FE FF
; UTF-16LE  FF FE
; UTF-32BE  00 00 FE FF
; UTF-32LE  FF FE 00 00

USE32



cp1251_to_866_char: ; al
       @@: cmp	al,0xc0
	   jb  .1
	   cmp	al,0xef
	   ja  .3
	   xor	al,0x40
	   jmp .2
       .1: cmp	al,0xa8
	   jnz .3
	   mov	al,0xf0
	   jmp .2
       .3: cmp	al,0xb8
	   jnz .4
	   mov	al,0xf1
	   jmp .2
       .4: cmp	al,0xf0
	   jb  .2
	   xor	al,0x10
       .2: ret





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


align 4
utf8_to_cp1251:
; in:  esi: string-Z, edi: out buffer

; out: -
ret


align 4
cp1251_to_utf8: ;--------------------------------;
; in:  esi: string-Z, edi: out buffer            ;
     push   eax ebx esi edi			 ;
 .n:						 ;
     lodsb					 ;
     or     al,  al				 ;
     jz     short .exit 			 ;
     cmp    al,  0x7F				 ;
     ja     short @f				 ;
     and    al, 01111111b			 ; clear hight bit
     stosb					 ;
     jmp    short .n				 ;
 @@: mov    bl,  al				 ;
     push   esi 				 ;
     mov    esi, cp1251utf8_array		 ;
 @@: lodsw					 ;
     or     ax, ax				 ;
     jz     short .k				 ;
     cmp    al, bl				 ;
     jne    short @b				 ;
     jmp    short @f				 ;
 .k: pop    esi 				 ;
     jmp    short .n				 ;
 @@: pop    esi 				 ;
     cmp    al, 184				 ;
     je     short @f				 ;
     cmp    al, 240				 ;
     jae    short @f				 ;
     mov    al, 208				 ;
     jmp    short .s				 ;
 @@: mov    al, 209				 ;
 .s: stosw					 ; store utf special byte
     jmp    short .n				 ;
 .exit: 					 ;
     xor    al, al				 ;
     stosb					 ;
     pop    edi esi ebx eax			 ;
; out: -                                         ;
ret ;--------------------------------------------;
