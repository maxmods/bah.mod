' Copyright (c) 2009 Bruce A Henderson
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
'  The Original Code is BaH.TagLib.
'  
'  The Initial Developer of the Original Code is Bruce A Henderson.
'
SuperStrict

Import BRL.Blitz
Import Pub.zlib

Import "../../pub.mod/zlib.mod/*.h"

Import "src/toolkit/*.h"
Import "src/*.h"
Import "src/toolkit/*.h"
Import "src/mpeg/*.h"
Import "src/mpeg/id3v1/*.h"
Import "src/mpeg/id3v2/*.h"
Import "src/mpeg/id3v2/frames/*.h"
Import "src/ogg/*.h"
Import "src/ogg/flac/*.h"
Import "src/flac/*.h"
Import "src/mpc/*.h"
Import "src/ape/*.h"
Import "src/asf/*.h"
Import "src/mp4/*.h"
Import "src/ogg/vorbis/*.h"
Import "src/ogg/speex/*.h"
Import "src/wavpack/*.h"
Import "src/trueaudio/*.h"
Import "src/riff/*.h"
Import "src/riff/aiff/*.h"
Import "src/riff/wav/*.h"
Import "src/mpeg/id3v2/*.h"

Import "src/audioproperties.cpp"
Import "src/tagunion.cpp"
Import "src/fileref.cpp"
Import "src/tag.cpp"

Import "src/ape/apefooter.cpp"
Import "src/ape/apetag.cpp"
Import "src/ape/apeitem.cpp"

Import "src/asf/asfattribute.cpp"
Import "src/asf/asftag.cpp"
Import "src/asf/asffile.cpp"
Import "src/asf/asfproperties.cpp"

Import "src/flac/flacfile.cpp"
Import "src/flac/flacproperties.cpp"

Import "src/mp4/mp4atom.cpp"
Import "src/mp4/mp4tag.cpp"
Import "src/mp4/mp4file.cpp"
Import "src/mp4/mp4item.cpp"
Import "src/mp4/mp4properties.cpp"

Import "src/mpc/mpcfile.cpp"
Import "src/mpc/mpcproperties.cpp"

Import "src/mpeg/mpegfile.cpp"
Import "src/mpeg/xingheader.cpp"
Import "src/mpeg/mpegheader.cpp"
Import "src/mpeg/mpegproperties.cpp"

Import "src/mpeg/id3v1/id3v1tag.cpp"
Import "src/mpeg/id3v1/id3v1genres.cpp"

Import "src/mpeg/id3v2/id3v2extendedheader.cpp"
Import "src/mpeg/id3v2/id3v2tag.cpp"
Import "src/mpeg/id3v2/id3v2footer.cpp"
Import "src/mpeg/id3v2/id3v2frame.cpp"
Import "src/mpeg/id3v2/id3v2framefactory.cpp"
Import "src/mpeg/id3v2/id3v2header.cpp"
Import "src/mpeg/id3v2/id3v2synchdata.cpp"

Import "src/mpeg/id3v2/frames/attachedpictureframe.cpp"
Import "src/mpeg/id3v2/frames/urllinkframe.cpp"
Import "src/mpeg/id3v2/frames/commentsframe.cpp"
Import "src/mpeg/id3v2/frames/generalencapsulatedobjectframe.cpp"
Import "src/mpeg/id3v2/frames/popularimeterframe.cpp"
Import "src/mpeg/id3v2/frames/privateframe.cpp"
Import "src/mpeg/id3v2/frames/relativevolumeframe.cpp"
Import "src/mpeg/id3v2/frames/textidentificationframe.cpp"
Import "src/mpeg/id3v2/frames/uniquefileidentifierframe.cpp"
Import "src/mpeg/id3v2/frames/unknownframe.cpp"
Import "src/mpeg/id3v2/frames/unsynchronizedlyricsframe.cpp"

Import "src/ogg/xiphcomment.cpp"
Import "src/ogg/oggpage.cpp"
Import "src/ogg/oggpageheader.cpp"
Import "src/ogg/oggfile.cpp"

Import "src/ogg/flac/oggflacfile.cpp"

Import "src/ogg/speex/speexfile.cpp"
Import "src/ogg/speex/speexproperties.cpp"

Import "src/ogg/vorbis/vorbisfile.cpp"
Import "src/ogg/vorbis/vorbisproperties.cpp"

Import "src/riff/rifffile.cpp"

Import "src/riff/aiff/aifffile.cpp"
Import "src/riff/aiff/aiffproperties.cpp"

Import "src/riff/wav/wavfile.cpp"
Import "src/riff/wav/wavproperties.cpp"

Import "src/toolkit/tbytevector.cpp"
Import "src/toolkit/unicode.cpp"
Import "src/toolkit/tbytevectorlist.cpp"
Import "src/toolkit/tdebug.cpp"
Import "src/toolkit/tfile.cpp"
Import "src/toolkit/tstring.cpp"
Import "src/toolkit/tstringlist.cpp"

Import "src/trueaudio/trueaudiofile.cpp"
Import "src/trueaudio/trueaudioproperties.cpp"

Import "src/wavpack/wavpackproperties.cpp"
Import "src/wavpack/wavpackfile.cpp"


Import "glue.cpp"

