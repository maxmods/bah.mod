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

Framework wx.wxApp
Import wx.wxFrame
Import wx.wxPanel
Import wx.wxTextCtrl
Import wx.wxListBox
Import wx.wxStaticBox
Import wx.wxButton
Import wx.wxStaticText
Import wx.wxComboBox
Import wx.wxFileDialog

Import BaH.Locale
Import BRL.System
'Import BRL.Timer
'Import BRL.EventQueue

Import "lochandler.bmx"

Incbin "blfbuilder.blf"

' load the application locale data
LoadLocaleFile("incbin::blfbuilder.blf")

Global version:String = "1.02"

AppTitle = GetLocaleText("apptitle")

Global builder:TBLFBuilder

New MyApp.Run()

Type MyApp Extends wxApp

	Method OnInit:Int()

		builder = TBLFBuilder(New TBLFBuilder.Create(Null, 1, AppTitle, , , 500, 430))
		
		builder.show()

		SetTopWindow(builder)
			
		Return True
	
	End Method

End Type

Type TBLFBuilder Extends wxFrame

	' store for the current BLF file
	Field BLFLoc:TLoc
	
	Const baseLanguage:String = "en"

	Field defaultLanguage:String = baseLanguage
	Field nativeLanguages:Int = False

	Const MENU_NEW:Int = 101
	Const MENU_OPEN:Int = 102
	Const MENU_SAVE:Int = 103
	Const MENU_QUIT:Int = wxID_EXIT
	
	Const MENU_VIEW_ENGLISH:Int = 401
	Const MENU_VIEW_NATIVE:Int = 402

	Const MENU_ABOUT:Int = wxID_ABOUT
	
	Const MENU_LANG_BASE:Int = 301

	Const MENU_UTILS:Int = 201
	Const MENU_IMPORTCSV:Int = 202
	Const MENU_EXPORTCSV:Int = 203

	Const MENU_IMPORTINI:Int = 204
	Const MENU_EXPORTINI:Int = 205

	Const CTRL_LSTTAGS:Int = 5000
	Const CTRL_TAGADD:Int = 5001
	Const CTRL_TAGDEL:Int = 5002
	Const CTRL_TAGNME:Int = 5003
	Const CTRL_COMBO:Int = 5004
	Const CTRL_LSTLANG:Int = 5005
	Const CTRL_BTNADD:Int = 5006
	Const CTRL_BTNDEL:Int = 5007
	Const CTRL_TAGDESC:Int = 5008
	
	Field mnuFile:wxMenu
	Field mnuUtils:wxMenu
	Field mnuLanguage:wxMenu
	Field mnuView:wxMenu
	Field mnuHelp:wxMenu

	Field pnlTags:wxPanel
	Field lstTags:wxListBox
	Field tfTagName:wxTextCtrl
	Field btnTagAdd:wxButton
	Field btnTagDel:wxButton
	
	Field pnlLang:wxPanel
	Field lblDefault:wxStaticText
	Field cmbDefault:wxComboBox
	Field lstLanguages:wxListBox
	Field btnLangAdd:wxButton
	Field btnLangDel:wxButton

	Field pnlTagText:wxPanel

	Field taTagText:wxTextCtrl
	Field lblValue:wxStaticText
	Field tfTagValue:wxTextCtrl

	Field languageMenus:TMap = New TMap
	Field currentLanguage:Int = 0

	Field submenus:TMap = New TMap
	
	' the language selection dialog
	'Field languageSelection:TLanguageSelection
	
	' list of languages in use in the BLF
	Field _settingLangs:String[] = [defaultLanguage]
	Field _settingDefault:Int = 0
	Field _settingFilename:String
	
	' keeps track of which tag and language is currently selected.
	Field tagIndex:Int = -1
	Field langIndex:Int = -1
	
	Method OnInit()

		mnuFile = wxMenu.CreateMenu()
		mnuUtils = wxMenu.CreateMenu()
		mnuLanguage = wxMenu.CreateMenu()
		mnuView = wxMenu.CreateMenu()
		mnuHelp = wxMenu.CreateMenu()
		Local menuBar:wxMenuBar = wxMenuBar.CreateMenuBar()
		
		menuBar.Append(mnuFile, GetLocaleText("menufile"))
		menuBar.Append(mnuUtils, GetLocaleText("menuutils"))
		menuBar.Append(mnuLanguage, GetLocaleText("menulanguage"))
		menuBar.Append(mnuView, GetLocaleText("menuview"))
		menuBar.Append(mnuHelp, GetLocaleText("menuhelp"))

		SetMenuBar(menuBar)
		

		Local panel:wxPanel = New wxPanel.Create(Self, -1)
		
		pnlTags = New wxPanel.Create(panel, -1, 10, 10, 200, 350)
		New wxStaticBox.Create(pnlTags, -1, GetLocaleText("tags"), 0, 0, 200, 350)
		
		lstTags = New wxListBox.Create(pnlTags, CTRL_LSTTAGS, Null, 10, 20, 170, 250)
		tfTagName = New wxTextCtrl.Create(pnlTags, CTRL_TAGNME, "", 10, 270, 170, 28)
		btnTagAdd = New wxButton.Create(pnlTags, CTRL_TAGADD, GetLocaleText("add"), 10, 300, 80, 28)
		btnTagDel = New wxButton.Create(pnlTags, CTRL_TAGDEL, GetLocaleText("delete"), 100, 300, 80, 28)
		
		pnlLang = New wxPanel.Create(panel, -1, 220, 10, 270, 200)
		New wxStaticBox.Create(pnlLang, -1, GetLocaleText("languages"), 0, 0, 270, 200)

		lblDefault = New wxStaticText.Create(pnlLang, -1, GetLocaleText("default"), 10, 20, 100, 28)
		cmbDefault = New wxComboBox.Create(pnlLang, CTRL_COMBO, "", Null, 120, 20, 140, 28)
		lstLanguages = New wxListBox.Create(pnlLang, CTRL_LSTLANG, Null, 10, 60, 150, 120)
		btnLangAdd = New wxButton.Create(pnlLang, CTRL_BTNADD, GetLocaleText("add"), 170, 120, 90, 28)
		btnLangDel = New wxButton.Create(pnlLang, CTRL_BTNDEL, GetLocaleText("delete"), 170, 150, 90, 28)

		pnlTagText = New wxPanel.Create(panel, -1, 220, 220, 270, 140)
		New wxStaticBox.Create(pnlTagText, -1, GetLocaleText("tagtext"), 0, 0, 270, 140)

		taTagText = New wxTextCtrl.Create(pnlTagText, CTRL_TAGDESC, "", 10, 20, 250, 70, wxTE_MULTILINE)
	
		lblValue = New wxStaticText.Create(pnlTagText, -1, GetLocaleText("value"), 10, 105, 100)
		tfTagValue = New wxTextCtrl.Create(pnlTagText, -1, "", 120, 100, 140, 28)

		'
		
		addLanguage(defaultLanguage, False)
		
		submenus.insert("menunew", mnuFile.Append(MENU_NEW, GetLocaleText("menunew")))
		submenus.insert("menuopen", mnuFile.Append(MENU_OPEN, GetLocaleText("menuopen")))
		mnuFile.AppendSeparator()
		submenus.insert("menusave", mnuFile.Append(MENU_SAVE, GetLocaleText("menusave")))
		mnuFile.AppendSeparator()
		submenus.insert("menuquit", mnuFile.Append(MENU_QUIT, GetLocaleText("menuquit")))

		submenus.insert("menuabout", mnuHelp.Append(MENU_ABOUT, GetLocaleText("menuabout")))

		submenus.insert("menuimportcsv", mnuUtils.Append(MENU_IMPORTCSV, GetLocaleText("menuimportcsv")))
		submenus.insert("menuexportcsv", mnuUtils.Append(MENU_EXPORTCSV, GetLocaleText("menuexportcsv")))
		mnuUtils.AppendSeparator()
		submenus.insert("menuimportini", mnuUtils.Append(MENU_IMPORTINI, GetLocaleText("menuimportini")))
		submenus.insert("menuexportini", mnuUtils.Append(MENU_EXPORTINI, GetLocaleText("menuexportini")))

		submenus.insert("menuvieweng", mnuView.AppendRadioItem(MENU_VIEW_ENGLISH, GetLocaleText("menuvieweng")))
		submenus.insert("menuviewnat", mnuView.AppendRadioItem(MENU_VIEW_NATIVE, GetLocaleText("menuviewnat")))
		mnuView.Check(MENU_VIEW_ENGLISH, True)

		' create languages menu for the application
		Local langs:String[] = GetAvailableLocales()
		For Local i:Int = 0 Until langs.length
			Local m:wxMenuItem = mnuLanguage.AppendRadioItem(MENU_LANG_BASE + i, GetLanguage(langs[i], nativeLanguages))
			languageMenus.insert(langs[i], m)
			If langs[i] = GetDefaultLocale() Then
				m.Check()
				currentLanguage = i
