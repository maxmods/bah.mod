SuperStrict


Import "addressbook.pb.bmx"


Local p:Person = New Person.Create()
p.SetId(1)
p.SetName("Brucey")

Local number:Person_PhoneNumber = p.AddPhone()
number.SetType(Person_PhoneType_HOME)
number.SetNumber("555-123-456")

Print ": Before Serialize.."
Print TTextFormat.PrintToString(p)

p.SerializeToFile("person.pb")

Local lp:Person = New Person.Create()

lp.ParseFromFile("person.pb")

Print ": After File Serialize.."
Print TTextFormat.PrintToString(lp)

Local s:String = lp.SerializeToString()

Local sp:Person = New Person.Create()
sp.ParseFromString(s)

Print ": After String Serialize.."
Print TTextFormat.PrintToString(sp)
