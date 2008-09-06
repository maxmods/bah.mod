SET ORACLE_INCLUDES=C:\stuff\instantclient-sdk-win32-11.1.0.6.0\instantclient_11_1\sdk\include
SET ORACLE_LIBS=C:\stuff\instantclient-basic-win32-11.1.0.6.0\instantclient_11_1

gcc -shared -I. -I%ORACLE_INCLUDES% -L%ORACLE_LIBS% -DOCI_CHARSET_MIXED -loci ocilib.c main.c -o ..\lib\win32\ocilibm.dll

