SuperStrict

Framework brl.standardio
Import bah.clipboard

Local clipboard:TClipboard = New TClipboard.Create()

Local txt:String = Input("Enter some text : ")

Print "Text = " + txt

clipboard.SetText(txt)

Print "Copied to clipboard...~n"

Print "Clipboard contents = " + clipboard.Text()

