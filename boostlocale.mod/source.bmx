' Copyright (c) 2013 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict


Import BRL.Blitz

Import "../boost.mod/src/*.hpp"


Import "../boost.mod/src/libs/locale/src/encoding/codepage.cpp"

Import "../boost.mod/src/libs/locale/src/shared/date_time.cpp"
Import "../boost.mod/src/libs/locale/src/shared/format.cpp"
Import "../boost.mod/src/libs/locale/src/shared/formatting.cpp"
Import "../boost.mod/src/libs/locale/src/shared/generator.cpp"
Import "../boost.mod/src/libs/locale/src/shared/ids.cpp"
Import "../boost.mod/src/libs/locale/src/shared/localization_backend.cpp"
Import "../boost.mod/src/libs/locale/src/shared/message.cpp"
Import "../boost.mod/src/libs/locale/src/shared/mo_lambda.cpp"

Import "../boost.mod/src/libs/locale/src/util/codecvt_converter.cpp"
Import "../boost.mod/src/libs/locale/src/util/default_locale.cpp"
Import "../boost.mod/src/libs/locale/src/util/gregorian.cpp"
Import "../boost.mod/src/libs/locale/src/util/info.cpp"
Import "../boost.mod/src/libs/locale/src/util/locale_data.cpp"


?Not Win32
Import "../boost.mod/src/libs/locale/src/posix/codecvt.cpp"
Import "../boost.mod/src/libs/locale/src/posix/collate.cpp"
Import "../boost.mod/src/libs/locale/src/posix/converter.cpp"
Import "../boost.mod/src/libs/locale/src/posix/numeric.cpp"
Import "../boost.mod/src/libs/locale/src/posix/posix_backend.cpp"


?Win32
Import "../boost.mod/src/libs/locale/src/win32/collate.cpp"
Import "../boost.mod/src/libs/locale/src/win32/converter.cpp"
Import "../boost.mod/src/libs/locale/src/win32/lcid.cpp"
Import "../boost.mod/src/libs/locale/src/win32/numeric.cpp"
Import "../boost.mod/src/libs/locale/src/win32/win_backend.cpp"
?


Import "glue.cpp"

