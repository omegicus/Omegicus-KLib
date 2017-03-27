use32




;str1 db 'This is a First Line',0
;str2 db 'And this is Second',0
;str3 db 'Third? Really?',0
;str4 db 'Yeah, I am Happy!',0
;str5 db 'Send me email ;)',0
;str6 db 'Be happy and healthy!',0
; Массив указателей на строки, который и будет отсортирован
;array dd str1
;      dd str2
;      dd str3
;      dd str4
;      dd str5
;      dd str6
;      dd 0
;------------------------------------------------------------------
; Функция сортировки массива строк
; by ManHunter / PCL
; http://www.manhunter.ru
;
; Сортировка выполняется усовершенствованным методом "пузырька"
; Параметры:
; lpArray - указатель на массив адресов строк
; dFlag   - метод сортировки: TRUE - по возрастанию,
;	    FALSE - по убыванию
; На выходе:
; отсортированный согласно правилу массив адресов строк
;------------------------------------------------------------------
SortArray: ; lpArray, dFlag
	mov	ebp, esp
	pusha
	; Указатель на массив адресов строк
	mov	esi,[ebp + 04]
	mov	ebx,[ebp + 08]
     sa_sort_1:
	; Проверка на 0 - признак окончания массива
	mov	eax,[esi]
	or	eax,eax
	jz	sa_stop_sort
	or	bh,bh
	jnz	sa_stop_sort
	; Сравниваем, начиная со следующего элемента
	lea	edi,[esi+4]
	; Установить флаг, что сортировка не требуется,
	; если дальнейший остаток массива уже отсортирован
	mov	bh,1
     sa_sort_2:
	; Следующий элемент 0 - массив закончился
	mov	edx,[edi]
	or	edx,edx
	jz	sa_next_sort
	; Сохранить текущие значения указателей
	push	esi edi
	; Указатели на строки
	mov	esi,eax
	mov	edi,edx
     sa_compare_string:
	; Сравнить символы в строках
	mov	cl,byte [edi]
	mov	ch,byte [esi]
	; Если символы равны, то сравнить следующие
	cmp	cl,ch
	je	sa_equal
	; Проверка больше-меньше в зависимости от флага
	cmp	bl,0
	jz	@f
	cmp	cl,ch
	ja	ss_next_string
	jmp	sa_change_offs
    @@: cmp	cl,ch
	jb	ss_next_string
     sa_change_offs:
	; Поменять местами адреса строк
	xchg	eax,edx
	; Сбросить флаг, что сортировка не требуется
	xor	bh,bh
	jmp	ss_next_string
     sa_equal:
	; Если закончилась строка, то перейти к следующей паре
	or	cl,cl
	jz	ss_next_string
	; Перейти к следующей паре символов
	inc	esi
	inc	edi
	jmp	sa_compare_string
     ss_next_string:
	; Восстановить значения указателей и записать в них
	; значения адресов строк, без разницы изменившихся или нет
	pop	edi esi
	mov	[esi],eax
	mov	[edi],edx
	; Перейти к следующему элементу массива
	add	edi,4
	jmp	sa_sort_2
     sa_next_sort:
	; Перейти к следующему элементу массива
	add	esi,4
	jmp	sa_sort_1
     sa_stop_sort:
	popa
ret 4*2








; Сортировка dword'ов в количестве ecx по адресу edx, функция сравнения в ebx
; Разрушает eax, ecx, esi, edi
dwsort:
	jecxz	.done
	mov	eax, ecx
    @@: push	eax
	call   .restore
	pop	eax
	dec	eax
	jnz	@b
    @@: cmp	ecx, 1
	jz     .done
	mov	esi, 1
	mov	edi, ecx
	call   .exchange
	dec	ecx
	mov	eax, 1
	call   .restore
	jmp	@b
 .done: ret
 .exchange:
	push	eax ecx
	mov	eax, [edx+esi*4-4]
	mov	ecx, [edx+edi*4-4]
	mov	[edx+esi*4-4], ecx
	mov	[edx+edi*4-4], eax
	pop	ecx eax
	ret
 .restore:
	lea	esi, [eax+eax]
	cmp	esi, ecx
	ja     .doner
	push	esi
	mov	esi, [edx+esi*4-4]
	mov	edi, [edx+eax*4-4]
	call	ebx
	pop	esi
	ja     .need_xchg
	cmp	esi, ecx
	jae    .doner
	push	esi
	mov	esi, [edx+esi*4]
	mov	edi, [edx+eax*4-4]
	call	ebx
	pop	esi
	jbe    .doner
  .need_xchg:
	cmp	esi, ecx
	jz     .do_xchg
	push	esi
	mov	edi, [edx+esi*4-4]
	mov	esi, [edx+esi*4]
	call	ebx
	pop	esi
	sbb	esi, -1
    .do_xchg:
	mov	edi, eax
	call   .exchange
	mov	eax, esi
	jmp    .restore
.doner: ret
