' Copyright (c) 2007-2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import "src/include/*.h"
Import "src/iodbcinst/*.h"
Import "src/iodbcadm/*.h"
Import "src/iodbc/*.h"
Import "src/iodbc/trace/*.h"

Import "src/iodbcinst/SQLGetConfigMode.c"
Import "src/iodbcinst/SQLGetPrivateProfileString.c"
Import "src/iodbcinst/SQLSetConfigMode.c"
Import "src/iodbcinst/SQLValidDSN.c"
Import "src/iodbcinst/SQLWritePrivateProfileString.c"
Import "src/iodbcinst/SQLReadFileDSN.c"
Import "src/iodbcinst/SQLWriteFileDSN.c"
Import "src/iodbcinst/dlf.c"
Import "src/iodbcinst/inifile.c"
Import "src/iodbcinst/iodbc_error.c"
Import "src/iodbcinst/misc.c"
Import "src/iodbcinst/unicode.c"

Import "src/iodbcinst/SQLConfigDataSource.c"
Import "src/iodbcinst/SQLConfigDriver.c"
Import "src/iodbcinst/SQLGetAvailableDrivers.c"
Import "src/iodbcinst/SQLGetInstalledDrivers.c"
Import "src/iodbcinst/SQLInstallDriver.c"
Import "src/iodbcinst/SQLRemoveDSNFromIni.c"
Import "src/iodbcinst/SQLRemoveDriver.c"
Import "src/iodbcinst/SQLWriteDSNToIni.c"
Import "src/iodbcinst/SQLInstallDriverEx.c"
Import "src/iodbcinst/SQLInstallODBC.c"
Import "src/iodbcinst/SQLInstallTranslator.c"
Import "src/iodbcinst/SQLCreateDataSource.c"
Import "src/iodbcinst/SQLManageDataSource.c"
Import "src/iodbcinst/SQLRemoveTranslator.c"
Import "src/iodbcinst/SQLRemoveDefaultDataSource.c"
Import "src/iodbcinst/SQLInstallDriverManager.c"
Import "src/iodbcinst/SQLRemoveDriverManager.c"
Import "src/iodbcinst/SQLInstallTranslatorEx.c"
Import "src/iodbcinst/SQLInstallerError.c"
Import "src/iodbcinst/SQLPostInstallerError.c"
Import "src/iodbcinst/SQLGetTranslator.c"
Import "src/iodbcinst/Info.c"

Import "src/iodbc/trace/AllocConnect.c"
Import "src/iodbc/trace/AllocEnv.c"
Import "src/iodbc/trace/AllocHandle.c"
Import "src/iodbc/trace/AllocStmt.c"
Import "src/iodbc/trace/BindCol.c"
Import "src/iodbc/trace/BindParameter.c"
Import "src/iodbc/trace/BrowseConnect.c"
Import "src/iodbc/trace/BulkOperations.c"
Import "src/iodbc/trace/Cancel.c"
Import "src/iodbc/trace/CloseCursor.c"
Import "src/iodbc/trace/ColAttribute.c"
Import "src/iodbc/trace/ColumnPrivileges.c"
Import "src/iodbc/trace/Columns.c"
Import "src/iodbc/trace/Connect.c"
Import "src/iodbc/trace/CopyDesc.c"
Import "src/iodbc/trace/DataSources.c"
Import "src/iodbc/trace/DescribeCol.c"
Import "src/iodbc/trace/DescribeParam.c"
Import "src/iodbc/trace/Disconnect.c"
Import "src/iodbc/trace/DriverConnect.c"
Import "src/iodbc/trace/Drivers.c"
Import "src/iodbc/trace/EndTran.c"
Import "src/iodbc/trace/Error.c"
Import "src/iodbc/trace/ExecDirect.c"
Import "src/iodbc/trace/Execute.c"
Import "src/iodbc/trace/ExtendedFetch.c"
Import "src/iodbc/trace/Fetch.c"
Import "src/iodbc/trace/FetchScroll.c"
Import "src/iodbc/trace/ForeignKeys.c"
Import "src/iodbc/trace/FreeConnect.c"
Import "src/iodbc/trace/FreeEnv.c"
Import "src/iodbc/trace/FreeHandle.c"
Import "src/iodbc/trace/FreeStmt.c"
Import "src/iodbc/trace/GetConnectAttr.c"
Import "src/iodbc/trace/GetConnectOption.c"
Import "src/iodbc/trace/GetCursorName.c"
Import "src/iodbc/trace/GetData.c"
Import "src/iodbc/trace/GetDescField.c"
Import "src/iodbc/trace/GetDescRec.c"
Import "src/iodbc/trace/GetDiagField.c"
Import "src/iodbc/trace/GetDiagRec.c"
Import "src/iodbc/trace/GetEnvAttr.c"
Import "src/iodbc/trace/GetFunctions.c"
Import "src/iodbc/trace/GetStmtAttr.c"
Import "src/iodbc/trace/GetStmtOption.c"
Import "src/iodbc/trace/GetTypeInfo.c"
Import "src/iodbc/trace/bmx_Info.c" ' note : name change! (because of object name clashing)
Import "src/iodbc/trace/MoreResults.c"
Import "src/iodbc/trace/NativeSql.c"
Import "src/iodbc/trace/NumParams.c"
Import "src/iodbc/trace/NumResultCols.c"
Import "src/iodbc/trace/ParamData.c"
Import "src/iodbc/trace/ParamOptions.c"
Import "src/iodbc/trace/Prepare.c"
Import "src/iodbc/trace/PrimaryKeys.c"
Import "src/iodbc/trace/ProcedureColumns.c"
Import "src/iodbc/trace/Procedures.c"
Import "src/iodbc/trace/PutData.c"
Import "src/iodbc/trace/RowCount.c"
Import "src/iodbc/trace/SetConnectAttr.c"
Import "src/iodbc/trace/SetConnectOption.c"
Import "src/iodbc/trace/SetCursorName.c"
Import "src/iodbc/trace/SetDescField.c"
Import "src/iodbc/trace/SetDescRec.c"
Import "src/iodbc/trace/SetEnvAttr.c"
Import "src/iodbc/trace/SetPos.c"
Import "src/iodbc/trace/SetScrollOptions.c"
Import "src/iodbc/trace/SetStmtAttr.c"
Import "src/iodbc/trace/SetStmtOption.c"
Import "src/iodbc/trace/SpecialColumns.c"
Import "src/iodbc/trace/Statistics.c"
Import "src/iodbc/trace/TablePrivileges.c"
Import "src/iodbc/trace/Tables.c"
Import "src/iodbc/trace/Transact.c"
Import "src/iodbc/trace/trace.c"

Import "src/iodbc/bmx_misc.c" ' note : name change! (because of object name clashing)
Import "src/iodbc/catalog.c"
Import "src/iodbc/connect.c"
Import "src/iodbc/dlproc.c"
Import "src/iodbc/execute.c"
Import "src/iodbc/fetch.c"
Import "src/iodbc/hdbc.c"
Import "src/iodbc/henv.c"
Import "src/iodbc/herr.c"
Import "src/iodbc/hstmt.c"
Import "src/iodbc/info.c"
Import "src/iodbc/prepare.c"
Import "src/iodbc/result.c"
Import "src/iodbc/odbc3.c"

