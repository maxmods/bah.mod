' Copyright (c) 2007 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

?Linux
'Framework BaH.QtMaxGUI
Framework BaH.GTKMaxGUI
?Win32
Framework BRL.Win32MaxGUI
?MacOs
Framework BRL.CocoaMaxGUI
?

Import BaH.Locale
Import BRL.Timer
Import BRL.EventQueue

Import "lochandler.bmx"

Incbin "blfbuilder.blf"

' load the application locale data
LoadLocaleFile("incbin::blfbuilder.blf")

Global version:String = "1.02"

AppTitle = GetLocaleText("apptitle")

' create and open the main window
Global builder:TBLFBuilder = New TBLFBuilder

' process events forever...
While True
	WaitEvent()
	
	?MacOs ' ACK! there's a bug with eventhooks!
	builder.onEvent(CurrentEvent)
	?

Wend

Type TBLFBuilder

	' store for the current BLF file
	Field BLFLoc:TLoc
	
	Const baseLanguage:String = "en"

	Field defaultLanguage:String = baseLanguage
	
	Field dw:Int = GadgetWidth(Desktop())
	Field dh:Int = GadgetHeight(Desktop())
	Field w:Int = 500
	?MacOs
	Field h:Int = 395
	?Linux
	Field h:Int = 395
	?Win32
	Field h:Int = 410
	?

	Field nativeLanguages:Int = False

	Field window:TGadget = CreateWindow(GetLocaleText("apptitle"),dw / 2 - w / 2, dh / 2 - h / 2,w,h, Null, WINDOW_TITLEBAR | WINDOW_MENU)

	Field mnuFile:TGadget = CreateMenu(GetLocaleText("menufile"),0,WindowMenu(window))
	Field mnuUtils:TGadget = CreateMenu(GetLocaleText("menuutils"),0,WindowMenu(window))
	Field mnuLanguage:TGadget = CreateMenu(GetLocaleText("menulanguage"),0,WindowMenu(window))
	Field mnuView:TGadget = CreateMenu(GetLocaleText("menuview"),0,WindowMenu(window))
	Field mnuHelp:TGadget = CreateMenu(GetLocaleText("menuhelp"),0,WindowMenu(window))
	
	Const MENU_NEW:Int = 101
	Const MENU_OPEN:Int = 102
	Const MENU_SAVE:Int = 103
	Const MENU_QUIT:Int = 104
	
	Const MENU_VIEW_ENGLISH:Int = 401
	Const MENU_VIEW_NATIVE:Int = 402

	Const MENU_ABOUT:Int = 501
	
	Field MENU_LANG_BASE:Int = 301

	Const MENU_UTILS:Int = 201
	Const MENU_IMPORTCSV:Int = 202
	Const MENU_EXPORTCSV:Int = 203

	Const MENU_IMPORTINI:Int = 204
	Const MENU_EXPORTINI:Int = 205

	' lots of gadgets...	
	Field pnlTags:TGadget = CreatePanel(10, 10, 200, 350, window, PANEL_GROUP, GetLocaleText("tags"))
	Field lstTags:TGadget = CreateListBox(10, 10, 170, 250, pnlTags)
	Field tfTagName:TGadget = CreateTextField(10, 270, 170, 28, pnlTags)
	Field btnTagAdd:TGadget = CreateButton(GetLocaleText("add"), 10, 300, 80, 28, pnlTags)
	Field btnTagDel:TGadget = CreateButton(GetLocaleText("delete"), 100, 300, 80, 28, pnlTags)
	
	Field pnlLang:TGadget = CreatePanel(220, 10, 270, 200, window, PANEL_GROUP, GetLocaleText("languages"))
	Field lblDefault:TGadget = CreateLabel(GetLocaleText("default"), 10, 10, 100, 28, pnlLang)
	Field cmbDefault:TGadget = CreateComboBox(120, 10, 140, 28, pnlLang)
	Field lstLanguages:TGadget = CreateListBox(10, 50, 150, 120, pnlLang)
	Field btnLangAdd:TGadget = CreateButton(GetLocaleText("add"), 170, 110, 90, 28, pnlLang)
	Field btnLangDel:TGadget = CreateButton(GetLocaleText("delete"), 170, 140, 90, 28, pnlLang)
	
	Field pnlTagText:TGadget = CreatePanel(220, 220, 270, 140, window, PANEL_GROUP, GetLocaleText("tagtext"))

	Field taTagText:TGadget = CreateTextArea(10, 10, 250, 70, pnlTagText, TEXTAREA_WORDWRAP)

	Field lblValue:TGadget = CreateLabel(GetLocaleText("value"), 10, 90, 100, 28, pnlTagText)
	Field tfTagValue:TGadget = CreateTextField(120, 90, 140, 28, pnlTagText)

	Field languageMenus:TMap = New TMap
	Field currentLanguage:Int = 0

	Field submenus:TMap = New TMap
	
	' the language selection dialog
	Field languageSelection:TLanguageSelection
	
	' list of languages in use in the BLF
	Field _settingLangs:String[] = [defaultLanguage]
	Field _settingDefault:Int = 0
	Field _settingFilename:String
	
	' keeps track of which tag and language is currently selected.
	Field tagIndex:Int = -1
	Field langIndex:Int = -1

	' initiates the eventhook, sets up menus, inits the UI.
	Method New()

		' There's a bug on Mac that breaks eventhooks... can't use them there.
		?Linux
		AddHook EmitEventHook,eventhook,Self
		?Win32
		AddHook EmitEventHook,eventhook,Self
		?
				
		addLanguage(defaultLanguage, False)
		
		submenus.insert("menunew", CreateMenu(GetLocaleText("menunew"), MENU_NEW, mnuFile, GetLocaleValue("menunew"), MODIFIER_COMMAND))
		submenus.insert("menuopen", CreateMenu(GetLocaleText("menuopen"), MENU_OPEN, mnuFile, GetLocaleValue("menuopen"), MODIFIER_COMMAND))
		CreateMenu"",0,mnuFile
		submenus.insert("menusave", CreateMenu(GetLocaleText("menusave"), MENU_SAVE, mnuFile, GetLocaleValue("menusave"), MODIFIER_COMMAND))
		CreateMenu"",0,mnuFile
		submenus.insert("menuquit", CreateMenu(GetLocaleText("menuquit"), MENU_QUIT, mnuFile, GetLocaleValue("menuquit"), MODIFIER_COMMAND))

		submenus.insert("menuabout", CreateMenu(GetLocaleText("menuabout"),MENU_ABOUT,mnuHelp))

		submenus.insert("menuimportcsv", CreateMenu(GetLocaleText("menuimportcsv"), MENU_IMPORTCSV, mnuUtils, GetLocaleValue("menuimportcsv"), MODIFIER_COMMAND))
		submenus.insert("menuexportcsv", CreateMenu(GetLocaleText("menuexportcsv"), MENU_EXPORTCSV, mnuUtils, GetLocaleValue("menuexportcsv"), MODIFIER_COMMAND))
		CreateMenu"",0,mnuUtils
		submenus.insert("menuimportini", CreateMenu(GetLocaleText("menuimportini"), MENU_IMPORTINI, mnuUtils, GetLocaleValue("menuimportini"), MODIFIER_COMMAND))
		submenus.insert("menuexportini", CreateMenu(GetLocaleText("menuexportini"), MENU_EXPORTINI, mnuUtils, GetLocaleValue("menuexportini"), MODIFIER_COMMAND))

		Local m:TGadget = CreateMenu(GetLocaleText("menuvieweng"),MENU_VIEW_ENGLISH,mnuView)
		submenus.insert("menuvieweng", m)
		CheckMenu(m)
		m = CreateMenu(GetLocaleText("menuviewnat"),MENU_VIEW_NATIVE,mnuView)
		submenus.insert("menuviewnat", m)
		UncheckMenu(m)
		
		' create languages menu for the application
		Local langs:String[] = GetAvailableLocales()
		For Local i:Int = 0 Until langs.length
			Local m:TGadget = CreateMenu(GetLanguage(langs[i], nativeLanguages), MENU_LANG_BASE + i, mnuLanguage)
			languageMenus.insert(langs[i], m)
			If langs[i] = GetDefaultLocale() Then
				CheckMenu(m)
				currentLanguage = i
			Else
				UncheckMenu(m)
			End If
		Next
		
		UpdateWindowMenu window
		
		updateLanguageTag(SelectedGadgetItem(lstTags), SelectedGadgetItem(lstLanguages))
		
	End Method
	
	Function eventhook:Object(id:Int, data:Object, context:Object)

		If TBLFBuilder(context) Then
			TBLFBuilder(context).OnEvent(TEvent(data))
		End If

		Return data
	End Function

	Method OnEvent(event:TEvent)
