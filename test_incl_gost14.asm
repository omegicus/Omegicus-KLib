; ;invoke  SetDlgItemInt,[hwnddlg],CIPH_ALGO,eax,FALSE
; todo:
; - add folders\subfolders
; - UTF8 ? Unicode
; - Extract all
; - ? remove file from arc
; - test file without saving file

; ! relative filename

FORMAT PE GUI 4.0
ENTRY  START

	include '../fasm/include/win32ax.inc'
	include '../fasm/include/encoding/win1251.inc'


	include 'klib_const.inc'

clrMain 	=     0FF0000h	; Обычный цвет ссылки (синий)
clrActive	=     00000FFh	; Цвет активной ссылки (красный)



KID_LIST	  = 500
ID_ADDFILE	  = 501
ID_PASSWORD	  = 502
ID_STATUS	  = 503
CIPH_ALGO	  = 504
CREATEARC	  = 505

LVN_FIRST		= 0-100
LVN_ITEMACTIVATE	= LVN_FIRST - 14
LVM_GETSELECTIONMARK	= LVM_FIRST + 66


KUZNECHIK = 1


section '{code}' code readable executable
  START:
	invoke	GetModuleHandle,0
	mov	[hinstance], eax

	invoke	DialogBoxParam, eax, 37, HWND_DESKTOP, DialogProc, 0
	or	eax, eax
	jz	@F
      @@:
	invoke	ExitProcess,0





proc DialogProc hwnddlg,msg,wparam,lparam
     local	lvItem:   LV_COLUMN
     local	tempItem: LV_ITEM

	push	ebx esi edi

	mov eax, [hwnddlg]
	mov [window_handle], eax

	cmp	[msg],WM_INITDIALOG
	je     .wminitdialog
	cmp	[msg],WM_COMMAND
	je     .wmcommand
	cmp	[msg],WM_CLOSE
	je     .wmclose
	;
	xor	eax,eax
	jmp    .finish




       .wminitdialog:
		mov	dword[cipher], 0 ; Magma GOST
		; DropDown list fill (CIPHER):
		invoke	GetDlgItem,[hwnddlg],CIPH_ALGO
		mov	[CtrlID],eax
		mov	esi,ciph_items	  ; Указатель на список элементов
	    @@: invoke	lstrlen,esi  ; Длина строки
		or	eax,eax
		jz	@f
		push	eax
		; Добавить строку в список
		invoke	SendMessage, [CtrlID], CB_ADDSTRING, 0, esi
		pop	eax
		add	esi,eax      ; Следующий элемент списка
		inc	esi
		jmp	@b
	   @@:	invoke	SendMessage, [CtrlID], CB_SETCURSEL, 0, FALSE ; Установить пункт ID=2 дефолтным, нумерация ID начинается с 0
		;-----------------------------------------------------;
	jmp    .processed



  .wmcommand:
	cmp	[wparam], BN_CLICKED shl 16 + IDCANCEL
	je     .wmclose
	cmp	[wparam], BN_CLICKED shl 16 + CREATEARC
	je     .testcore
	cmp	[wparam], CBN_SELENDOK shl 16 + CIPH_ALGO
	je     .change_cipher
	jmp    .processed


  .change_cipher:
	invoke	SendMessage,  [CtrlID], CB_GETCURSEL, 0, 0
	cmp	eax, 2
	je     .cno
	mov	dword[cipher], eax
	jmp    .processed
  .cno: invoke	MessageBox,   [hwnddlg], _lerror_nocipher, _title, MB_ICONERROR+MB_OK
	jmp    .processed




  .testcore:
	invoke	GetDlgItemText,[hwnddlg],ID_PASSWORD, password, 64

	invoke	CreateFile,    fname_in, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, t1
	cmp	eax,	       INVALID_HANDLE_VALUE
	je     .ferror
	mov	[file_in],     eax
	invoke	GetFileSize,   eax, t2
	or	eax, eax
	jz     .error
	mov	[file_in_sz],  eax

	invoke	VirtualAlloc,  0, [file_in_sz], MEM_COMMIT, PAGE_READWRITE	; +MEM_RESERVE
	mov	[mem_in],      eax
	or	eax, eax
	jz     .error
	invoke	ReadFile,      [file_in], [mem_in], [file_in_sz], t1, 0
	or	eax, eax
	jz     .error
	invoke	CloseHandle,   [file_in]
	mov	esi, [mem_in]


	stdcall crypt_create_key,password, 32
	stdcall encrypt,	 [cipher], [mem_in], [file_in_sz], key512, 0; algo, data, size, key, outBuffer

	invoke	CreateFile,	fname_out, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0
	mov	[file_out],	eax
	;invoke  GetFileSize,	 [file_out], t2
	;xor	 eax, eax
	;invoke  SetFilePointer, [file_out], eax, 0, FILE_BEGIN
	invoke	WriteFile,	[file_out], [mem_in], [file_in_sz], t1, 0  ; file, buffer, size, written, 0
	invoke	SetEndOfFile,	[file_out]
	invoke	CloseHandle,	[file_out]


	stdcall crypt_create_key,password, 32
	stdcall decrypt,	 [cipher], [mem_in], [file_in_sz], key512, 0; algo, data, size, key, outBuffer

	invoke	CreateFile,	fname_out2, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0
	mov	[file_out],	eax
	;invoke  GetFileSize,	 [file_out], t2
	;xor	 eax, eax
	;invoke  SetFilePointer, [file_out], eax, 0, FILE_BEGIN
	invoke	WriteFile,	[file_out], [mem_in], [file_in_sz], t1, 0  ; file, buffer, size, written, 0
	invoke	SetEndOfFile,	[file_out]
	invoke	CloseHandle,	[file_out]



	invoke	VirtualFree,	[mem_in],   0, MEM_RELEASE
