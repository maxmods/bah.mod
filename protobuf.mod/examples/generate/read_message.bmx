'
'  Reads the entire address book from a file and prints all the information inside.
'
SuperStrict

Framework BaH.Protobuf
Import BRL.StandardIO
Import BRL.FileSystem


Import "addressbook.pb.bmx"

If AppArgs.length < 2 Then
	Print "usage : " + AppArgs[0] + " ADDRESS_BOOK_FILE"
	End
End If

Local address_book:AddressBook = New AddressBook.Create()

' Read the existing address book.
If Not address_book.ParseFromFile(AppArgs[1]) Then
	Print "Failed to parse address book."
	End
End If

ListPeople(address_book)

End

' Iterates though all people in the AddressBook and prints info about them.
Function ListPeople(address_book:AddressBook)

	For Local i:Int = 0 Until address_book.PersonSize()
	
		Local p:Person = address_book.GetPerson(i)
		
		Print "Person ID: " + p.GetId()
		Print "  Name: " + p.GetName()
		If p.HasEmail() Then
			Print "  E-mail address: " + p.GetEmail()
		End If
		
		For Local j:Int = 0 Until p.PhoneSize()
			Local phone_number:Person_PhoneNumber = p.GetPhone(j)
			Local s:String
			
			Select phone_number.GetType()
				Case Person._MOBILE
					s = "  Mobile phone #: "
				Case Person._HOME
					s = "  Home phone #: "
				Case Person._WORK
					s = "  Work phone #: "
			End Select
			Print s + phone_number.GetNumber()
				
		Next
	
		Print ""
		
	Next

End Function