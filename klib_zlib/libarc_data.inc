; LZW:
dword_10008900	dd 0
dword_10008904	dd 0
aUnknownError	db 'Unknown error', 0
aWarningInvalid db 'Invalid Crc32', 0
aErrorInvalidLz db 'Invalid Lzw signature', 0
aErrorLzwHeader db 'Error: Lzw header is corrupted', 0
aWarningPossibl db 'Warning: Possible corrupted data or invalid data size', 0
aErrorPackedDat db 'Error: Packed data is corrupted (invalid index)', 0

header: 	dd 0 ; type
		dd 0 ; filesize original
		dd 0 ; crc32 original
		db 64 - 4*3 dup 0

LZO_BUF0	dd LZO_BUF
LZO_VAR_RES	dd 0
LZO_SEMAFOR	dd 0
unpacked	dd 0



; LZMA:
;---------------------------------------------------------------------
align 4
lzma_dictsize		dd 0
workmem 		dd 0
align 4
LiteralNextStates:	db 0,0,0,0,1,2,3,4,5,6,4,5
MatchNextStates:	db 7,7,7,7,7,7,7,10,10,10,10,10
RepNextStates:		db 8,8,8,8,8,8,8,11,11,11,11,11
ShortRepNextStates:	db 9,9,9,9,9,9,9,11,11,11,11,11
;---------------------------------------------------------------------



LZO_BUF 	rb 0x10000 ;dup 0     ; 64k Buffer for compression

; Deconpress data area start
align 4
unpack.p	rb	0x1F36*4
unpack.code_:	rd	1
unpack.range:	rd	1
unpack.rep0	rd	1
unpack.rep1	rd	1
unpack.rep2	rd	1
unpack.rep3	rd	1
unpack.previousByte rb	1
; Deconpress data area end
;---------------------------------------------------------------------
;---------------------------------------------------------------------
; Compress data area start
align 4
_lenEncoder:		rd 8451
_prices:		rd 4384
			rd 17
_finished:		rb 1
_writeEndMark:		rb 1
_longestMatchWasFound:	rb 1
_previousByte:		rb 1
_longestMatchLength:	rd 1
g_FastPos:		rb 1024
_posSlotPrices: 	rd 256
_isRep0Long:		rd 192
distances:		rd 274
_optimumCurrentIndex:	rd 1
_additionalOffset:	rd 1
_isRepG1:		rd 12
_isMatch:		rd 192
_alignPriceCount:	rd 1
_numLiteralContextBits: rd 1
_literalEncoder:	rd 114
nowPos64:		rd 2
_distancesPrices:	rd 512
_repDistances:		rd 4
_posSlotEncoder:	rd 1028
lastPosSlotFillingPos:	rd 2
_numFastBytes:		rd 1
_posStateMask:		rd 1
_isRepG0:		rd 12
_repMatchLenEncoder:	rd 8451
			rd 4384
			rd 17
_isRepG2:		rd 12
_dictionarySize:	rd 1
_numLiteralPosStateBits:rd 1
_distTableSize: 	rd 1
_optimumEndIndex:	rd 1
state.State:		rb 1
state.Prev1IsChar:	rb 1
state.Prev2:		rb 2
state.PosPrev2: 	rd 1
state.BackPrev2:	rd 1
state.Price:		rd 1
state.PosPrev:		rd 1
state.BackPrev: 	rd 1
state.Backs:		rd 4
			rd 40950
_alignPrices:		rd 16
_isRep: 		rd 12
_posAlignEncoder:	rd 256
i_01:			rd 1
_state: 		rb 1
_cache: 		rb 1
_state.Prev2:		rb 2
_posEncoders:		rd 1
_numPrevBits:		rd 1
_numPosBits:		rd 1
_posMask:		rd 1
_posStateBits:		rd 1
_range: 		rd 1
_cacheSize:		rd 1
_cyclicBufferSize:	rd 1
low:			rd 2
Models: 		rd 512
_matchMaxLen:		rd 1
pack_pos:		rd 1
_cutValue:		rd 1
_hash:			rd 1
;crc_table:		 rd 256
_buffer:		rd 1
_pos:			rd 1
_streamPos:		rd 1
pack_length:		rd 1
; Compress data area end
;---------------------------------------------------------------------
