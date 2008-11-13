Rem DLL build script form Win32
Rem 
Rem 
Rem SET ORACLE_INCLUDES=C:\stuff\instantclient-sdk-win32-11.1.0.6.0\instantclient_11_1\sdk\include
Rem SET ORACLE_LIBS=C:\stuff\instantclient-basic-win32-11.1.0.6.0\instantclient_11_1
SET ORACLE_INCLUDES=C:\000_programming\c\oracle\instantclient_11_1\sdk\include
SET ORACLE_LIBS=C:\000_programming\c\oracle\instantclient_11_1

gcc -shared -I. -I..\include -I%ORACLE_INCLUDES% -L%ORACLE_LIBS% -DOCI_CHARSET_MIXED -loci callback.c collection.c column.c connection.c connpool.c date.c define.c element.c error.c exception.c file.c format.c handle.c hash.c interval.c iterator.c library.c list.c lob.c long.c memory.c mutex.c number.c object.c resultset.c schema.c statement.c string.c thread.c threadkey.c timestamp.c transaction.c main.c -o ..\lib\win32\ocilibm.dll
