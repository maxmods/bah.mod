/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with html or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum htmltokentype {
     T_end_br = 258,
     T_end_img = 259,
     T_row = 260,
     T_end_row = 261,
     T_html = 262,
     T_end_html = 263,
     T_end_table = 264,
     T_end_cell = 265,
     T_end_font = 266,
     T_string = 267,
     T_error = 268,
     T_BR = 269,
     T_br = 270,
     T_IMG = 271,
     T_img = 272,
     T_table = 273,
     T_cell = 274,
     T_font = 275
   };
#endif
/* Tokens.  */
#define T_end_br 258
#define T_end_img 259
#define T_row 260
#define T_end_row 261
#define T_html 262
#define T_end_html 263
#define T_end_table 264
#define T_end_cell 265
#define T_end_font 266
#define T_string 267
#define T_error 268
#define T_BR 269
#define T_br 270
#define T_IMG 271
#define T_img 272
#define T_table 273
#define T_cell 274
#define T_font 275




/* Copy the first part of user declarations.  */
#line 17 "../../lib/common/htmlparse.y"


#include "render.h"
#include "htmltable.h"
#include "htmllex.h"

extern int htmlparse(void);

typedef struct sfont_t {
    htmlfont_t *cfont;	
    struct sfont_t *pfont;
} sfont_t;

static struct {
  htmllabel_t* lbl;       /* Generated label */
  htmltbl_t*   tblstack;  /* Stack of tables maintained during parsing */
#ifdef OLD
  Dt_t*        paras;     /* Dictionary for paras of text */
#endif
  Dt_t*        fitemList; /* Dictionary for font text items */
  Dt_t*        fparaList; 
  agxbuf*      str;       /* Buffer for text */
  sfont_t*     fontstack;
} HTMLstate;

/* free_ritem:
 * Free row. This closes and frees row's list, then
 * the pitem itself is freed.
 */
static void
free_ritem(Dt_t* d, pitem* p,Dtdisc_t* ds)
{
  dtclose (p->u.rp);
  free (p);
}

/* free_item:
 * Generic Dt free. Only frees container, assuming contents
 * have been copied elsewhere.
 */
static void
free_item(Dt_t* d, void* p,Dtdisc_t* ds)
{
  free (p);
}

/* cleanTbl:
 * Clean up table if error in parsing.
 */
static void
cleanTbl (htmltbl_t* tp)
{
  dtclose (tp->u.p.rows);
  free_html_data (&tp->data);
  free (tp);
}

/* cleanCell:
 * Clean up cell if error in parsing.
 */
static void
cleanCell (htmlcell_t* cp)
{
  if (cp->child.kind == HTML_TBL) cleanTbl (cp->child.u.tbl);
  else if (cp->child.kind == HTML_TEXT) free_html_text (cp->child.u.txt);
  free_html_data (&cp->data);
  free (cp);
}

/* free_citem:
 * Free cell item during parsing. This frees cell and pitem.
 */
static void
free_citem(Dt_t* d, pitem* p,Dtdisc_t* ds)
{
  cleanCell (p->u.cp);
  free (p);
}

