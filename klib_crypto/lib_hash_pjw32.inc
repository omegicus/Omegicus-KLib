; �������� 32 ��������� ������� Peter J. Weinberger
use32

align 4
pjw32:
; IN:  esi: source, ecx: ����� ������
; OUT: eax: hash
 push  esi ebx edx ecx
    xor   edx, edx		 ; Hash=0
    xor   ebx, ebx		 ; tmp=0
    cld 			 ;
  @pjwnext:			 ;
    xor   eax, eax		 ;
    lodsb			 ;
    shl   edx, 4		 ;
    add   edx, eax		 ; ���������� ������ �����
    mov   ebx, edx		 ; �������� ��������
    and   ebx, 0x0F000000	 ; ebx = edx AND 0x0F000000
   jz	 @pjwl1 		 ; �������, ���� ebx=0
    shr   ebx, 24		 ;
    xor   edx, ebx		 ; edx = edx xor (ebx>>24)
  @pjwl1:			 ;
    not   ebx			 ; !!!???
    and   edx, ebx		 ; !!!??? edx = edx and (not ebx)
    dec   ecx			 ; ��������� �������
  jnz	@pjwnext		 ;
  @pjwdone:			 ;
    mov   eax, edx		 ; ������� 4 ���� � eax
 pop  ecx edx ebx esi
ret