'			Else
'				m.Check(False)
			End If
		Next

		Connect(MENU_OPEN, wxEVT_COMMAND_MENU_SELECTED, OnOpen)
		Connect(MENU_VIEW_NATIVE, wxEVT_COMMAND_MENU_SELECTED, OnViewNative)
		Connect(MENU_VIEW_ENGLISH, wxEVT_COMMAND_MENU_SELECTED, OnViewEnglish)
		ConnectRange(MENU_LANG_BASE, MENU_LANG_BASE + 99, wxEVT_COMMAND_MENU_SELECTED, OnLanguage)
		Connect(CTRL_TAGADD, wxEVT_COMMAND_BUTTON_CLICKED, OnTagAdd)
		Connect(CTRL_TAGDEL, wxEVT_COMMAND_BUTTON_CLICKED, OnTagDel)
		tfTagName.ConnectAny(wxEVT_KILL_FOCUS, OnFocusLost)
		Connect(CTRL_LSTTAGS, wxEVT_COMMAND_LISTBOX_SELECTED, OnTagSelection)
		Connect(CTRL_LSTLANG, wxEVT_COMMAND_LISTBOX_SELECTED, OnLangSelection)
	End Method
	
	Function OnOpen(event:wxEvent)
		If builder.checkModified() = 1 Then
			builder.openBLFFile()
		End If
	End Function
	
	Function OnViewNative(event:wxEvent)
		If Not builder.nativeLanguages Then
			builder.nativeLanguages = True
			
			builder.setLanguageTexts()
		End If
	End Function

	Function OnViewEnglish(event:wxEvent)
		If builder.nativeLanguages Then
			builder.nativeLanguages = False
			
			builder.setLanguageTexts()
		End If
	End Function
	
	Function OnLanguage(event:wxEvent)
		Local s:String[] = GetAvailableLocales()
		For Local i:Int = 0 Until s.length
			If event.GetId() = MENU_LANG_BASE + i Then
				If builder.currentLanguage <> i Then
					' language has changed...
					
					builder.currentLanguage = i
					SetCurrentLocale(s[i])
					builder.setGadgetTexts()
				End If
				Exit
			End If
		Next
	End Function
	
	Function OnTagAdd(event:wxEvent)
		' create a new BLF if it doesn't already exist
		If Not builder.BLFLoc Then
			builder.BLFLoc = New TLoc
		End If
