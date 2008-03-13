SuperStrict

' xmlcatalog : a small utility program to handle XML catalogs
' daniel@veillard.com
' BlitzMax port by Bruce A Henderson

Framework BaH.Libxml
Import BRL.StandardIO
Import BRL.Filesystem

Global shell:Int = 0
Global sgml:Int = 0
Global noout:Int = 0
Global _create:Int = 0
Global add:Int = 0
Global del:Int = 0
Global convert:Int = 0
Global no_super_update:Int = 0
Global verbose:Int = 0
Global filename:String = Null

Function usage()
	Print "Usage : " + StripDir(AppFile) + " [options] catalogfile entities..."
	Print "~tParse the catalog file And query it For the entities"
	Print "~t--sgml : handle SGML Super catalogs For --add And --del"
	Print "~t--shell : run a shell allowing interactive queries"
	Print "~t--create : create a New catalog"
	Print "~t--add 'type' 'orig' 'replace' : add an XML entry"
	Print "~t--add 'entry' : add an SGML entry"

	Print "~t--del 'values' : remove values"
	Print "~t--noout: avoid dumping the result on stdout"
	Print "~t         used with --add Or --del, it saves the catalog changes"
	Print "~t         And with --sgml it automatically updates the Super catalog"
	Print "~t--no-Super-update: do Not update the SGML Super catalog"
	Print "~t-v --verbose : provide debug informations"
End Function

Function xmlShellReadline:String(prompt:String)
	Local line_read:String
	
	line_read = Input(prompt)
	
	Return line_read
End Function

