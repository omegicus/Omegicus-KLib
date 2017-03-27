; Processor: PII (200 MHz)
; Algorithm: serpent
; Static Key Schedule (C)
; 256 Bit:   Key Setup: 1273/1273 cycles
; Encrypt:   769 cycles = 33.2 mbits/sec
; Decrypt:   767 cycles = 33.3 mbits/sec

use32

;include 'serpent_data.inc'



macro sb0 r0,r1,r2,r3,r4 {
       xor r3,r0
       mov r4,r1
       and r1,r3
       xor r4,r2
       xor r1,r0
       or  r0,r3
       xor r0,r4
       xor r4,r3
       xor r3,r2
       or  r2,r1
       xor r2,r4
       not r4
       or  r4,r1
       xor r1,r3
       xor r1,r4
       or  r3,r0
       xor r1,r3
       xor r4,r3
}

macro ib0 r0,r1,r2,r3,r4 {
       not r2
       mov r4,r1
	or r1,r0
       not r4
       xor r1,r2
       or  r2,r4
       xor r1,r3
       xor r0,r4
       xor r2,r0
       and r0,r3
       xor r4,r0
       or  r0,r1
       xor r0,r2
       xor r3,r4
       xor r2,r1
       xor r3,r0
       xor r3,r1
       and r2,r3
       xor r4,r2
}

macro sb1 r0,r1,r2,r3,r4 {
       not r0
       not r2
       mov r4,r0
       and r0,r1
       xor r2,r0
       or  r0,r3
       xor r3,r2
       xor r1,r0
       xor r0,r4
       or  r4,r1
       xor r1,r3
       or  r2,r0
       and r2,r4
       xor r0,r1
       and r1,r2
       xor r1,r0
       and r0,r2
       xor r0,r4
}

macro ib1 r0,r1,r2,r3,r4 {
       mov r4,r1
       xor r1,r3
       and r3,r1
       xor r4,r2
       xor r3,r0
       or  r0,r1
       xor r2,r3
       xor r0,r4
       or  r0,r2
       xor r1,r3
       xor r0,r1
       or  r1,r3
       xor r1,r0
       not r4
       xor r4,r1
       or  r1,r0
       xor r1,r0
       or  r1,r4
       xor r3,r1
}

macro sb2 r0,r1,r2,r3,r4 {
       mov r4,r0
       and r0,r2
       xor r0,r3
       xor r2,r1
       xor r2,r0
       or  r3,r4
       xor r3,r1
       xor r4,r2
       mov r1,r3
       or  r3,r4
       xor r3,r0
       and r0,r1
       xor r4,r0
       xor r1,r3
       xor r1,r4
       not r4
}

macro ib2 r0,r1,r2,r3,r4 {
       xor r2,r3
       xor r3,r0
       mov r4,r3
       and r3,r2
       xor r3,r1
       or  r1,r2
       xor r1,r4
       and r4,r3
       xor r2,r3
       and r4,r0
       xor r4,r2
       and r2,r1
       or  r2,r0
       not r3
       xor r2,r3
       xor r0,r3
       and r0,r1
       xor r3,r4
       xor r3,r0
}

macro sb3 r0,r1,r2,r3,r4
{
       mov r4,r0
       or  r0,r3
       xor r3,r1
       and r1,r4
       xor r4,r2
       xor r2,r3
       and r3,r0
       or  r4,r1
       xor r3,r4
       xor r0,r1
       and r4,r0
       xor r1,r3
       xor r4,r2
       or  r1,r0
       xor r1,r2
       xor r0,r3
       mov r2,r1
       or  r1,r3
       xor r1,r0
}

macro ib3 r0,r1,r2,r3,r4 {
       mov r4,r2
       xor r2,r1
       and r1,r2
       xor r1,r0
       and r0,r4
       xor r4,r3
       or  r3,r1
       xor r3,r2
       xor r0,r4
       xor r2,r0
       or  r0,r3
       xor r0,r1
       xor r4,r2
       and r2,r3
       or  r1,r3
       xor r1,r2
       xor r4,r0
       xor r2,r4
}