'DebugStop
		' add a new tag for editing
		If Not builder.BLFLoc.tagExists("NEWTAG") Then
			builder.lstTags.Append("NEWTAG","NEWTAG")
			Local i:Int = builder.lstTags.GetCount() - 1

			builder.lstTags.SetSelection(i)

			builder.updateLanguageTag(i, builder.lstLanguages.GetSelection())

			builder.tagIndex = i

			builder.tfTagName.SetFocus()
		End If
	End Function
	
	Function OnTagDel(event:wxEvent)
		If builder.tagIndex >= 0 Then
			If wxMessageBox(GetLocaleText("deletetag"),, wxYES_NO | wxICON_QUESTION, builder) = wxYES Then
				builder.BLFLoc.deleteTag(String(builder.lstTags.GetItemClientdata(builder.tagIndex)))
			
				builder.lstTags.DeleteItem(builder.tagIndex)
				builder.tagIndex = -1
				
				builder.updateLanguageTag(builder.lstTags.GetSelection(), builder.lstLanguages.GetSelection(), True)

			End If
		End If
	End Function
	
	Function OnFocusLost(event:wxEvent)
		Select event.GetId()
			Case CTRL_TAGNME, CTRL_TAGDESC
				builder.updateLanguageTag(builder.tagIndex, builder.langIndex, True)
		End Select
	End Function
	
	Function OnTagSelection(event:wxEvent)
		builder.updateLanguageTag(wxCommandEvent(event).GetInt(), builder.lstLanguages.GetSelection())
	End Function
	
	Function OnLangSelection(event:wxEvent)
	End Function

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
					lstLanguages.SetSelection(i)
					Exit
				End If
			Next
		End If
		
		If isModified Then
			BLFLoc.modified = True
		End If
	End Method

	' recreates the languages list
	' We store the iso code in the Extra field of the item.
	Method populateLanguages()
		lstLanguages.Clear()
		
		For Local i:Int = 0 Until _settingLangs.length
			lstLanguages.Append(GetLanguage(_settingLangs[i], nativeLanguages),_settingLangs[i])
		Next
	
		If langIndex >= 0 Then
			lstLanguages.SetSelection(langIndex)
		End If
	End Method
	
	Method populateDefault()
		cmbDefault.Clear()

		For Local i:Int = 0 Until _settingLangs.length
			If BLFLoc.defaultLang = _settingLangs[i] Then
				_settingDefault = i
			End If
			cmbDefault.Append(GetLanguage(_settingLangs[i], nativeLanguages),_settingLangs[i])
		Next
		cmbDefault.SetSelection(_settingDefault)
	
	End Method

	' Shows open requestor, and loads if required.
	Method openBLFFile()
		Local filename:String = wxFileSelector(GetLocaleText("openfile"), "", "", "*.blf", GetLocaleText("blffiles") + "(*.blf)|*.blf", wxFD_OPEN)
		If filename Then
			_settingFilename = filename
			loadBLF(filename)
		End If

	End Method
	
	' Shows save requestor, and saves if required.
	Method saveBLFFile()