Function usershell()

    While True
	Local cmdline:String = xmlShellReadline("> ")
	Local cur:String = ""
	Local nbargs:Int
	Local command:String = ""
	Local arg:String = ""
	Local argv:String[20]
	Local i:Int
	Local ret:Int
	Local ans:String
	
	If cmdline = Null Or cmdline = "" Then
	    Return
	End If

	'
	' Parse the command itself
	'
	cur = cmdline
	nbargs = 0
	Local pos:Int = 0
	Local char:String = cur[pos..pos+1]

	While pos < cur.length And (char = " " Or char = "~t")
		pos:+1
		If pos < cur.length Then
			char = cur[pos..pos+1]
		End If
	Wend

	i = 0
	If pos < cur.length Then
		char = cur[pos..pos+1]
	End If
	While pos < cur.length And (char <> " " And char <>  "~t" And ..
	       char <>  "~n" And char <>  "~r")

	    i:+1
	    command:+ char
		pos:+1
		If pos < cur.length Then
			char = cur[pos..pos+1]
		End If
	Wend
	'command[i] = 0;
	If i = 0 Then
		Continue
	End If
	nbargs:+1

	'
	' Parse the argument String
	'
	'memset(arg, 0, SizeOf(arg));
	While pos < cur.length And (char = " " Or char = "~t")
		pos:+1
		If pos < cur.length Then
			char = cur[pos..pos+1]
		End If
	Wend
	i = 0
	While pos < cur.length And (char <>  "~n" And char <>  "~r")
		i:+1
		arg:+ char
		pos:+1
		If pos < cur.length Then
			char = cur[pos..pos+1]
		End If
	Wend

	If i <> 0 Then
		nbargs:+1
	End If


	'
	' Parse the arguments
	'
	i = 0
	nbargs = 0
	cur = arg

	pos = 0
	'memset(argv, 0, SizeOf(argv));
	While pos < cur.length
		char = cur[pos..pos+1]
		While pos < cur.length And (char = " " Or char = "~t")
			pos:+1
			If pos < cur.length Then
				char = cur[pos..pos+1]
			End If
		Wend

		If char = "'" Then
			pos:+1
			If pos < cur.length Then
				char = cur[pos..pos+1]
			End If
			While pos < cur.length And char <>  "'"
				argv[i]:+ char
				pos:+1
				If pos < cur.length Then
					char = cur[pos..pos+1]
				End If
			Wend
			If char = "'" Then
				cur = cur[pos+1..]
				pos = 0
				nbargs:+1
				i:+1
			End If
		Else If char = "~q" Or char = Chr(63394) Then
			pos:+1
			If pos < cur.length Then
				char = cur[pos..pos+1]
			End If
			
			While pos < cur.length And char <>  "~q" And char <> Chr(63394)
				argv[i]:+ char
				pos:+1
				If pos < cur.length Then
					char = cur[pos..pos+1]
				End If
			Wend
			
			If char = "~q" Or char = Chr(63394) Then
				cur = cur[pos+1..]
				pos = 0
				nbargs:+1
				i:+1
			End If
		Else
			If pos < cur.length Then
				char = cur[pos..pos+1]
			End If
			While pos < cur.length And char <>  " " And char <> "~t"
				argv[i]:+ char
				pos:+1
				If pos < cur.length Then
					char = cur[pos..pos+1]
				End If
			Wend
			cur = cur[pos+1..]
			pos = 0
			nbargs:+1
			i:+1
		End If
	Wend

	'
	' start interpreting the command
	'
        If command = "exit" Then
	    Exit
	End If
        If command = "quit" Then
	    Exit
	End If
        If command = "bye" Then
	    Exit
	End If
	If command = "public" Then
	    If nbargs <> 1 Then
		Print "public requires 1 arguments"
	    Else
		ans = TxmlCatalog.defaultResolvePublic(argv[0])
		If ans = Null Then
		    Print "No entry for PUBLIC " + argv[0]
		Else
		    Print ans
		End If
	    End If
	Else If command = "system" Then
	    If nbargs <> 1 Then
		Print "system requires 1 arguments"
	    Else
		ans = TxmlCatalog.defaultResolveSystem(argv[0])
		If ans = Null Then
		    Print "No entry for SYSTEM " + argv[0]
		Else
		    Print ans
		End If
	    End If
	Else If command = "add" Then
	    If sgml Then
		If nbargs <> 3 And nbargs <> 2 Then
		    Print "add requires 2 or 3 arguments"
		Else
		    If nbargs = 2 Then
			ret = TxmlCatalog.addDefault(argv[0], Null, argv[1])
		    Else
			ret = TxmlCatalog.addDefault(argv[0], argv[1], argv[2])
			End If
		    If ret <> 0 Then
			Print "add command failed"
			End If
		End If
	    Else 
		If nbargs <> 3 And nbargs <> 2 Then
		    Print "add requires 2 or 3 arguments"
		Else
		    If nbargs = 2 Then
			ret = TxmlCatalog.addDefault(argv[0], Null, argv[1])
		    Else
			ret = TxmlCatalog.addDefault(argv[0], argv[1], argv[2])
			End If
		    If ret <> 0 Then
			Print "add command failed"
			End If
		End If
	    End If
	Else If command = "del" Then
	    If nbargs <> 1 Then
		Print "del requires 1 argument"
	    Else
		ret = TxmlCatalog.defaultRemove(argv[0])
		If ret <= 0 Then
		    Print "del command failed"
		End If
	    End If
	Else If command = "resolve" Then
	    If nbargs <> 2 Then
		Print "resolve requires 2 arguments"
	    Else
		ans = TxmlCatalog.defaultResolve(argv[0], argv[1])
		If ans = Null Then
		    Print "Resolver failed to find an answer"
		Else
		    Print ans
		End If
	    End If
	Else If command = "dump" Then
	    If nbargs <> 0 Then
		Print "dump has no arguments"
	    Else
		xmlCatalogDump(stdout_)
	    End If
	Else If command = "debug" Then
	    If nbargs <> 0 Then
		Print "debug has no arguments"
	    Else
		verbose:+1
		xmlCatalogSetDebug(verbose)
	    End If
	Else If command = "quiet" Then
	    If nbargs <> 0 Then
		Print "quiet has no arguments"
	    Else
		If verbose > 0 Then
		    verbose = 0
		End If
		xmlCatalogSetDebug(verbose)
	    End If
	Else
		If command <> "help" Then
			Print "Unrecognized command " + command
		End If
		Print "Commands available:"
		Print "~tpublic PublicID: make a PUBLIC identifier lookup"
		Print "~tsystem SystemID: make a SYSTEM identifier lookup"
		Print "~tresolve PublicID SystemID: do a full resolver lookup"
		Print "~tadd 'type' 'orig' 'replace' : add an entry"
		Print "~tdel 'values' : remove values"
		Print "~tdump: print the current catalog state"
		Print "~tdebug: increase the verbosity level"
		Print "~tquiet: decrease the verbosity level"
		Print "~texit:  quit the shell"
	End If
	'free(cmdline); ' Not xmlFree here ! */

	cmdline = Null
    Wend
