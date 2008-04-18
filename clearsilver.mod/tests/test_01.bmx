SuperStrict

Framework BaH.ClearSilver
Import BRL.StandardIO

Local hdf:THDF = THDF.Create()

If Not hdf Then
	End
End If

Print "Testing HDF set and dump~n"
hdf.setValue("Foo.Bar","10")
hdf.setValue("Foo.Baz","20")
Print hdf.dump()

Print "Testing HDF get~n"
Local foo:String = hdf.getValue("Foo.Bar", "30")
Print foo 
foo = hdf.getValue("Foo.Baz", "30")
Print foo

Print "----"

Print "Testing HDF setSymLink~n"
hdf.setSymLink("Foo.Baz2","Foo.Baz")
foo = hdf.getValue("Foo.Baz2", "30")
Print foo

Print "----"

Print "Testing HDF get where default value is null~n"
foo = hdf.getValue("Foo.Bar", Null)
Print "foo = " + foo
foo = hdf.getValue("Foo.Nonexistent", Null)
Print "foo = " + foo

Print "----"

Local fooInt:Int = hdf.getIntValue("Foo.Bar", 30)
Print "Testing HDF get int~n"
Print fooInt

Print "----"

Local cs:TCSParse = TCSParse.Create(hdf)

Print "Testing HDF parse/render~n"
Local tmplstr:String = "Foo.Bar:<?cs var:Foo.Bar ?>~nFoo.Baz:<?cs var:Foo.Baz ?>~n";
Print tmplstr
Print "----"

cs.parseString(tmplstr)
Print cs.render()

' test registered functions
Print "Testing registered string functions~n"
hdf.setValue("Foo.EscapeTest","abc& 231<>/?");

tmplstr = " <?cs var:url_escape(Foo.EscapeTest) ?> <?cs var:html_escape(Foo.EscapeTest) ?>";

cs.parseString(tmplstr)
Print cs.render()

cs = TCSParse.Create(hdf)

Print "Testing white space stripping~n"
' test white space stripping
tmplstr = "      <?cs var:Foo.Bar ?> This is a       string     without whitespace stripped"
cs.parseString(tmplstr)
Print cs.render()

hdf.setValue("ClearSilver.WhiteSpaceStrip", "1")
Print cs.render()

' Now, test debug dump
Print "Testing debug dump~n"
hdf.setValue("ClearSilver.DisplayDebug", "1")
Print cs.render()

Print "Final HDF dump~n"
Print hdf.dump()
