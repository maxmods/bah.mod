' requestfile.bmx

Framework BaH.gtkmaxgui

filter$="Image Files:png,jpg,bmp;Text Files:txt;BlitzMax Files:bmx;All Files:*"
filename$=RequestFile( "Select graphic file to open",filter$ )

Print filename