macro sb4 r0,r1,r2,r3,r4 {
       xor r1,r3
       not r3
       xor r2,r3
       xor r3,r0
       mov r4,r1
       and r1,r3
       xor r1,r2
       xor r4,r3
       xor r0,r4
       and r2,r4
       xor r2,r0
       and r0,r1
       xor r3,r0
       or  r4,r1
       xor r4,r0
       or  r0,r3
       xor r0,r2
       and r2,r3
       not r0
       xor r4,r2
}

macro ib4 r0,r1,r2,r3,r4
{
       mov r4,r2
       and r2,r3
       xor r2,r1
       or  r1,r3
       and r1,r0
       xor r4,r2
       xor r4,r1
       and r1,r2
       not r0
       xor r3,r4
       xor r1,r3
       and r3,r0
       xor r3,r2
       xor r0,r1
       and r2,r0
       xor r3,r0
       xor r2,r4
       or  r2,r3
       xor r3,r0
       xor r2,r1
}

macro sb5 r0,r1,r2,r3,r4
{
       xor r0,r1
       xor r1,r3
       not r3
       mov r4,r1
       and r1,r0
       xor r2,r3
       xor r1,r2
       or  r2,r4
       xor r4,r3
       and r3,r1
       xor r3,r0
       xor r4,r1
       xor r4,r2
       xor r2,r0
       and r0,r3
       not r2
       xor r0,r4
       or  r4,r3
       xor r2,r4
}

macro ib5 r0,r1,r2,r3,r4
{
       not r1
       mov r4,r3
       xor r2,r1
       or  r3,r0
       xor r3,r2
       or  r2,r1
       and r2,r0
       xor r4,r3
       xor r2,r4
       or  r4,r0
       xor r4,r1
       and r1,r2
       xor r1,r3
       xor r4,r2
       and r3,r4
       xor r4,r1
       xor r3,r0
       xor r3,r4
       not r4
}

macro sb6 r0,r1,r2,r3,r4
{
       not r2
       mov r4,r3
       and r3,r0
       xor r0,r4
       xor r3,r2
       or  r2,r4
       xor r1,r3
       xor r2,r0
       or  r0,r1
       xor r2,r1
       xor r4,r0
       or  r0,r3
       xor r0,r2
       xor r4,r3
       xor r4,r0
       not r3
       and r2,r4
       xor r2,r3
}

macro ib6 r0,r1,r2,r3,r4
{
       xor r0,r2
       mov r4,r2
       and r2,r0
       xor r4,r3
       not r2
       xor r3,r1
       xor r2,r3
       or  r4,r0
       xor r0,r2
       xor r3,r4
       xor r4,r1
       and r1,r3
       xor r1,r0
       xor r0,r3
       or  r0,r2
       xor r3,r1
       xor r4,r0
}

macro sb7 r0,r1,r2,r3,r4
{
       mov r4,r2
       and r2,r1
       xor r2,r3
       and r3,r1
       xor r4,r2
       xor r2,r1
       xor r1,r0
       or  r0,r4
       xor r0,r2
       xor r3,r1
       xor r2,r3
       and r3,r0
       xor r3,r4
       xor r4,r2
       and r2,r0
       not r4
       xor r2,r4
       and r4,r0
       xor r1,r3
       xor r4,r1
}

macro ib7 r0,r1,r2,r3,r4
{
       mov r4,r2
       xor r2,r0
       and r0,r3
       not r2
       or  r4,r3
       xor r3,r1
       or  r1,r0
       xor r0,r2
       and r2,r4
       xor r1,r2
       xor r2,r0
       or  r0,r2
       and r3,r4
       xor r0,r3
       xor r4,r1
       xor r3,r4
       or  r4,r0
       xor r3,r2
       xor r4,r2
}

macro f_key r0,r1,r2,r3
{
       mov r0,[esi]
       mov r1,[esi+4]
       mov r2,[esi+8]
       mov r3,[esi+12]
}

macro t_key r0,r1,r2,r3
{
       mov [esi  ],r0
       mov [esi+4],r1
       mov [esi+8],r2
       mov [esi+12],r3
       add esi,16
}