'DebugLog(event.toString())
		Select event.id
			Case EVENT_WINDOWCLOSE
				Select event.source
					Case window
						updateLanguageTag(SelectedGadgetItem(lstTags), SelectedGadgetItem(lstLanguages), True)
						If checkModified() = 1 Then
							End
						End If
				End Select
			Case EVENT_APPTERMINATE
				updateLanguageTag(SelectedGadgetItem(lstTags), SelectedGadgetItem(lstLanguages), True)
				If checkModified() = 1 Then
					End
				End If
				
			Case EVENT_GADGETACTION
				Select event.source
					Case btnTagAdd ' add new tag
					
						' create a new BLF if it doesn't already exist
						If Not BLFLoc Then
							BLFLoc = New TLoc
						End If
						
						' add a new tag for editing
						If Not BLFLoc.tagExists("NEWTAG") Then
							AddGadgetItem(lstTags, "NEWTAG",,,,"NEWTAG")
							Local i:Int = CountGadgetItems(lstTags) - 1
	
							SelectGadgetItem(lstTags, i)
							updateLanguageTag(i, SelectedGadgetItem(lstLanguages))
							tagIndex = i
	
							ActivateGadget(tfTagName)
						End If
						
					Case btnLangAdd ' add new language
					
						' create the language selection window if it doesn't already exist
						If Not languageSelection Then
							languageSelection = TLanguageSelection.Create(Self)
						End If
						
						' choose a new language
						languageSelection.show(_settingLangs)
						
					Case cmbDefault ' change default language

						updateDefaultLanguage(event.data)
						_settingDefault = event.data
						
					Case btnTagDel ' delete tag
						
						If tagIndex >= 0 Then
							If Confirm(GetLocaleText("deletetag")) Then
								BLFLoc.deleteTag(String(GadgetItemExtra(lstTags, tagIndex)))
								
								RemoveGadgetItem(lstTags, tagIndex)
								tagIndex = -1
								
								updateLanguageTag(SelectedGadgetItem(lstTags), SelectedGadgetItem(lstLanguages), True)

							End If
						End If
						
				End Select
				
			Case EVENT_GADGETSELECT
				Select event.source
					Case lstTags

						updateLanguageTag(event.data, SelectedGadgetItem(lstLanguages))
						'tagIndex = event.data
						
					Case lstLanguages

						updateLanguageTag(SelectedGadgetItem(lstTags), event.data)
						'langIndex = event.data

				End Select
				
			Case EVENT_GADGETLOSTFOCUS
				Select event.source
					Case tfTagName
						updateLanguageTag(tagIndex, langIndex, True)
					Case taTagText
						updateLanguageTag(tagIndex, langIndex, True)
					Case tfTagValue
						updateLanguageTag(tagIndex, langIndex, True)
				End Select
				
			Case EVENT_MENUACTION
				Select event.data
					Case MENU_OPEN
						If checkModified() = 1 Then
							openBLFFile()
						End If
					Case MENU_ABOUT
						Notify(GetLocaleText("apptitle") + " ( " + version + " )~n~n© 2006, 2007 Bruce A Henderson")
					Case MENU_SAVE
						saveBLFFile()
					Case MENU_NEW
						If checkModified() = 1 Then
							resetBLF()
						End If
					Case MENU_QUIT
						If checkModified() = 1 Then
							End
						End If
					Case MENU_VIEW_ENGLISH
						If Not nativeLanguages Then
							' make sure the menu stays checked...
							CheckMenu(TGadget(submenus.ValueForKey("menuvieweng")))
						Else
							CheckMenu(TGadget(submenus.ValueForKey("menuvieweng")))
							UncheckMenu(TGadget(submenus.ValueForKey("menuviewnat")))
							
							UpdateWindowMenu window
							
							nativeLanguages = False
							
							setLanguageTexts()
						End If
					Case MENU_VIEW_NATIVE
						If nativeLanguages Then
							' make sure the menu stays checked...
							CheckMenu(TGadget(submenus.ValueForKey("menuviewnat")))
						Else
							CheckMenu(TGadget(submenus.ValueForKey("menuviewnat")))
							UncheckMenu(TGadget(submenus.ValueForKey("menuvieweng")))
							
							UpdateWindowMenu window
							
							nativeLanguages = True
							
							setLanguageTexts()
						End If
					Case MENU_IMPORTCSV
						importCSV()
					Case MENU_EXPORTCSV
						exportCSV()
				Default
					Local s:String[] = GetAvailableLocales()
					For Local i:Int = 0 Until s.length
						If event.data = MENU_LANG_BASE + i Then
							If currentLanguage = i Then
								' make sure the menu stays checked...
								CheckMenu(TGadget(languageMenus.valueForKey(s[i])))
							Else
								' language has changed...
								
								' uncheck the old selection
								UncheckMenu(TGadget(languageMenus.valueForKey(s[currentLanguage])))
								
								currentLanguage = i
								CheckMenu(TGadget(languageMenus.valueForKey(s[i])))
								SetCurrentLocale(s[i])
								setGadgetTexts()
							End If
							Exit
						End If
					Next
					
					UpdateWindowMenu window
				End Select
		End Select

	End Method
	
	Method resetBLF()
		BLFLoc.free()
		
		BLFLoc = New TLoc
		
		addLanguage(defaultLanguage, False)
		rebuildDisplay()
		
	End Method
	
	Method checkModified:Int()
		Local ret:Int = 1
		
		If BLFLoc And BLFLoc.modified Then
			' There are unsaved changes which will be lost if you continue.
			' Are you sure you wish to proceed?
			ret = Proceed(GetLocaleText("losechanges"), True)
		End If
	
		Return ret
	End Method
	
	' core method to look after interaction between the UI and the BLF instance.
	' Makes sure things are always in sync.
	Method updateLanguageTag(tag:Int, lang:Int, force:Int = False)

		If tag <> tagIndex Or lang <> langIndex Or force Then
			' do we need to update the text ?
			If tagIndex >= 0 Then
			
				Local tagName:String = String(GadgetItemExtra(lstTags, tagIndex))

				' tag name change ???
				If TextFieldText(tfTagName) <> tagName Then

					' does the tag already exist ?
					If BLFLoc.tagExists(tagName) Then
						' we need to rename it in BLFLoc...
						BLFLoc.renameTag(tagName, TextFieldText(tfTagName))
					End If

					' set the new tag name
					tagName = TextFieldText(tfTagName)
					
					ModifyGadgetItem(lstTags, tagIndex, tagName,,,,tagName)

				End If
				
				If langIndex >= 0 Then
			
					If BLFLoc Then
						' get the current text
						Local txt:String = BLFLoc.getText(tagName, String(GadgetItemExtra(lstLanguages, langIndex)))
	
						' get the current value
						Local v:String = BLFLoc.getValue(tagName, String(GadgetItemExtra(lstLanguages, langIndex)))
						
						' have either changed?
						If (Not BLFLoc.tagExists(String(GadgetItemExtra(lstTags, tagIndex)))) Or txt <> TextAreaText(taTagText) Or v <> TextFieldText(tfTagValue) Then
							' update with new data
							BLFLoc.addText(tagName, ..
								TextAreaText(taTagText), String(GadgetItemExtra(lstLanguages, langIndex)), ..
								TextFieldText(tfTagValue))
								
							BLFLoc.modified = True
						End If
					End If
				End If
			End If
			
			' do we need to update the tags list display?
			checkTagsforLocale(lang)

			' issues with Event Hooks mean we need to set these here...
			tagIndex = tag
			langIndex = lang
		End If
		
	
		If tag < 0 Then
			DisableGadget(btnTagDel)
			DisableGadget(pnlTagText)
			DisableGadget(tfTagName)

			SetTextAreaText(taTagText, "")
			SetGadgetText(tfTagValue, "")
			SetGadgetText(tfTagName, "")
		Else
			EnableGadget(btnTagDel)
			EnableGadget(tfTagName)
			
			SetGadgetText(tfTagName, String(GadgetItemExtra(lstTags, tag)))
		
			If lang >= 0 Then
				EnableGadget(pnlTagText)

				Local s:String = BLFLoc.getText(String(GadgetItemExtra(lstTags, tag)), String(GadgetItemExtra(lstLanguages, lang)))
				SetTextAreaText(taTagText, s)
			
				s = BLFLoc.getValue(String(GadgetItemExtra(lstTags, tag)), String(GadgetItemExtra(lstLanguages, lang)))
				SetGadgetText(tfTagValue, s)
			Else
			
				DisableGadget(pnlTagText)
				SetTextAreaText(taTagText, "")
				SetGadgetText(tfTagValue, "")
				
			End If
		End If
	End Method
	
	Method updateDefaultLanguage(def:Int)

		If _settingDefault <> def Then
		
			If def >= 0 Then
		
				BLFLoc.defaultLang = _settingLangs[def]
				_settingDefault = def
				BLFLoc.modified = True

			End If
		End If
		
	End Method

	' Shows open requestor, and loads if required.
	Method openBLFFile()
		Local filename:String = RequestFile(GetLocaleText("openfile"), GetLocaleText("blffiles") + ":blf")' ONLY BLF for now... ;" + GetLocaleText("allfiles") + ":*" )

		If filename Then
			_settingFilename = filename
			loadBLF(filename)
		End If

	End Method
	
	' Shows save requestor, and saves if required.
	Method saveBLFFile()
