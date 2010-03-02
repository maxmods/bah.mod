'
' Reads the entire address book from a file, adds one person based on user input, then writes
' it back out to the same file.
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


If Not FileType(AppArgs[1]) Then
	Print AppArgs[1] + ": File not found.  Creating a new file."
Else
	' Read the existing address book.
	If Not address_book.ParseFromFile(AppArgs[1]) Then
		Print "Failed to parse address book."
		End
	End If
End If

' Add an address.
PromptForAddress(address_book.AddPerson())

' Write the new address book back to disk.
If Not address_book.SerializeToFile(AppArgs[1]) Then
	Print "Failed to write address book."
	End
End If

End

' This function fills in a Person message based on user input.
Function PromptForAddress(p:Person)

	Local id:Int = Input("Enter person ID number: ").ToInt()
	
	p.SetId(id)
	p.SetName(Input("Enter name: "))
	
	Local email:String = Input("Enter email address (blank for none): ")
	If email Then
		p.SetEmail(email)
	End If
	
	While True
	
		Local number:String = Input("Enter a phone number (or leave blank to finish): ")
		If Not number Then
			Exit
		End If
		
		Local phone_number:Person_PhoneNumber = p.AddPhone()
		phone_number.SetNumber(number)
		
		Local kind:String = Input("Is this a mobile, home, or work phone? ")
		If kind = "mobile" Then
			phone_number.SetType(Person._MOBILE)
		Else If kind = "home" Then
			phone_number.SetType(Person._HOME)
		Else If kind = "work" Then
			phone_number.SetType(Person._WORK)
		Else
			Print "Unknown phone type.  Using default."
		End If
	
	Wend
	

End Function