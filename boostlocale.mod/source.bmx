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

?


Import "glue.cpp"

