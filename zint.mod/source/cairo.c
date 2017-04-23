/* cairo.c - Cairographics Renderer */

/*
    libzint - the open source barcode library
    Copyright (C) 2009-2016 Robin Stuart <rstuart114@gmail.com>
    Copyright (C) 2017 Bruce A Henderson

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright 
       notice, this list of conditions and the following disclaimer.  
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.  
    3. Neither the name of the project nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission. 

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
    SUCH DAMAGE.
 */

#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef _MSC_VER
#include <malloc.h>
#endif

#include "common.h"

#ifdef CAIRO_RENDER

#include "cairo.h"

#define SSET	"0123456789ABCDEF"

int cairo_plot(struct zint_symbol *symbol) {
    int i, block_width, latch, r, this_row;
    float textpos, large_bar_height, preset_height, row_height, row_posn = 0.0;

    int error_number = 0;
    int textoffset, xoffset, yoffset, textdone, main_width;
    char textpart[10], addon[6];
    int large_bar_count, comp_offset;
    float addon_text_posn;
    float scaler = symbol->scale;
    float default_text_posn;
    const char *locale = NULL;
#ifndef _MSC_VER
    unsigned char local_text[ustrlen(symbol->text) + 1];
#else
    unsigned char* local_text = (unsigned char*) _alloca(ustrlen(symbol->text) + 1);
#endif
	double fgred, fggrn, fgblu, bgred, bggrn, bgblu;

    row_height = 0;
    textdone = 0;
    main_width = symbol->width;
    strcpy(addon, "");
    comp_offset = 0;
    addon_text_posn = 0.0;

    if (symbol->show_hrt != 0) {
        /* Copy text from symbol */
        ustrcpy(local_text, symbol->text);
    } else {
        /* No text needed */
        switch (symbol->symbology) {
            case BARCODE_EANX:
            case BARCODE_EANX_CC:
            case BARCODE_ISBNX:
            case BARCODE_UPCA:
            case BARCODE_UPCE:
            case BARCODE_UPCA_CC:
            case BARCODE_UPCE_CC:
                /* For these symbols use dummy text to ensure formatting is done
                 * properly even if no text is required */
                for (i = 0; i < ustrlen(symbol->text); i++) {
                    if (symbol->text[i] == '+') {
                        local_text[i] = '+';
                    } else {
                        local_text[i] = ' ';
                    }
                    local_text[ustrlen(symbol->text)] = '\0';
                }
                break;
            default:
                /* For everything else, just remove the text */
                local_text[0] = '\0';
                break;
        }
    }

    /* sort out colour options */
    to_upper((unsigned char*) symbol->fgcolour);
    to_upper((unsigned char*) symbol->bgcolour);

    if (strlen(symbol->fgcolour) != 6) {
        strcpy(symbol->errtxt, "Malformed foreground colour target (F61)");
        return ZINT_ERROR_INVALID_OPTION;
    }
    if (strlen(symbol->bgcolour) != 6) {
        strcpy(symbol->errtxt, "Malformed background colour target (F62)");
        return ZINT_ERROR_INVALID_OPTION;
    }
    error_number = is_sane(SSET, (unsigned char*) symbol->fgcolour, strlen(symbol->fgcolour));
    if (error_number == ZINT_ERROR_INVALID_DATA) {
        strcpy(symbol->errtxt, "Malformed foreground colour target (F63)");
        return ZINT_ERROR_INVALID_OPTION;
    }
    error_number = is_sane(SSET, (unsigned char*) symbol->bgcolour, strlen(symbol->bgcolour));
    if (error_number == ZINT_ERROR_INVALID_DATA) {
        strcpy(symbol->errtxt, "Malformed background colour target (F64)");
        return ZINT_ERROR_INVALID_OPTION;
    }
    locale = setlocale(LC_ALL, "C");

    if (symbol->height == 0) {
        symbol->height = 50;
    }

    large_bar_count = 0;
    preset_height = 0.0;
    for (i = 0; i < symbol->rows; i++) {
        preset_height += symbol->row_height[i];
        if (symbol->row_height[i] == 0) {
            large_bar_count++;
        }
    }
    large_bar_height = (symbol->height - preset_height) / large_bar_count;

    if (large_bar_count == 0) {
        symbol->height = preset_height;
    }

    while (!(module_is_set(symbol, symbol->rows - 1, comp_offset))) {
        comp_offset++;
    }

    /* Certain symbols need whitespace otherwise characters get chopped off the sides */
    if ((((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC))
            || (symbol->symbology == BARCODE_ISBNX)) {
        switch (ustrlen(local_text)) {
            case 13: /* EAN 13 */
            case 16:
            case 19:
                if (symbol->whitespace_width == 0) {
                    symbol->whitespace_width = 10;
                }
                main_width = 96 + comp_offset;
                break;
            default:
                main_width = 68 + comp_offset;
        }
    }

    if (((symbol->symbology == BARCODE_UPCA) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCA_CC)) {
        if (symbol->whitespace_width == 0) {
            symbol->whitespace_width = 10;
            main_width = 96 + comp_offset;
        }
    }

    if (((symbol->symbology == BARCODE_UPCE) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCE_CC)) {
        if (symbol->whitespace_width == 0) {
            symbol->whitespace_width = 10;
            main_width = 51 + comp_offset;
        }
    }

    latch = 0;
    r = 0;
    /* Isolate add-on text */
    if (is_extendable(symbol->symbology)) {
        for (i = 0; i < ustrlen(local_text); i++) {
            if (latch == 1) {
                addon[r] = local_text[i];
                r++;
            }
            if (local_text[i] == '+') {
                latch = 1;
            }
        }
    }
    addon[r] = '\0';

    if (ustrlen(local_text) != 0) {
        textoffset = 9;
    } else {
        textoffset = 0;
    }
    xoffset = symbol->border_width + symbol->whitespace_width;
    yoffset = symbol->border_width;
    
    
    cairo_t * context = (cairo_t*)symbol->context;
    cairo_save(context);


    fgred = ((16 * ctoi(symbol->fgcolour[0])) + ctoi(symbol->fgcolour[1])) / 255.0;
    fggrn = ((16 * ctoi(symbol->fgcolour[2])) + ctoi(symbol->fgcolour[3])) / 255.0;
    fgblu = ((16 * ctoi(symbol->fgcolour[4])) + ctoi(symbol->fgcolour[5])) / 255.0;
    bgred = ((16 * ctoi(symbol->bgcolour[0])) + ctoi(symbol->bgcolour[1])) / 255.0;
    bggrn = ((16 * ctoi(symbol->bgcolour[2])) + ctoi(symbol->bgcolour[3])) / 255.0;
    bgblu = ((16 * ctoi(symbol->bgcolour[4])) + ctoi(symbol->bgcolour[5])) / 255.0;

	cairo_set_source_rgb(context, fgred, fggrn, fgblu);
	
    if (symbol->symbology != BARCODE_MAXICODE) {
		cairo_rectangle(context, 0, 0, ceil((symbol->width + xoffset + xoffset) * scaler), ceil((symbol->height + textoffset + yoffset + yoffset) * scaler));
    } else {
		cairo_rectangle(context, 0, 0, ceil((74.0F + xoffset + xoffset) * scaler), ceil((72.0F + yoffset + yoffset) * scaler));
    }
    
    cairo_fill(context);
    
   	cairo_set_source_rgb(context, bgred, bggrn, bgblu);

    if (symbol->symbology != BARCODE_MAXICODE) {
    	cairo_rectangle(context, 0.0, 0.0, ceil((symbol->width + xoffset + xoffset) * scaler), ceil((symbol->height + textoffset + yoffset + yoffset) * scaler));
    } else {
    	cairo_rectangle(context, 0.0, 0.0, ceil((74.0F + xoffset + xoffset) * scaler), ceil((symbol->height + textoffset + yoffset + yoffset) * scaler));
    }

   	cairo_fill(context);
	cairo_set_source_rgb(context, fgred, fggrn, fgblu);

    if ((symbol->output_options & BARCODE_BOX) || (symbol->output_options & BARCODE_BIND)) {
        default_text_posn = (symbol->height + textoffset + symbol->border_width + symbol->border_width) * scaler;
    } else {
        default_text_posn = (symbol->height + textoffset + symbol->border_width) * scaler;
    }

    if (symbol->symbology == BARCODE_MAXICODE) {
        /* Maxicode uses hexagons */
        float ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy, mx, my;


        textoffset = 0.0;
        if ((symbol->output_options & BARCODE_BOX) || (symbol->output_options & BARCODE_BIND)) {
			cairo_rectangle(context, 0.0, 0.0, (74.0 + xoffset + xoffset) * scaler, symbol->border_width * scaler);
			cairo_rectangle(context, 0.0, (72.0 + symbol->border_width) * scaler, (74.0 + xoffset + xoffset) * scaler, symbol->border_width * scaler);
        }
        if (symbol->output_options & BARCODE_BOX) {
            /* side bars */
			cairo_rectangle(context, 0.0, 0.0, symbol->border_width * scaler, (72.0 + (2 * symbol->border_width)) * scaler);
			cairo_rectangle(context, (74.0 + xoffset + xoffset - symbol->border_width) * scaler, 0.0, symbol->border_width * scaler, (72.0 + (2 * symbol->border_width)) * scaler);
        }
        cairo_set_source_rgb(context, fgred, fggrn, fgblu);
        cairo_arc(context, (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 10.85 * scaler, 0, 2 * M_PI);
        cairo_fill(context);
        cairo_set_source_rgb(context, bgred, bggrn, bgblu);
        cairo_arc(context, (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 8.97 * scaler, 0, 2 * M_PI);
        cairo_fill(context);
        cairo_set_source_rgb(context, fgred, fggrn, fgblu);
        cairo_arc(context, (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 7.10 * scaler, 0, 2 * M_PI);
        cairo_fill(context);
        cairo_set_source_rgb(context, bgred, bggrn, bgblu);
        cairo_arc(context, (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 5.22 * scaler, 0, 2 * M_PI);
        cairo_fill(context);
        cairo_set_source_rgb(context, fgred, fggrn, fgblu);
        cairo_arc(context, (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 3.31 * scaler, 0, 2 * M_PI);
        cairo_fill(context);
        cairo_set_source_rgb(context, bgred, bggrn, bgblu);
        cairo_arc(context, (35.76 + xoffset) * scaler, (35.60 + yoffset) * scaler, 1.43 * scaler, 0, 2 * M_PI);
        cairo_fill(context);
        cairo_set_source_rgb(context, fgred, fggrn, fgblu);

        for (r = 0; r < symbol->rows; r++) {
            for (i = 0; i < symbol->width; i++) {
                if (module_is_set(symbol, r, i)) {
                    /* Dump a hexagon */
                    my = r * 2.135 + 1.43;
                    ay = my + 1.0 + yoffset;
                    by = my + 0.5 + yoffset;
                    cy = my - 0.5 + yoffset;
                    dy = my - 1.0 + yoffset;
                    ey = my - 0.5 + yoffset;
                    fy = my + 0.5 + yoffset;
                    if (r & 1) {
                        mx = (2.46 * i) + 1.23 + 1.23;
                    } else {
                        mx = (2.46 * i) + 1.23;
                    }
                    ax = mx + xoffset;
                    bx = mx + 0.86 + xoffset;
                    cx = mx + 0.86 + xoffset;
                    dx = mx + xoffset;
                    ex = mx - 0.86 + xoffset;
                    fx = mx - 0.86 + xoffset;
                    
                    cairo_new_path(context);
                    cairo_move_to(context, ax * scaler, ay * scaler);
                    cairo_line_to(context, bx * scaler, by * scaler);
                    cairo_line_to(context, cx * scaler, cy * scaler);
                    cairo_line_to(context, dx * scaler, dy * scaler);
                    cairo_line_to(context, ex * scaler, ey * scaler);
                    cairo_line_to(context, fx * scaler, fy * scaler);
                    cairo_fill(context);
                }
            }
        }
        cairo_fill(context);
    }

    if (symbol->symbology != BARCODE_MAXICODE) {
        /* everything else uses rectangles (or squares) */
        /* Works from the bottom of the symbol up */
        int addon_latch = 0;

        for (r = 0; r < symbol->rows; r++) {
            this_row = r;
            if (symbol->row_height[this_row] == 0) {
                row_height = large_bar_height;
            } else {
                row_height = symbol->row_height[this_row];
            }
            row_posn = 0;
            for (i = 0; i < r; i++) {
                if (symbol->row_height[i] == 0) {
                    row_posn += large_bar_height;
                } else {
                    row_posn += symbol->row_height[i];
                }
            }
            row_posn += yoffset;

            if (symbol->output_options & BARCODE_DOTTY_MODE) {
				cairo_set_source_rgb(context, fgred, fggrn, fgblu);
                /* Use (currently undocumented) dot mode - see SF ticket #29 */
                for (i = 0; i < symbol->width; i++) {
                    if (module_is_set(symbol, this_row, i)) {
						cairo_arc(context, ((i + xoffset) * scaler) + (scaler / 2.0), (row_posn * scaler) + (scaler / 2.0), (symbol->dot_size / 2.0) * scaler, 0, 2 * M_PI);
                    }
                }
                cairo_fill(context);
            } else {
                /* Normal mode, with rectangles */
                i = 0;
                if (module_is_set(symbol, this_row, 0)) {
                    latch = 1;
                } else {
                    latch = 0;
                }

                do {
                    block_width = 0;
                    do {
                        block_width++;
                    } while (module_is_set(symbol, this_row, i + block_width) == module_is_set(symbol, this_row, i));
                    if ((addon_latch == 0) && (r == (symbol->rows - 1)) && (i > main_width)) {
                        addon_text_posn = (row_posn + 8.0) * scaler;
                        addon_latch = 1;
                    }
                    if (latch == 1) {
                        /* a bar */
                        if (addon_latch == 0) {
                        	cairo_rectangle(context, (i + xoffset) * scaler, row_posn * scaler, block_width * scaler, row_height * scaler);
                        } else {
                        	cairo_rectangle(context, (i + xoffset) * scaler, (row_posn + 10.0) * scaler, block_width * scaler, (row_height - 5.0) * scaler);
                        }
                        latch = 0;
                    } else {
                        /* a space */
                        latch = 1;
                    }
                    i += block_width;

                } while (i < symbol->width);
            }
        }
    }
    /* That's done the actual data area, everything else is human-friendly */
	cairo_fill(context);
	
    xoffset += comp_offset;
    row_posn = (row_posn + large_bar_height) * scaler;

	cairo_text_extents_t extents;

    if ((((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC)) ||
            (symbol->symbology == BARCODE_ISBNX)) {
        /* guard bar extensions and text formatting for EAN8 and EAN13 */
        switch (ustrlen(local_text)) {
            case 8: /* EAN-8 */
            case 11:
            case 14:
            	cairo_rectangle(context, (0 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (2 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (32 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (34 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (64 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (66 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
                for (i = 0; i < 4; i++) {
                    textpart[i] = local_text[i];
                }
                textpart[4] = '\0';
                textpos = 17;

                cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
                cairo_set_font_size(context, 11.0 * scaler);
                
				cairo_text_extents(context, textpart, &extents);
                
                cairo_set_source_rgb(context, fgred, fggrn, fgblu);
                cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
                cairo_show_text(context, textpart);

                for (i = 0; i < 4; i++) {
                    textpart[i] = local_text[i + 4];
                }
                textpart[4] = '\0';
                textpos = 50;

                cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
                cairo_set_font_size(context, 11.0 * scaler);
                
				cairo_text_extents(context, textpart, &extents);
                
                cairo_set_source_rgb(context, fgred, fggrn, fgblu);
                cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
                cairo_show_text(context, textpart);

                textdone = 1;
                switch (strlen(addon)) {
                    case 2:
                        textpos = xoffset + 86;

						cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
						cairo_set_font_size(context, 11.0 * scaler);
				
						cairo_text_extents(context, addon, &extents);
				
						cairo_set_source_rgb(context, fgred, fggrn, fgblu);
						cairo_move_to(context, textpos * scaler - (extents.width / 2.0), addon_text_posn * scaler);
						cairo_show_text(context, addon);

                        break;
                    case 5:
                        textpos = xoffset + 100;

						cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
						cairo_set_font_size(context, 11.0 * scaler);
				
						cairo_text_extents(context, addon, &extents);
				
						cairo_set_source_rgb(context, fgred, fggrn, fgblu);
						cairo_move_to(context, textpos * scaler - (extents.width / 2.0), addon_text_posn * scaler);
						cairo_show_text(context, addon);

                        break;
                }

                break;
            case 13: /* EAN 13 */
            case 16:
            case 19:
            	cairo_rectangle(context, (0 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (2 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (46 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (48 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (92 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_rectangle(context, (94 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
            	cairo_fill(context);

                textpart[0] = local_text[0];
                textpart[1] = '\0';
                textpos = -7;
                
                cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
                cairo_set_font_size(context, 11.0 * scaler);
                
				cairo_text_extents(context, textpart, &extents);
                
                cairo_set_source_rgb(context, fgred, fggrn, fgblu);
                cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
                cairo_show_text(context, textpart);

                for (i = 0; i < 6; i++) {
                    textpart[i] = local_text[i + 1];
                }
                textpart[6] = '\0';
                textpos = 24;
                
                cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
                cairo_set_font_size(context, 11.0 * scaler);
                
				cairo_text_extents(context, textpart, &extents);

                cairo_set_source_rgb(context, fgred, fggrn, fgblu);
                cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
                cairo_show_text(context, textpart);
                
                for (i = 0; i < 6; i++) {
                    textpart[i] = local_text[i + 7];
                }
                textpart[6] = '\0';
                textpos = 70;

                cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
                cairo_set_font_size(context, 11.0 * scaler);

				cairo_text_extents(context, textpart, &extents);
                
                cairo_set_source_rgb(context, fgred, fggrn, fgblu);
                cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
                cairo_show_text(context, textpart);

                textdone = 1;
                switch (strlen(addon)) {
                    case 2:
                        textpos = xoffset + 114;

						cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
						cairo_set_font_size(context, 11.0 * scaler);
				
						cairo_text_extents(context, addon, &extents);
						
						cairo_set_source_rgb(context, fgred, fggrn, fgblu);
						cairo_move_to(context, textpos * scaler - (extents.width / 2.0), addon_text_posn * scaler);
						cairo_show_text(context, addon);

                        break;
                    case 5:
                        textpos = xoffset + 128;
                        
						cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
						cairo_set_font_size(context, 11.0 * scaler);
						
						cairo_text_extents(context, addon, &extents);
				
						cairo_set_source_rgb(context, fgred, fggrn, fgblu);
						cairo_move_to(context, textpos * scaler - (extents.width / 2.0), addon_text_posn * scaler);
						cairo_show_text(context, addon);

                        break;
                }
                break;

        }
    }
 
    if (((symbol->symbology == BARCODE_UPCA) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCA_CC)) {
        /* guard bar extensions and text formatting for UPCA */
        latch = 1;

        i = 0 + comp_offset;
        do {
            block_width = 0;
            do {
                block_width++;
            } while (module_is_set(symbol, symbol->rows - 1, i + block_width) == module_is_set(symbol, symbol->rows - 1, i));
            if (latch == 1) {
                /* a bar */
				cairo_rectangle(context, (i + xoffset - comp_offset) * scaler, row_posn, block_width * scaler, 5.0 * scaler);
                latch = 0;
            } else {
                /* a space */
                latch = 1;
            }
            i += block_width;
        } while (i < 11 + comp_offset);
			cairo_rectangle(context, (46 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
			cairo_rectangle(context, (48 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
			cairo_fill(context);
        latch = 1;
        i = 85 + comp_offset;
        do {
            block_width = 0;
            do {
                block_width++;
            } while (module_is_set(symbol, symbol->rows - 1, i + block_width) == module_is_set(symbol, symbol->rows - 1, i));
            if (latch == 1) {
                /* a bar */
				cairo_rectangle(context, (i + xoffset - comp_offset) * scaler, row_posn, block_width * scaler, 5.0 * scaler);
                latch = 0;
            } else {
                /* a space */
                latch = 1;
            }
            i += block_width;
        } while (i < 96 + comp_offset);
        
        cairo_fill(context);
        
        textpart[0] = local_text[0];
        textpart[1] = '\0';
        textpos = -5;

		cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(context, 8.0 * scaler);
		
		cairo_text_extents(context, textpart, &extents);

		cairo_set_source_rgb(context, fgred, fggrn, fgblu);
		cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
		cairo_show_text(context, textpart);

        for (i = 0; i < 5; i++) {
            textpart[i] = local_text[i + 1];
        }
        textpart[5] = '\0';
        textpos = 27;

		cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(context, 11.0 * scaler);
		
		cairo_text_extents(context, textpart, &extents);

		cairo_set_source_rgb(context, fgred, fggrn, fgblu);
		cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
		cairo_show_text(context, textpart);

        for (i = 0; i < 5; i++) {
            textpart[i] = local_text[i + 6];
        }
        textpart[6] = '\0';
        textpos = 68;

		cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(context, 11.0 * scaler);
		
		cairo_text_extents(context, textpart, &extents);

		cairo_set_source_rgb(context, fgred, fggrn, fgblu);
		cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
		cairo_show_text(context, textpart);

        textpart[1] = '\0';
        textpos = 100;

		cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(context, 8.0 * scaler);
		
		cairo_text_extents(context, textpart, &extents);

		cairo_set_source_rgb(context, fgred, fggrn, fgblu);
		cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
		cairo_show_text(context, textpart);

        textdone = 1;
        switch (strlen(addon)) {
            case 2:
                textpos = xoffset + 116;

				cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
				cairo_set_font_size(context, 11.0 * scaler);
		
				cairo_text_extents(context, addon, &extents);

				cairo_set_source_rgb(context, fgred, fggrn, fgblu);
				cairo_move_to(context, textpos * scaler - (extents.width / 2.0), addon_text_posn * scaler);
				cairo_show_text(context, addon);
				
                break;
            case 5:
                textpos = xoffset + 130;

				cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
				cairo_set_font_size(context, 11.0 * scaler);
		
				cairo_text_extents(context, addon, &extents);

				cairo_set_source_rgb(context, fgred, fggrn, fgblu);
				cairo_move_to(context, textpos * scaler - (extents.width / 2.0), addon_text_posn * scaler);
				cairo_show_text(context, addon);

                break;
        }

    }

    if (((symbol->symbology == BARCODE_UPCE) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCE_CC)) {
        /* guard bar extensions and text formatting for UPCE */
		cairo_rectangle(context, (0 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
		cairo_rectangle(context, (2 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
		cairo_rectangle(context, (46 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
		cairo_rectangle(context, (48 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
		cairo_rectangle(context, (50 + xoffset) * scaler, row_posn, scaler, 5.0 * scaler);
		cairo_fill(context);

        textpart[0] = local_text[0];
        textpart[1] = '\0';
        textpos = -5;

		cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(context, 8.0 * scaler);

		cairo_text_extents(context, textpart, &extents);

		cairo_set_source_rgb(context, fgred, fggrn, fgblu);
		cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
		cairo_show_text(context, textpart);

        for (i = 0; i < 6; i++) {
            textpart[i] = local_text[i + 1];
        }
        textpart[6] = '\0';
        textpos = 24;

		cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(context, 11.0 * scaler);

		cairo_text_extents(context, textpart, &extents);

		cairo_set_source_rgb(context, fgred, fggrn, fgblu);
		cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
		cairo_show_text(context, textpart);

        textpart[0] = local_text[7];
        textpart[1] = '\0';
        textpos = 55;

		cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(context, 8.0 * scaler);

		cairo_text_extents(context, textpart, &extents);

		cairo_set_source_rgb(context, fgred, fggrn, fgblu);
		cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
		cairo_show_text(context, textpart);

        textdone = 1;
        switch (strlen(addon)) {
            case 2:
                textpos = xoffset + 70;

				cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
				cairo_set_font_size(context, 11.0 * scaler);

				cairo_text_extents(context, addon, &extents);

				cairo_set_source_rgb(context, fgred, fggrn, fgblu);
				cairo_move_to(context, textpos * scaler - (extents.width / 2.0), addon_text_posn * scaler);
				cairo_show_text(context, addon);

                break;
            case 5:
                textpos = xoffset + 84;

				cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
				cairo_set_font_size(context, 11.0 * scaler);

				cairo_text_extents(context, addon, &extents);

				cairo_set_source_rgb(context, fgred, fggrn, fgblu);
				cairo_move_to(context, textpos * scaler - (extents.width / 2.0), addon_text_posn * scaler);
				cairo_show_text(context, addon);

                break;
        }

    }

    xoffset -= comp_offset;

    switch (symbol->symbology) {
        case BARCODE_MAXICODE:
            /* Do nothing! (It's already been done) */
            break;
        default:
            if (symbol->output_options & BARCODE_BIND) {
                if ((symbol->rows > 1) && (is_stackable(symbol->symbology) == 1)) {
                    /* row binding */
                    if (symbol->symbology != BARCODE_CODABLOCKF) {
                        for (r = 1; r < symbol->rows; r++) {
							cairo_rectangle(context, xoffset * scaler, ((r * row_height) + yoffset - 1) * scaler, symbol->width * scaler, 2.0 * scaler);
                        }
                    } else {
                        for (r = 1; r < symbol->rows; r++) {
							cairo_rectangle(context, (xoffset + 11) * scaler, ((r * row_height) + yoffset - 1) * scaler, (symbol->width - 25) * scaler, 2.0 * scaler);
                        }
                    }
                    cairo_fill(context);
                }
            }
            if ((symbol->output_options & BARCODE_BOX) || (symbol->output_options & BARCODE_BIND)) {
                if (symbol->symbology != BARCODE_CODABLOCKF) {
                	cairo_rectangle(context, 0.0, 0.0, (symbol->width + xoffset + xoffset) * scaler, symbol->border_width * scaler);
                	cairo_rectangle(context, 0.0, (symbol->height + symbol->border_width) * scaler, (symbol->width + xoffset + xoffset) * scaler, symbol->border_width * scaler);
                } else {
                	cairo_rectangle(context, xoffset * scaler, 0.0, symbol->width * scaler, symbol->border_width * scaler);
                	cairo_rectangle(context, xoffset * scaler, (symbol->height + symbol->border_width) * scaler, symbol->width * scaler, symbol->border_width * scaler);
                }
                cairo_fill(context);
            }
            if (symbol->output_options & BARCODE_BOX) {
                /* side bars */
            	cairo_rectangle(context, 0.0, 0.0, symbol->border_width * scaler, (symbol->height + (2 * symbol->border_width)) * scaler);
            	cairo_rectangle(context, (symbol->width + xoffset + xoffset - symbol->border_width) * scaler, 0.0, symbol->border_width * scaler, (symbol->height + (2 * symbol->border_width)) * scaler);
            	cairo_fill(context);
            }
            break;
    }

    /* Put the human readable text at the bottom */
    if ((textdone == 0) && ustrlen(local_text)) {
        textpos = symbol->width / 2.0;

		cairo_select_font_face(context, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(context, 8.0 * scaler);
		
		cairo_text_extents(context, local_text, &extents);

		cairo_set_source_rgb(context, fgred, fggrn, fgblu);
		cairo_move_to(context, (textpos + xoffset) * scaler - (extents.width / 2.0), default_text_posn);
		cairo_show_text(context, local_text);

    }

    if (locale)
        setlocale(LC_ALL, locale);

	cairo_restore(context);

    return error_number;
}

#endif /* CAIRO_RENDER */