static Dtdisc_t rowDisc = {
    offsetof(pitem,u),
    sizeof(void*),
    offsetof(pitem,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_ritem,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};
static Dtdisc_t cellDisc = {
    offsetof(pitem,u),
    sizeof(void*),
    offsetof(pitem,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_item,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

#ifdef OLD
typedef struct {
  Dtlink_t      link;
  const char*   s;          /* para of text */
  char          c;          /* alignment of text */
} sitem;
#endif

typedef struct {
    Dtlink_t    link;
    textitem_t  ti;
} fitem;

typedef struct {
    Dtlink_t     link;
    htextpara_t  lp;
} fpara;

#ifdef OLD
static void
free_sitem(Dt_t* d, sitem* p,Dtdisc_t* ds)
{
  free (p);
}
#endif

static void 
free_fitem(Dt_t* d, fitem* p, Dtdisc_t* ds)
{
    if (p->ti.str)
	free (p->ti.str);
    if (p->ti.font)
        free_html_font (p->ti.font);
    free (p);
}

static void 
free_fpara(Dt_t* d, fpara* p, Dtdisc_t* ds)
{
    textitem_t* ti;

    if (p->lp.nitems) {
	int i;
	ti = p->lp.items;
	for (i = 0; i < p->lp.nitems; i++) {
	    if (ti->str) free (ti->str);
	    if (ti->font) free_html_font (ti->font);
	    ti++;
	}
	free (p->lp.items);
    }
    free (p);
}

#ifdef OLD
static Dtdisc_t strDisc = {
    offsetof(sitem,s),
    sizeof(char*),
    offsetof(sitem,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_sitem,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};
#endif

static Dtdisc_t fstrDisc = {
    0,
    0,
    offsetof(fitem,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_item,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};


static Dtdisc_t fparaDisc = {
    0,
    0,
    offsetof(fpara,link),
    NIL(Dtmake_f),
    (Dtfree_f)free_item,
    NIL(Dtcompar_f),
    NIL(Dthash_f),
    NIL(Dtmemory_f),
    NIL(Dtevent_f)
};

#ifdef OLD
static void
appendStrList(const char* p,int v)
{
  sitem*  sp = NEW(sitem);
  sp->s = strdup(p);
  sp->c = v;
  dtinsert (HTMLstate.paras, sp);
}
#endif

/* dupFont:
 */
static htmlfont_t *
dupFont (htmlfont_t *f)
{
    if (f) f->cnt++;
    return f;
}

/* appendFItemList:
 * Append a new fitem to the list.
 */
static void
appendFItemList (agxbuf *ag)
{
    fitem *fi = NEW(fitem);

    fi->ti.str = strdup(agxbuse(ag));
    fi->ti.font = dupFont (HTMLstate.fontstack->cfont);
    dtinsert(HTMLstate.fitemList, fi);
}	

/* appendFLineList:
 */
static void 
appendFLineList (int v)
{
    int cnt;
    fpara *ln = NEW(fpara);
    fitem *fi;
    Dt_t *ilist = HTMLstate.fitemList;

    cnt = dtsize(ilist);
    ln->lp.nitems = cnt;
    ln->lp.just = v;
    if (cnt) {
        int i = 0;
	ln->lp.items = N_NEW(cnt, textitem_t); 

	fi = (fitem*)dtflatten(ilist);
	for (; fi; fi = (fitem*)dtlink(fitemList,(Dtlink_t*)fi)) {
	    ln->lp.items[i] = fi->ti;
	    i++;
	}
    }

    dtclear(ilist);

    dtinsert(HTMLstate.fparaList, ln);
}

#ifdef OLD
/* mkText:
 * Construct htmltxt_t from list of paras in HTMLstate.paras.
 * lastl is a last, odd para with no <BR>, so we use n by default.
 */
static htmltxt_t*
mkText (const char* lastl)
{
  int         cnt;
  textpara_t* lp;
  sitem*      sp;
  Dt_t*       paras = HTMLstate.paras;
  htmltxt_t* tp = NEW(htmltxt_t);

  if (paras)
    cnt = dtsize (paras);
  else
    cnt = 0;
  if (lastl) cnt++;

  tp->nparas = cnt;
  tp->para = N_NEW(cnt+1,textpara_t);

  lp = tp->para;
  if (paras) {
    sp = (sitem*)dtflatten(paras);
    for (; sp; sp = (sitem*)dtlink(paras,(Dtlink_t*)sp)) {
      lp->str = (char*)(sp->s);
      lp->xshow = NULL;
      lp->just = sp->c;
      lp++;
    }
  }
  if (lastl) {
    lp->str = strdup(lastl);
    lp->just = '\0';
  }

  if (paras) dtclear (paras);

  return tp;
}
#endif

static htmltxt_t*
mkText(void)
{
    int cnt;
    Dt_t * ipara = HTMLstate.fparaList;
    fpara *fl ;
    htmltxt_t *hft = NEW(htmltxt_t);
    
    if (dtsize (HTMLstate.fitemList)) 
	appendFLineList (UNSET_ALIGN);

    cnt = dtsize(ipara);
    hft->nparas = cnt;
    	
    if (cnt) {
	int i = 0;
	hft->paras = N_NEW(cnt,htextpara_t);	
    	for(fl=(fpara *)dtfirst(ipara); fl; fl=(fpara *)dtnext(ipara,fl)) {
    	    hft->paras[i] = fl->lp;
    	    i++;
    	}
    }
    
    dtclear(ipara);

    return hft;
}

/* addRow:
 * Add new cell row to current table.
 */
static void addRow (void)
{
  Dt_t*      dp = dtopen(&cellDisc, Dtqueue);
  htmltbl_t* tbl = HTMLstate.tblstack;
  pitem*     sp = NEW(pitem);
  sp->u.rp = dp;
  dtinsert (tbl->u.p.rows, sp);
}

/* setCell:
 * Set cell body and type and attach to row
 */
static void setCell (htmlcell_t* cp, void* obj, int kind)
{
  pitem*     sp = NEW(pitem);
  htmltbl_t* tbl = HTMLstate.tblstack;
  pitem*     rp = (pitem*)dtlast (tbl->u.p.rows);
  Dt_t*      row = rp->u.rp;
  sp->u.cp = cp;
  dtinsert (row, sp);
  cp->child.kind = kind;
  
  if(kind == HTML_TEXT)
  	cp->child.u.txt = (htmltxt_t*)obj;
  else if (kind == HTML_IMAGE)
    cp->child.u.img = (htmlimg_t*)obj;
  else
    cp->child.u.tbl = (htmltbl_t*)obj;
}

#ifdef OLD
/* setFont:
 * Copy in font attributes. fp has the new attributes.
 * curf corresponds to the current font info of the object.
 * From the parser, we are moving out from the object. Since
 * the inmost value is the one used, we only use a new value
 * if the attribute has not already been assigned.
 */
static htmlfont_t* setFont (htmlfont_t* fp, htmlfont_t*  curf)
{
  if (curf) {
    if (curf->size < 0.0) curf->size = fp->size;
    if (!curf->color) curf->color = fp->color;
    else if (fp->color) free (fp->color);
    if (!curf->name) curf->name = fp->name;
    else if (fp->name) free (fp->name);
    free (fp);
    return curf;
  }
  else
    return fp;
}

/* fontText:
 * Attach font information to text.
 */
static void fontText (htmlfont_t* fp, htmltxt_t* cp)
{
  cp->font = setFont (fp, cp->font);
}

/* fontTable:
 * Attach font information to table.
 */
static void fontTable (htmlfont_t* fp, htmltbl_t* cp)
{
  cp->font = setFont (fp, cp->font);
}
#endif

/* mkLabel:
 * Create label, given body and type.
 */
static htmllabel_t* mkLabel (void* obj, int kind)
{
  htmllabel_t* lp = NEW(htmllabel_t);

  lp->kind = kind;
  if (kind == HTML_TEXT)
    lp->u.txt = (htmltxt_t*)obj;
  else
    lp->u.tbl = (htmltbl_t*)obj;
  return lp;
}

/* freeFontstack:
 * Free all stack items but the last, which is
 * put on artificially during in parseHTML.
 */
static void
freeFontstack(void)
{
    sfont_t* s;
    sfont_t* next;

    for (s = HTMLstate.fontstack; (next = s->pfont); s = next) {
	free_html_font (s->cfont);
	free(s);
    }
}

/* cleanup:
 * Called on error. Frees resources allocated during parsing.
 * This includes a label, plus a walk down the stack of
 * tables. Note that we use the free_citem function to actually
 * free cells.
 */
static void cleanup (void)
{
  htmltbl_t* tp = HTMLstate.tblstack;
  htmltbl_t* next;

  if (HTMLstate.lbl) {
    free_html_label (HTMLstate.lbl,1);
    HTMLstate.lbl = NULL;
  }
  cellDisc.freef = (Dtfree_f)free_citem;
  while (tp) {
    next = tp->u.p.prev;
    cleanTbl (tp);
    tp = next;
  }
  cellDisc.freef = (Dtfree_f)free_item;

  fstrDisc.freef = (Dtfree_f)free_fitem;
  dtclear (HTMLstate.fitemList);
  fstrDisc.freef = (Dtfree_f)free_item;

  fparaDisc.freef = (Dtfree_f)free_fpara;
  dtclear (HTMLstate.fparaList);
  fparaDisc.freef = (Dtfree_f)free_item;

  freeFontstack();
}

/* nonSpace:
 * Return 1 if s contains a non-space character.
 */
static int nonSpace (char* s)
{
  char   c;

  while ((c = *s++)) {
    if (c != ' ') return 1;
  }
  return 0;
}

/* pushFont:
 * Fonts are allocated in the lexer.
 */
static void
pushFont (htmlfont_t *f)
{
    sfont_t *ft = NEW(sfont_t);
    htmlfont_t* curfont = HTMLstate.fontstack->cfont;

    if (curfont) {
	if (!f->color && curfont->color)
	    f->color = strdup(curfont->color);
	if ((f->size < 0.0) && (curfont->size >= 0.0))
	    f->size = curfont->size;
	if (!f->name && curfont->name)
	    f->name = strdup(curfont->name);
    }

    ft->cfont = dupFont (f);
    ft->pfont = HTMLstate.fontstack;
    HTMLstate.fontstack = ft;
}

/* popFont:
 */
static void 
popFont (void)
{
    sfont_t* curfont = HTMLstate.fontstack;
    sfont_t* prevfont = curfont->pfont;

    free_html_font (curfont->cfont);
    free (curfont);
    HTMLstate.fontstack = prevfont;
}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 535 "../../lib/common/htmlparse.y"
typedef union YYSTYPE {
  int    i;
  htmltxt_t*  txt;
  htmlcell_t*  cell;
  htmltbl_t*   tbl;
  htmlfont_t*  font;
  htmlimg_t*   img;
} YYSTYPE;
/* Line 196 of yacc.c.  */
#line 652 "y.tab.c"
# define htmlstype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */
#line 664 "y.tab.c"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (htmloverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (htmloverflow) || YYERROR_VERBOSE */


#if (! defined (htmloverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union htmlalloc
{
  short int htmlss;
  YYSTYPE htmlvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union htmlalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T htmli;				\
	  for (htmli = 0; htmli < (Count); htmli++)	\
	    (To)[htmli] = (From)[htmli];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T htmlnewbytes;						\
	YYCOPY (&htmlptr->Stack, Stack, htmlsize);				\
	Stack = &htmlptr->Stack;						\
	htmlnewbytes = htmlstacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	htmlptr += htmlnewbytes / sizeof (*htmlptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char htmlsigned_char;
#else
   typedef short int htmlsigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  17
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   61

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  21
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  23
/* YYNRULES -- Number of rules. */
#define YYNRULES  38
/* YYNRULES -- Number of states. */
#define YYNSTATES  59

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   275

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? htmltranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char htmltranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char htmlprhs[] =
{
       0,     0,     3,     7,    11,    13,    15,    18,    20,    22,
      24,    28,    30,    32,    35,    37,    39,    42,    43,    50,
      52,    56,    58,    59,    61,    64,    65,    70,    72,    75,
      76,    81,    82,    87,    88,    93,    94,    98,   101
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const htmlsigned_char htmlrhs[] =
{
      22,     0,    -1,     7,    23,     8,    -1,     7,    32,     8,
      -1,     1,    -1,    24,    -1,    24,    25,    -1,    25,    -1,
      29,    -1,    28,    -1,    26,    24,    27,    -1,    20,    -1,
      11,    -1,    15,     3,    -1,    14,    -1,    12,    -1,    29,
      12,    -1,    -1,    33,    18,    31,    34,     9,    33,    -1,
      30,    -1,    26,    30,    27,    -1,    29,    -1,    -1,    35,
      -1,    34,    35,    -1,    -1,     5,    36,    37,     6,    -1,
      38,    -1,    37,    38,    -1,    -1,    19,    32,    39,    10,
      -1,    -1,    19,    23,    40,    10,    -1,    -1,    19,    43,
      41,    10,    -1,    -1,    19,    42,    10,    -1,    17,     4,
      -1,    16,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int htmlrline[] =
{
       0,   561,   561,   562,   563,   566,   569,   570,   573,   574,
     575,   578,   581,   584,   585,   588,   589,   592,   592,   613,
     614,   617,   618,   621,   622,   625,   625,   628,   629,   632,
     632,   633,   633,   634,   634,   635,   635,   638,   639
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const htmltname[] =
{
  "$end", "error", "$undefined", "T_end_br", "T_end_img", "T_row",
  "T_end_row", "T_html", "T_end_html", "T_end_table", "T_end_cell",
  "T_end_font", "T_string", "T_error", "T_BR", "T_br", "T_IMG", "T_img",
  "T_table", "T_cell", "T_font", "$accept", "html", "fonttext", "text",
  "textitem", "sfont", "nfont", "br", "string", "table", "@1", "fonttable",
  "opt_space", "rows", "row", "@2", "cells", "cell", "@3", "@4", "@5",
  "@6", "image", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int htmltoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char htmlr1[] =
{
       0,    21,    22,    22,    22,    23,    24,    24,    25,    25,
      25,    26,    27,    28,    28,    29,    29,    31,    30,    32,
      32,    33,    33,    34,    34,    36,    35,    37,    37,    39,
      38,    40,    38,    41,    38,    42,    38,    43,    43
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char htmlr2[] =
{
       0,     2,     3,     3,     1,     1,     2,     1,     1,     1,
       3,     1,     1,     2,     1,     1,     2,     0,     6,     1,
       3,     1,     0,     1,     2,     0,     4,     1,     2,     0,
       4,     0,     4,     0,     4,     0,     3,     2,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char htmldefact[] =
{
       0,     4,    22,     0,    15,    14,     0,    11,     0,     5,
       7,    22,     9,     8,    19,     0,     0,     1,    13,     2,
       6,     0,     8,     0,     0,    16,     3,    17,    12,    10,
      20,     0,    25,     0,    23,     0,    22,    24,    22,     0,
      27,    21,    18,    38,     0,    31,    29,     0,    33,    26,
      28,    37,     0,     0,    36,     0,    32,    30,    34
};

/* YYDEFGOTO[NTERM-NUM]. */
static const htmlsigned_char htmldefgoto[] =
{
      -1,     3,     8,     9,    10,    21,    29,    12,    13,    14,
      31,    15,    16,    33,    34,    35,    39,    40,    53,    52,
      55,    47,    48
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -9
static const htmlsigned_char htmlpact[] =
{
      24,    -9,    18,     3,    -9,    -9,    16,    -9,     4,    18,
      -9,    18,    -9,    22,    -9,    19,    21,    -9,    -9,    -9,
      -9,    18,    23,     9,    26,    -9,    -9,    -9,    -9,    -9,
      -9,    36,    -9,    17,    -9,    25,    30,    -9,    -6,    -1,
      -9,    23,    -9,    -9,    39,    -9,    -9,    35,    -9,    -9,
      -9,    -9,    37,    38,    -9,    40,    -9,    -9,    -9
};

/* YYPGOTO[NTERM-NUM].  */
static const htmlsigned_char htmlpgoto[] =
{
      -9,    -9,     8,    -4,    -7,    -2,    27,    -9,    -8,    41,
      -9,    11,    20,    -9,    28,    -9,    -9,    14,    -9,    -9,
      -9,    -9,    -9
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -36
static const htmlsigned_char htmltable[] =
{
      11,    22,    20,    17,   -35,    49,     4,    23,     5,     6,
      43,    44,    19,    22,     7,    22,    20,    23,    38,    18,
      28,     4,    32,     5,     6,     1,    36,    26,    41,     7,
       4,     2,     5,     6,    25,    25,    11,    28,     7,    27,
     -21,    32,     4,    51,    38,    54,    45,    56,    57,    46,
      58,    30,    24,    50,     0,     0,    42,     0,     0,     0,
       0,    37
};

static const htmlsigned_char htmlcheck[] =
{
       2,     9,     9,     0,    10,     6,    12,    11,    14,    15,
      16,    17,     8,    21,    20,    23,    23,    21,    19,     3,
      11,    12,     5,    14,    15,     1,     9,     8,    36,    20,
      12,     7,    14,    15,    12,    12,    38,    11,    20,    18,
      18,     5,    12,     4,    19,    10,    38,    10,    10,    38,
      10,    24,    11,    39,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    33
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char htmlstos[] =
{
       0,     1,     7,    22,    12,    14,    15,    20,    23,    24,
      25,    26,    28,    29,    30,    32,    33,     0,     3,     8,
      25,    26,    29,    24,    30,    12,     8,    18,    11,    27,
      27,    31,     5,    34,    35,    36,     9,    35,    19,    37,
      38,    29,    33,    16,    17,    23,    32,    42,    43,     6,
      38,     4,    40,    39,    10,    41,    10,    10,    10
};

#define htmlerrok		(htmlerrstatus = 0)
#define htmlclearin	(htmlchar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto htmlacceptlab
#define YYABORT		goto htmlabortlab
#define YYERROR		goto htmlerrorlab


/* Like YYERROR except do call htmlerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto htmlerrlab

#define YYRECOVERING()  (!!htmlerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (htmlchar == YYEMPTY && htmllen == 1)				\
    {								\
      htmlchar = (Token);						\
      htmllval = (Value);						\
      htmltoken = YYTRANSLATE (htmlchar);				\
      YYPOPSTACK;						\
      goto htmlbackup;						\
    }								\
  else								\
    {								\
      htmlerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `htmllex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX htmllex (YYLEX_PARAM)
#else
# define YYLEX htmllex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (htmldebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (htmldebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      htmlsymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| html_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
html_stack_print (short int *bottom, short int *top)
#else
static void
html_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (htmldebug)							\
    html_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
html_reduce_print (int htmlrule)
#else
static void
html_reduce_print (htmlrule)
    int htmlrule;
#endif
{
  int htmli;
  unsigned long int htmllno = htmlrline[htmlrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             htmlrule - 1, htmllno);
  /* Print the symbols being reduced, and their result.  */
  for (htmli = htmlprhs[htmlrule]; 0 <= htmlrhs[htmli]; htmli++)
    YYFPRINTF (stderr, "%s ", htmltname[htmlrhs[htmli]]);
  YYFPRINTF (stderr, "-> %s\n", htmltname[htmlr1[htmlrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (htmldebug)				\
    html_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int htmldebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef htmlstrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define htmlstrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
htmlstrlen (const char *htmlstr)
#   else
htmlstrlen (htmlstr)
     const char *htmlstr;
#   endif
{
  const char *htmls = htmlstr;

  while (*htmls++ != '\0')
    continue;

  return htmls - htmlstr - 1;
}
#  endif
# endif

# ifndef htmlstpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define htmlstpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
htmlstpcpy (char *htmldest, const char *htmlsrc)
#   else
htmlstpcpy (htmldest, htmlsrc)
     char *htmldest;
     const char *htmlsrc;
#   endif
{
  char *htmld = htmldest;
  const char *htmls = htmlsrc;

  while ((*htmld++ = *htmls++) != '\0')
    continue;

  return htmld - 1;
}
#  endif
# endif

# ifndef htmltnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for htmlerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from htmltname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
htmltnamerr (char *htmlres, const char *htmlstr)
{
  if (*htmlstr == '"')
    {
      size_t htmln = 0;
      char const *htmlp = htmlstr;

      for (;;)
	switch (*++htmlp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++htmlp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (htmlres)
	      htmlres[htmln] = *htmlp;
	    htmln++;
	    break;

	  case '"':
	    if (htmlres)
	      htmlres[htmln] = '\0';
	    return htmln;
	  }
    do_not_strip_quotes: ;
    }

  if (! htmlres)
    return htmlstrlen (htmlstr);

  return htmlstpcpy (htmlres, htmlstr) - htmlres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
htmlsymprint (FILE *htmloutput, int htmltype, YYSTYPE *htmlvaluep)
#else
static void
htmlsymprint (htmloutput, htmltype, htmlvaluep)
    FILE *htmloutput;
    int htmltype;
    YYSTYPE *htmlvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) htmlvaluep;

  if (htmltype < YYNTOKENS)
    YYFPRINTF (htmloutput, "token %s (", htmltname[htmltype]);
  else
    YYFPRINTF (htmloutput, "nterm %s (", htmltname[htmltype]);


# ifdef YYPRINT
  if (htmltype < YYNTOKENS)
    YYPRINT (htmloutput, htmltoknum[htmltype], *htmlvaluep);
# endif
  switch (htmltype)
    {
      default:
        break;
    }
  YYFPRINTF (htmloutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
htmldestruct (const char *htmlmsg, int htmltype, YYSTYPE *htmlvaluep)
#else
static void
htmldestruct (htmlmsg, htmltype, htmlvaluep)
    const char *htmlmsg;
    int htmltype;
    YYSTYPE *htmlvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) htmlvaluep;

  if (!htmlmsg)
    htmlmsg = "Deleting";
  YY_SYMBOL_PRINT (htmlmsg, htmltype, htmlvaluep, htmllocationp);

  switch (htmltype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int htmlparse (void *YYPARSE_PARAM);
# else
int htmlparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int htmlparse (void);
#else
int htmlparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int htmlchar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE htmllval;

/* Number of syntax errors so far.  */
int htmlnerrs;



/*----------.
| htmlparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int htmlparse (void *YYPARSE_PARAM)
# else
int htmlparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
htmlparse (void)
#else
int
htmlparse ()

#endif
#endif
{
  
  int htmlstate;
  int htmln;
  int htmlresult;
  /* Number of tokens to shift before error messages enabled.  */
  int htmlerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int htmltoken = 0;

  /* Three stacks and their tools:
     `htmlss': related to states,
     `htmlvs': related to semantic values,
     `htmlls': related to locations.

     Refer to the stacks thru separate pointers, to allow htmloverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int htmlssa[YYINITDEPTH];
  short int *htmlss = htmlssa;
  short int *htmlssp;

  /* The semantic value stack.  */
  YYSTYPE htmlvsa[YYINITDEPTH];
  YYSTYPE *htmlvs = htmlvsa;
  YYSTYPE *htmlvsp;



#define YYPOPSTACK   (htmlvsp--, htmlssp--)

  YYSIZE_T htmlstacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE htmlval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int htmllen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  htmlstate = 0;
  htmlerrstatus = 0;
  htmlnerrs = 0;
  htmlchar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  htmlssp = htmlss;
  htmlvsp = htmlvs;

  goto htmlsetstate;

/*------------------------------------------------------------.
| htmlnewstate -- Push a new state, which is found in htmlstate.  |
`------------------------------------------------------------*/
 htmlnewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  htmlssp++;

 htmlsetstate:
  *htmlssp = htmlstate;

  if (htmlss + htmlstacksize - 1 <= htmlssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T htmlsize = htmlssp - htmlss + 1;

#ifdef htmloverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *htmlvs1 = htmlvs;
	short int *htmlss1 = htmlss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if htmloverflow is a macro.  */
	htmloverflow (YY_("memory exhausted"),
		    &htmlss1, htmlsize * sizeof (*htmlssp),
		    &htmlvs1, htmlsize * sizeof (*htmlvsp),

		    &htmlstacksize);

	htmlss = htmlss1;
	htmlvs = htmlvs1;
      }
#else /* no htmloverflow */
# ifndef YYSTACK_RELOCATE
      goto htmlexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= htmlstacksize)
	goto htmlexhaustedlab;
      htmlstacksize *= 2;
      if (YYMAXDEPTH < htmlstacksize)
	htmlstacksize = YYMAXDEPTH;

      {
	short int *htmlss1 = htmlss;
	union htmlalloc *htmlptr =
	  (union htmlalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (htmlstacksize));
	if (! htmlptr)
	  goto htmlexhaustedlab;
	YYSTACK_RELOCATE (htmlss);
	YYSTACK_RELOCATE (htmlvs);

#  undef YYSTACK_RELOCATE
	if (htmlss1 != htmlssa)
	  YYSTACK_FREE (htmlss1);
      }
# endif
#endif /* no htmloverflow */

      htmlssp = htmlss + htmlsize - 1;
      htmlvsp = htmlvs + htmlsize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) htmlstacksize));

      if (htmlss + htmlstacksize - 1 <= htmlssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", htmlstate));

  goto htmlbackup;

/*-----------.
| htmlbackup.  |
`-----------*/
htmlbackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* htmlresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  htmln = htmlpact[htmlstate];
  if (htmln == YYPACT_NINF)
    goto htmldefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (htmlchar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      htmlchar = YYLEX;
    }

  if (htmlchar <= YYEOF)
    {
      htmlchar = htmltoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      htmltoken = YYTRANSLATE (htmlchar);
      YY_SYMBOL_PRINT ("Next token is", htmltoken, &htmllval, &htmllloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  htmln += htmltoken;
  if (htmln < 0 || YYLAST < htmln || htmlcheck[htmln] != htmltoken)
    goto htmldefault;
  htmln = htmltable[htmln];
  if (htmln <= 0)
    {
      if (htmln == 0 || htmln == YYTABLE_NINF)
	goto htmlerrlab;
      htmln = -htmln;
      goto htmlreduce;
    }

  if (htmln == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", htmltoken, &htmllval, &htmllloc);

  /* Discard the token being shifted unless it is eof.  */
  if (htmlchar != YYEOF)
    htmlchar = YYEMPTY;

  *++htmlvsp = htmllval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (htmlerrstatus)
    htmlerrstatus--;

  htmlstate = htmln;
  goto htmlnewstate;


/*-----------------------------------------------------------.
| htmldefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
htmldefault:
  htmln = htmldefact[htmlstate];
  if (htmln == 0)
    goto htmlerrlab;
  goto htmlreduce;


/*-----------------------------.
| htmlreduce -- Do a reduction.  |
`-----------------------------*/
htmlreduce:
  /* htmln is the number of a rule to reduce with.  */
  htmllen = htmlr2[htmln];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  htmlval = htmlvsp[1-htmllen];


  YY_REDUCE_PRINT (htmln);
  switch (htmln)
    {
        case 2:
#line 561 "../../lib/common/htmlparse.y"
    { HTMLstate.lbl = mkLabel((htmlvsp[-1].txt),HTML_TEXT); }
    break;

  case 3:
#line 562 "../../lib/common/htmlparse.y"
    { HTMLstate.lbl = mkLabel((htmlvsp[-1].tbl),HTML_TBL); }
    break;

  case 4:
#line 563 "../../lib/common/htmlparse.y"
    { cleanup(); YYABORT; }
    break;

  case 5:
#line 566 "../../lib/common/htmlparse.y"
    { (htmlval.txt) = mkText(); }
    break;

  case 8:
#line 573 "../../lib/common/htmlparse.y"
    { appendFItemList(HTMLstate.str);}
    break;

  case 9:
#line 574 "../../lib/common/htmlparse.y"
    {appendFLineList((htmlvsp[0].i));}
    break;

  case 11:
#line 578 "../../lib/common/htmlparse.y"
    { pushFont ((htmlvsp[0].font)); }
    break;

  case 12:
#line 581 "../../lib/common/htmlparse.y"
    { popFont (); }
    break;

  case 13:
#line 584 "../../lib/common/htmlparse.y"
    { (htmlval.i) = (htmlvsp[-1].i); }
    break;

  case 14:
#line 585 "../../lib/common/htmlparse.y"
    { (htmlval.i) = (htmlvsp[0].i); }
    break;

  case 17:
#line 592 "../../lib/common/htmlparse.y"
    { 
          if (nonSpace(agxbuse(HTMLstate.str))) {
            htmlerror ("Syntax error: non-space string used before <TABLE>");
            cleanup(); YYABORT;
          }
          (htmlvsp[0].tbl)->u.p.prev = HTMLstate.tblstack;
          (htmlvsp[0].tbl)->u.p.rows = dtopen(&rowDisc, Dtqueue);
          HTMLstate.tblstack = (htmlvsp[0].tbl);
          (htmlvsp[0].tbl)->font = dupFont (HTMLstate.fontstack->cfont);
          (htmlval.tbl) = (htmlvsp[0].tbl);
        }
    break;

  case 18:
#line 603 "../../lib/common/htmlparse.y"
    {
          if (nonSpace(agxbuse(HTMLstate.str))) {
            htmlerror ("Syntax error: non-space string used after </TABLE>");
            cleanup(); YYABORT;
          }
          (htmlval.tbl) = HTMLstate.tblstack;
          HTMLstate.tblstack = HTMLstate.tblstack->u.p.prev;
        }
    break;

  case 19:
#line 613 "../../lib/common/htmlparse.y"
    { (htmlval.tbl) = (htmlvsp[0].tbl); }
    break;

  case 20:
#line 614 "../../lib/common/htmlparse.y"
    { (htmlval.tbl)=(htmlvsp[-1].tbl); }
    break;

  case 25:
#line 625 "../../lib/common/htmlparse.y"
    { addRow (); }
    break;

  case 29:
#line 632 "../../lib/common/htmlparse.y"
    { setCell((htmlvsp[-1].cell),(htmlvsp[0].tbl),HTML_TBL); }
    break;

  case 31:
#line 633 "../../lib/common/htmlparse.y"
    { setCell((htmlvsp[-1].cell),(htmlvsp[0].txt),HTML_TEXT); }
    break;

  case 33:
#line 634 "../../lib/common/htmlparse.y"
    { setCell((htmlvsp[-1].cell),(htmlvsp[0].img),HTML_IMAGE); }
    break;

  case 35:
#line 635 "../../lib/common/htmlparse.y"
    { setCell((htmlvsp[0].cell),mkText(),HTML_TEXT); }
    break;

  case 37:
#line 638 "../../lib/common/htmlparse.y"
    { (htmlval.img) = (htmlvsp[-1].img); }
    break;

  case 38:
#line 639 "../../lib/common/htmlparse.y"
    { (htmlval.img) = (htmlvsp[0].img); }
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 1821 "y.tab.c"

  htmlvsp -= htmllen;
  htmlssp -= htmllen;


  YY_STACK_PRINT (htmlss, htmlssp);

  *++htmlvsp = htmlval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  htmln = htmlr1[htmln];

  htmlstate = htmlpgoto[htmln - YYNTOKENS] + *htmlssp;
  if (0 <= htmlstate && htmlstate <= YYLAST && htmlcheck[htmlstate] == *htmlssp)
    htmlstate = htmltable[htmlstate];
  else
    htmlstate = htmldefgoto[htmln - YYNTOKENS];

  goto htmlnewstate;


/*------------------------------------.
| htmlerrlab -- here on detecting error |
`------------------------------------*/
htmlerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!htmlerrstatus)
    {
      ++htmlnerrs;
#if YYERROR_VERBOSE
      htmln = htmlpact[htmlstate];

      if (YYPACT_NINF < htmln && htmln < YYLAST)
	{
	  int htmltype = YYTRANSLATE (htmlchar);
	  YYSIZE_T htmlsize0 = htmltnamerr (0, htmltname[htmltype]);
	  YYSIZE_T htmlsize = htmlsize0;
	  YYSIZE_T htmlsize1;
	  int htmlsize_overflow = 0;
	  char *htmlmsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *htmlarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int htmlx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *htmlfmt;
	  char const *htmlf;
	  static char const htmlunexpected[] = "syntax error, unexpected %s";
	  static char const htmlexpecting[] = ", expecting %s";
	  static char const htmlor[] = " or %s";
	  char htmlformat[sizeof htmlunexpected
			+ sizeof htmlexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof htmlor - 1))];
	  char const *htmlprefix = htmlexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int htmlxbegin = htmln < 0 ? -htmln : 0;

	  /* Stay within bounds of both htmlcheck and htmltname.  */
	  int htmlchecklim = YYLAST - htmln;
	  int htmlxend = htmlchecklim < YYNTOKENS ? htmlchecklim : YYNTOKENS;
	  int htmlcount = 1;

	  htmlarg[0] = htmltname[htmltype];
	  htmlfmt = htmlstpcpy (htmlformat, htmlunexpected);

	  for (htmlx = htmlxbegin; htmlx < htmlxend; ++htmlx)
	    if (htmlcheck[htmlx + htmln] == htmlx && htmlx != YYTERROR)
	      {
		if (htmlcount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    htmlcount = 1;
		    htmlsize = htmlsize0;
		    htmlformat[sizeof htmlunexpected - 1] = '\0';
		    break;
		  }
		htmlarg[htmlcount++] = htmltname[htmlx];
		htmlsize1 = htmlsize + htmltnamerr (0, htmltname[htmlx]);
		htmlsize_overflow |= htmlsize1 < htmlsize;
		htmlsize = htmlsize1;
		htmlfmt = htmlstpcpy (htmlfmt, htmlprefix);
		htmlprefix = htmlor;
	      }

	  htmlf = YY_(htmlformat);
	  htmlsize1 = htmlsize + htmlstrlen (htmlf);
	  htmlsize_overflow |= htmlsize1 < htmlsize;
	  htmlsize = htmlsize1;

	  if (!htmlsize_overflow && htmlsize <= YYSTACK_ALLOC_MAXIMUM)
	    htmlmsg = (char *) YYSTACK_ALLOC (htmlsize);
	  if (htmlmsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *htmlp = htmlmsg;
	      int htmli = 0;
	      while ((*htmlp = *htmlf))
		{
		  if (*htmlp == '%' && htmlf[1] == 's' && htmli < htmlcount)
		    {
		      htmlp += htmltnamerr (htmlp, htmlarg[htmli++]);
		      htmlf += 2;
		    }
		  else
		    {
		      htmlp++;
		      htmlf++;
		    }
		}
	      htmlerror (htmlmsg);
	      YYSTACK_FREE (htmlmsg);
	    }
	  else
	    {
	      htmlerror (YY_("syntax error"));
	      goto htmlexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	htmlerror (YY_("syntax error"));
    }



  if (htmlerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (htmlchar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (htmlchar == YYEOF)
	    YYABORT;
        }
      else
	{
	  htmldestruct ("Error: discarding", htmltoken, &htmllval);
	  htmlchar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto htmlerrlab1;


/*---------------------------------------------------.
| htmlerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
htmlerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label htmlerrorlab therefore never appears in user
     code.  */
  if (0)
     goto htmlerrorlab;

htmlvsp -= htmllen;
  htmlssp -= htmllen;
  htmlstate = *htmlssp;
  goto htmlerrlab1;


/*-------------------------------------------------------------.
| htmlerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
htmlerrlab1:
  htmlerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      htmln = htmlpact[htmlstate];
      if (htmln != YYPACT_NINF)
	{
	  htmln += YYTERROR;
	  if (0 <= htmln && htmln <= YYLAST && htmlcheck[htmln] == YYTERROR)
	    {
	      htmln = htmltable[htmln];
	      if (0 < htmln)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (htmlssp == htmlss)
	YYABORT;


      htmldestruct ("Error: popping", htmlstos[htmlstate], htmlvsp);
      YYPOPSTACK;
      htmlstate = *htmlssp;
      YY_STACK_PRINT (htmlss, htmlssp);
    }

  if (htmln == YYFINAL)
    YYACCEPT;

  *++htmlvsp = htmllval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", htmlstos[htmln], htmlvsp, htmllsp);

  htmlstate = htmln;
  goto htmlnewstate;


/*-------------------------------------.
| htmlacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
htmlacceptlab:
  htmlresult = 0;
  goto htmlreturn;

/*-----------------------------------.
| htmlabortlab -- YYABORT comes here.  |
`-----------------------------------*/
htmlabortlab:
  htmlresult = 1;
  goto htmlreturn;

#ifndef htmloverflow
/*-------------------------------------------------.
| htmlexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
htmlexhaustedlab:
  htmlerror (YY_("memory exhausted"));
  htmlresult = 2;
  /* Fall through.  */
#endif

htmlreturn:
  if (htmlchar != YYEOF && htmlchar != YYEMPTY)
     htmldestruct ("Cleanup: discarding lookahead",
		 htmltoken, &htmllval);
  while (htmlssp != htmlss)
    {
      htmldestruct ("Cleanup: popping",
		  htmlstos[*htmlssp], htmlvsp);
      YYPOPSTACK;
    }
#ifndef htmloverflow
  if (htmlss != htmlssa)
    YYSTACK_FREE (htmlss);
#endif
  return htmlresult;
}


#line 642 "../../lib/common/htmlparse.y"


#ifdef OLD
htmllabel_t*
simpleHTML (char* txt)
{
  htmltxt_t*   tobj = mkText (txt);
  htmllabel_t* l = mkLabel(tobj,HTML_TEXT);
  return l;
}
#endif

/* parseHTML:
 * Return parsed label or NULL if failure.
 * Set warn to 0 on success; 1 for warning message; 2 if no expat.
 */
htmllabel_t*
parseHTML (char* txt, int* warn, int charset)
{
  unsigned char buf[SMALLBUF];
  agxbuf        str;
  htmllabel_t*  l;
  sfont_t       dfltf;

  dfltf.cfont = NULL;
  dfltf.pfont = NULL;
  HTMLstate.fontstack = &dfltf;
  HTMLstate.tblstack = 0;
  HTMLstate.lbl = 0;
  HTMLstate.fitemList = dtopen(&fstrDisc, Dtqueue);
  HTMLstate.fparaList = dtopen(&fparaDisc, Dtqueue);

  agxbinit (&str, SMALLBUF, buf);
  HTMLstate.str = &str;
  
  if (initHTMLlexer (txt, &str, charset)) {/* failed: no libexpat - give up */
    *warn = 2;
    l = NULL;
  }
  else {
    htmlparse();
    *warn = clearHTMLlexer ();
    l = HTMLstate.lbl;
  }

  dtclose (HTMLstate.fitemList);
  dtclose (HTMLstate.fparaList);
  
  HTMLstate.fitemList = NULL;
  HTMLstate.fparaList = NULL;
  HTMLstate.fontstack = NULL;
  
  agxbfree (&str);

  return l;
}


