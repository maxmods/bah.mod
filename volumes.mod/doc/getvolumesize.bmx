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

Print "Total space :"
Print vol + " = " + (GetVolumeSize(vol) / 1024) + " kb"
