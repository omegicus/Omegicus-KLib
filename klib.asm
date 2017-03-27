use32

;KUZNECHIK = 1

include 'klib_charset.inc'
include 'klib_sort.inc'
include 'klib_strings.inc'
	; string2float: ;lpStr db '...',lpResult dq
	; strlen  IN: esi - string, OUT: ecx - string length
	; strcopy esi edi
	; strtoupper esi edi
	; strtolower esi edi
	; strcmp esi edi, OUT: ja, je , ...
	; strpos esi=string, edi=search, OUT: edx=pos| -1
	; stdcall _replace,buff1,szStr1,szStr2,buff2,1

;include 'klib_time.inc'

include 'klib_zlib.inc'
include 'klib_crypto.inc'



include 'klib_data.inc'