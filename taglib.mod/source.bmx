' Copyright (c) 2009-2010 Bruce A Henderson
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

Import "include/*.h"

Import "taglib/audioproperties.cpp"
Import "taglib/tagunion.cpp"
Import "taglib/fileref.cpp"
Import "taglib/tag.cpp"

Import "taglib/ape/apefooter.cpp"
Import "taglib/ape/apetag.cpp"
Import "taglib/ape/apeitem.cpp"

Import "taglib/asf/asfattribute.cpp"
Import "taglib/asf/asftag.cpp"
Import "taglib/asf/asffile.cpp"
Import "taglib/asf/asfproperties.cpp"

Import "taglib/flac/flacfile.cpp"
Import "taglib/flac/flacproperties.cpp"

Import "taglib/mp4/mp4atom.cpp"
Import "taglib/mp4/mp4tag.cpp"
Import "taglib/mp4/mp4file.cpp"
Import "taglib/mp4/mp4item.cpp"
Import "taglib/mp4/mp4properties.cpp"
Import "taglib/mp4/mp4coverart.cpp"

Import "taglib/mpc/mpcfile.cpp"
Import "taglib/mpc/mpcproperties.cpp"

Import "taglib/mpeg/mpegfile.cpp"
Import "taglib/mpeg/xingheader.cpp"
Import "taglib/mpeg/mpegheader.cpp"
Import "taglib/mpeg/mpegproperties.cpp"

Import "taglib/mpeg/id3v1/id3v1tag.cpp"
Import "taglib/mpeg/id3v1/id3v1genres.cpp"

Import "taglib/mpeg/id3v2/id3v2extendedheader.cpp"
Import "taglib/mpeg/id3v2/id3v2tag.cpp"
Import "taglib/mpeg/id3v2/id3v2footer.cpp"
Import "taglib/mpeg/id3v2/id3v2frame.cpp"
Import "taglib/mpeg/id3v2/id3v2framefactory.cpp"
Import "taglib/mpeg/id3v2/id3v2header.cpp"
Import "taglib/mpeg/id3v2/id3v2synchdata.cpp"

Import "taglib/mpeg/id3v2/frames/attachedpictureframe.cpp"
Import "taglib/mpeg/id3v2/frames/urllinkframe.cpp"
Import "taglib/mpeg/id3v2/frames/commentsframe.cpp"
Import "taglib/mpeg/id3v2/frames/generalencapsulatedobjectframe.cpp"
Import "taglib/mpeg/id3v2/frames/popularimeterframe.cpp"
Import "taglib/mpeg/id3v2/frames/privateframe.cpp"
Import "taglib/mpeg/id3v2/frames/relativevolumeframe.cpp"
Import "taglib/mpeg/id3v2/frames/textidentificationframe.cpp"
Import "taglib/mpeg/id3v2/frames/uniquefileidentifierframe.cpp"
Import "taglib/mpeg/id3v2/frames/unknownframe.cpp"
Import "taglib/mpeg/id3v2/frames/unsynchronizedlyricsframe.cpp"

Import "taglib/ogg/xiphcomment.cpp"
Import "taglib/ogg/oggpage.cpp"
Import "taglib/ogg/oggpageheader.cpp"
Import "taglib/ogg/oggfile.cpp"

Import "taglib/ogg/flac/oggflacfile.cpp"

Import "taglib/ogg/speex/speexfile.cpp"
Import "taglib/ogg/speex/speexproperties.cpp"

Import "taglib/ogg/vorbis/vorbisfile.cpp"
Import "taglib/ogg/vorbis/vorbisproperties.cpp"

Import "taglib/riff/rifffile.cpp"

Import "taglib/riff/aiff/aifffile.cpp"
Import "taglib/riff/aiff/aiffproperties.cpp"

Import "taglib/riff/wav/wavfile.cpp"
Import "taglib/riff/wav/wavproperties.cpp"

Import "taglib/toolkit/tbytevector.cpp"
Import "taglib/toolkit/unicode.cpp"
Import "taglib/toolkit/tbytevectorlist.cpp"
Import "taglib/toolkit/tdebug.cpp"
Import "taglib/toolkit/tfile.cpp"
Import "taglib/toolkit/tstring.cpp"
Import "taglib/toolkit/tstringlist.cpp"

Import "taglib/trueaudio/trueaudiofile.cpp"
Import "taglib/trueaudio/trueaudioproperties.cpp"

Import "taglib/wavpack/wavpackproperties.cpp"
Import "taglib/wavpack/wavpackfile.cpp"


Import "glue.cpp"

