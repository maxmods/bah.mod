/*
  Copyright (c) 2007-2019 Bruce A Henderson
  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the auther nor the names of its contributors may be used to 
        endorse or promote products derived from this software without specific
        prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#import <sqlite3.h>
#import <brl.mod/blitz.mod/blitz.h>

// this is a hack, because defining a function in Max which returns Long, breaks.
// So instead we pass in a Long Ptr, and populate that.
void bmx_sqlite3_column_int64(sqlite3_stmt * handle , int index, BBInt64 * value) {
	 *value = sqlite3_column_int64(handle, index);
}

void bmx_sqlite3_last_insert_rowid(sqlite3 * handle , BBInt64 * value) {
	*value = sqlite3_last_insert_rowid(handle);
}


int bmx_sqlite3_bind_text64(sqlite3_stmt * handle , int index, const char * value, BBInt64 size, int how) {
	return sqlite3_bind_text64(handle, index, value, size, (int)how, SQLITE_UTF8);
}

int bmx_sqlite3_bind_blob64(sqlite3_stmt * handle , int index, const void * value, BBInt64 size, int how) {
	return sqlite3_bind_blob64(handle, index, value, size, (int)how);
}
