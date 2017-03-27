macro RSA_multiply_32 s1,s2,dest {
	dest=s1*(s2 and 07fffffffh)
	dest=dest+(s1*(s2 shr 31) shl 31)
}

macro RSA_multiply_add_32 s1,s2,dest,a1 {
	dest=s1*(s2 and 07fffffffh)
	dest=dest+(s1*(s2 shr 31) shl 31)+a1
}

macro RSA_schoolboy_multiply source1,source2,destination {
	rept max_modulus_words count:0 \{
		load RSA_source1\#count dword from source1+(count mod modulus_words)*4
		load RSA_source2\#count dword from source2+(count mod modulus_words)*4
	\}
  virtual at 0
	local ..s1,..s2,..answ
..s1=$
	rept max_modulus_words count:0 \{ dd RSA_source1\#count \}
..s2=$
	rept max_modulus_words count:0 \{ dd RSA_source2\#count \}
..answ=$
	rq max_modulus_words
    repeat modulus_words
      RSA_source1_count=%-1
      load RSA_source1 dword from ..s1+(%-1)*4
      load RSA_temp_answer_l dword from ..answ+RSA_source1_count*4
      repeat modulus_words
	RSA_source2_count=%-1
	load RSA_source2 dword from ..s2+(%-1)*4
	RSA_multiply_32 RSA_source1,RSA_source2,RSA_temp_result
	load RSA_temp_answer_h dword from ..answ+(RSA_source1_count+RSA_source2_count)*4+4
	RSA_temp_answer_l=RSA_temp_answer_l+(RSA_temp_result and 0ffffffffh)
	RSA_temp_answer_h=RSA_temp_answer_h+(RSA_temp_result shr 32)+(RSA_temp_answer_l shr 32)
	store dword (RSA_temp_answer_l and 0ffffffffh) at ..answ+(RSA_source1_count+RSA_source2_count)*4
	RSA_temp_answer_l=RSA_temp_answer_h
      end repeat
      store dword (RSA_temp_answer_l and 0ffffffffh) at ..answ+(RSA_source1_count+RSA_source2_count)*4+4
    end repeat
	rept max_modulus_words count:0 \{ load RSA_destination\#count qword from ..answ+count*8 \}
  end virtual
	rept max_modulus_words count:0 \{
	    if count<modulus_words
		store qword RSA_destination\#count at destination+count*8
	    end if
	\}
}

macro RSA_modulus source,modulus,destination {
	rept max_modulus_words count:0 \{
		load RSA_source\#count qword from source+(count mod modulus_words)*8
		load RSA_modulus\#count dword from modulus+(count mod modulus_words)*4
	\}
  virtual at 0
	local ..s,..m
..s=$
	rept max_modulus_words count:0 \{ dq RSA_source\#count \}
..m=$
	rept max_modulus_words count:0 \{ dd RSA_modulus\#count \}
    load RSA_modulus_h dword from ..m+modulus_words*4-4
    if RSA_modulus_h<080000000h
      display "Warning: Modulus not in correct format, results may be incorrect",13,10
    end if
    if modulus_words>1
      load RSA_modulus_m dword from ..m+modulus_words*4-8
    else
      RSA_modulus_m=0
    end if
    if modulus_words>2
      load RSA_modulus_l dword from ..m+modulus_words*4-12
    else
      RSA_modulus_l=0
    end if
    RSA_temp_h=0
    repeat modulus_words+1
      RSA_source_pointer=modulus_words*2-%
      load RSA_temp_m dword from ..s+RSA_source_pointer*4
      if RSA_source_pointer>0
	load RSA_temp_l dword from ..s+RSA_source_pointer*4-4
      else
	RSA_temp_l=0
      end if
      if RSA_temp_h>=RSA_modulus_h
	RSA_divisor=0ffffffffh
      else
	RSA_divisor=((RSA_temp_h shl 31)+(RSA_temp_m shr 1))/(RSA_modulus_h shr 1)
	if RSA_divisor>0ffffffffh
	  RSA_divisor=0ffffffffh
	end if
      end if
      repeat 4
	RSA_multiply_32 RSA_divisor,RSA_modulus_m,RSA_temp1l
	RSA_multiply_add_32 RSA_divisor,RSA_modulus_h,RSA_temp1m,(RSA_temp1l shr 32)
	RSA_temp1l=RSA_temp1l and 0ffffffffh
	RSA_temp1h=RSA_temp1m shr 32
	RSA_temp1m=RSA_temp1m and 0ffffffffh
	if (RSA_temp1h>RSA_temp_h) | ((RSA_temp1h=RSA_temp_h) & (RSA_temp1m>RSA_temp_m)) | (RSA_temp1h=RSA_temp_h & RSA_temp1m=RSA_temp_m & RSA_temp1l>RSA_temp_l)
	  RSA_divisor=RSA_divisor-1
	end if
      end repeat
      RSA_temp_answer=0
      repeat modulus_words
	RSA_source_count=%-1
	load RSA_source dword from ..m+(%-1)*4
	RSA_multiply_add_32 RSA_source,RSA_divisor,RSA_temp_result,RSA_temp_answer
	load RSA_temp_answer dword from ..s+(RSA_source_pointer-modulus_words+1+RSA_source_count)*4
	RSA_temp_answer=RSA_temp_answer-(RSA_temp_result and 0ffffffffh)
	store dword (RSA_temp_answer and 0ffffffffh) at ..s+(RSA_source_pointer-modulus_words+1+RSA_source_count)*4
	RSA_temp_answer=(RSA_temp_result shr 32)-(RSA_temp_answer and (not 0ffffffffh) or RSA_temp_answer shr 32)
      end repeat
      if (RSA_temp_h-RSA_temp_answer)<0
	RSA_temp_result=0
	repeat modulus_words
	  load RSA_source dword from ..m+(%-1)*4
	  load RSA_answer dword from ..s+(RSA_source_pointer-modulus_words+1+%-1)*4
	  RSA_temp_result=RSA_temp_result+RSA_answer+RSA_source
	  store dword (RSA_temp_result and 0ffffffffh) at ..s+(RSA_source_pointer-modulus_words+1+%-1)*4
	  RSA_temp_result=RSA_temp_result shr 32
	end repeat
      end if
      load RSA_temp_h dword from ..s+RSA_source_pointer*4      
    end repeat
	rept max_modulus_words count:0 \{ load RSA_destination\#count dword from ..s+count*4 \}
  end virtual
	rept max_modulus_words count:0 \{
	    if count<modulus_words
		store dword RSA_destination\#count at destination+count*4
	    end if
	\}
}

macro RSA_encrypt source,destination,modulus,[key] {
  common
  virtual at 0
    if ~ key eq
	dq key
    end if
	dd max_modulus_words dup (0)
	rept max_modulus_words count:0 \{ load RSA_exponent\#count dword from count*4 \}
  end virtual
	rept max_modulus_words count:0 \{
		load RSA_source\#count dword from source+(count mod modulus_words)*4
		load RSA_modulus\#count dword from modulus+(count mod modulus_words)*4
	\}
  virtual at 0
	local ..s,..e,..m,..answ,..temp
..s=$
	rept max_modulus_words count:0 \{ dd RSA_source\#count \}
..e=$
	rept max_modulus_words count:0 \{ dd RSA_exponent\#count \}
..m=$
	rept max_modulus_words count:0 \{ dd RSA_modulus\#count \}
..answ=$
	dd 1
	rd max_modulus_words-1
..temp=$
	rq max_modulus_words
    RSA_answer_valid=0
    repeat modulus_words
      load RSA_exponent_word dword from ..e+(modulus_words-%)*4
      repeat 32
	if RSA_answer_valid
	  RSA_schoolboy_multiply ..answ,..answ,..temp
	  RSA_modulus ..temp,..m,..answ
	end if
	if RSA_exponent_word and (1 shl 31)
	  if RSA_answer_valid
	    RSA_schoolboy_multiply ..answ,..s,..temp
	    RSA_modulus ..temp,..m,..answ
	  else
	    repeat modulus_words
	      load RSA_source_temp dword from ..s+(%-1)*4
	      store dword RSA_source_temp at ..answ+(%-1)*4
	    end repeat
	    RSA_answer_valid=1
	  end if
	end if
	RSA_exponent_word=RSA_exponent_word+RSA_exponent_word
      end repeat
    end repeat
	rept max_modulus_words count:0 \{ load RSA_destination\#count dword from ..answ+count*4 \}
  end virtual
	rept max_modulus_words count:0 \{
	    if count<modulus_words
		store dword RSA_destination\#count at destination+count*4
	    end if
	\}
}
