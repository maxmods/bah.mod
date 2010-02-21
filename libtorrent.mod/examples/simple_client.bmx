SuperStrict 

Framework BaH.Libtorrent
Import BRL.StandardIO

Local session:TSession = New TSession
session.listenOn(6881, 6889)

Local params:TAddTorrentParams = New TAddTorrentParams
params.setSavePath("./")
params.setTorrentInfo(TTorrentInfo.Load("artofwar.torrent"))

session.addTorrent(params)

' wait for the user to end..

Input "enter to end..."
