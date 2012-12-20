SuperStrict

Framework BaH.Cairo
Import BaH.QREncode


Local cairo:TCairo = TCairo.Create(TCairoPDFSurface.CreateForPDF("qrcode.pdf", TCairoPaperSize.PAPER_A4.width, TCairoPaperSize.PAPER_A4.height))
Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(1, 1)
cairo.SetMatrix(normalizeMat)


' white background for page
cairo.Save()
cairo.SetSourceRGB(1,1,1)
cairo.rectangle( 0, 0, TCairoPaperSize.PAPER_A4.width, TCairoPaperSize.PAPER_A4.height)
cairo.fill()
cairo.Restore()





cairo.showPage() ' output the page.


cairo.getTarget().Finish() ' Finish off the document.

cairo.destroy()

' now, open it in your favourite pdf viewer...
OpenURL "qrcode.pdf"

