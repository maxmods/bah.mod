' Copyright (c) 2007-2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Extern
	Function isspace:Int(char:Int)
	Function isdigit:Int(char:Int)
End Extern

Function dbStrptime:Int(date:String, format:String, y:Int Var, m:Int Var, d:Int Var, hh:Int Var, mm:Int Var, ss:Int Var)

	Local b:Int = 0
	Local c:Int = 0
	Local p:Int = 0

	While p < format.length
	
		c = p
		p:+1
	
		If format[c] <> 37 Then ' "%"
		
			If isspace(format[c]) Then
				While b < date.length And isspace(date[b])
					b:+1
				Wend
			Else
				If format[c] <> date[b] Then
					Return False
				End If

				b:+ 1

			End If
			
			Continue
			
		End If
	
		c = p
		p:+ 1
		
		If c >= format.length Then
			Return 0
		End If
	
		Select format[c]
			Case 37 ' "%"
				If date[b] <> 37 Then  ' "%"
					Return False
				End If
				
				Continue
			Case 77, 83  ' "M", "S"
				If b < date.length And Not isspace(date[b]) Then
				
					If Not isdigit(date[b]) Then
						Return False
					End If
					
					Local i:Int = 0
					While b < date.length And isdigit(date[b])
						i:* 10 
						i:+ date[b] - 48 ' "0"
						b:+ 1
					Wend
					
					If i > 59 Then
						Return False
					End If
					
					If format[c] = 77 Then ' "M"
						mm = i
					Else
						ss = i
					End If
					
				Else
					Continue
				End If
			Case 72   ' "H"

				If Not isdigit(date[b]) Then
					Return False
				End If

				Local i:Int = 0
				While b < date.length And isdigit(date[b])
					i:* 10 
					i:+ date[b] - 48 ' "0"
					b:+ 1
				Wend

				If i > 23 Then
					Return False
				End If
				
				hh = i

			Case 100  ' "d"

				If Not isdigit(date[b]) Then
					Return False
				End If

				Local i:Int = 0
				While b < date.length And isdigit(date[b])
					i:* 10 
					i:+ date[b] - 48 ' "0"
					b:+ 1
				Wend
				
				If i > 31 Then
					Return False
				End If
				
				d = i

			Case 109  ' "m"
			
				If Not isdigit(date[b]) Then
					Return False
				End If
				
				Local i:Int = 0
				While b < date.length And isdigit(date[b])
					i:* 10 
					i:+ date[b] - 48 ' "0"
					b:+ 1
				Wend
				
				If i < 1 Or i > 12 Then
					Return False
				End If
				
				m = i
				
			Case 89   ' "Y"
				If b < date.length And Not isspace(date[b]) Then
				
					If Not isdigit(date[b]) Then
						Return False
					End If
				
					Local i:Int = 0
					While b < date.length And isdigit(date[b])
						i:* 10 
						i:+ date[b] - 48 ' "0"
						b:+ 1
					Wend
					
					y = i
					
				End If
		End Select

		If b < date.length And isspace(date[b]) Then
			While p < format.length And Not isspace(format[p])
				p:+ 1
			Wend
		End If
	
	Wend
	
	Return True
	
End Function


Rem
' test

Local y:Int, m:Int, d:Int, hh:Int, mm:Int, ss:Int
Local date:String = "2007-02-05 12:03:25"
Local format:String = "%Y-%m-%d %H:%M:%S"
DebugStop
Print strptime(date, format, y, m, d, hh, mm, ss)

Print y + ", " + m + ", " + d + ", " + hh + ", " + mm + ", " + ss
End Rem

