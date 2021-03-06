; �����������:
;	* ����������� ���� � ������� ������ ����� 11 ��������� ���:
;		- � 1 ������ 5843880 �. �� �.�. �� 3 ������� 5915100 �.
;		  ��� ���������� ���������,
;		- � 30 ������� 5844001 �. �� �.�. �� 17 ������ 5915222 �.
;		  ��� �������������� ���������,
;	* ��������� ��������� ��� ����� ����������� ��������� � �������������
;	  � ���������� �������,
;	* ����������� ��� ������ ��� ������ ����,
;	* ���������� ������ ��� � ���� ��� ������ ������� ������ � ���
;	  � ������,
;	* ����������� ������������ ���� � ������ ���������,
;	* ���������� "�����������" ������ ��� ���������������� ������
;	  ����, ��������� ���� ����� ����� ������������ ���������� ����
;	  ����� ����� ������.
;
; (C) Mikolaj Hajduk, 16.06.2008.
;

; ����������� ������������ ��������.
;
C1	= 365			; ���������� ���� � ������� ����.
C4	= 4*C1 + 1		; ���������� ���� � ������������ ����� (��������  ����� ���������� ���������).
C100	= 25*C4 - 1		; ���������� ���� � "�������" �������� �������������� ���������
				; (�.�. � ��������, ������� ������������� ������������ �����).
C400	= 4*C100 + 1		; ���������� ���� � ������ 400-������ ����� �������������� ���������.
k	= 30
J	= 194796		; ���������, ������� ���������� ���������� ������ ��� ���������� ����������
G	= 194800		; � �������������� ���������� � ������� ������� ������ "�������� �����" T.


; �������, ������� �������� ����� ������� �������� � ����������� �����.
MonthLen	db 31,	28,  31,  30,  31,  30,  31,  31,  30,	31,  30,  31
		db 31,	29,  31,  30,  31,  30,  31,  31,  30,	31,  30,  31

; �������, ������� �������� �������� ������� 'DaySum' ��� ���� ��� (����� ������, ���� ������������ ����).
DaySum		dw  0,	31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
		dw  0,	31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335



; DWORD DayOfWeek(DWORD Y, DWORD M, DWORD D, DWORD Gregorian)
; ������� ������������ ��� ������ ���� ����� ��� ������. ������� ��������� �����������: 0 - �����������,
; 1 - �����������, 2 - �������, 3 - �����, 4 - �������, 5 - �������, 6 - �������.
; ���������:
;	Y - ���,
;	M - �����,
;	D - ����,
;	Gregorian - ����������� ��������� (0 - ���������, 1 - �������������).
; ������������ ��������:
;	* 0, 1, ..., 6, ���� ���� �����,
;	* -1, ���� ������ �������.
proc	DayOfWeek, Y, M, D, Gregorian
	pushfd
	push	ebx edx
	stdcall DateToAbsDayNum, [Y], [M], [D], [Gregorian]		; eax := N
	test	eax, eax
	jz     .err
	mov	ebx, 7							;
	xor	edx, edx						;
	add	eax, 5							; edx := (eax + 5) mod 7 = (N + 5) mod 7
	adc	edx, edx						;
	div	ebx							;
	xchg	eax, edx						; eax := edx
	jmp    .exit
       .err:
	mov	eax, -1
       .exit:
	pop	edx ebx
	popfd
	ret


; DWORD IsLeapYear(DWORD Y, DWORD Gregorian)
; ������� ���������� ������������ ���� � ������ ���������.
; ���������:
;	Y - ���,
;	Gregorian - ����������� ��������� (0 - ���������, 1 - �������������).
; ������������ ��������:
;	* 1, ���� ��� Y ����������, 0 - � ��������� ������.
;	* -1, ���� ������ �������.
proc	IsLeapYear, Y, Gregorian

	pushfd
	push	ebx edx
       .CheckParameters:
	test	[Gregorian], -2
	jnz	.Error
       .IsYNegative:
	mov	eax, [Y]
	test	eax, eax
	jz	.Error
	jns	.CheckCalendar
	;
	inc	eax
	neg	eax
	;
       .CheckCalendar:
	cmp	[Gregorian], 0
	je     .mod4
       .Gregorian:
	xor	edx, edx
	mov	ebx, 100
	div	ebx
	test	edx, edx
	jz     .mod4
	mov	eax, edx
	;
       .mod4:
	shr	eax, 1
	jc     .Result
	shr	eax, 1
	jmp    .Result
       .Error:
	mov	eax, -1
	jmp    .End
       .Result:
	setnc	al
	movzx	eax, al
       .End:
	pop	edx ebx
	popfd
	ret





