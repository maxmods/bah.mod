' XML compression
SuperStrict

Framework BaH.Libxml
Import BRL.standardio

Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")

Print doc.getCompressMode()

doc.setCompressMode(9)

Print doc.getCompressMode()