'		Local filename:String = RequestFile(GetLocaleText("savefile"), GetLocaleText("blffiles") + ":blf;" + GetLocaleText("allfiles") + ":*", True,  _settingFilename)
		Local filename:String = RequestFile(GetLocaleText("savefile"), GetLocaleText("blffiles") + ":blf", True,  _settingFilename) ' ONLY BLF for now
		
		If filename Then
			_settingFilename = filename
			saveBLF(filename)
		End If
		
	End Method

	' Shows save requestor, and exports.
	Method exportCSV()
		Local filename:String = RequestFile(GetLocaleText("savecsvfile"), GetLocaleText("csvfiles") + ":csv", True) ' ONLY BLF for now
		
		If filename Then
			saveCSV(filename)
		End If
		
	End Method

	' Shows save requestor, and exports.
	Method importCSV()
		Local filename:String = RequestFile(GetLocaleText("savecsvfile"), 	GetLocaleText("csvfiles") + ":csv") ' ONLY BLF for now
		
		If filename Then
			loadCSV(filename)
		End If
	End Method

	' Loads a BLF file
	Method loadBLF(filename:String)

		Local doc:TxmlDoc = TxmlDoc.parseFile(filename)
		
		If doc Then
		
			If BLFLoc Then
				BLFLoc.free()
			End If
			
			BLFLoc = New TLoc
		
			Local root:TxmlNode = doc.getRootElement()
			
			' get the default
			BLFLoc.defaultLang = root.getAttribute("default")
			If BLFLoc.defaultLang Then
				BLFLoc.addLanguage(BLFLoc.defaultLang)
			End If
			
			For Local node:TxmlNode = EachIn root.getChildren()
			
				If node.getName() = "tag" Then
				
					Local tag:String = node.getAttribute("name")
				
					For Local trans:TxmlNode = EachIn node.getChildren()
						
						If trans.getName() = "trans" Then
						
							BLFLoc.addText(tag, trans.getContent(), trans.getAttribute("lang"), trans.getAttribute("value"))
						
						End If
						
					Next
					
				End If
			
			Next
		
			doc.free()
			
			rebuildDisplay()
		End If

	End Method
	
	' Saves the BLF file
	Method saveBLF(filename:String)
	
		Local doc:TxmlDoc = TxmlDoc.newDoc("1.0")
		
		If doc Then
			
			Local rootNode:TxmlNode = TxmlNode.newNode("locale")

			doc.setRootElement(rootNode)
			
			rootNode.addAttribute("default", _settingLangs[_settingDefault])
			rootNode.addComment("Generated by BLFBuilder ( " + version + " )  " + CurrentDate() + " " + CurrentTime())

			Local tags:String[] = BLFLoc.getTags()
			
			For Local i:Int = 0 Until tags.length
			
				Local tagNode:TxmlNode = rootNode.addChild("tag", Null, Null)
				tagNode.addAttribute("name", tags[i])
				
				
				Local lt:TLocEntry = TLocEntry(BLFLoc.textMap.ValueForKey(tags[i]))
			
				Local locs:String[] = lt.getLocales()
				
				For Local n:Int = 0 Until locs.length
				
					Local part:TLocPart = TLocPart(lt.locales.ValueForKey(locs[n]))
					
					If part.text Or part.text.length > 0 Or part.value <> 0 Then
						
						Local transNode:TxmlNode = tagNode.addTextChild("trans", Null, part.text)
						transNode.addAttribute("lang", locs[n])
						
						If part.value <> 0 Then
							transNode.addAttribute("value", String.fromInt(part.value))
						End If
						
					End If
				
				Next
			
			Next
			
			BLFLoc.modified = False

			' save and free
			doc.saveFormatFile(filename, True)
			doc.free()
		
		End If
		
	End Method

	' Saves the CSV file
	Method saveCSV(filename:String)
		Local csvfile:TStream=WriteStream("utf8::" + filename)
		If Not csvfile Then RuntimeError GetLocaleText("savefailcsv")
		
		Local str:String = ""
		Local tags:String[] = BLFLoc.getTags()
		Local locs:String[] = BLFLoc.getLocales()
		
		' Write the header
		str = "Tag,"
		
		For Local i:Int = 0 Until locs.length
			str:+locs[i]+","
		Next
		WriteLine(csvfile, str)
		
		' Write the tags and texts
		For Local i:Int = 0 Until tags.length
			str = tags[i]+","
			
			Local lt:TLocEntry = TLocEntry(BLFLoc.textMap.ValueForKey(tags[i]))
			
			Local locs:String[] = lt.getLocales()
			
			For Local n:Int = 0 Until locs.length
			
				Local part:TLocPart = TLocPart(lt.locales.ValueForKey(locs[n]))
				
				If part.Text Or part.Text.length > 0 Then
					Local s:String = part.text.Replace(",", ";")
					s = s.Replace("~n", "~~n") ' replace new lines !
					str:+ s + ","
				EndIf
			Next
			
			WriteLine(csvfile, str)
		Next

		CloseStream(csvfile)
	End Method
	
	' Loads a CSV file
	Method loadCSV(filename:String)

		Local csvfile:TStream=ReadStream("utf8::" + filename)
		If Not csvfile Then
			RuntimeError GetLocaleText("loadfailcsv")
		End If
	
		If BLFLoc Then
			BLFLoc.free()
		End If
		
		BLFLoc = New TLoc
		
		' Get header (Read languages)
		Local langlist:TList = CreateList()
		Local str:String = ReadLine(csvfile)
		Local fld:String = ""

		While Len(str) > 0
			Local comma:Int = str.Find(",")
			
			If comma > -1 Then
				fld = str[..comma]
			Else
				' Last fields so kill string
				fld = str
				str = ""
			EndIf
			
			If fld <> "Tag"
				langlist.AddLast(fld)
			End If
			
			' Erase data that has been read
			If str.length Then
				str = str[comma + 1..]
				If str = "," Then
					str = ""
				End If
			EndIf
		Wend
		
		' Read tags and translations
		While Not Eof(csvfile)
			str = ReadLine(csvfile)
			
			Local comma:Int = str.Find(",")
			
			' Get Tag name
			Local tag:String = str[..comma]
			
			' Remove tag and comma from string
			str = str[comma+1..]
			
			' Loop through languages
			For Local lang:String = EachIn langlist
				comma = str.Find(",")
				
				If comma Then
					fld = str[..comma]
				Else
					' Last fields so kill string
					fld = str
					str = ""
				EndIf
					
				fld = fld.Replace(";", ",")
				fld = fld.Replace("~~n", "~n") ' create new lines!
				If Len(fld) Then
					BLFLoc.addText(tag, fld, lang, "")
				End If
						
				' Erase data that has been read from the string
				If Len(str) Then
					str = str[comma+1..]
				End If
			Next
		Wend
		
		CloseStream(csvfile)
		
		rebuildDisplay()
	
	End Method

	' Adds a language to the BLF, updating the GUI appropriately.
	' We also remember where the language was previously selected and reselect it (it might be at a new index)
	Method addLanguage(lang:String, isModified:Int)
		Local tempLang:String = Null
		
		' remember language for index
		If langIndex >= 0 Then
			tempLang = _settingLangs[langIndex]
		End If

		If Not BLFLoc Then
			BLFLoc = New TLoc
		End If
		
		BLFLoc.addLanguage(lang)
		
		_settingLangs = BLFLoc.getLocales()

		populateLanguages()
		populateDefault()
		
		If tempLang Then
			For Local i:Int = 0 Until _settingLangs.length
				If tempLang = _settingLangs[i] Then
					langIndex = i
					SelectGadgetItem(lstLanguages, i)
					Exit
				End If
			Next
		End If
		
		If isModified Then
			BLFLoc.modified = True
		End If
	End Method
	
	Method rebuildDisplay()

		ClearGadgetItems(lstTags)
		ClearGadgetItems(cmbDefault)
		
		' languages
		_settingLangs = BLFLoc.getLocales()
		populateLanguages()

		'SelectGadgetItem(lstLanguages, 0)
		'langIndex = 0

		' default
		_settingDefault = 0
		For Local i:Int = 0 Until _settingLangs.length
			If BLFLoc.defaultLang = _settingLangs[i] Then
				_settingDefault = i

				SelectGadgetItem(lstLanguages, i)
				langIndex = i

			End If
			AddGadgetItem(cmbDefault, GetLanguage(_settingLangs[i], nativeLanguages),,,,_settingLangs[i])
		Next
		SelectGadgetItem(cmbDefault, _settingDefault)
		
		' tags
		Local tags:String[] = BLFLoc.getTags()
		For Local i:Int = 0 Until tags.length
			AddGadgetItem(lstTags, tags[i],,,,tags[i])
		Next
		
	End Method
	
	' recreates the languages list
	' We store the iso code in the Extra field of the item.
	Method populateLanguages()
		ClearGadgetItems(lstLanguages)
		
		For Local i:Int = 0 Until _settingLangs.length
			AddGadgetItem(lstLanguages, GetLanguage(_settingLangs[i], nativeLanguages),,,,_settingLangs[i])
		Next
	
		If langIndex >= 0 Then
			SelectGadgetItem(lstLanguages, langIndex)
		End If
	End Method
	
	Method populateDefault()
		ClearGadgetItems(cmbDefault)

		For Local i:Int = 0 Until _settingLangs.length
			If BLFLoc.defaultLang = _settingLangs[i] Then
				_settingDefault = i
			End If
			AddGadgetItem(cmbDefault, GetLanguage(_settingLangs[i], nativeLanguages),,,,_settingLangs[i])
		Next
		SelectGadgetItem(cmbDefault, _settingDefault)
	
	End Method

	' tags that are missing the current language text appear with a "+ " in front of the tag.
	' We store the original tag text in the Extra field of the gadget item.
	Method checkTagsforLocale(index:Int)
		Local c:Int = CountGadgetItems(lstTags)
		Local lang:String = Null
		If index >=0 Then
			lang = _settingLangs[index]
		End If
		
		For Local i:Int = 0 Until c
			Local s:String = String(GadgetItemExtra(lstTags, i))
			
			' only show missing if 1) a language is selected, 2) it is missing.
			If lang And Not BLFLoc.localizedTagExists(s, lang) Then
				ModifyGadgetItem(lstTags, i, "+ " + s,,,,s)
			Else
				' only update to "normal" if it isn't normal already
				If GadgetItemText(lstTags, i).startsWith("+") Then
					ModifyGadgetItem(lstTags, i, s,,,,s)
				End If
			End If
		Next
		
	End Method
	
	Method setGadgetTexts()
		' set gadget texts
		SetGadgetText(window, GetLocaleText("apptitle"))
		SetGadgetText(pnlTags, GetLocaleText("tags"))
		SetGadgetText(btnTagAdd, GetLocaleText("add"))
		SetGadgetText(btnTagDel, GetLocaleText("delete"))
		SetGadgetText(pnlLang, GetLocaleText("languages"))
		SetGadgetText(lblDefault, GetLocaleText("default"))
		SetGadgetText(btnLangAdd, GetLocaleText("add"))
		SetGadgetText(btnLangDel, GetLocaleText("delete"))
		SetGadgetText(pnlTagText, GetLocaleText("tagtext"))
		SetGadgetText(lblValue, GetLocaleText("value"))
		
		' app title
		AppTitle = GetLocaleText("apptitle")
		
		' set the main menu texts
		SetMenuText(mnuFile, GetLocaleText("menufile"))
		SetMenuText(mnuUtils, GetLocaleText("menuutils"))
		SetMenuText(mnuLanguage, GetLocaleText("menulanguage"))
		SetMenuText(mnuView, GetLocaleText("menuview"))
		SetMenuText(mnuHelp, GetLocaleText("menuhelp"))
		
		' set the submenu texts
		For Local s:String = EachIn submenus.Keys()
			SetMenuText(TGadget(submenus.ValueForKey(s)), GetLocaleText(s))
		Next
		
	End Method
	
	Method setLanguageTexts()
		populateLanguages()
		populateDefault()
	End Method
	
