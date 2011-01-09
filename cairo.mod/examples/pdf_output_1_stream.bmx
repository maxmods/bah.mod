SuperStrict


Import BaH.Cairo

' This is a very crude example of creating a PDF in Cairo using a TStream.
' A better way would be to have methods specifically for doing things like page-numbering etc...


Local stream:TStream = WriteStream("test2.pdf")

Local cairo:TCairo = TCairo.Create(TCairoPDFSurface.CreateForPDF(stream, TCairoPaperSize.PAPER_A4.width, TCairoPaperSize.PAPER_A4.height))

Local normalizeMat:TCairoMatrix = TCairoMatrix.CreateScale(1, 1)
cairo.SetMatrix(normalizeMat)
cairo.SetLineWidth(0.05)


' Draw the background 
' get a picture, and lay it out diagonally across the page
Local photo:TPixmap = LoadPixmap("bmax160_2.png")


Local photosurf:TCairoSurface = TCairoImageSurface.CreateFromPixmap(photo)

cairo.Save()

Local pattern:TCairoPattern = TCairoPattern.CreateForSurface(photosurf)
pattern.setExtend(CAIRO_EXTEND_REPEAT)

Local mat:TCairoMatrix = TCairoMatrix.CreateScale(5, 5)
mat.Rotate(-45)
pattern.setMatrix(mat)

cairo.setSource( pattern)

cairo.rectangle( 0, 0, TCairoPaperSize.PAPER_A4.width, TCairoPaperSize.PAPER_A4.height)
cairo.fill()

'mat.destroy()
pattern.destroy()


cairo.Restore()

' make the background faded...
cairo.Save()
cairo.SetSourceRGBA(1,1,1,0.75)
cairo.Rectangle(0, 0, TCairoPaperSize.PAPER_A4.width, TCairoPaperSize.PAPER_A4.height)
cairo.Fill()

cairo.SetSourceRGBA(0,0,0,1)
cairo.Restore()


' Now we draw some text

cairo.SelectFontFace( "arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL)
cairo.SetFontSize(30)

cairo.MoveTo( 100, 40)
cairo.ShowText( "PDFs with Cairo in BlitzMax")

cairo.SetFontSize(14)
cairo.MoveTo( 80, 80)
cairo.ShowText( "Just when you thought it was safe to venture out into the playground...")

cairo.stroke()  ' this is a hack to stop a line appearing on the page


' Draw some circles
cairo.SetLineWidth(2)
cairo.SetSourceRGB(1, 0, 0)
cairo.Arc(250, 400, 50, 0, 360)
cairo.Stroke()

cairo.SetSourceRGBA(1, 0.5, 0, 0.5)
cairo.Arc(300, 450, 50, 0, 360)
cairo.Fill()

cairo.SetSourceRGBA(0.3, 0.5, 0.7, 0.4)
cairo.Arc(300, 400, 50, 0, 360)
cairo.Fill()


' A page number
cairo.SetSourceRGB(0, 0, 0)
cairo.MoveTo(TCairoPaperSize.PAPER_A4.width - 60, TCairoPaperSize.PAPER_A4.height - 20)
cairo.ShowText( "Page 1" )

cairo.showPage() ' output the page.


cairo.Save()
cairo.MoveTo(TCairoPaperSize.PAPER_A4.width /2 - 70, TCairoPaperSize.PAPER_A4.height /2 - 100)
cairo.Rotate(60)
cairo.SetFontSize(24)
cairo.ShowText("Nothing to see here!")
cairo.Restore()


' Another page number
cairo.MoveTo(TCairoPaperSize.PAPER_A4.width - 60, TCairoPaperSize.PAPER_A4.height - 20)
cairo.ShowText( "Page 2" )

cairo.showPage() ' output the page.


cairo.getTarget().Finish() ' Finish off the document.

cairo.destroy()

stream.close()

' now, open it in your favourite pdf viewer...
OpenURL "test2.pdf"
