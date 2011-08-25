Rem DLL build script for Win32
Rem 
Rem 
Rem SET ORACLE_INCLUDES=C:\000_programming\instantclient_11_2x\sdk\include
Rem SET ORACLE_LIBS=C:\000_programming\instantclient_11_2x
SET ORACLE_INCLUDES=C:\000_programming\instantclient_11_2\sdk\include
SET ORACLE_LIBS=C:\000_programming\instantclient_11_2

gcc -shared -I. -I..\include -I%ORACLE_INCLUDES% -L%ORACLE_LIBS% -DOCI_CHARSET_MIXED -loci agent.c array.c bind.c callback.c collection.c column.c connection.c date.c define.c dequeue.c dirpath.c element.c enqueue.c error.c event.c exception.c file.c format.c handle.c hash.c interval.c iterator.c library.c list.c lob.c long.c memory.c msg.c mutex.c number.c object.c pool.c queue.c ref.c resultset.c statement.c string.c subscription.c thread.c threadkey.c timestamp.c transaction.c typeinfo.c main.c -o ..\lib\win32\ocilibm.dll
