SuperStrict

Framework BaH.libxslt
Import BaH.libxml

Local cur:TxsltStylesheet
Local doc:TxmlDoc
Local res:TxmlDoc

xmlSubstituteEntitiesDefault(1)

xmlLoadExtDtdDefaultValue = 1


cur = TxsltStylesheet.parseStylesheetFile("example_sort.xslt")


doc = TxmlDoc.parseFile("example_sort.xml")

res = cur.applyStylesheet(doc)


cur.saveResultToFilename("example_sort.html", res)



cur.free()

res.free()

doc.free()



xsltCleanupGlobals()

xmlCleanupParser()


