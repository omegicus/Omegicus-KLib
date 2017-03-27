; -----------------------------------------------------------------------------
;
; core schedule routine macros
; -----------------------------------------------------------------------------
 macro normalkey     {
     xor    eax, [edi-32]
     stosd	     }

 macro firstkey rcon {
     ror    eax, 16
     substitute
     xor    eax, [rcon]
     xor    eax, [edi-32]
     stosd	     }

 macro middlekey     {
     substitute
     ror    eax, 8
     normalkey	     }

 macro substitute    {
     xlatb
     ror    eax, 8
     xlatb
     ror    eax, 8
     xlatb
     ror    eax, 8
     xlatb	     }

 macro table_lookup_keys_d source {

    movzx   eax, byte[source+0]
    movzx   ebx, byte[source+1]
    movzx   ecx, byte[source+2]
    movzx   edx, byte[source+3]
    mov     eax, dword[eax*4+aes_tables.tab_u1]
    xor     eax, dword[ebx*4+aes_tables.tab_u2]
    xor     eax, dword[ecx*4+aes_tables.tab_u3]
    xor     eax, dword[edx*4+aes_tables.tab_u4]
    stosd  }




; -----------------------------------------------------------------------------
; core aes routine macros
; -----------------------------------------------------------------------------
; encryption operations
 macro table_lookup_key_add source,dest,key {

  rept 4 column {

    movzx   eax, byte[source+((column-1) mod 4)*4+0]
    movzx   ebx, byte[source+((column+0) mod 4)*4+1]
    movzx   ecx, byte[source+((column+1) mod 4)*4+2]
    mov     eax, dword[eax*4+aes_tables.tab_t1]
    xor     eax, dword[ebx*4+aes_tables.tab_t2]
    movzx   ebx, byte[source+((column+2) mod 4)*4+3]
    xor     eax, dword[ecx*4+aes_tables.tab_t3]
    xor     eax, dword[ebx*4+aes_tables.tab_t4]
    xor     eax, dword[key+(column-1)*4]
    mov     dword[dest+(column-1)*4],eax  \}  }

 macro table_lookup_final source,dest,key {

  rept 4 column  {

    movzx   eax, byte[source+((column+2) mod 4)*4+3]
    mov     bl , byte[source+((column+1) mod 4)*4+2]
    mov     cl , byte[source+((column+0) mod 4)*4+1]
    mov     dl , byte[source+((column-1) mod 4)*4+0]
    mov     ah , byte[eax+aes_tables.tab_s]
    mov     al , byte[ebx+aes_tables.tab_s]
    shl     eax, 16
    mov     ah , byte[ecx+aes_tables.tab_s]
    mov     al , byte[edx+aes_tables.tab_s]
    xor     eax, dword[key+(column-1)*4]
    mov     dword[dest+(column-1)*4],eax  \}  }

; decryption operations

 macro table_lookup_key_add_d source,dest,key {

  rept 4 column {

    movzx   eax, byte[source+((column-1) mod 4)*4+0]
    movzx   ebx, byte[source+((column+2) mod 4)*4+1]
    movzx   ecx, byte[source+((column+1) mod 4)*4+2]
    mov     eax, dword[eax*4+aes_tables.tab_t5]
    xor     eax, dword[ebx*4+aes_tables.tab_t6]
    movzx   ebx, byte[source+((column+0) mod 4)*4+3]
    xor     eax, dword[ecx*4+aes_tables.tab_t7]
    xor     eax, dword[ebx*4+aes_tables.tab_t8]
    xor     eax, dword[key+(column-1)*4]
    mov     dword[dest+(column-1)*4], eax   \}	}

 macro table_lookup_final_d source,dest,key {

  rept 4 column {

    movzx  eax , byte[source+((column+0) mod 4)*4+3]
    mov     bl , byte[source+((column+1) mod 4)*4+2]
    mov     cl , byte[source+((column+2) mod 4)*4+1]
    mov     dl , byte[source+((column-1) mod 4)*4+0]
    mov     ah , byte[eax+aes_tables.tab_si]
    mov     al , byte[ebx+aes_tables.tab_si]
    shl     eax, 16
    mov     ah , byte[ecx+aes_tables.tab_si]
    mov     al , byte[edx+aes_tables.tab_si]
    xor     eax, dword[key+(column-1)*4]
    mov     dword[dest+(column-1)*4], eax   \} }

; -----------------------------------------------------------------------------
;
; common stack routine macros
; -----------------------------------------------------------------------------
 macro preserve_common_registers {

    sub     esp, aes_stack_space
    mov     [esp+ 0], ebx
    mov     [esp+ 4], ebp
    mov     [esp+ 8], edi
    mov     [esp+12], esi    }

 macro restore_common_registers ret_val {

    mov     ebx, [esp+ 0]
    mov     ebp, [esp+ 4]
    mov     edi, [esp+ 8]
    mov     esi, [esp+12]
    add     esp, aes_stack_space
    ret     ret_val  }