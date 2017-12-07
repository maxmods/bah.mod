' Copyright (c) 2006-2017 Bruce A Henderson
'
'  The contents of this file are subject to the Mozilla Public License
'  Version 1.1 (the "License"); you may not use this file except in
'  compliance with the License. You may obtain a copy of the License at
'  http://www.mozilla.org/MPL/
'  
'  Software distributed under the License is distributed on an "AS IS"
'  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
'  License for the specific language governing rights and limitations
'  under the License.
'  
'  The Original Code is BaH.Cairo.
'  
'  The Initial Developer of the Original Code is Duncan Cross.
'
SuperStrict

Rem
bbdoc: Pixman - Pixel manipulation
End Rem
Module BaH.Pixman

ModuleInfo "Version: 1.27"
ModuleInfo "License: MPL / LGPL"
ModuleInfo "Copyright: Pixman -  University of Southern California and Carl D. Worth"
ModuleInfo "Copyright: Wrapper - Bruce A Henderson, based on initial work by Duncan Cross."
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.27"
ModuleInfo "History: Pixman update to 0.33.6."
ModuleInfo "History: Extracted from BaH.Cairo into separate module."


ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"

Import "common.bmx"