;	 invoke  VirtualFree,	[mem_out],   0, MEM_RELEASE
	invoke	MessageBox,   [hwnddlg], _lok_ready, _title, MB_OK
	jmp    .processed

  .ferror:
	invoke	MessageBox,   [hwnddlg], _lerror_noinput, _title, MB_ICONERROR+MB_OK
  .error:
	jmp    .processed

  .wmclose:
	invoke	EndDialog, [hwnddlg],0
  .processed:
	mov	eax,1
  .finish:
	pop	edi esi ebx
	ret
endp

include 'klib_strings.inc'
include 'klib_crypto.inc'

section '{data}' data readable writeable

  password	  db 64 dup 0

  hinstance	  dd 0
  window_handle   dd 0

  file_in	  dd 0
  file_in_sz	  dd 0
  file_out	  dd 0
  file_ouе_sz	  dd 0
  mem_in	  dd 0
  mem_out	  dd 0

  fname_in	  db 'in.data',0
  fname_out	  db 'out_encrypted.data',0
  fname_out2	  db 'out_decrypted.data',0

  t1		    dd 0
  t2		    dd 0

  _title	    db 'Omegicus CIPH',0
  _lerror_nocipher  db 'Selected algorithm not implemented', 0
  _lerror_noinput   db 'File "in.data" missed', 0
  _lok_ready	    db '1. "in.data" encrypted to "encrypted.data"',13,10
		    db '2. "encrypted.data" decrypted to "decrypted.data"', 0

cipher	dd 0
CtrlID	dd ?	       ; Хэндл списка
ciph_items:
	db 'GOST 28147-89 Magma',0
	db 'GOST 28147-14 Kuznechik',0
	db '<not used>',0
	db 'IDEA',0
	db 'AES',0
	db 'RC4',0
	db 'RC4-VMPC',0
	db 'RC6',0
	db 'Serpent',0
	db 'Blowfish',0
	db 0	       ; Признак окончания списка


include 'klib_data.inc'





section '{idata}' import data readable writeable
  library kernel32,'KERNEL32.DLL',\
	  user32,  'USER32.DLL'
  include '../fasm/include/api/kernel32.inc'
  include '../fasm/include/api/user32.inc'

section '.rsrc' resource data readable
  IDR_LOGO   = 7
  directory RT_DIALOG,		dialogs
  resource dialogs, 37,      LANG_ENGLISH+SUBLANG_DEFAULT,maindialog
  dialog maindialog, 'Omegicus WinKAR',50, 50,150,90,WS_CAPTION+WS_POPUP+WS_SYSMENU;+DS_MODALFRAME
    dialogitem 'STATIC','Password:'  ,		-1,  010, 05,130, 10, WS_VISIBLE
    dialogitem 'EDIT',	'',ID_PASSWORD, 	     010, 15,130, 13, WS_VISIBLE+WS_TABSTOP+WS_BORDER+ES_AUTOHSCROLL+ES_PASSWORD
    dialogitem 'BUTTON','TEST',        CREATEARC,    010, 65, 50, 14, WS_VISIBLE+BS_PUSHBUTTON
    dialogitem 'BUTTON','Quit',       IDCANCEL,       90, 65, 50, 14, WS_VISIBLE+WS_TABSTOP+BS_PUSHBUTTON
    dialogitem 'COMBOBOX', '',		 CIPH_ALGO,  010, 40,130, 90, WS_VISIBLE+CBS_DROPDOWNLIST+CBS_HASSTRINGS+WS_VSCROLL
  enddialog




