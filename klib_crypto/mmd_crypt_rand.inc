use32


align 4
init_rnd:
       rdtsc
       ;mov [seed], eax
       ret

align 4
get_rnd:
       push ecx
       push edx
      ; mov eax, [seed]
       mov ecx, 0x41C64E6D
       mul ecx
       add eax, 0x00003039
       and eax, 0x7FFFFFFF
      ; mov [seed],eax
       pop edx
       pop ecx
       ret

align 4
get_rnd_range:
       push ecx
       push edx
       mov ecx, eax
       call get_rnd
       xor edx, edx
       div ecx
       mov eax, edx
       pop edx
       pop ecx
       ret