End Type

' the dialog listing all available languages to choose from.
' Only displays languages not currently being used.
Type TLanguageSelection

	Field dw:Int = GadgetWidth(Desktop())
	Field dh:Int = GadgetHeight(Desktop())
	Field w:Int = 200
	?MacOs
	Field h:Int = 320
	?Win32
	Field h:Int = 320
	?Linux
	Field h:Int = 300
	?
	
	Field parent:TBLFBuilder

'	Field opening:Int = False

	Field window:TGadget
	Field lstLang:TGadget
	Field btnCancel:TGadget
	Field btnOK:TGadget

	Function Create:TLanguageSelection(parent:TBLFBuilder)
		Local this:TLanguageSelection = New TLanguageSelection
		
		this.parent = parent
		
		this.init()
		
		Return this
	End Function

	Method New()
	
		AddHook EmitEventHook,eventhook,Self

	End Method

	Method init()
		window = CreateWindow(GetLocaleText("selectlang"),dw / 2 - w / 2, dh / 2 - h / 2,w,h, parent.window, WINDOW_TITLEBAR | WINDOW_HIDDEN)		
		
		lstLang = CreateListBox(10, 10, 180, 250, window)
		btnCancel = CreateButton(GetLocaleText("cancel"), 10, 265, 85, 28, window)
		btnOK = CreateButton(GetLocaleText("ok"), 105, 265, 85, 28, window)
		
	End Method
	
	Method hide()
		EnableGadget(parent.window)
		HideGadget(window)
	End Method
	
	Method show(langs:String[])

		ClearGadgetItems(lstLang)
		
		Local locs:String[] = GetSystemLocales()
		
		For Local i:Int = 0 Until locs.length
		
			Local found:Int = False
			
			For Local n:Int = 0 Until langs.length
				If langs[n] = locs[i] Then
					found = True
					Exit
				End If
			Next
			
			If Not found Then
				AddGadgetItem(lstLang, locs[i] + "  - " + GetLanguage(locs[i], parent.nativeLanguages),,,,locs[i])
			End If

		Next

		SelectGadgetItem(lstLang, -1)
		DisableGadget(btnOK)
	
		DisableGadget(parent.window)
		ShowGadget(window)
		EnableGadget(window)
		ActivateGadget(window)
	End Method

	Function eventhook:Object(id:Int, data:Object, context:Object)

		If TLanguageSelection(context) Then
			TLanguageSelection(context).OnEvent(TEvent(data))
		End If

		Return data
	End Function

	Method OnEvent(event:TEvent)
		Select event.id
			Case EVENT_WINDOWCLOSE
				Select event.source
					Case window
						hide()
				End Select
			Case EVENT_GADGETSELECT
				Select event.source
					Case lstLang
						If event.data < 0 Then
							DisableGadget(btnOK)
						Else
							EnableGadget(btnOK)
						End If
				End Select	
			Case EVENT_GADGETACTION
				Select event.source
					Case btnCancel
						hide()
					Case btnOK
						parent.addLanguage(String(GadgetItemExtra(lstLang, SelectedGadgetItem(lstLang))), True)
						hide()
				End Select
		End Select
	End Method	
End Type