'		Local filename:String = RequestFile(GetLocaleText("savefile"), GetLocaleText("blffiles") + ":blf;" + GetLocaleText("allfiles") + ":*", True,  _settingFilename)
'		Local filename:String = RequestFile(GetLocaleText("savefile"), GetLocaleText("blffiles") + ":blf", True,  _settingFilename) ' ONLY BLF for now
		
'		If filename Then
'			_settingFilename = filename
'			saveBLF(filename)
'		End If
		
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

	Method rebuildDisplay()

		lstTags.Clear()
		cmbDefault.Clear()
		
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

				lstLanguages.SetSelection(i)
				langIndex = i

			End If
			cmbDefault.Append(GetLanguage(_settingLangs[i], nativeLanguages),_settingLangs[i])
		Next
		cmbDefault.SetSelection(_settingDefault)
		
		' tags
		Local tags:String[] = BLFLoc.getTags()
		For Local i:Int = 0 Until tags.length
			lstTags.Append(tags[i],tags[i])
		Next
		
	End Method

	Method checkModified:Int()
		Local ret:Int = 1
		
		If BLFLoc And BLFLoc.modified Then
			' There are unsaved changes which will be lost if you continue.
			' Are you sure you wish to proceed?
			ret = wxMessageBox(GetLocaleText("losechanges"),, wxOK | wxICON_EXCLAMATION, Self)
		End If
	
		Return ret
	End Method

	Method setGadgetTexts()
		' set gadget texts
		SetTitle(GetLocaleText("apptitle"))
		'SetGadgetText(pnlTags, GetLocaleText("tags"))
		btnTagAdd.SetLabel(GetLocaleText("add"))
		btnTagDel.SetLabel(GetLocaleText("delete"))
		'SetGadgetText(pnlLang, GetLocaleText("languages"))
		lblDefault.SetLabel(GetLocaleText("default"))
		btnLangAdd.SetLabel(GetLocaleText("add"))
		btnLangDel.SetLabel(GetLocaleText("delete"))
		'SetGadgetText(pnlTagText, GetLocaleText("tagtext"))
