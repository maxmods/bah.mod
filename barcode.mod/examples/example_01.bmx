SuperStrict

Framework BaH.Barcode
Import BRL.JPGLoader
Import BRL.PNGLoader
Import BRL.StandardIO

Local pix:TPixmap = LoadPixmap("media/qrcode_01.png")

Local barcode:TBCResult = decodeBarcode(pix)

If Not barcode.error Then
	Print "Text : " + barcode.getText()
Else
	Print "Error : " + barcode.error
End If


