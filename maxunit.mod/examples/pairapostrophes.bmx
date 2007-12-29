SuperStrict

Function PairApostrophes:String(s_text:String)

	'	Do nothing if no text
	If s_text.length <> 0 
	
		'	We are searching for apostrphe's
		Local s_substring:String = "'"
		
		'	Grab the first apostophe position - will return -1 if none
		Local index:Int = s_text.Find(s_substring)
		
		'	We use this to copy the new string across
		Local s_temp:String
		
		'	Whilst we still have an apostrophe to parse
		While index <> -1
		
			' Is the next char not an apostrophe?
			If index < s_text.length-1 And s_text[index+1] <> Asc(s_substring) Or index = s_text.length-1
				'	Insert a second apostrophe after the first
				s_temp = s_text[0..index] + s_substring + s_text[index..s_text.length]
				'	Copy result back to our string
				s_text = s_temp
			End If
			
			'	Look for another apostrophe, skipping the one we just added
			index = s_text.Find(s_substring, index+2)
		
		Wend
	
	EndIf	 ' if text length is ok
	
	'	Return our clean string
	Return s_text

End Function