; DWORD MDToDayNum(DWORD M, DWORD D, DWORD LeapYearFlag)
; ������� ������������ ���������� ����� ��� � ���� (� ����������� �� ������������ ����).
; ���������:
;	M - �����,
;	D - ����,
;	LeapYearFlag - ���� ������������ ���� (0 - ��� ����������, 1 - ����������).
; ������������ ��������:
;	* 1, 2, ..., 365 ��� ����������� ����, 1, 2, ..., 366 ��� ����������� ����,
;	* -1, ���� ������ �������.
;
proc	MDToDayNum, M, D, LeapYearFlag
	pushfd
	push	ebx edx
       .LeapYearFlag:
	test	[LeapYearFlag], -2				; 0 <= LeapYearFlag <= 1
	jnz    .Error					       ;
       .Month:
	cmp	[M], 1						;
	jb     .Error					       ; 1 <= M <= 12
	cmp	[M], 12 					;
	ja     .Error					       ;
       .Day:
	cmp	[D], 1						; D >= 1
	jb	.Error						;
	mov	ebx, [LeapYearFlag]				; ebx := LeapYearFlag
	lea	ebx, [ebx + 2*ebx]				; ebx := 3*ebx = 3*LeapYearFlag
	shl	ebx, 2						; ebx := 4*ebx = 12*LeapYearFlag
	mov	edx, [M]					; eax := MonthLen[M - 1 + 12*LeapYearFlag]
	movzx	eax, [MonthLen - 1 + ebx + edx] 		;
	cmp	[D], eax					; D <= MonthLen[M - 1 + 12*LeapYearFlag]
	ja	.Error						;
       .CalculateDayNum:
	shl	ebx, 1						; ebx := 2*ebx = 24*LeapYearFlag
	movzx	eax, [DaySum - 2 + ebx + 2*edx] 		; eax := DaySum(M, LeapYearFlag)
	add	eax, [D]					; eax := eax + D = DaySum(M, LeapYearFlag) + D
	jmp	.End
       .Error:
	mov	eax, -1
       .End:
	pop	edx ebx
	popfd
	ret





; DWORD DayNumToMD(DWORD n, DWORD LeapYearFlag, DWORD* M, DWORD* D)
; ������� ������������ ������ ����� ��� � ���� � ��������������� ������ ������ � ��� � ������.
; ��������� ������ ������� �� �������� ����� ������������ ����.
; ���������:
;	n - ����� ��� � ����,
;	LeapYearFlag - ���� ������������ ���� (0 - ��� ����������, 1 - ����������),
;	M - ��������� �� ���������� � ������� ����� ������� ����������� ����� ������,
;	D - ��������� �� ���������� � ������� ����� ������� ����������� ����� ���. 
; ������������ ��������:
;	* 0, ���� ������ ����� (n, LeapYearFlag),
;	* -1 � ��������� ������.
proc	DayNumToMD, n, LeapYearFlag, M, D
	pushfd
	push	ebx ecx edx
       .CheckParameters:
	test	[LeapYearFlag], -2
	jnz    .Error
	cmp	[n], 1
	jb     .Error
	mov	eax, 365
	add	eax, [LeapYearFlag]
	cmp	[n], eax
	ja     .Error
       .CalculateMD:
	mov	ebx, [LeapYearFlag]
	lea	ebx, [ebx + 2*ebx]
	shl	ebx, 3
	mov	ecx, 12
       .Loop:
	movzx	edx, [DaySum - 2 + ebx + 2*ecx]
	cmp	[n], edx
	ja     .LoopEnd
	loop   .Loop
       .LoopEnd:
	mov	eax, [M]
	mov	[eax], ecx
	mov	ecx, [n]
	sub	ecx, edx
	mov	eax, [D]
	mov	[eax], ecx
	xor	eax, eax
	jmp	.End
       .Error:
	mov	eax, -1
       .End:
	pop	edx ecx ebx
	popfd
	ret