End Function

' main

Local i:Int
Local ret:Int
Local exit_value:Int = 0

If appargs.length <= 1 Then
	usage()
	End
End If

For i = 1 Until AppArgs.length
	If AppArgs[i] = "-" Then
		Exit
	End If
	
	If AppArgs[i][..1] <> "-" Then
		Exit
	End If
	
	If AppArgs[i] = "-verbose" Or AppArgs[i] = "-v" Or AppArgs[i] = "--verbose" Then
		verbose = 1
		TxmlCatalog.setDebug(verbose)
	Else If AppArgs[i] = "-noout" Or AppArgs[i] = "--noout" Then
		noout = 1
	Else If AppArgs[i] = "-shell" Or AppArgs[i] = "--shell" Then
		shell = 1
		noout = 1
	Else If AppArgs[i] = "-sgml" Or AppArgs[i] = "--sgml" Then
		sgml = 1
	Else If AppArgs[i] = "-create" Or AppArgs[i] = "--create" Then
		_create = 1
	Else If AppArgs[i] = "-convert"Or AppArgs[i] = "--convert" Then
		convert = 1
	Else If AppArgs[i] = "-no-super-update" Or AppArgs[i] = "--no-super-update" Then
		no_super_update = 1
	Else If AppArgs[i] = "-add" Or AppArgs[i] = "--add" Then
		If sgml Then
			i :+ 2
		Else
			i :+ 3
		End If
		add = 1
	Else If AppArgs[i] = "-del" Or AppArgs[i] = "--del" Then
		i :+ 1
		del = 1
	Else
		Print "Unknown option " + AppArgs[i]
		usage()
		Exit
	End If
Next

For i = 1 Until AppArgs.length
	If AppArgs[i] = "-add" Or AppArgs[i] = "--add" Then
		If sgml Then
			i :+ 2
		Else
			i :+ 3
		End If
		Continue
	Else If AppArgs[i] = "-del" Or AppArgs[i] = "--del" Then
		i :+ 1
	
		' No catalog entry specified
		If i = AppArgs.length Or (sgml And i + 1 = AppArgs.length) Then
			Print "No catalog entry specified to remove from"
			usage()
			Exit
		End If
	
		Continue
	Else If AppArgs[i][..1] = "-" Then
		Continue
	End If
	
	filename = AppArgs[i]
	ret = xmlLoadCatalog(AppArgs[i])
	If ret < 0 And _create Then
		TxmlCatalog.addDefault("catalog", AppArgs[i], Null)
	End If
	Exit
Next

If convert Then
	ret = TxmlCatalog.convertDefault()
End If

