'
' Find Strings in a string buffer
'
SuperStrict

Framework bah.stringbuffer
Import brl.standardio

Local sb:TStringBuffer = New TStringBuffer

sb.Append("Strings are used to store sequences of characters.")

Print ""

Print "                  1         2         3         4"
Print "        01234567890123456789012345678901234567890123456789"
Print "Text = ~q" + sb.ToString() + "~q"

Print ""

Print "Contains 'store' ? : " + sb.Contains("store")
Print "Contains 'Store' ? : " + sb.Contains("Store")

Print ""

Print "Contains 'Strin' ? : " + sb.Contains("Strin")
Print "Contains 'ers.' ? : " + sb.Contains("ers.")

Print ""

Print "Index of 'seq' : " + sb.Find("seq")
Print "Index of '.' : " + sb.Find(".")
Print "Index of 'X' : " + sb.Find("X")

Print ""

Print "Last index of 'ar' : " + sb.FindLast("ar")
Print "Last index of '.' : " + sb.Find(".")
Print "Last index of 'X' : " + sb.Find("X")