macro f_xor r0,r1,r2,r3
{
       xor r0,[esi]
       xor r1,[esi+4]
       xor r2,[esi+8]
       xor r3,[esi+12]
       add esi,16
}

macro i_xor r0,r1,r2,r3
{
       sub esi,16
       xor r0,[esi]
       xor r1,[esi+4]
       xor r2,[esi+8]
       xor r3,[esi+12]
}

; the linear transformation and its inverse
macro rot r0,r1,r2,r3,r4
{
       rol r0,13
       rol r2,3
       mov r4,r0
       sal r4,3
       xor r3,r2
       xor r3,r4
       xor r1,r0
       xor r1,r2
       rol r1,1
       rol r3,7
       mov r4,r1
       xor r0,r1
       xor r0,r3
       sal r4,7
       xor r2,r3
       xor r2,r4
       rol r0,5
       rol r2,22
}

macro irot r0,r1,r2,r3,r4
{
       ror r2,22
       ror r0,5
       mov r4,r1
       sal r4,7
       xor r2,r3
       xor r2,r4
       xor r0,r1
       xor r0,r3
       ror r3,7
       mov r4,r0
       ror r1,1
       sal r4,3
       xor r3,r2
       xor r3,r4
       xor r1,r0
       xor r1,r2
       ror r2,3
       ror r0,13
}



; initialise the key schedule from the user supplied key
macro k_loop {								       \
    f_key    eax,ebx,ecx,edx
    sb3      eax,ebx,ecx,edx,edi
    t_key    ebx,ecx,edx,edi
    f_key    eax,ebx,ecx,edx
    sb2      eax,ebx,ecx,edx,edi
    t_key    ecx,edx,ebx,edi
    f_key    eax,ebx,ecx,edx
    sb1      eax,ebx,ecx,edx,edi
    t_key    ecx,eax,edx,ebx
    f_key    eax,ebx,ecx,edx
    sb0      eax,ebx,ecx,edx,edi
    t_key    ebx,edi,ecx,eax
    f_key    eax,ebx,ecx,edx
    sb7      eax,ebx,ecx,edx,edi
    t_key    ecx,edi,edx,eax
    f_key    eax,ebx,ecx,edx
    sb6      eax,ebx,ecx,edx,edi
    t_key    eax,ebx,edi,ecx
    f_key    eax,ebx,ecx,edx
    sb5      eax,ebx,ecx,edx,edi
    t_key    ebx,edx,eax,ecx
    f_key    eax,ebx,ecx,edx
    sb4      eax,ebx,ecx,edx,edi
    t_key    ebx,edi,eax,edx
}

; encrypt a block of text
macro f_loop a,b,c,d,t {
    f_xor    a,b,c,d
    sb0      a,b,c,d,t
    rot      b,t,c,a,d
    f_xor    b,t,c,a
    sb1      b,t,c,a,d
    rot      c,b,a,t,d
    f_xor    c,b,a,t
    sb2      c,b,a,t,d
    rot      a,t,b,d,c
    f_xor    a,t,b,d
    sb3      a,t,b,d,c
    rot      t,b,d,c,a
    f_xor    t,b,d,c
    sb4      t,b,d,c,a
    rot      b,a,t,c,d
    f_xor    b,a,t,c
    sb5      b,a,t,c,d
    rot      a,c,b,t,d
    f_xor    a,c,b,t
    sb6      a,c,b,t,d
    rot      a,c,d,b,t
    f_xor    a,c,d,b
    sb7      a,c,d,b,t
}

; decrypt a block of text
macro i_loop a,b,c,d,t {
    ib7      a,b,c,d,t
    i_xor    d,a,b,t
    irot     d,a,b,t,c
    ib6      d,a,b,t,c
    i_xor    a,b,c,t
    irot     a,b,c,t,d
    ib5      a,b,c,t,d
    i_xor    b,d,t,c
    irot     b,d,t,c,a
    ib4      b,d,t,c,a
    i_xor    b,c,t,a
    irot     b,c,t,a,d
    ib3      b,c,t,a,d
    i_xor    a,b,t,c
    irot     a,b,t,c,d
    ib2      a,b,t,c,d
    i_xor    b,d,t,c
    irot     b,d,t,c,a
    ib1      b,d,t,c,a
    i_xor    a,b,c,t
    irot     a,b,c,t,d
    ib0      a,b,c,t,d
    i_xor    a,d,b,t
}