'		lblValue.SetLabel(GetLocaleText("value"))
		
		' app title
		AppTitle = GetLocaleText("apptitle")
		
		' set the main menu texts
		GetMenuBar().SetLabelTop(0, GetLocaleText("menufile"))
		GetMenuBar().SetLabelTop(1, GetLocaleText("menuutils"))
		GetMenuBar().SetLabelTop(2, GetLocaleText("menulanguage"))
		GetMenuBar().SetLabelTop(3, GetLocaleText("menuview"))
		GetMenuBar().SetLabelTop(4, GetLocaleText("menuhelp"))
		
		' set the submenu texts
		For Local s:String = EachIn submenus.Keys()
			wxMenuItem(submenus.ValueForKey(s)).SetText(GetLocaleText(s))
		Next
		
	End Method

	Method setLanguageTexts()
		populateLanguages()
		populateDefault()
	End Method

	' core method to look after interaction between the UI and the BLF instance.
	' Makes sure things are always in sync.
	Method updateLanguageTag(tag:Int, lang:Int, force:Int = False)

		If tag <> tagIndex Or lang <> langIndex Or force Then
			' do we need to update the text ?
			If tagIndex >= 0 Then
			
				Local tagName:String = String(lstTags.GetItemClientData(tagIndex))

				' tag name change ???
				If tfTagName.GetValue() <> tagName Then

					' does the tag already exist ?
					If BLFLoc.tagExists(tagName) Then
						' we need to rename it in BLFLoc...
						BLFLoc.renameTag(tagName, tfTagName.GetValue())
					End If

					' set the new tag name
					tagName = tfTagName.GetValue()

					lstTags.SetString(tagIndex, tagName)
					lstTags.SetItemClientData(tagIndex,tagName)

				End If
				
				If langIndex >= 0 Then
			
					If BLFLoc Then
						' get the current text
						Local txt:String = BLFLoc.getText(tagName, String(lstLanguages.GetItemClientData(langIndex)))
	
						' get the current value
						Local v:String = BLFLoc.getValue(tagName, String(lstLanguages.GetItemClientData(langIndex)))
						
						' have either changed?
						If (Not BLFLoc.tagExists(String(lstTags.GetItemClientData(tagIndex)))) Or txt <> taTagText.GetValue() Or v <> tfTagValue.GetValue() Then
							' update with new data
							BLFLoc.addText(tagName, ..
								taTagText.GetValue(), String(lstLanguages.GetItemClientData(langIndex)), ..
								tfTagValue.GetValue())
								
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
			btnTagDel.Disable()
			pnlTagText.Disable()
			tfTagName.Disable()

			taTagText.ChangeValue("")
			tfTagValue.ChangeValue("")
			tfTagName.ChangeValue("")
		Else
			btnTagDel.Enable()
			tfTagName.Enable()
			
			tfTagName.ChangeValue(String(lstTags.GetItemClientData(tag)))
		
			If lang >= 0 Then
				pnlTagText.Enable()

				Local s:String = BLFLoc.getText(String(lstTags.GetItemClientData(tag)), String(lstLanguages.GetItemClientData(lang)))
				taTagText.ChangeValue(s)
			
				s = BLFLoc.getValue(String(lstTags.GetItemClientData(tag)), String(lstLanguages.GetItemClientData(lang)))
				tfTagValue.ChangeValue(s)
			Else
			
				pnlTagText.Disable()
				taTagText.ChangeValue("")
				tfTagValue.ChangeValue("")
				
			End If
		End If
	End Method

	' tags that are missing the current language text appear with a "+ " in front of the tag.
	' We store the original tag text in the Extra field of the gadget item.
	Method checkTagsforLocale(index:Int)
		Local c:Int = lstTags.GetCount()
		Local lang:String = Null
		If index >=0 Then
			lang = _settingLangs[index]
		End If
		
		For Local i:Int = 0 Until c
			Local s:String = String(lstTags.GetItemClientData(i))
			
			' only show missing if 1) a language is selected, 2) it is missing.
			If lang And Not BLFLoc.localizedTagExists(s, lang) Then
				lstTags.SetString(i, "+ " + s)
				lstTags.SetItemClientData(i,s)
			Else
				' only update to "normal" if it isn't normal already
				If lstTags.GetString(i).startsWith("+") Then
					lstTags.SetString(i, s)
					lstTags.SetItemClientData(i,s)
				End If
			End If
		Next
		
	End Method

End Type


