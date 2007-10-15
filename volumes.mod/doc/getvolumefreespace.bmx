SuperStrict

Framework bah.volumes
Import brl.standardio

Local vol:String

?win32
vol = "C:\"
?linux
vol = "/"
?macos
vol = "/"
?

Print "Freespace :"
Print vol + " = " + (GetVolumeFreeSpace(vol) / 1024) + " kb"