; DWORD DateToAbsDayNum(DWORD Y, DWORD M, DWORD D, DWORD Gregorian)
; ������� ������������ ��� ������ ���� ��������������� ���������� ����� ���.
; ���������:
;	Y - ���,
;	M - �����,
;	D - ����,
;	Gregorian - ����������� ��������� (0 - ���������, 1 - �������������).
; ������������ ��������:
;	* 1, 2, ..., 2^32-1, ���� ���� ������� ��������� �����,
;	* 0, ���� ������ �������.
proc	DateToAbsDayNum, Y, M, D, Gregorian
	pushfd
	push	ebx ecx edx
	test	[Gregorian], -2
	jnz    .Error
	stdcall IsLeapYear, [Y], [Gregorian]
	cmp	eax, -1
	je     .Error
	;
	mov	ebx, eax
	stdcall MDToDayNum, [M], [D], ebx
	cmp	eax, -1
	je     .Error
	mov	ecx, [Y]
	cmp	ecx, 0
	jg     .CalculateDayNum
	inc	ecx
	;
       .CalculateDayNum:
	add	ecx, k*J
	cmp	[Gregorian], 0
	je     .Yprim0
	add	ecx, k*(G-J)
       .Yprim0:
	cmp	ecx, 0
	jne    .YprimPositive
	sub	eax, 364
	jmp    .End
       .YprimPositive:
	dec	ecx
	mov	ebx, eax
	mov	eax, 365
	mul	ecx
	shr	ecx, 2
	add	eax, ecx
	add	eax, ebx
	;
	cmp	[Gregorian], 0
	jz     .End
       .Gregorian:
	push	eax
	xor	edx, edx
	mov	eax, ecx
	mov	ebx, 25
	div	ebx
	mov	ecx, eax
	pop	eax
	sub	eax, ecx
	shr	ecx, 2
	add	eax, ecx
	add	eax, 2
	jmp	.End
	;
       .Error:	xor	eax, eax
       .End:
	pop	edx ecx ebx
	popfd
	ret



; DWORD AbsDayNumToDate(DWORD N, DWORD Gregorian, DWORD* Y, DWORD* M, DWORD* D)
; ������� ������������ ���������� ����� ��� N = 1, 2, ..., 2^32-1 � ��������������� ���� (� ������ ���������).
; ���������:
;	N - ���������� ����� ���,
;	Gregorian - ����������� ��������� (0 - ���������, 1 - �������������),
;	Y - ��������� �� ���������� � ������� ����� ������� ����������� ����� ����,
;	M - ��������� �� ���������� � ������� ����� ������� ����������� ����� ������,
;	D - ��������� �� ���������� � ������� ����� ������� ����������� ����� ���.
; ������������ ��������:
;	* 0, ���� ������ ����� (N, Gregorian),
;	* -1 � ��������� ������.
proc	AbsDayNumToDate, N, Gregorian, Y, M, D
	pushfd
	push	ebx ecx edx
	cmp	[N], 0
	je     .Error
	test	[Gregorian], -2
	jnz    .Error
	xor	ecx, ecx
	mov	eax, [N]
	dec	eax
	cmp	[Gregorian], 0
	je     .Julian
       .Gregorian:
	cmp	eax, 1
	ja     .NextDays
	mov	ebx, [M]
	mov	dword [ebx], 12
	add	eax, 30
	mov	ebx, [D]
	mov	[ebx], eax
	mov	ecx, -k*G - 1
	jmp    .ReturnY
       .NextDays:
	sub	eax, 2
	xor	edx, edx
	mov	ebx, C400
	div	ebx
	lea	eax, [eax + 4*eax]
	lea	eax, [eax + 4*eax]
	shl	eax, 4
	xchg	ecx, eax
	xchg	eax, edx
       .Centuries:
	cmp	eax, C100
	jb     .Julian
	add	ecx, 100
	sub	eax, C100
	cmp	eax, C100
	jb     .Julian
	add	ecx, 100
	sub	eax, C100
	cmp	eax, C100
	jb     .Julian
	add	ecx, 100
	sub	eax, C100
       .Julian:
	xor	edx, edx
	mov	ebx, C4
	div	ebx
	shl	eax, 2
	add	ecx, eax
       .Years:
	inc	ecx
	cmp	edx, C1
	jb     .MD
	sub	edx, C1
	inc	ecx
	cmp	edx, C1
	jb     .MD
	sub	edx, C1
	inc	ecx
	cmp	edx, C1
	jb     .MD
	sub	edx, C1
	inc	ecx
       .MD:
	inc	edx
	stdcall IsLeapYear, ecx, [Gregorian]
	stdcall DayNumToMD, edx, eax, [M], [D]
	cmp	[Gregorian], 0
	je     .JulianYears
       .GregorianYears:
	sub	ecx, k*(G - J)
       .JulianYears:
	sub	ecx, k*J
	cmp	ecx, 0
	jg     .ReturnY
	dec	ecx
       .ReturnY:
	mov	eax, [Y]
	mov	[eax], ecx
	xor	eax, eax
	jmp    .End
       .Error:
	mov	eax, -1
       .End:
	pop	edx ecx ebx
	popfd
	ret





