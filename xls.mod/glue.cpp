/*
  Copyright (c) 2013 Bruce A Henderson
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "ExcelFormat.h"

class MaxExcelFont;
class MaxCellFormat;

extern "C" {

#include "blitz.h"

	YExcel::BasicExcel * bmx_xls_basicexcel_CreateFromFile(BBString * filename);
	YExcel::BasicExcel * bmx_xls_basicexcel_Create();

	void bmx_xls_basicexcel_New(YExcel::BasicExcel * xls, int sheets);
	int bmx_xls_basicexcel_Save(YExcel::BasicExcel * xls);
	int bmx_xls_basicexcel_SaveAs(YExcel::BasicExcel * xls, BBString * filename);
	void bmx_xls_basicexcel_Close(YExcel::BasicExcel * xls);

	int bmx_xls_basicexcel_GetTotalWorkSheets(YExcel::BasicExcel * xls);
	YExcel::BasicExcelWorksheet * bmx_xls_basicexcel_GetWorksheet(YExcel::BasicExcel * xls, int index);
	YExcel::BasicExcelWorksheet * bmx_xls_basicexcel_GetWorksheetByName(YExcel::BasicExcel * xls, BBString * name);
	YExcel::BasicExcelWorksheet * bmx_xls_basicexcel_AddWorksheet(YExcel::BasicExcel * xls, int index);
	YExcel::BasicExcelWorksheet * bmx_xls_basicexcel_AddWorksheetByName(YExcel::BasicExcel * xls, BBString * name, int index);

	int bmx_xls_basicexcelworksheet_GetTotalRows(YExcel::BasicExcelWorksheet * sheet);
	int bmx_xls_basicexcelworksheet_GetTotalCols(YExcel::BasicExcelWorksheet * sheet);
	YExcel::BasicExcelCell * bmx_xls_basicexcelworksheet_Cell(YExcel::BasicExcelWorksheet * sheet, int row, int col);
	int bmx_xls_basicexcelworksheet_EraseCell(YExcel::BasicExcelWorksheet * sheet, int row, int col);

	void bmx_xls_basicexcelcell_Set(YExcel::BasicExcelCell * cell, int value);
	void bmx_xls_basicexcelcell_SetDouble(YExcel::BasicExcelCell * cell, double value);
	void bmx_xls_basicexcelcell_SetText(YExcel::BasicExcelCell * cell, BBString * value);
	void bmx_xls_basicexcelcell_EraseContents(YExcel::BasicExcelCell * cell);
	int bmx_xls_basicexcelcell_Get(YExcel::BasicExcelCell * cell);
	double bmx_xls_basicexcelcell_GetDouble(YExcel::BasicExcelCell * cell);
	BBString * bmx_xls_basicexcelcell_GetText(YExcel::BasicExcelCell * cell);
	int bmx_xls_basicexcelcell_Type(YExcel::BasicExcelCell * cell);
	int bmx_xls_basicexcelcell_GetMergedRows(YExcel::BasicExcelCell * cell);
	int bmx_xls_basicexcelcell_GetMergedColumns(YExcel::BasicExcelCell * cell);
	void bmx_xls_basicexcelcell_SetMergedRows(YExcel::BasicExcelCell * cell, int rows);
	void bmx_xls_basicexcelcell_SetMergedColumns(YExcel::BasicExcelCell * cell, int cols);
	void bmx_xls_basicexcelcell_SetFormat(YExcel::BasicExcelCell * cell, MaxCellFormat * format);

	MaxExcelFont * bmx_xls_excelfont_create();
	void bmx_xls_excelfont_free(MaxExcelFont * font);
	void bmx_xls_excelfont_SetWeight(MaxExcelFont * font, int weight);
	void bmx_xls_excelfont_SetHeight(MaxExcelFont * font, int height);
	void bmx_xls_excelfont_SetItalic(MaxExcelFont * font, int italic);
	void bmx_xls_excelfont_SetColorIndex(MaxExcelFont * font, int index);
	void bmx_xls_excelfont_SetUnderlineType(MaxExcelFont * font, int underline);
	void bmx_xls_excelfont_SetEscapement(MaxExcelFont * font, int escapement);
	void bmx_xls_excelfont_SetFontName(MaxExcelFont * font, BBString * name);
	void bmx_xls_excelfont_SetOptions(MaxExcelFont * font, int options);
	int bmx_xls_excelfont_GetOptions(MaxExcelFont * font);

	ExcelFormat::XLSFormatManager * bmx_xls_xlsformatmanager_create(YExcel::BasicExcel * xls);
	void bmx_xls_xlsformatmanager_free(ExcelFormat::XLSFormatManager * manager);

	MaxCellFormat * bmx_xls_cellformat_create(ExcelFormat::XLSFormatManager * manager);
	void bmx_xls_cellformat_SetFont(MaxCellFormat * format, MaxExcelFont * font);
	void bmx_xls_cellformat_free(MaxCellFormat * format);
	MaxExcelFont * bmx_xls_cellformat_GetFont(MaxCellFormat * format);
	void bmx_xls_cellformat_SetFormatString(MaxCellFormat * format, BBString * text);
	BBString * bmx_xls_cellformat_GetFormatString(MaxCellFormat * format);
	void bmx_xls_cellformat_SetRotation(MaxCellFormat * format, int rotation);
	int bmx_xls_cellformat_GetRotation(MaxCellFormat * format);

}

// ********************************************************

#ifdef WIN32
#define WCHAR_FREE(s) bbMemFree(s)
#else
#define WCHAR_FREE(s) free(s)
#endif

wchar_t * bbStringToWchar_t(BBString * text) {
#ifdef WIN32
	return (wchar_t*)bbStringToWString(text);
#else
	if (text->length > 0) {
		char * s = bbStringToUTF8String(text);
		int len = strlen(s) + 1;
		wchar_t * ws = (wchar_t*)malloc(len * sizeof(wchar_t*));

		mbstowcs(ws, s, len);

		bbMemFree(s);

		return ws;
	} else {
		return 0;
	}
#endif
}

BBString * bbStringFromWchar_t(const wchar_t * text) {
	if (!text) {
		return &bbEmptyString;
	}
#ifdef WIN32
	return bbStringFromWString((BBChar*)text);
#else
	int len = wcslen(text) + 1;
	char * s = (char*)malloc(len * 3);
	wcstombs(s, text, len);

	BBString * txt = bbStringFromUTF8String(s);

	free(s);

	return txt;
#endif
}

// ********************************************************

class MaxExcelFont
{
public:
	MaxExcelFont() {}

	MaxExcelFont(ExcelFormat::ExcelFont & f) : font(f) {}

	~MaxExcelFont() {}

	ExcelFormat::ExcelFont & Font() {
		return font;
	}

private:
	ExcelFormat::ExcelFont font;

};

// ********************************************************

class MaxCellFormat
{
public:
	MaxCellFormat(ExcelFormat::XLSFormatManager & manager) : format(manager) {}
	
	MaxCellFormat(ExcelFormat::CellFormat & f) : format(f) {}

	~MaxCellFormat() {}

	ExcelFormat::CellFormat & Format() {
		return format;
	}

private:
	ExcelFormat::CellFormat format;
};

// ********************************************************

YExcel::BasicExcel * bmx_xls_basicexcel_CreateFromFile(BBString * filename) {
	char * s = bbStringToUTF8String(filename);
	YExcel::BasicExcel * xls = new YExcel::BasicExcel(s);
	bbMemFree(s);
	return xls;
}

YExcel::BasicExcel * bmx_xls_basicexcel_Create() {
	return new YExcel::BasicExcel();
}

void bmx_xls_basicexcel_New(YExcel::BasicExcel * xls, int sheets) {
	xls->New(sheets);
}

int bmx_xls_basicexcel_Save(YExcel::BasicExcel * xls) {
	return static_cast<int>(xls->Save());
}

int bmx_xls_basicexcel_SaveAs(YExcel::BasicExcel * xls, BBString * filename) {
	wchar_t * s = bbStringToWchar_t(filename);
	int result = static_cast<int>(xls->SaveAs(s));
	WCHAR_FREE(s);
}

void bmx_xls_basicexcel_Close(YExcel::BasicExcel * xls) {
	xls->Close();
}

int bmx_xls_basicexcel_GetTotalWorkSheets(YExcel::BasicExcel * xls) {
	return xls->GetTotalWorkSheets();
}

YExcel::BasicExcelWorksheet * bmx_xls_basicexcel_GetWorksheet(YExcel::BasicExcel * xls, int index) {
	return xls->GetWorksheet(index);
}

YExcel::BasicExcelWorksheet * bmx_xls_basicexcel_GetWorksheetByName(YExcel::BasicExcel * xls, BBString * name) {
	YExcel::BasicExcelWorksheet * sheet = 0;

	// Try ANSI first...	
	char * s = bbStringToUTF8String(name);
	sheet = xls->GetWorksheet(s);
	bbMemFree(s);
	
	if (sheet) {
		return sheet;
	}

	// Try Wide...
	wchar_t * ws = bbStringToWchar_t(name);
	sheet = xls->GetWorksheet(ws);
	WCHAR_FREE(ws);
	
	return sheet;
}

YExcel::BasicExcelWorksheet * bmx_xls_basicexcel_AddWorksheet(YExcel::BasicExcel * xls, int index) {
	return xls->AddWorksheet(index);
}

YExcel::BasicExcelWorksheet * bmx_xls_basicexcel_AddWorksheetByName(YExcel::BasicExcel * xls, BBString * name, int index) {
	wchar_t * s = bbStringToWchar_t(name);
	YExcel::BasicExcelWorksheet * sheet = xls->AddWorksheet(s, index);
	WCHAR_FREE(s);
	return sheet;
}

// ********************************************************

int bmx_xls_basicexcelworksheet_GetTotalRows(YExcel::BasicExcelWorksheet * sheet) {
	return sheet->GetTotalRows();
}

int bmx_xls_basicexcelworksheet_GetTotalCols(YExcel::BasicExcelWorksheet * sheet) {
	return sheet->GetTotalCols();
}

YExcel::BasicExcelCell * bmx_xls_basicexcelworksheet_Cell(YExcel::BasicExcelWorksheet * sheet, int row, int col) {
	return sheet->Cell(row, col);
}

int bmx_xls_basicexcelworksheet_EraseCell(YExcel::BasicExcelWorksheet * sheet, int row, int col) {
	return static_cast<int>(sheet->EraseCell(row, col));
}

// ********************************************************

void bmx_xls_basicexcelcell_Set(YExcel::BasicExcelCell * cell, int value) {
	cell->Set(value);
}

void bmx_xls_basicexcelcell_SetDouble(YExcel::BasicExcelCell * cell, double value) {
	cell->Set(value);
}

void bmx_xls_basicexcelcell_SetText(YExcel::BasicExcelCell * cell, BBString * value) {
	wchar_t * s = bbStringToWchar_t(value);
	cell->Set(s);
	WCHAR_FREE(s);
}

void bmx_xls_basicexcelcell_EraseContents(YExcel::BasicExcelCell * cell) {
	cell->EraseContents();
}

int bmx_xls_basicexcelcell_Get(YExcel::BasicExcelCell * cell) {
	return cell->GetInteger();
}

double bmx_xls_basicexcelcell_GetDouble(YExcel::BasicExcelCell * cell) {
	return cell->GetDouble();
}

BBString * bmx_xls_basicexcelcell_GetText(YExcel::BasicExcelCell * cell) {
	const char * s = cell->GetString();
	if (s) {
		return bbStringFromCString(s);
	}
	
	return bbStringFromWchar_t(cell->GetWString());
}

int bmx_xls_basicexcelcell_Type(YExcel::BasicExcelCell * cell) {
	int type = cell->Type();
	if (type == YExcel::BasicExcelCell::WSTRING) {
		return YExcel::BasicExcelCell::STRING;
	}
	return type;
}

int bmx_xls_basicexcelcell_GetMergedRows(YExcel::BasicExcelCell * cell) {
	return static_cast<int>(cell->GetMergedRows());
}

int bmx_xls_basicexcelcell_GetMergedColumns(YExcel::BasicExcelCell * cell) {
	return static_cast<int>(cell->GetMergedColumns());
}

void bmx_xls_basicexcelcell_SetMergedRows(YExcel::BasicExcelCell * cell, int rows) {
	cell->SetMergedRows(static_cast<USHORT>(rows));
}

void bmx_xls_basicexcelcell_SetMergedColumns(YExcel::BasicExcelCell * cell, int cols) {
	cell->SetMergedColumns(static_cast<USHORT>(cols));
}

void bmx_xls_basicexcelcell_SetFormat(YExcel::BasicExcelCell * cell, MaxCellFormat * format) {
	cell->SetFormat(format->Format());
}

// ********************************************************

MaxExcelFont * bmx_xls_excelfont_create() {
	return new MaxExcelFont();
}

void bmx_xls_excelfont_free(MaxExcelFont * font) {
	delete font;
}

void bmx_xls_excelfont_SetWeight(MaxExcelFont * font, int weight) {
	font->Font()._weight = static_cast<short>(weight);
}

void bmx_xls_excelfont_SetHeight(MaxExcelFont * font, int height) {
	font->Font()._height = static_cast<short>(height);
}

void bmx_xls_excelfont_SetItalic(MaxExcelFont * font, int italic) {
	font->Font().set_italic(static_cast<bool>(italic));
}

void bmx_xls_excelfont_SetColorIndex(MaxExcelFont * font, int index) {
	font->Font()._color_index = static_cast<short>(index);
}

void bmx_xls_excelfont_SetUnderlineType(MaxExcelFont * font, int underline) {
	font->Font()._underline_type = static_cast<char>(underline);
}

void bmx_xls_excelfont_SetEscapement(MaxExcelFont * font, int escapement) {
	font->Font()._escapement_type = static_cast<short>(escapement);
}

void bmx_xls_excelfont_SetFontName(MaxExcelFont * font, BBString * name) {
	wchar_t * s = bbStringToWchar_t(name);
	font->Font()._name = s;
	WCHAR_FREE(s);
}

void bmx_xls_excelfont_SetOptions(MaxExcelFont * font, int options) {
	font->Font()._options = static_cast<short>(options);
}

int bmx_xls_excelfont_GetOptions(MaxExcelFont * font) {
	return static_cast<int>(font->Font()._options);
}

// ********************************************************

ExcelFormat::XLSFormatManager * bmx_xls_xlsformatmanager_create(YExcel::BasicExcel * xls) {
	return new ExcelFormat::XLSFormatManager(*xls);
}

void bmx_xls_xlsformatmanager_free(ExcelFormat::XLSFormatManager * manager) {
	delete manager;
}

// ********************************************************

MaxCellFormat * bmx_xls_cellformat_create(ExcelFormat::XLSFormatManager * manager) {
	return new MaxCellFormat(*manager);
}

void bmx_xls_cellformat_SetFont(MaxCellFormat * format, MaxExcelFont * font) {
	format->Format().set_font(font->Font());
}

void bmx_xls_cellformat_free(MaxCellFormat * format) {
	delete format;
}

MaxExcelFont * bmx_xls_cellformat_GetFont(MaxCellFormat * format) {
	ExcelFormat::ExcelFont font(format->Format().get_font());
	return new MaxExcelFont(font);
}

void bmx_xls_cellformat_SetFormatString(MaxCellFormat * format, BBString * text) {
	wchar_t * s = bbStringToWchar_t(text);
	format->Format().set_format_string(s);
	WCHAR_FREE(s);
}

BBString * bmx_xls_cellformat_GetFormatString(MaxCellFormat * format) {
	return bbStringFromWchar_t(format->Format().get_format_string().c_str());
}

void bmx_xls_cellformat_SetRotation(MaxCellFormat * format, int rotation) {
	format->Format().set_rotation(static_cast<char>(rotation));
}

int bmx_xls_cellformat_GetRotation(MaxCellFormat * format) {
	return static_cast<int>(format->Format().get_rotation());
}