If add Or del Then
	For i = 1 Until AppArgs.length
		If AppArgs[i] = "-" Then
			Exit
		End If

		If AppArgs[i][0..1] <> "-" Then
			Continue
		End If
		
		If AppArgs[i] <> "-add" And AppArgs[i] <> "--add" And ..
				AppArgs[i] <> "-del" And AppArgs[i] <> "--del" Then
			Continue
		End If

		If sgml Then
			'
			' Maintenance of SGML catalogs.
			'
			Local catal:TxmlCatalog = Null
			Local _super:TxmlCatalog = Null

			catal = TxmlCatalog.LoadSGMLSuperCatalog(AppArgs[i + 1])
	
			If AppArgs[i] = "-add" Or AppArgs[i] = "--add" Then
			    If catal = Null Then
				catal = TxmlCatalog.newCatalog(1)
				End If
			    catal.add("CATALOG", AppArgs[i + 2], Null)
	
			    If Not no_super_update Then
				_super = TxmlCatalog.loadSGMLSuperCatalog(XML_SGML_DEFAULT_CATALOG)
				If _super = Null Then
				    _super = TxmlCatalog.newCatalog(1)
	
				_super.add("CATALOG", AppArgs[i + 1], Null)
			    End If
			Else
			    If catal <> Null Then
				ret = catal.remove(AppArgs[i + 2])
			    Else
				ret = -1
				End If
			    If ret < 0 Then
				Print "Failed to remove entry from " + AppArgs[i + 1]
				exit_value = 1
			    End If
			    If Not no_super_update And noout And catal <> Null And catal.isEmpty() Then
				_super = TxmlCatalog.loadSGMLSuperCatalog(XML_SGML_DEFAULT_CATALOG)
				If _super <> Null Then
				    ret = _super.remove(AppArgs[i + 1])
				    If ret < 0 Then
					Print "Failed to remove entry from " + XML_SGML_DEFAULT_CATALOG
					exit_value = 1
				    End If
				End If
			    End If
			End If
			
			If noout Then
			Rem
			    FILE *out;
	
			    If (xmlCatalogIsEmpty(catal)) {
				remove(argv[i + 1]);
			    } Else {
				out = fopen(argv[i + 1], "w");
				If (out == Null) {
				    fprint stderr, "could not open %s for saving\n",
					    argv[i + 1]);
				    exit_value = 2;
				    noout = 0;
				} Else {
				    xmlACatalogDump(catal, out);
				    fclose(out);
				}
			    }
			    If (!no_super_update && Super != Null) {
				If (xmlCatalogIsEmpty(Super)) {
				    remove(XML_SGML_DEFAULT_CATALOG);
				} Else {
				    out = fopen(XML_SGML_DEFAULT_CATALOG, "w");
				    If (out == Null) {
					fprint stderr,
						"could not open %s for saving\n",
						XML_SGML_DEFAULT_CATALOG);
					exit_value = 2;
					noout = 0;
				    } Else {
					
					xmlACatalogDump(Super, out);
					fclose(out);
				    }
				}
			    }
			End Rem
			Else
			Print "DUMPING..."
			    catal.dump(stdout_)
			End If
			i:+ 2
		Else
		Rem
			If ((!strcmp(argv[i], "-add")) ||
			    (!strcmp(argv[i], "--add"))) {
				If ((argv[i + 3] == Null) || (argv[i + 3][0] == 0))
				    ret = xmlCatalogAdd(BAD_CAST argv[i + 1], Null,
							BAD_CAST argv[i + 2]);
				Else
				    ret = xmlCatalogAdd(BAD_CAST argv[i + 1],
							BAD_CAST argv[i + 2],
							BAD_CAST argv[i + 3]);
				If (ret != 0) {
				    print "add command failed"
				    exit_value = 3;
				}
				i += 3;
			} Else If ((!strcmp(argv[i], "-del")) ||
			    (!strcmp(argv[i], "--del"))) {
			    ret = xmlCatalogRemove(BAD_CAST argv[i + 1]);
			    If (ret < 0) {
				fprint stderr, "Failed to remove entry %s\n",
					argv[i + 1]);
				exit_value = 1;
			    }
			    i += 1;
			}
End Rem
		End If
	   End If
	Next
Else If shell Then
	usershell()
Else
	i:+ 1
	While i < AppArgs.length
		Local uri:TxmlURI
		Local ans:String
	    
		uri = TxmlURI.parseURI(AppArgs[i])
		If uri = Null Then
			ans = TxmlCatalog.DefaultResolvePublic(AppArgs[i])
			If ans = Null Then
				Print "No entry for PUBLIC " + AppArgs[i]
				exit_value = 4
			Else
				Print ans
				ans = Null
			End If
		Else
			uri.free()
			ans = TxmlCatalog.DefaultResolveSystem(AppArgs[i])
			If ans = Null Then
				Print "No entry for SYSTEM " + AppArgs[i]
				ans = TxmlCatalog.DefaultResolveURI(AppArgs[i])
				If ans = Null Then
					Print "No entry for URI " + AppArgs[i]
					exit_value = 4
				Else
					Print ans
					ans = Null
				End If
			Else
				Print ans
				ans = Null
			End If
		End If
		i:+1
	Wend
End If
Rem
    If Not sgml And (add Or del Or _create Or convert) Then
	If noout And filename And *filename Then
    FILE *out;

	    out = fopen(filename, "w");
	    If out = Null Then
		fprint stderr, "could not open %s for saving\n", filename);
		exit_value = 2
		noout = 0
	    Else
		xmlCatalogDump(out)
	    End If
	Else
	    xmlCatalogDump(stdout)
	End If
    End If
End Rem

'
' Cleanup And check For memory leaks
'
xmlCleanupParser()
'xmlMemoryDump()
End