; DWORD GregorianToJulian(DWORD Yg, DWORD Mg, DWORD Dg, DWORD* Yj, DWORD* Mj, DWORD* Dj)
;
; ������� ������������ ������������� ���� � ��������������� ���� ���������� ���������.
;
; ���������:
;	Yg - ��� ������������� ����,
;	Mg - ����� ������������� ����,
;	Dg - ���� ������������� ����,
;	Yj - ��������� �� ���������� � ������� ����� ������� ����������� ����� ���� ��������� ����,
;	Mj - ��������� �� ���������� � ������� ����� ������� ����������� ����� ������ ��������� ����,
;	Dj - ��������� �� ���������� � ������� ����� ������� ����������� ����� ��� ��������� ����.
;
; ������������ ��������:
;	* 0, ���� ������������� ���� �����,
;	* -1 � ��������� ������.
;
dates_GregToJul, Yg, Mg, Dg, Yj, Mj, Dj
	.GregorianToNum:
	 stdcall DateToAbsDayNum, [Yg], [Mg], [Dg], 1
	 test	 eax, eax
	 jz	.Error
	.NumToJulian:
	 stdcall AbsDayNumToDate, eax, 0, [Yj], [Mj], [Dj]
	 jmp	.End
	 ;
	.Error:
	 mov	 eax, -1
	.End:
	 ret




; DWORD JulianToGregorian(DWORD Yj, DWORD Mj, DWORD Dj, DWORD* Yg, DWORD* Mg, DWORD* Dg)
; ������� ������������ ��������� ���� � ��������������� ���� �������������� ���������.
;
; ���������:
;	Yj - ��� ��������� ����,
;	Mj - ����� ��������� ����,
;	Dj - ���� ��������� ����,
;	Yg - ��������� �� ���������� � ������� ����� ������� ����������� ����� ���� ������������� ����,
;	Mg - ��������� �� ���������� � ������� ����� ������� ����������� ����� ������ ������������� ����,
;	Dg - ��������� �� ���������� � ������� ����� ������� ����������� ����� ��� ������������� ����.
;
; ������������ ��������:
;	* 0, ���� ��������� ���� �����,
;	* -1 � ��������� ������.
;
align 4
dates_JulToGreg: ;, Yj, Mj, Dj, Yg, Mg, Dg
       .JulianToNum:
	stdcall DateToAbsDayNum, [Yj], [Mj], [Dj], 0
	test	eax, eax
	jz     .Error
       .NumToGregorian:
	stdcall AbsDayNumToDate, eax, 1, [Yg], [Mg], [Dg]
	jmp    .End
	;
       .Error:
	mov	eax, -1
       .End:
	ret


