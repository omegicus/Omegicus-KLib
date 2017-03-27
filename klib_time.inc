; Возможности:
;	* обслуживает даты с периода длиной сверх 11 миллионов лет:
;		- с 1 января 5843880 г. до н.э. по 3 августа 5915100 г.
;		  для юлианского календаря,
;		- с 30 декабря 5844001 г. до н.э. по 17 января 5915222 г.
;		  для григорианского календаря,
;	* свободная конверсия дат между календарями юлианском и григорианском
;	  в упомянутом периоде,
;	* определение дня недели для данной даты,
;	* вычисление номера дня в году для данных номеров месяца и дня
;	  в месяце,
;	* определение високосности года в данном календаре,
;	* вычисление "абсолютного" номера дня соответствующего данной
;	  дате, благодаря чему можно легко рассчитывать количество дней
;	  между двумя датами.
;
; (C) Mikolaj Hajduk, 16.06.2008.
;

; Определение используемых констант.
;
C1	= 365			; Количество дней в обычном году.
C4	= 4*C1 + 1		; Количество дней в четырёхлетнем цикле (основном  цикле юлианского календаря).
C100	= 25*C4 - 1		; Количество дней в "обычном" столетии григорианского календаря
				; (т.е. в столетии, которое заканчивается невисокосным годом).
C400	= 4*C100 + 1		; Количество дней в полном 400-летнем цикле григорианского календаря.
k	= 30
J	= 194796		; Константы, которые определяют количество полных лет календарей юлианского
G	= 194800		; и григорианского содержимых в отрезке времени равном "Великому Циклу" T.


; Таблица, которая содержит длину месяцев обычного и високосного годов.
MonthLen	db 31,	28,  31,  30,  31,  30,  31,  31,  30,	31,  30,  31
		db 31,	29,  31,  30,  31,  30,  31,  31,  30,	31,  30,  31

; Таблица, которая содержит значения функции 'DaySum' для всех пар (номер месяца, флаг високосности года).
DaySum		dw  0,	31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334
		dw  0,	31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335



; DWORD DayOfWeek(DWORD Y, DWORD M, DWORD D, DWORD Gregorian)
; Функция рассчитывает для данной даты номер дня недели. Принято следующее определение: 0 - воскресенье,
; 1 - понедельник, 2 - вторник, 3 - среда, 4 - четверг, 5 - пятница, 6 - суббота.
; Параметры:
;	Y - год,
;	M - месяц,
;	D - день,
;	Gregorian - определение календаря (0 - юлианский, 1 - григорианский).
; Возвращаемые значения:
;	* 0, 1, ..., 6, если дата верна,
;	* -1, если данные неверны.
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
; Функция определяет високосность года в данном календаре.
; Параметры:
;	Y - год,
;	Gregorian - определение календаря (0 - юлианский, 1 - григорианский).
; Возвращаемые значения:
;	* 1, если год Y високосный, 0 - в противном случае.
;	* -1, если данные неверны.
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
; Функция рассчитывает порядковый номер дня в году (в зависимости от високосности года).
; Параметры:
;	M - месяц,
;	D - день,
;	LeapYearFlag - флаг високосности года (0 - год нормальный, 1 - високосный).
; Возвращаемые значения:
;	* 1, 2, ..., 365 для нормального года, 1, 2, ..., 366 для високосного года,
;	* -1, если данные неверны.
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
; Функция конвертирует данный номер дня в году в соответствующие номера месяца и дня в месяце.
; Результат сильно зависит от значения флаги високосности года.
; Параметры:
;	n - номер дня в году,
;	LeapYearFlag - флаг високосности года (0 - год нормальный, 1 - високосный),
;	M - указатель на переменную в которую будет записан вычисленный номер месяца,
;	D - указатель на переменную в которую будет записан вычисленный номер дня. 
; Возвращаемые значения:
;	* 0, если данные верны (n, LeapYearFlag),
;	* -1 в противном случае.
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
; Функция рассчитывает для данной даты соответствующий абсолютный номер дня.
; Параметры:
;	Y - год,
;	M - месяц,
;	D - день,
;	Gregorian - определение календаря (0 - юлианский, 1 - григорианский).
; Возвращаемые значения:
;	* 1, 2, ..., 2^32-1, если дата данного календаря верна,
;	* 0, если данные неверны.
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
; Функция конвертирует абсолютный номер дня N = 1, 2, ..., 2^32-1 в соответствующую дату (в данном календаре).
; Параметры:
;	N - абсолютный номер дня,
;	Gregorian - определение календаря (0 - юлианский, 1 - григорианский),
;	Y - указатель на переменную в которую будет записан вычисленный номер года,
;	M - указатель на переменную в которую будет записан вычисленный номер месяца,
;	D - указатель на переменную в которую будет записан вычисленный номер дня.
; Возвращаемые значения:
;	* 0, если данные верны (N, Gregorian),
;	* -1 в противном случае.
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
; Функция конвертирует григорианскую дату в соответствующую дату юлианского календаря.
;
; Параметры:
;	Yg - год григорианской даты,
;	Mg - месяц григорианской даты,
;	Dg - день григорианской даты,
;	Yj - указатель на переменную в которую будет записан вычисленный номер года юлианской даты,
;	Mj - указатель на переменную в которую будет записан вычисленный номер месяца юлианской даты,
;	Dj - указатель на переменную в которую будет записан вычисленный номер дня юлианской даты.
;
; Возвращаемые значения:
;	* 0, если григорианская дата верна,
;	* -1 в противном случае.
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
; Функция конвертирует юлианскую дату в соответствующую дату григорианского календаря.
;
; Параметры:
;	Yj - год юлианской даты,
;	Mj - месяц юлианской даты,
;	Dj - день юлианской даты,
;	Yg - указатель на переменную в которую будет записан вычисленный номер года григорианской даты,
;	Mg - указатель на переменную в которую будет записан вычисленный номер месяца григорианской даты,
;	Dg - указатель на переменную в которую будет записан вычисленный номер дня григорианской даты.
;
; Возвращаемые значения:
;	* 0, если юлианская дата верна,
;	* -1 в противном случае.
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