SERPENT_set_key: ; ecx = key_len(256,...); esi: key^
    mov     edx, ecx
    cmp     edx, 256
    ja	   .l3
    mov     ebx, SERPENT_l_key
    mov     ecx, edx
    add     ecx, 31
    sar     ecx, 5
    je	   .l0
    mov     edi,ebx
    mov     eax,ecx
    rep     movsd
  .l0:
    cmp     edx,256
    je	   .l1
    mov     ecx,8
    sub     ecx,eax
    xor     eax,eax
    rep     stosd
    mov     ecx,edx
    mov     edx,1
    sal     edx,cl
    sar     ecx,5
    mov     eax,edx
    dec     eax
    and     eax,[ebx+4*ecx]
    or	    eax,edx
    mov     [ebx+4*ecx],eax
  .l1:
    mov     esi,ebx
    mov     eax,0x9e3779b9
    mov     ebx,eax
    xor     eax,[esi+ 8]
    xor     ebx,[esi+12]
    xor     eax,[esi+16]
    xor     ebx,[esi+20]
    xor     eax,[esi+24]
    xor     ebx,[esi+28]
    mov     ecx,0
    push    esi
  .l2:
    mov     edx,[esi]
    xor     edx,ecx
    xor     edx,ebx
    ror     edx,21
    mov     [esi+32],edx
    xor     eax,[esi+8]
    xor     eax,edx
    add     esi,4
    inc     ecx
    mov     edx,[esi]
    xor     edx,ecx
    xor     edx,eax
    ror     edx,21
    mov     [esi+32],edx
    xor     ebx,[esi+8]
    xor     ebx,edx
    add     esi,4
    inc     ecx
    cmp     ecx,132
    jne    .l2
    pop     esi
    add     esi,4*8

    k_loop; 
    k_loop; 
    k_loop; 
    k_loop;

    f_key	eax,ebx,ecx,edx
    sb3 	eax,ebx,ecx,edx,edi
    t_key	ebx,ecx,edx,edi
 .l3:
    ret




SERPENT_encrypt: ; ESI: 16byte block, EDI: out_16byteBlock
    ;mov     esi, in_blk
    mov     eax, [esi+00]
    mov     ebx, [esi+04]
    mov     ecx, [esi+08]
    mov     edx, [esi+12]
    lea     esi, [SERPENT_l_key + 4*8]
    push    edi
    f_loop  eax,ebx,ecx,edx,edi
    rot     edx,edi,ebx,eax,ecx
    f_loop  edx,edi,ebx,eax,ecx
    rot     eax,ecx,edi,edx,ebx
    f_loop  eax,ecx,edi,edx,ebx
    rot     edx,ebx,ecx,eax,edi
    f_loop  edx,ebx,ecx,eax,edi
    f_xor   eax,edi,ebx,edx
    pop     esi
    ;mov     esi,out_blk
    mov     [esi+00], eax
    mov     [esi+04], edi
    mov     [esi+08], ebx
    mov     [esi+12], edx
ret


SERPENT_decrypt: ; ESI: 16byteblock_IN, edi: 16byteblockOUT
    ;mov esi,in_blk
    mov     eax, [esi+00]
    mov     ebx, [esi+04]
    mov     ecx, [esi+08]
    mov     edx, [esi+12]
    lea     esi, [SERPENT_l_key + 4*140]
    push    edi
    i_xor   eax,ebx,ecx,edx
    i_loop  eax,ebx,ecx,edx,edi
    irot    eax,edx,ebx,edi,ecx
    i_loop  eax,edx,ebx,edi,ecx
    irot    eax,edi,edx,ecx,ebx
    i_loop  eax,edi,edx,ecx,ebx
    irot    eax,ecx,edi,ebx,edx
    i_loop  eax,ecx,edi,ebx,edx
    pop     esi
    ;mov esi,out_blk
    mov    [esi+00], eax
    mov    [esi+04], ebx
    mov    [esi+08], ecx
    mov    [esi+12], edx
ret
