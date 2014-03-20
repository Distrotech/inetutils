/* A Bison parser, made by GNU Bison 2.7.1.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 72 "ftpcmd.y"


#include <config.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/ftp.h>

#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>	/* strtoimax */
#elif defined HAVE_STDINT_H
# include <stdint.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <sys/utsname.h>
/* Include glob.h last, because it may define "const" which breaks
   system headers on some platforms. */
#include <glob.h>

#include "extern.h"

#if !defined NBBY && defined CHAR_BIT
#define NBBY CHAR_BIT
#endif

off_t restart_point;

static char cbuf[512];           /* Command Buffer.  */
static char *fromname;
static int cmd_type;
static int cmd_form;
static int cmd_bytesz;

struct tab
{
  const char	*name;
  short	token;
  short	state;
  short	implemented;	/* 1 if command is implemented */
  const char	*help;
};

static struct tab cmdtab[];
static struct tab sitetab[];
static char *extlist[];
static char *copy         (char *);
static void help          (struct tab *, char *);
static struct tab *lookup (struct tab *, char *);
static void sizecmd       (char *);
static int yylex          (void);
static void yyerror       (const char *s);


/* Line 371 of yacc.c  */
#line 139 "ftpcmd.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     A = 258,
     B = 259,
     C = 260,
     E = 261,
     F = 262,
     I = 263,
     L = 264,
     N = 265,
     P = 266,
     R = 267,
     S = 268,
     T = 269,
     SP = 270,
     CRLF = 271,
     COMMA = 272,
     USER = 273,
     PASS = 274,
     ACCT = 275,
     REIN = 276,
     QUIT = 277,
     PORT = 278,
     PASV = 279,
     TYPE = 280,
     STRU = 281,
     MODE = 282,
     RETR = 283,
     STOR = 284,
     APPE = 285,
     MLFL = 286,
     MAIL = 287,
     MSND = 288,
     MSOM = 289,
     MSAM = 290,
     MRSQ = 291,
     MRCP = 292,
     ALLO = 293,
     REST = 294,
     RNFR = 295,
     RNTO = 296,
     ABOR = 297,
     DELE = 298,
     CWD = 299,
     LIST = 300,
     NLST = 301,
     SITE = 302,
     STAT = 303,
     HELP = 304,
     NOOP = 305,
     MKD = 306,
     RMD = 307,
     PWD = 308,
     CDUP = 309,
     STOU = 310,
     SMNT = 311,
     SYST = 312,
     SIZE = 313,
     MDTM = 314,
     FEAT = 315,
     OPTS = 316,
     EPRT = 317,
     EPSV = 318,
     LPRT = 319,
     LPSV = 320,
     UMASK = 321,
     IDLE = 322,
     CHMOD = 323,
     LEXERR = 324,
     STRING = 325,
     NUMBER = 326,
     CHAR = 327
   };
#endif
/* Tokens.  */
#define A 258
#define B 259
#define C 260
#define E 261
#define F 262
#define I 263
#define L 264
#define N 265
#define P 266
#define R 267
#define S 268
#define T 269
#define SP 270
#define CRLF 271
#define COMMA 272
#define USER 273
#define PASS 274
#define ACCT 275
#define REIN 276
#define QUIT 277
#define PORT 278
#define PASV 279
#define TYPE 280
#define STRU 281
#define MODE 282
#define RETR 283
#define STOR 284
#define APPE 285
#define MLFL 286
#define MAIL 287
#define MSND 288
#define MSOM 289
#define MSAM 290
#define MRSQ 291
#define MRCP 292
#define ALLO 293
#define REST 294
#define RNFR 295
#define RNTO 296
#define ABOR 297
#define DELE 298
#define CWD 299
#define LIST 300
#define NLST 301
#define SITE 302
#define STAT 303
#define HELP 304
#define NOOP 305
#define MKD 306
#define RMD 307
#define PWD 308
#define CDUP 309
#define STOU 310
#define SMNT 311
#define SYST 312
#define SIZE 313
#define MDTM 314
#define FEAT 315
#define OPTS 316
#define EPRT 317
#define EPSV 318
#define LPRT 319
#define LPSV 320
#define UMASK 321
#define IDLE 322
#define CHMOD 323
#define LEXERR 324
#define STRING 325
#define NUMBER 326
#define CHAR 327



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 143 "ftpcmd.y"

	intmax_t i;
	char   *s;


/* Line 387 of yacc.c  */
#line 329 "ftpcmd.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 357 "ftpcmd.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   313

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  20
/* YYNRULES -- Number of rules.  */
#define YYNRULES  89
/* YYNRULES -- Number of states.  */
#define YYNSTATES  290

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   327

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    15,    20,    26,    30,
      35,    40,    45,    50,    59,    65,    71,    77,    81,    87,
      91,    97,   103,   106,   112,   118,   121,   125,   131,   134,
     139,   142,   148,   154,   158,   162,   166,   172,   176,   182,
     187,   194,   200,   208,   218,   223,   231,   237,   240,   246,
     252,   264,   268,   274,   280,   284,   287,   290,   296,   301,
     303,   304,   306,   308,   310,   312,   314,   326,   344,   386,
     388,   390,   392,   394,   398,   400,   404,   406,   408,   412,
     415,   417,   419,   421,   423,   425,   427,   429,   431,   433
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      74,     0,    -1,    -1,    74,    75,    -1,    74,    76,    -1,
      18,    15,    77,    16,    -1,    19,    15,    78,    16,    -1,
      23,    92,    15,    83,    16,    -1,    24,    92,    16,    -1,
      25,    15,    86,    16,    -1,    26,    15,    87,    16,    -1,
      27,    15,    88,    16,    -1,    38,    15,    71,    16,    -1,
      38,    15,    71,    15,    12,    15,    71,    16,    -1,    28,
      92,    15,    89,    16,    -1,    29,    92,    15,    89,    16,
      -1,    30,    92,    15,    89,    16,    -1,    46,    92,    16,
      -1,    46,    92,    15,    70,    16,    -1,    45,    92,    16,
      -1,    45,    92,    15,    89,    16,    -1,    48,    92,    15,
      89,    16,    -1,    48,    16,    -1,    43,    92,    15,    89,
      16,    -1,    41,    92,    15,    89,    16,    -1,    42,    16,
      -1,    44,    92,    16,    -1,    44,    92,    15,    89,    16,
      -1,    49,    16,    -1,    49,    15,    70,    16,    -1,    50,
      16,    -1,    51,    92,    15,    89,    16,    -1,    52,    92,
      15,    89,    16,    -1,    53,    92,    16,    -1,    54,    92,
      16,    -1,    60,    92,    16,    -1,    60,    92,    15,    70,
      16,    -1,    61,    92,    16,    -1,    61,    92,    15,    70,
      16,    -1,    47,    15,    49,    16,    -1,    47,    15,    49,
      15,    70,    16,    -1,    47,    15,    66,    92,    16,    -1,
      47,    15,    66,    92,    15,    91,    16,    -1,    47,    15,
      68,    92,    15,    91,    15,    89,    16,    -1,    47,    15,
      67,    16,    -1,    47,    15,    67,    92,    15,    71,    16,
      -1,    55,    92,    15,    89,    16,    -1,    57,    16,    -1,
      58,    92,    15,    89,    16,    -1,    59,    92,    15,    89,
      16,    -1,    62,    92,    15,    72,    80,    72,    82,    72,
      81,    72,    16,    -1,    63,    92,    16,    -1,    63,    92,
      15,    80,    16,    -1,    64,    92,    15,    84,    16,    -1,
      65,    92,    16,    -1,    22,    16,    -1,     1,    16,    -1,
      40,    92,    15,    89,    16,    -1,    39,    15,    79,    16,
      -1,    70,    -1,    -1,    70,    -1,    71,    -1,    71,    -1,
      71,    -1,    70,    -1,    71,    17,    71,    17,    71,    17,
      71,    17,    71,    17,    71,    -1,    71,    17,    71,    17,
      71,    17,    71,    17,    71,    17,    71,    17,    71,    17,
      71,    17,    71,    -1,    71,    17,    71,    17,    71,    17,
      71,    17,    71,    17,    71,    17,    71,    17,    71,    17,
      71,    17,    71,    17,    71,    17,    71,    17,    71,    17,
      71,    17,    71,    17,    71,    17,    71,    17,    71,    17,
      71,    17,    71,    17,    71,    -1,    10,    -1,    14,    -1,
       5,    -1,     3,    -1,     3,    15,    85,    -1,     6,    -1,
       6,    15,    85,    -1,     8,    -1,     9,    -1,     9,    15,
      79,    -1,     9,    79,    -1,     7,    -1,    12,    -1,    11,
      -1,    13,    -1,     4,    -1,     5,    -1,    90,    -1,    70,
      -1,    71,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   183,   183,   185,   191,   195,   200,   206,   245,   250,
     288,   300,   312,   316,   320,   326,   332,   338,   343,   349,
     354,   360,   366,   370,   376,   391,   395,   400,   406,   410,
     428,   432,   438,   444,   449,   454,   467,   479,   486,   494,
     498,   503,   514,   530,   544,   550,   568,   574,   618,   633,
     661,   812,   817,   832,   873,   879,   884,   890,   905,   917,
     922,   925,   929,   933,   946,   950,   954,  1003,  1065,  1134,
    1138,  1142,  1149,  1154,  1159,  1164,  1169,  1173,  1178,  1184,
    1192,  1196,  1200,  1207,  1211,  1215,  1222,  1263,  1267,  1296
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "A", "B", "C", "E", "F", "I", "L", "N",
  "P", "R", "S", "T", "SP", "CRLF", "COMMA", "USER", "PASS", "ACCT",
  "REIN", "QUIT", "PORT", "PASV", "TYPE", "STRU", "MODE", "RETR", "STOR",
  "APPE", "MLFL", "MAIL", "MSND", "MSOM", "MSAM", "MRSQ", "MRCP", "ALLO",
  "REST", "RNFR", "RNTO", "ABOR", "DELE", "CWD", "LIST", "NLST", "SITE",
  "STAT", "HELP", "NOOP", "MKD", "RMD", "PWD", "CDUP", "STOU", "SMNT",
  "SYST", "SIZE", "MDTM", "FEAT", "OPTS", "EPRT", "EPSV", "LPRT", "LPSV",
  "UMASK", "IDLE", "CHMOD", "LEXERR", "STRING", "NUMBER", "CHAR",
  "$accept", "cmd_list", "cmd", "rcmd", "username", "password",
  "byte_size", "net_proto", "tcp_port", "net_addr", "host_port",
  "long_host_port", "form_code", "type_code", "struct_code", "mode_code",
  "pathname", "pathstring", "octal_number", "check_login", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    73,    74,    74,    74,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    76,    76,    77,
      78,    78,    79,    80,    81,    82,    83,    84,    84,    85,
      85,    85,    86,    86,    86,    86,    86,    86,    86,    86,
      87,    87,    87,    88,    88,    88,    89,    90,    91,    92
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     4,     4,     5,     3,     4,
       4,     4,     4,     8,     5,     5,     5,     3,     5,     3,
       5,     5,     2,     5,     5,     2,     3,     5,     2,     4,
       2,     5,     5,     3,     3,     3,     5,     3,     5,     4,
       6,     5,     7,     9,     4,     7,     5,     2,     5,     5,
      11,     3,     5,     5,     3,     2,     2,     5,     4,     1,
       0,     1,     1,     1,     1,     1,    11,    17,    41,     1,
       1,     1,     1,     3,     1,     3,     1,     1,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,    89,    89,     0,
       0,     0,    89,    89,    89,     0,     0,    89,    89,     0,
      89,    89,    89,    89,     0,    89,     0,     0,    89,    89,
      89,    89,    89,     0,    89,    89,    89,    89,    89,    89,
      89,    89,     3,     4,    56,     0,    60,    55,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,     0,     0,     0,     0,     0,    22,     0,     0,    28,
      30,     0,     0,     0,     0,     0,    47,     0,     0,     0,
       0,     0,     0,     0,     0,    59,     0,    61,     0,     0,
       8,    72,    74,    76,    77,     0,    80,    82,    81,     0,
      84,    85,    83,     0,     0,     0,     0,     0,    62,     0,
       0,     0,     0,     0,    26,     0,    19,     0,    17,     0,
      89,    89,    89,     0,     0,     0,     0,    33,    34,     0,
       0,     0,     0,    35,     0,    37,     0,     0,    51,     0,
      54,     5,     6,     0,     0,     0,     0,     0,    79,     9,
      10,    11,    87,     0,    86,     0,     0,     0,    12,    58,
       0,     0,     0,     0,     0,     0,     0,    39,     0,    44,
       0,     0,     0,    29,     0,     0,     0,     0,     0,     0,
       0,     0,    63,     0,     0,     0,     0,     7,    71,    69,
      70,    73,    75,    78,    14,    15,    16,     0,    57,    24,
      23,    27,    20,    18,     0,     0,    41,     0,     0,    21,
      31,    32,    46,    48,    49,    36,    38,     0,    52,     0,
      53,     0,     0,    40,    88,     0,     0,     0,     0,     0,
       0,     0,    42,    45,     0,    65,     0,     0,     0,    13,
       0,     0,     0,     0,    43,    64,     0,     0,     0,     0,
       0,     0,    50,     0,     0,     0,     0,     0,    66,     0,
       0,     0,     0,     0,     0,    67,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    68
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    42,    43,    86,    88,   109,   183,   246,   236,
     144,   185,   191,    95,    99,   103,   153,   154,   225,    48
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -106
static const yytype_int16 yypact[] =
{
    -106,    41,  -106,    -7,   -12,    -3,     1,  -106,  -106,    16,
      19,    24,  -106,  -106,  -106,    33,    61,  -106,  -106,     3,
    -106,  -106,  -106,  -106,    92,    14,    46,   102,  -106,  -106,
    -106,  -106,  -106,   116,  -106,  -106,  -106,  -106,  -106,  -106,
    -106,  -106,  -106,  -106,  -106,    63,    64,  -106,   123,   124,
      69,     4,     9,   126,   127,   128,    68,    73,   130,   131,
    -106,   132,    58,    94,    96,    48,  -106,   133,    79,  -106,
    -106,   135,   136,   137,   138,   140,  -106,   141,   142,   104,
     106,   143,   108,   144,   145,  -106,   146,  -106,   147,    81,
    -106,   149,   150,  -106,   -13,   151,  -106,  -106,  -106,   152,
    -106,  -106,  -106,   153,    90,    90,    90,   110,  -106,   154,
      90,    90,    90,    90,  -106,    90,  -106,   101,  -106,   112,
    -106,   156,  -106,    90,   157,    90,    90,  -106,  -106,    90,
      90,    90,   105,  -106,   107,  -106,   109,    95,  -106,   111,
    -106,  -106,  -106,   159,   158,   103,   103,    73,  -106,  -106,
    -106,  -106,  -106,   162,  -106,   163,   164,   171,  -106,  -106,
     168,   169,   170,   172,   173,   174,   117,  -106,   115,  -106,
     176,   177,   178,  -106,   179,   180,   181,   182,   183,   184,
     185,    95,  -106,   186,   187,   189,   122,  -106,  -106,  -106,
    -106,  -106,  -106,  -106,  -106,  -106,  -106,   188,  -106,  -106,
    -106,  -106,  -106,  -106,   190,   139,  -106,   148,   139,  -106,
    -106,  -106,  -106,  -106,  -106,  -106,  -106,   155,  -106,   160,
    -106,   191,   161,  -106,  -106,   193,   195,   192,   165,   196,
     166,   198,  -106,  -106,    90,  -106,   167,   175,   199,  -106,
     201,   194,   203,   197,  -106,  -106,   200,   202,   204,   206,
     207,   205,  -106,   208,   209,   211,   210,   212,  -106,   213,
     214,   216,   215,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   230,   229,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,  -106
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -106,  -106,  -106,  -106,  -106,  -106,   -90,    31,  -106,  -106,
    -106,  -106,    72,  -106,  -106,  -106,  -105,  -106,     7,    15
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
     155,   156,   147,    45,   148,   160,   161,   162,   163,    44,
     164,    96,    46,   100,   101,    97,    98,    47,   172,    60,
     174,   175,   102,    49,   176,   177,   178,    53,    54,    55,
      66,    50,    58,    59,    51,    61,    62,    63,    64,    52,
      67,     2,     3,    71,    72,    73,    74,    75,    56,    77,
      78,    79,    80,    81,    82,    83,    84,   193,   108,     4,
       5,    68,    69,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    91,   113,   114,    92,    57,    93,    94,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,   119,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    65,   188,   115,
     116,   117,   118,   189,   120,   121,   122,   190,    70,   132,
     133,   134,   135,   137,   138,   157,   158,   166,   167,   240,
     205,   206,    76,    85,    87,   168,   170,   171,    89,   107,
      90,   104,   105,   106,   108,   110,   111,   112,   123,   124,
     125,   126,   143,   127,   128,   129,   130,   131,   136,   139,
     152,   140,   141,   142,   145,   146,   182,   149,   150,   151,
     159,   165,   169,   173,   187,   179,   186,   180,   194,   195,
     196,   181,   184,   197,   198,   199,   200,   204,   201,   202,
     203,   207,   208,   221,   209,   210,   211,   212,   213,   214,
     215,   216,   218,   222,   219,   220,   223,   234,   230,   232,
     224,   233,   217,   237,   239,   227,   243,   244,   192,   226,
     247,   251,   252,     0,   253,     0,   256,   228,   257,     0,
     260,   229,   231,   262,   264,   235,   266,   238,   268,   241,
     270,     0,   272,     0,   274,     0,   242,   276,   278,     0,
     280,     0,   282,     0,   284,     0,   286,     0,   288,     0,
       0,     0,     0,     0,     0,   245,     0,     0,   248,     0,
       0,     0,   249,   250,     0,     0,   254,     0,     0,   255,
       0,   258,     0,   259,     0,   261,   263,     0,     0,   265,
       0,   267,     0,   269,     0,   271,     0,   273,     0,   275,
     277,     0,     0,   279,     0,   281,     0,   283,     0,   285,
       0,   287,     0,   289
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-106)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
     105,   106,    15,    15,    94,   110,   111,   112,   113,    16,
     115,     7,    15,     4,     5,    11,    12,    16,   123,    16,
     125,   126,    13,     8,   129,   130,   131,    12,    13,    14,
      16,    15,    17,    18,    15,    20,    21,    22,    23,    15,
      25,     0,     1,    28,    29,    30,    31,    32,    15,    34,
      35,    36,    37,    38,    39,    40,    41,   147,    71,    18,
      19,    15,    16,    22,    23,    24,    25,    26,    27,    28,
      29,    30,     3,    15,    16,     6,    15,     8,     9,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    49,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    15,     5,    15,
      16,    15,    16,    10,    66,    67,    68,    14,    16,    15,
      16,    15,    16,    15,    16,    15,    16,    15,    16,   234,
      15,    16,    16,    70,    70,   120,   121,   122,    15,    71,
      16,    15,    15,    15,    71,    15,    15,    15,    15,    70,
      15,    15,    71,    16,    16,    15,    15,    15,    15,    15,
      70,    16,    16,    16,    15,    15,    71,    16,    16,    16,
      16,    70,    16,    16,    16,    70,    17,    70,    16,    16,
      16,    72,    71,    12,    16,    16,    16,    70,    16,    16,
      16,    15,    15,    71,    16,    16,    16,    16,    16,    16,
      16,    16,    16,    15,    17,    16,    16,    15,    17,    16,
      71,    16,   181,    17,    16,   208,    17,    16,   146,    71,
      17,    17,    16,    -1,    17,    -1,    17,    72,    17,    -1,
      17,    71,    71,    17,    17,    70,    17,    71,    17,    72,
      17,    -1,    17,    -1,    17,    -1,    71,    17,    17,    -1,
      17,    -1,    17,    -1,    17,    -1,    17,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,    71,    -1,    -1,    71,    -1,
      -1,    -1,    72,    71,    -1,    -1,    71,    -1,    -1,    71,
      -1,    71,    -1,    71,    -1,    71,    71,    -1,    -1,    71,
      -1,    71,    -1,    71,    -1,    71,    -1,    71,    -1,    71,
      71,    -1,    -1,    71,    -1,    71,    -1,    71,    -1,    71,
      -1,    71,    -1,    71
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    74,     0,     1,    18,    19,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    75,    76,    16,    15,    15,    16,    92,    92,
      15,    15,    15,    92,    92,    92,    15,    15,    92,    92,
      16,    92,    92,    92,    92,    15,    16,    92,    15,    16,
      16,    92,    92,    92,    92,    92,    16,    92,    92,    92,
      92,    92,    92,    92,    92,    70,    77,    70,    78,    15,
      16,     3,     6,     8,     9,    86,     7,    11,    12,    87,
       4,     5,    13,    88,    15,    15,    15,    71,    71,    79,
      15,    15,    15,    15,    16,    15,    16,    15,    16,    49,
      66,    67,    68,    15,    70,    15,    15,    16,    16,    15,
      15,    15,    15,    16,    15,    16,    15,    15,    16,    15,
      16,    16,    16,    71,    83,    15,    15,    15,    79,    16,
      16,    16,    70,    89,    90,    89,    89,    15,    16,    16,
      89,    89,    89,    89,    89,    70,    15,    16,    92,    16,
      92,    92,    89,    16,    89,    89,    89,    89,    89,    70,
      70,    72,    71,    80,    71,    84,    17,    16,     5,    10,
      14,    85,    85,    79,    16,    16,    16,    12,    16,    16,
      16,    16,    16,    16,    70,    15,    16,    15,    15,    16,
      16,    16,    16,    16,    16,    16,    16,    80,    16,    17,
      16,    71,    15,    16,    71,    91,    71,    91,    72,    71,
      17,    71,    16,    16,    15,    70,    82,    17,    71,    16,
      89,    72,    71,    17,    16,    71,    81,    17,    71,    72,
      71,    17,    16,    17,    71,    71,    17,    17,    71,    71,
      17,    71,    17,    71,    17,    71,    17,    71,    17,    71,
      17,    71,    17,    71,    17,    71,    17,    71,    17,    71,
      17,    71,    17,    71,    17,    71,    17,    71,    17,    71
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
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

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YYUSE (yytype);
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
/* Line 1787 of yacc.c  */
#line 186 "ftpcmd.y"
    {
			free (fromname);
			fromname = (char *) 0;
			restart_point = (off_t) 0;
		}
    break;

  case 5:
/* Line 1787 of yacc.c  */
#line 196 "ftpcmd.y"
    {
			user ((yyvsp[(3) - (4)].s));
			free ((yyvsp[(3) - (4)].s));
		}
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 201 "ftpcmd.y"
    {
			pass ((yyvsp[(3) - (4)].s));
			memset ((yyvsp[(3) - (4)].s), 0, strlen ((yyvsp[(3) - (4)].s)));
			free ((yyvsp[(3) - (4)].s));
		}
    break;

  case 7:
/* Line 1787 of yacc.c  */
#line 207 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i))
			  {
			    if ((yyvsp[(4) - (5)].i)
				&& ((his_addr.ss_family == AF_INET
				     && memcmp (&((struct sockaddr_in *) &his_addr)->sin_addr,
						&((struct sockaddr_in *) &data_dest)->sin_addr,
						sizeof (struct in_addr))
					== 0
				     && ntohs (((struct sockaddr_in *) &data_dest)->sin_port)
					> IPPORT_RESERVED)
				    ||
				    (his_addr.ss_family == AF_INET6
				     && memcmp (&((struct sockaddr_in6 *) &his_addr)->sin6_addr,
						&((struct sockaddr_in6 *) &data_dest)->sin6_addr,
						sizeof (struct in6_addr))
					== 0
				     && ntohs (((struct sockaddr_in6 *) &data_dest)->sin6_port)
					> IPPORT_RESERVED)
				   )
			       )
			      {
				usedefault = 0;
				if (pdata >= 0)
				  {
				    close (pdata);
				    pdata = -1;
				  }
				reply (200, "PORT command successful.");
			      }
			    else
			      {
				usedefault = 1;
				memset (&data_dest, 0, sizeof (data_dest));
				reply (500, "Illegal PORT Command");
			      }
			  }
		}
    break;

  case 8:
/* Line 1787 of yacc.c  */
#line 246 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  passive (PASSIVE_PASV, AF_INET);
		}
    break;

  case 9:
/* Line 1787 of yacc.c  */
#line 251 "ftpcmd.y"
    {
			switch (cmd_type)
			  {
			  case TYPE_A:
			    if (cmd_form == FORM_N)
			      {
				reply (200, "Type set to A.");
				type = cmd_type;
				form = cmd_form;
			      }
			    else
			      reply (504, "Form must be N.");
			    break;

			  case TYPE_E:
			    reply (504, "Type E not implemented.");
			    break;

			  case TYPE_I:
			    reply (200, "Type set to I.");
			    type = cmd_type;
			    break;

			  case TYPE_L:
#if defined NBBY && NBBY == 8
			    if (cmd_bytesz == 8)
			      {
				reply (200, "Type set to L (byte size 8).");
				type = cmd_type;
			      }
			    else
			      reply (504, "Byte size must be 8.");
#else /* NBBY == 8 */
			  UNIMPLEMENTED for NBBY != 8
#endif /* NBBY == 8 */
			  }
		}
    break;

  case 10:
/* Line 1787 of yacc.c  */
#line 289 "ftpcmd.y"
    {
			switch ((yyvsp[(3) - (4)].i))
			  {
			  case STRU_F:
			    reply (200, "STRU F ok.");
			    break;

			  default:
			    reply (504, "Unimplemented STRU type.");
			  }
		}
    break;

  case 11:
/* Line 1787 of yacc.c  */
#line 301 "ftpcmd.y"
    {
			switch ((yyvsp[(3) - (4)].i))
			  {
			  case MODE_S:
			    reply (200, "MODE S ok.");
			    break;

			  default:
			    reply (502, "Unimplemented MODE type.");
			  }
		}
    break;

  case 12:
/* Line 1787 of yacc.c  */
#line 313 "ftpcmd.y"
    {
			reply (202, "ALLO command ignored.");
		}
    break;

  case 13:
/* Line 1787 of yacc.c  */
#line 317 "ftpcmd.y"
    {
			reply (202, "ALLO command ignored.");
		}
    break;

  case 14:
/* Line 1787 of yacc.c  */
#line 321 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  retrieve ((char *) 0, (yyvsp[(4) - (5)].s));
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 15:
/* Line 1787 of yacc.c  */
#line 327 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  store ((yyvsp[(4) - (5)].s), "w", 0);
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 16:
/* Line 1787 of yacc.c  */
#line 333 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  store ((yyvsp[(4) - (5)].s), "a", 0);
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 17:
/* Line 1787 of yacc.c  */
#line 339 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  send_file_list (".");
		}
    break;

  case 18:
/* Line 1787 of yacc.c  */
#line 344 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  send_file_list ((yyvsp[(4) - (5)].s));
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 19:
/* Line 1787 of yacc.c  */
#line 350 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  retrieve ("/bin/ls -lgA", "");
		}
    break;

  case 20:
/* Line 1787 of yacc.c  */
#line 355 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  retrieve ("/bin/ls -lgA %s", (yyvsp[(4) - (5)].s));
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 21:
/* Line 1787 of yacc.c  */
#line 361 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  statfilecmd ((yyvsp[(4) - (5)].s));
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 22:
/* Line 1787 of yacc.c  */
#line 367 "ftpcmd.y"
    {
			statcmd ();
		}
    break;

  case 23:
/* Line 1787 of yacc.c  */
#line 371 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  delete ((yyvsp[(4) - (5)].s));
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 24:
/* Line 1787 of yacc.c  */
#line 377 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i))
			  {
			    if (fromname)
			      {
				renamecmd (fromname, (yyvsp[(4) - (5)].s));
				free (fromname);
				fromname = (char *) 0;
			      }
			    else
			      reply (503, "Bad sequence of commands.");
			  }
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 25:
/* Line 1787 of yacc.c  */
#line 392 "ftpcmd.y"
    {
			reply (225, "ABOR command successful.");
		}
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 396 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  cwd (cred.homedir);
		}
    break;

  case 27:
/* Line 1787 of yacc.c  */
#line 401 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  cwd ((yyvsp[(4) - (5)].s));
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 28:
/* Line 1787 of yacc.c  */
#line 407 "ftpcmd.y"
    {
			help (cmdtab, (char *) 0);
		}
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 411 "ftpcmd.y"
    {
			char *cp = (yyvsp[(3) - (4)].s);

			if (strncasecmp (cp, "SITE", 4) == 0)
			  {
			    cp = (yyvsp[(3) - (4)].s) + 4;
			    if (*cp == ' ')
			      cp++;
			    if (*cp)
			      help (sitetab, cp);
			    else
			      help (sitetab, (char *) 0);
			  }
			else
			  help (cmdtab, (yyvsp[(3) - (4)].s));
			free ((yyvsp[(3) - (4)].s));
		}
    break;

  case 30:
/* Line 1787 of yacc.c  */
#line 429 "ftpcmd.y"
    {
			reply (200, "NOOP command successful.");
		}
    break;

  case 31:
/* Line 1787 of yacc.c  */
#line 433 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  makedir ((yyvsp[(4) - (5)].s));
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 439 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  removedir ((yyvsp[(4) - (5)].s));
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 33:
/* Line 1787 of yacc.c  */
#line 445 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  pwd ();
		}
    break;

  case 34:
/* Line 1787 of yacc.c  */
#line 450 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  cwd ("..");
		}
    break;

  case 35:
/* Line 1787 of yacc.c  */
#line 455 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  {
			    char **name;

			    lreply (211, "Supported extensions:");
			    for (name = extlist; *name; name++)
			      printf (" %s\r\n", *name);
			    reply (211, "End");
			  }
		}
    break;

  case 36:
/* Line 1787 of yacc.c  */
#line 468 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i))
			  {
			    reply (501, "Not accepting arguments.");
			    free ((yyvsp[(4) - (5)].s));
			  }
		}
    break;

  case 37:
/* Line 1787 of yacc.c  */
#line 480 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  {
			    reply (501, "Must have an argument.");
			  }
		}
    break;

  case 38:
/* Line 1787 of yacc.c  */
#line 487 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i))
			  {
			    reply (501, "No options are available.");
			    free ((yyvsp[(4) - (5)].s));
			  }
		}
    break;

  case 39:
/* Line 1787 of yacc.c  */
#line 495 "ftpcmd.y"
    {
			help (sitetab, (char *) 0);
		}
    break;

  case 40:
/* Line 1787 of yacc.c  */
#line 499 "ftpcmd.y"
    {
			help (sitetab, (yyvsp[(5) - (6)].s));
			free ((yyvsp[(5) - (6)].s));
		}
    break;

  case 41:
/* Line 1787 of yacc.c  */
#line 504 "ftpcmd.y"
    {
			int oldmask;

			if ((yyvsp[(4) - (5)].i))
			  {
			    oldmask = umask (0);
			    umask (oldmask);
			    reply (200, "Current UMASK is %03o", oldmask);
			  }
		}
    break;

  case 42:
/* Line 1787 of yacc.c  */
#line 515 "ftpcmd.y"
    {
			int oldmask;

			if ((yyvsp[(4) - (7)].i))
			  {
			    if (((yyvsp[(6) - (7)].i) == -1) || ((yyvsp[(6) - (7)].i) > 0777))
			      reply (501, "Bad UMASK value");
			    else
			      {
				oldmask = umask ((yyvsp[(6) - (7)].i));
				reply (200, "UMASK set to %03o (was %03o)",
				      (yyvsp[(6) - (7)].i), oldmask);
			      }
			  }
		}
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 531 "ftpcmd.y"
    {
			if ((yyvsp[(4) - (9)].i) && ((yyvsp[(8) - (9)].s) != NULL))
			  {
			    if ((yyvsp[(6) - (9)].i) > 0777)
			      reply (501,
				     "CHMOD: Mode value must be between 0 and 0777");
			    else if (chmod ((yyvsp[(8) - (9)].s), (yyvsp[(6) - (9)].i)) < 0)
			      perror_reply (550, (yyvsp[(8) - (9)].s));
			    else
			      reply (200, "CHMOD command successful.");
			  }
			free ((yyvsp[(8) - (9)].s));
		}
    break;

  case 44:
/* Line 1787 of yacc.c  */
#line 545 "ftpcmd.y"
    {
			reply (200,
			       "Current IDLE time limit is %d seconds; max %d",
			       timeout, maxtimeout);
		}
    break;

  case 45:
/* Line 1787 of yacc.c  */
#line 551 "ftpcmd.y"
    {
			if ((yyvsp[(4) - (7)].i))
			  {
			    if ((yyvsp[(6) - (7)].i) < 30 || (yyvsp[(6) - (7)].i) > maxtimeout)
			      reply (501,
				     "Maximum IDLE time must be between 30 and %d seconds",
				     maxtimeout);
			    else
			      {
				timeout = (yyvsp[(6) - (7)].i);
				alarm ((unsigned) timeout);
				reply (200,
				       "Maximum IDLE time set to %d seconds",
				       timeout);
			      }
			  }
		}
    break;

  case 46:
/* Line 1787 of yacc.c  */
#line 569 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  store ((yyvsp[(4) - (5)].s), "w", 1);
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 47:
/* Line 1787 of yacc.c  */
#line 575 "ftpcmd.y"
    {
		        const char *sys_type; /* Official rfc-defined os type.  */
			char *version = 0; /* A more specific type. */

#ifdef HAVE_UNAME
			struct utsname u;

			if (uname (&u) >= 0)
			  {
			    version = malloc (strlen (u.sysname) + 1
					      + strlen (u.release) + 1);
			    if (version)
			      sprintf (version, "%s %s", u.sysname, u.release);
			  }
#else /* !HAVE_UNAME */
# ifdef BSD
			version = "BSD";
# endif /* BSD */
#endif /* !HAVE_UNAME */

#if defined unix || defined __unix || defined __unix__
			sys_type = "UNIX";
#else
			sys_type = "UNKNOWN";
#endif

			if (!no_version && version)
			  reply (215, "%s Type: L%d Version: %s",
				 sys_type, NBBY, version);
			else
			  reply (215, "%s Type: L%d", sys_type, NBBY);

#ifdef HAVE_UNAME
			free (version);
#endif
		}
    break;

  case 48:
/* Line 1787 of yacc.c  */
#line 619 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  sizecmd ((yyvsp[(4) - (5)].s));
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 49:
/* Line 1787 of yacc.c  */
#line 634 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s) != NULL)
			  {
			    struct stat stbuf;

			    if (stat ((yyvsp[(4) - (5)].s), &stbuf) < 0)
			      reply (550, "%s: %s", (yyvsp[(4) - (5)].s), strerror (errno));
			    else if (!S_ISREG (stbuf.st_mode))
			      reply (550, "%s: not a plain file.", (yyvsp[(4) - (5)].s));
			    else
			      {
				struct tm *t;

				t = gmtime (&stbuf.st_mtime);
				reply (213,
				       "%04d%02d%02d%02d%02d%02d",
				       1900 + t->tm_year, t->tm_mon+1,
				       t->tm_mday, t->tm_hour,
				       t->tm_min, t->tm_sec);
			      }
			  }
			free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 50:
/* Line 1787 of yacc.c  */
#line 662 "ftpcmd.y"
    {
			usedefault = 0;
			if (pdata >= 0)
			  {
			    close (pdata);
			    pdata = -1;
			  }
			/* A first sanity check.  */
			if ((yyvsp[(2) - (11)].i)				/* valid login */
			    && ((yyvsp[(5) - (11)].i) > 0)			/* valid protocols */
			    && ((yyvsp[(4) - (11)].i) > 32 && (yyvsp[(4) - (11)].i) < 127)	/* legal first delimiter */
							/* identical delimiters */
			    && ((yyvsp[(4) - (11)].i) == (yyvsp[(6) - (11)].i) && (yyvsp[(4) - (11)].i) == (yyvsp[(8) - (11)].i) && (yyvsp[(4) - (11)].i) == (yyvsp[(10) - (11)].i)))
			  {
			    /* We only accept connections using
			     * the same address family as is
			     * currently in use, unless we
			     * detect IPv4-mapped-to-IPv6.
			     */
			    if (his_addr.ss_family == (yyvsp[(5) - (11)].i)
				|| ((yyvsp[(5) - (11)].i) == AF_INET6
				    && his_addr.ss_family == AF_INET)
				|| ((yyvsp[(5) - (11)].i) == AF_INET
				    && his_addr.ss_family == AF_INET6))
			      {
				int err;
				char p[8];
				struct addrinfo hints, *res;

				memset (&hints, 0, sizeof (hints));
				snprintf (p, sizeof (p), "%jd", (yyvsp[(9) - (11)].i) & 0xffff);
				hints.ai_family = (yyvsp[(5) - (11)].i);
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

				err = getaddrinfo ((yyvsp[(7) - (11)].s), p, &hints, &res);
				if (err)
				  reply (500, "Illegal EPRT Command");
				else if (/* sanity check */
					 (his_addr.ss_family == AF_INET
					  && memcmp (&((struct sockaddr_in *) &his_addr)->sin_addr,
						     &((struct sockaddr_in *) res->ai_addr)->sin_addr,
						     sizeof (struct in_addr))
					     == 0
					  && ntohs (((struct sockaddr_in *) res->ai_addr)->sin_port)
					     > IPPORT_RESERVED
					 )
					 ||
					 (his_addr.ss_family == AF_INET6
					  && memcmp (&((struct sockaddr_in6 *) &his_addr)->sin6_addr,
						     &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr,
						     sizeof (struct in6_addr))
					     == 0
					  && ntohs (((struct sockaddr_in6 *) res->ai_addr)->sin6_port)
					     > IPPORT_RESERVED
					 )
					 ||
					 (his_addr.ss_family == AF_INET
					  && res->ai_family == AF_INET6
					  && IN6_IS_ADDR_V4MAPPED (&((struct sockaddr_in6 *) res->ai_addr)->sin6_addr)
					  && memcmp (&((struct sockaddr_in *) &his_addr)->sin_addr,
						     &((struct in_addr *) &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr)[3],
						     sizeof (struct in_addr))
					     == 0
					  && ntohs (((struct sockaddr_in6 *) res->ai_addr)->sin6_port)
					     > IPPORT_RESERVED
					 )
					 ||
					 (his_addr.ss_family == AF_INET6
					  && res->ai_family == AF_INET
					  && IN6_IS_ADDR_V4MAPPED (&((struct sockaddr_in6 *) &his_addr)->sin6_addr)
					  && memcmp (&((struct in_addr *) &((struct sockaddr_in6 *) &his_addr)->sin6_addr)[3],
						     &((struct sockaddr_in *) res->ai_addr)->sin_addr,
						     sizeof (struct in_addr))
					     == 0
					  && ntohs (((struct sockaddr_in *) res->ai_addr)->sin_port)
					     > IPPORT_RESERVED
					 )
					)
				  {
				    /* In the case of IPv4 mapped as IPv6,
				     * the addresses were proven to coincide,
				     * only the extraction remains.
				     * Since non-mapped is the standard,
				     * test that situation first.
				     */
				    if (his_addr.ss_family == res->ai_family)
				      {
					memcpy (&data_dest, res->ai_addr,
						res->ai_addrlen);
					data_dest_len = res->ai_addrlen;
				      }
				    else if (his_addr.ss_family == AF_INET
					     && res->ai_family == AF_INET6)
				      {
					/* `his_addr' contains the reduced
					 * IPv4 address.
					 */
					memcpy (&data_dest, &his_addr,
						sizeof (struct sockaddr_in));
					data_dest_len =
					  sizeof (struct sockaddr_in);
					((struct sockaddr_in *) &data_dest)->sin_port =
					  ((struct sockaddr_in6 *) res->ai_addr)->sin6_port;
				      }
				    else
				      {
					/* `res->ai_addr' contains the reduced
					 * IPv4 address, but the connection
					 * stands on `his_addr', which is
					 * an IPv4-to-IPv6-mapped address.
					 */
					memcpy (&data_dest, &his_addr,
						sizeof (struct sockaddr_in6));
					data_dest_len =
					  sizeof (struct sockaddr_in6);
					((struct sockaddr_in6 *) &data_dest)->sin6_port =
					  ((struct sockaddr_in *) res->ai_addr)->sin_port;
				      }

				    freeaddrinfo (res);
				    reply (200, "EPRT command successful.");
				  }
				else
				  {
				    /* failed identity check */
				    if (res)
				      freeaddrinfo (res);
				    reply (500, "Illegal EPRT Command");
				  }
			      }
			    else
			      /* Not fit for established connection.  */
			      reply (522,
				     "Network protocol not supported, use (%d)",
				     ((yyvsp[(5) - (11)].i) == 1) ? 2 : 1);
			  }
			else if ((yyvsp[(2) - (11)].i) && ((yyvsp[(5) - (11)].i) <= 0))
			    reply (522,
				   "Network protocol not supported, use (1,2)");
			else if ((yyvsp[(2) - (11)].i))
			  /* Incorrect delimiters detected,
			   * the other conditions are met.
			   */
			  reply (500, "Illegal EPRT Command");
		}
    break;

  case 51:
/* Line 1787 of yacc.c  */
#line 813 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  passive (PASSIVE_EPSV, AF_UNSPEC);
		}
    break;

  case 52:
/* Line 1787 of yacc.c  */
#line 818 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i))
			  {
			    if ((yyvsp[(4) - (5)].i) > 0)
			      passive (PASSIVE_EPSV, (yyvsp[(4) - (5)].i));
			    else
			      reply (522,
				     "Network protocol not supported, use (1,2)");
			  }
		}
    break;

  case 53:
/* Line 1787 of yacc.c  */
#line 833 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (5)].i))
			  {
			    if ((yyvsp[(4) - (5)].i) &&
				((his_addr.ss_family == AF_INET
				  && memcmp (&((struct sockaddr_in *) &his_addr)->sin_addr,
					     &((struct sockaddr_in *) &data_dest)->sin_addr,
					     sizeof (struct in_addr)) == 0
				  && ntohs (((struct sockaddr_in *) &data_dest)->sin_port)
					> IPPORT_RESERVED)
				 ||
				 (his_addr.ss_family == AF_INET6
				  && memcmp (&((struct sockaddr_in6 *) &his_addr)->sin6_addr,
					     &((struct sockaddr_in6 *) &data_dest)->sin6_addr,
					     sizeof (struct in6_addr)) == 0
				  && ntohs (((struct sockaddr_in6 *) &data_dest)->sin6_port)
					> IPPORT_RESERVED)
				)
			       )
			      {
				usedefault = 0;
				if (pdata >= 0)
				  {
				    close (pdata);
				    pdata = -1;
				  }
				  reply (200, "LPRT command successful.");
			      }
			    else
			      {
				usedefault = 1;
				memset (&data_dest, 0, sizeof (data_dest));
				reply (500, "Illegal LPRT Command");
			      }
			  } /* check_login */
		}
    break;

  case 54:
/* Line 1787 of yacc.c  */
#line 874 "ftpcmd.y"
    {
			if ((yyvsp[(2) - (3)].i))
			  passive (PASSIVE_LPSV, 0 /* not used */);
		}
    break;

  case 55:
/* Line 1787 of yacc.c  */
#line 880 "ftpcmd.y"
    {
			reply (221, "Goodbye.");
			dologout (0);
		}
    break;

  case 56:
/* Line 1787 of yacc.c  */
#line 885 "ftpcmd.y"
    {
			yyerrok;
		}
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 891 "ftpcmd.y"
    {
			restart_point = (off_t) 0;
			if ((yyvsp[(2) - (5)].i) && (yyvsp[(4) - (5)].s))
			  {
			    free (fromname);
			    fromname = renamefrom ((yyvsp[(4) - (5)].s));
			  }
			if (fromname == (char *) 0 && (yyvsp[(4) - (5)].s))
			  free ((yyvsp[(4) - (5)].s));
		}
    break;

  case 58:
/* Line 1787 of yacc.c  */
#line 906 "ftpcmd.y"
    {
		        free (fromname);
			fromname = (char *) 0;
			restart_point = (yyvsp[(3) - (4)].i);
			reply (350, "Restarting at %jd. %s",
			       (intmax_t) restart_point,
			       "Send STORE or RETRIEVE to initiate transfer.");
		}
    break;

  case 60:
/* Line 1787 of yacc.c  */
#line 922 "ftpcmd.y"
    {
			(yyval.s) = (char *) calloc (1, sizeof (char));
		}
    break;

  case 63:
/* Line 1787 of yacc.c  */
#line 934 "ftpcmd.y"
    {
			/* Rewrite as valid address family.  */
			if ((yyvsp[(1) - (1)].i) == 1)
			  (yyval.i) = AF_INET;
			else if ((yyvsp[(1) - (1)].i) == 2)
			  (yyval.i) = AF_INET6;
			else
			  (yyval.i) = -1;	/* Invalid protocol.  */
		}
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 956 "ftpcmd.y"
    {
			int err;
			char a[INET6_ADDRSTRLEN], p[8];
			struct addrinfo hints, *res;

			snprintf (a, sizeof (a), "%jd.%jd.%jd.%jd",
				  (yyvsp[(1) - (11)].i) & 0xff, (yyvsp[(3) - (11)].i) & 0xff,
				  (yyvsp[(5) - (11)].i) & 0xff, (yyvsp[(7) - (11)].i) & 0xff);
			snprintf (p, sizeof (p), "%jd",
				  (((yyvsp[(9) - (11)].i) & 0xff) << 8) + ((yyvsp[(11) - (11)].i) & 0xff));
			memset (&hints, 0, sizeof (hints));
			hints.ai_family = his_addr.ss_family;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

			if (his_addr.ss_family == AF_INET6)
			  {
			    /* IPv4 mapped to IPv6.  */
			    hints.ai_family = AF_INET6;
#ifdef AI_V4MAPPED
			    hints.ai_flags |= AI_V4MAPPED;
#endif
			    snprintf (a, sizeof (a),
				      "::ffff:%jd.%jd.%jd.%jd",
				      (yyvsp[(1) - (11)].i) & 0xff, (yyvsp[(3) - (11)].i) & 0xff,
				      (yyvsp[(5) - (11)].i) & 0xff, (yyvsp[(7) - (11)].i) & 0xff);
			  }

			err = getaddrinfo (a, p, &hints, &res);
			if (err)
			  {
			    reply (550, "Address failure: %s,%s", a, p);
			    memset (&data_dest, 0, sizeof (data_dest));
			    data_dest_len = 0;
			    (yyval.i) = 0;
			  }
			else
			  {
			    memcpy (&data_dest, res->ai_addr, res->ai_addrlen);
			    data_dest_len = res->ai_addrlen;
			    freeaddrinfo (res);
			    (yyval.i) = 1;
			  }
		}
    break;

  case 67:
/* Line 1787 of yacc.c  */
#line 1006 "ftpcmd.y"
    {
			int err;
			char a[INET6_ADDRSTRLEN], p[8];
			struct addrinfo hints, *res;

			/* Well formed input for IPv4?  */
			if ((yyvsp[(1) - (17)].i) != 4 || (yyvsp[(3) - (17)].i) != 4 || (yyvsp[(13) - (17)].i) != 2
			    || (yyvsp[(5) - (17)].i) < 0 || (yyvsp[(5) - (17)].i) > 255 || (yyvsp[(7) - (17)].i) < 0 || (yyvsp[(7) - (17)].i) > 255
			    || (yyvsp[(9) - (17)].i) < 0 || (yyvsp[(9) - (17)].i) > 255 || (yyvsp[(11) - (17)].i) < 0 || (yyvsp[(11) - (17)].i) > 255
			    || (yyvsp[(15) - (17)].i) < 0 || (yyvsp[(15) - (17)].i) > 255
			    || (yyvsp[(17) - (17)].i) < 0 || (yyvsp[(17) - (17)].i) > 255)
			  {
			    reply (500, "Invalid address.");
			    memset (&data_dest, 0, sizeof (data_dest));
			    data_dest_len = 0;
			    (yyval.i) = 0;
			  }
			else
			  {
			    snprintf (a, sizeof (a), "%jd.%jd.%jd.%jd",
				      (yyvsp[(5) - (17)].i), (yyvsp[(7) - (17)].i), (yyvsp[(9) - (17)].i), (yyvsp[(11) - (17)].i));
			    snprintf (p, sizeof (p), "%jd", ((yyvsp[(15) - (17)].i) << 8) + (yyvsp[(17) - (17)].i));

			    memset (&hints, 0, sizeof (hints));
			    hints.ai_family = his_addr.ss_family;
			    hints.ai_socktype = SOCK_STREAM;
			    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

			    if (his_addr.ss_family == AF_INET6)
			      {
				/* IPv4 mapped to IPv6.  */
				hints.ai_family = AF_INET6;
#ifdef AI_V4MAPPED
				hints.ai_flags |= AI_V4MAPPED;
#endif
				snprintf (a, sizeof (a),
					  "::ffff:%jd.%jd.%jd.%jd",
					  (yyvsp[(5) - (17)].i), (yyvsp[(7) - (17)].i), (yyvsp[(9) - (17)].i), (yyvsp[(11) - (17)].i));
			      }

			    err = getaddrinfo (a, p, &hints, &res);
			    if (err)
			      {
				reply (550, "LPRT address failure: %s,%s",
				       a, p);
				memset (&data_dest, 0, sizeof (data_dest));
				data_dest_len = 0;
				(yyval.i) = 0;
			      }
			    else
			      {
				memcpy (&data_dest, res->ai_addr,
					res->ai_addrlen);
				data_dest_len = res->ai_addrlen;
				freeaddrinfo (res);
				(yyval.i) = 1;
			      }
			  }
		}
    break;

  case 68:
/* Line 1787 of yacc.c  */
#line 1071 "ftpcmd.y"
    {
			int err;
			char a[INET6_ADDRSTRLEN], p[8];
			struct addrinfo hints, *res;

			/* Well formed input for IPv6?  */
			if ((yyvsp[(1) - (41)].i) != 6 || (yyvsp[(3) - (41)].i) != 16 || (yyvsp[(37) - (41)].i) != 2
			    || (yyvsp[(5) - (41)].i) < 0 || (yyvsp[(5) - (41)].i) > 255 || (yyvsp[(7) - (41)].i) < 0 || (yyvsp[(7) - (41)].i) > 255
			    || (yyvsp[(9) - (41)].i) < 0 || (yyvsp[(9) - (41)].i) > 255 || (yyvsp[(11) - (41)].i) < 0 || (yyvsp[(11) - (41)].i) > 255
			    || (yyvsp[(13) - (41)].i) < 0 || (yyvsp[(13) - (41)].i) > 255 || (yyvsp[(15) - (41)].i) < 0 || (yyvsp[(15) - (41)].i) > 255
			    || (yyvsp[(17) - (41)].i) < 0 || (yyvsp[(17) - (41)].i) > 255 || (yyvsp[(19) - (41)].i) < 0 || (yyvsp[(19) - (41)].i) > 255
			    || (yyvsp[(21) - (41)].i) < 0 || (yyvsp[(21) - (41)].i) > 255 || (yyvsp[(23) - (41)].i) < 0 || (yyvsp[(23) - (41)].i) > 255
			    || (yyvsp[(25) - (41)].i) < 0 || (yyvsp[(25) - (41)].i) > 255 || (yyvsp[(27) - (41)].i) < 0 || (yyvsp[(27) - (41)].i) > 255
			    || (yyvsp[(29) - (41)].i) < 0 || (yyvsp[(29) - (41)].i) > 255 || (yyvsp[(31) - (41)].i) < 0 || (yyvsp[(31) - (41)].i) > 255
			    || (yyvsp[(33) - (41)].i) < 0 || (yyvsp[(33) - (41)].i) > 255 || (yyvsp[(35) - (41)].i) < 0 || (yyvsp[(35) - (41)].i) > 255
			    || (yyvsp[(39) - (41)].i) < 0 || (yyvsp[(39) - (41)].i) > 255 || (yyvsp[(41) - (41)].i) < 0 || (yyvsp[(41) - (41)].i) > 255)
			  {
			    reply (500, "Invalid address.");
			    memset (&data_dest, 0, sizeof (data_dest));
			    data_dest_len = 0;
			    (yyval.i) = 0;
			  }
			else
			  {
			    snprintf (a, sizeof (a),
				     "%02jx%02jx:%02jx%02jx:"
				     "%02jx%02jx:%02jx%02jx:"
				     "%02jx%02jx:%02jx%02jx:"
				     "%02jx%02jx:%02jx%02jx",
				      (yyvsp[(5) - (41)].i), (yyvsp[(7) - (41)].i), (yyvsp[(9) - (41)].i), (yyvsp[(11) - (41)].i),
				      (yyvsp[(13) - (41)].i), (yyvsp[(15) - (41)].i), (yyvsp[(17) - (41)].i), (yyvsp[(19) - (41)].i),
				      (yyvsp[(21) - (41)].i), (yyvsp[(23) - (41)].i), (yyvsp[(25) - (41)].i), (yyvsp[(27) - (41)].i),
				      (yyvsp[(29) - (41)].i), (yyvsp[(31) - (41)].i), (yyvsp[(33) - (41)].i), (yyvsp[(35) - (41)].i));
			    snprintf (p, sizeof (p), "%jd",
				      ((yyvsp[(39) - (41)].i) << 8) + (yyvsp[(41) - (41)].i));

			    memset (&hints, 0, sizeof (hints));
			    hints.ai_family = his_addr.ss_family;
			    hints.ai_socktype = SOCK_STREAM;
			    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

			    err = getaddrinfo (a, p, &hints, &res);
			    if (err)
			      {
				reply (550, "LPRT address failure: %s,%s",
				       a, p);
				memset (&data_dest, 0, sizeof (data_dest));
				data_dest_len = 0;
				(yyval.i) = 0;
			      }
			    else
			      {
				memcpy (&data_dest, res->ai_addr,
					res->ai_addrlen);
				data_dest_len = res->ai_addrlen;
				freeaddrinfo (res);
				(yyval.i) = 1;
			      }
			  }
		}
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 1135 "ftpcmd.y"
    {
			(yyval.i) = FORM_N;
		}
    break;

  case 70:
/* Line 1787 of yacc.c  */
#line 1139 "ftpcmd.y"
    {
			(yyval.i) = FORM_T;
		}
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 1143 "ftpcmd.y"
    {
			(yyval.i) = FORM_C;
		}
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 1150 "ftpcmd.y"
    {
			cmd_type = TYPE_A;
			cmd_form = FORM_N;
		}
    break;

  case 73:
/* Line 1787 of yacc.c  */
#line 1155 "ftpcmd.y"
    {
			cmd_type = TYPE_A;
			cmd_form = (yyvsp[(3) - (3)].i);
		}
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 1160 "ftpcmd.y"
    {
			cmd_type = TYPE_E;
			cmd_form = FORM_N;
		}
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 1165 "ftpcmd.y"
    {
			cmd_type = TYPE_E;
			cmd_form = (yyvsp[(3) - (3)].i);
		}
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 1170 "ftpcmd.y"
    {
			cmd_type = TYPE_I;
		}
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 1174 "ftpcmd.y"
    {
			cmd_type = TYPE_L;
			cmd_bytesz = NBBY;
		}
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 1179 "ftpcmd.y"
    {
			cmd_type = TYPE_L;
			cmd_bytesz = (yyvsp[(3) - (3)].i);
		}
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 1185 "ftpcmd.y"
    {
			cmd_type = TYPE_L;
			cmd_bytesz = (yyvsp[(2) - (2)].i);
		}
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 1193 "ftpcmd.y"
    {
			(yyval.i) = STRU_F;
		}
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 1197 "ftpcmd.y"
    {
			(yyval.i) = STRU_R;
		}
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 1201 "ftpcmd.y"
    {
			(yyval.i) = STRU_P;
		}
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 1208 "ftpcmd.y"
    {
			(yyval.i) = MODE_S;
		}
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 1212 "ftpcmd.y"
    {
			(yyval.i) = MODE_B;
		}
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 1216 "ftpcmd.y"
    {
			(yyval.i) = MODE_C;
		}
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 1223 "ftpcmd.y"
    {
			/*
			 * Problem: this production is used for all pathname
			 * processing, but only gives a 550 error reply.
			 * This is a valid reply in some cases but not in others.
			 */
			if (cred.logged_in && (yyvsp[(1) - (1)].s) && *(yyvsp[(1) - (1)].s) == '~')
			  {
			    glob_t gl;
			    int flags = GLOB_NOCHECK;

#ifdef GLOB_BRACE
			    flags |= GLOB_BRACE;
#endif
#ifdef GLOB_QUOTE
			    flags |= GLOB_QUOTE;
#endif
#ifdef GLOB_TILDE
			    flags |= GLOB_TILDE;
#endif

			    memset (&gl, 0, sizeof (gl));
			    if (glob ((yyvsp[(1) - (1)].s), flags, NULL, &gl)
				|| gl.gl_pathc == 0)
			      {
				reply (550, "not found");
				(yyval.s) = NULL;
			      }
			    else
			      (yyval.s) = strdup (gl.gl_pathv[0]);

			    globfree (&gl);
			    free ((yyvsp[(1) - (1)].s));
			  }
			else
			  (yyval.s) = (yyvsp[(1) - (1)].s);
		}
    break;

  case 88:
/* Line 1787 of yacc.c  */
#line 1268 "ftpcmd.y"
    {
			int ret, dec, multby, digit;

			/*
			 * Convert a number that was read as decimal number
			 * to what it would be if it had been read as octal.
			 */
			dec = (yyvsp[(1) - (1)].i);
			multby = 1;
			ret = 0;
			while (dec)
			  {
			    digit = dec % 10;
			    if (digit > 7)
			      {
				ret = -1;
				break;
			      }
			    ret += digit * multby;
			    multby *= 8;
			    dec /= 10;
			  }
			(yyval.i) = ret;
		}
    break;

  case 89:
/* Line 1787 of yacc.c  */
#line 1296 "ftpcmd.y"
    {
			if (cred.logged_in)
			  (yyval.i) = 1;
			else
			  {
			    reply (530, "Please login with USER and PASS.");
			    (yyval.i) = 0;
			  }
		}
    break;


/* Line 1787 of yacc.c  */
#line 3108 "ftpcmd.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2050 of yacc.c  */
#line 1307 "ftpcmd.y"


#define	CMD	0	/* beginning of command */
#define	ARGS	1	/* expect miscellaneous arguments */
#define	STR1	2	/* expect SP followed by STRING */
#define	STR2	3	/* expect STRING (must be STR2 + 1)*/
#define	OSTR	4	/* optional SP then STRING */
#define	ZSTR1	5	/* SP then optional STRING */
#define	ZSTR2	6	/* optional STRING after SP (must be ZSTR1 + 1) */
#define	SITECMD	7	/* SITE command */
#define	NSTR	8	/* Number followed by a string */
#define	DLIST	9	/* SP and delimited list for EPRT/EPSV */

static struct tab cmdtab[] = {
  /* In the order defined by RFC 959.  See also RFC 1123.  */
  /* Access control commands.  */
  { "USER", USER, STR1, 1,	"<sp> username" },
  { "PASS", PASS, ZSTR1, 1,	"<sp> password" },
  { "ACCT", ACCT, STR1, 0,	"(specify account)" },
  { "CWD",  CWD,  OSTR, 1,	"[ <sp> directory-name ]" },
  { "CDUP", CDUP, ARGS, 1,	"(change to parent directory)" },
  { "SMNT", SMNT, ARGS, 0,	"(structure mount)" },
  { "REIN", REIN, ARGS, 0,	"(reinitialize server state)" },
  { "QUIT", QUIT, ARGS, 1,	"(terminate service)", },
  /* Transfer parameter commands.  */
  { "PORT", PORT, ARGS, 1,	"<sp> b0, b1, b2, b3, b4" },
  { "PASV", PASV, ARGS, 1,	"(set server in passive mode)" },
  { "TYPE", TYPE, ARGS, 1,	"<sp> [ A | E | I | L ]" },
  { "STRU", STRU, ARGS, 1,	"(specify file structure)" },
  { "MODE", MODE, ARGS, 1,	"(specify transfer mode)" },
  /* FTP service commands.  */
  { "RETR", RETR, STR1, 1,	"<sp> file-name" },
  { "STOR", STOR, STR1, 1,	"<sp> file-name" },
  { "STOU", STOU, STR1, 1,	"<sp> file-name" },
  { "APPE", APPE, STR1, 1,	"<sp> file-name" },
  { "ALLO", ALLO, ARGS, 1,	"allocate storage (vacuously)" },
  { "REST", REST, ARGS, 1,	"<sp> offset (restart command)" },
  { "RNFR", RNFR, STR1, 1,	"<sp> file-name" },
  { "RNTO", RNTO, STR1, 1,	"<sp> file-name" },
  { "ABOR", ABOR, ARGS, 1,	"(abort operation)" },
  { "DELE", DELE, STR1, 1,	"<sp> file-name" },
  { "RMD",  RMD,  STR1, 1,	"<sp> path-name" },
  { "MKD",  MKD,  STR1, 1,	"<sp> path-name" },
  { "PWD",  PWD,  ARGS, 1,	"(return current directory)" },
  { "LIST", LIST, OSTR, 1,	"[ <sp> path-name ]" },
  { "NLST", NLST, OSTR, 1,	"[ <sp> path-name ]" },
  { "SITE", SITE, SITECMD, 1,	"site-cmd [ <sp> arguments ]" },
  { "SYST", SYST, ARGS, 1,	"(get type of operating system)" },
  { "STAT", STAT, OSTR, 1,	"[ <sp> path-name ]" },
  { "HELP", HELP, OSTR, 1,	"[ <sp> <string> ]" },
  { "NOOP", NOOP, ARGS, 1,	"" },
  /* Experimental commands, as mentioned in RFC 1123.  Now obsolete.  */
  { "XMKD", MKD,  STR1, 1,	"<sp> path-name" },
  { "XRMD", RMD,  STR1, 1,	"<sp> path-name" },
  { "XPWD", PWD,  ARGS, 1,	"(return current directory)" },
  { "XCUP", CDUP, ARGS, 1,	"(change to parent directory)" },
  { "XCWD", CWD,  OSTR, 1,	"[ <sp> directory-name ]" },
  /* Commands in RFC 2389.  */
  { "FEAT", FEAT, OSTR, 1,	"(display command extensions)" },
  /* XXX: Replace OSTR once some functionality exists.  */
  { "OPTS", OPTS, OSTR, 1,	"<sp> cmd-name [ <sp> options ]" },
  /* Commands in RFC 3659.  */
  { "SIZE", SIZE, OSTR, 1,	"<sp> path-name" },
  { "MDTM", MDTM, OSTR, 1,	"<sp> path-name" },
  /* Unimplemented, but reserved in RFC ???.  */
  { "MLFL", MLFL, OSTR, 0,	"(mail file)" },
  { "MAIL", MAIL, OSTR, 0,	"(mail to user)" },
  { "MSND", MSND, OSTR, 0,	"(mail send to terminal)" },
  { "MSOM", MSOM, OSTR, 0,	"(mail send to terminal or mailbox)" },
  { "MSAM", MSAM, OSTR, 0,	"(mail send to terminal and mailbox)" },
  { "MRSQ", MRSQ, OSTR, 0,	"(mail recipient scheme question)" },
  { "MRCP", MRCP, STR1, 0,	"(mail recipient)" },
  /* Extended addressing in RFC 2428.  */
  { "EPRT", EPRT, DLIST, 1,	"<sp> <d> proto <d> addr <d> port <d>" },
  { "EPSV", EPSV, ARGS, 1,	"[ <sp> af ]" },
  /* Long addressing in RFC 1639.  Obsoleted in RFC 5797.  */
  { "LPRT", LPRT, ARGS, 1,	"<sp> af,hal,h0..hn,2,p0,p1" },
  { "LPSV", LPSV, ARGS, 1,	"(set server in long passive mode)" },
  { NULL,   0,    0,    0,	NULL }
};

static struct tab sitetab[] = {
  { "CHMOD", CHMOD, NSTR, 1,	"<sp> mode <sp> file-name" },
  { "HELP", HELP, OSTR, 1,	"[ <sp> <string> ]" },
  { "IDLE", IDLE, ARGS, 1,	"[ <sp> maximum-idle-time ]" },
  { "UMASK", UMASK, ARGS, 1,	"[ <sp> umask ]" },
  { NULL,   0,    0,    0,	NULL }
};

/* Extensions beyond RFC 959 and RFC 2389.  Ordered as implemented.  */
static char *extlist[] = {
  "MDTM", "SIZE", "REST STREAM",
  "EPRT", "EPSV", "LPRT", "LPSV",
  NULL };

static struct tab *
lookup (struct tab *p, char *cmd)
{
  for (; p->name != NULL; p++)
    if (strcmp (cmd, p->name) == 0)
      return (p);
  return (0);
}

#include <arpa/telnet.h>

/*
 * getline - a hacked up version of fgets to ignore TELNET escape codes.
 */
char *
telnet_fgets (char *s, int n, FILE *iop)
{
  int c;
  register char *cs;

  cs = s;
/* tmpline may contain saved command from urgent mode interruption */
  for (c = 0; tmpline[c] != '\0' && --n > 0; ++c)
    {
      *cs++ = tmpline[c];
      if (tmpline[c] == '\n')
	{
	  *cs++ = '\0';
	  if (debug)
	    syslog (LOG_DEBUG, "command: %s", s);
	  tmpline[0] = '\0';
	  return (s);
	}

      if (c == 0)
	tmpline[0] = '\0';
    }

  while ((c = getc (iop)) != EOF)
    {
      c &= 0377;
      if (c == IAC)
	{
	  c = getc (iop);
	  if (c != EOF)
	    {
	      c &= 0377;
	      switch (c)
		{
		case WILL:
		case WONT:
		  c = getc (iop);
		  printf ("%c%c%c", IAC, DONT, 0377 & c);
		  fflush (stdout);
		  continue;

		case DO:
		case DONT:
		  c = getc (iop);
		  printf ("%c%c%c", IAC, WONT, 0377 & c);
		  fflush (stdout);
		  continue;

		case IAC:
		  break;

		default:
		  continue;	/* ignore command */
		}
	    }
	}

      *cs++ = c;
      if (--n <= 0 || c == '\n')
	break;
    }

  if (c == EOF && cs == s)
    return (NULL);

  *cs++ = '\0';

  if (debug)
    {
      if (!cred.guest && strncasecmp ("pass ", s, 5) == 0)
	{
	  /* Don't syslog passwords.  */
	  syslog (LOG_DEBUG, "command: %.5s ???", s);
	}
      else
	{
	  register char *cp;
	  register int len;

	  /* Don't syslog trailing CR-LF.  */
	  len = strlen (s);
	  cp = s + len - 1;

	  while (cp >= s && (*cp == '\n' || *cp == '\r'))
	    {
	      --cp;
	      --len;
	    }

	  syslog (LOG_DEBUG, "command: %.*s", len, s);
	}
    }
  return (s);
}

void
toolong (int signo)
{
  (void) signo;
  reply (421, "Timeout (%d seconds): closing control connection.",
	 timeout);
  if (logging)
    syslog (LOG_INFO, "User %s timed out after %d seconds",
	    (cred.name ? cred.name : "unknown"), timeout);
  dologout (1);
}

static int
yylex (void)
{
  static int cpos, state;
  char *cp, *cp2;
  struct tab *p;
  int n;
  char c;

  for (;;)
    {
      switch (state)
	{
	case CMD:
	  signal (SIGALRM, toolong);
	  alarm ((unsigned) timeout);
	  if (telnet_fgets (cbuf, sizeof (cbuf)-1, stdin) == NULL)
	    {
	      reply (221, "You could at least say goodbye.");
	      dologout (0);
	    }
	  alarm (0);

#ifdef HAVE_SETPROCTITLE
	  if (strncasecmp (cbuf, "PASS", 4) != 0)
	    setproctitle ("%s: %s", proctitle, cbuf);
#endif /* HAVE_SETPROCTITLE */

	  cp = strchr (cbuf, '\r');
	  if (cp)
	    {
	      *cp++ = '\n';
	      *cp = '\0';
	    }

	  cp = strpbrk (cbuf, " \n");
	  if (cp)
	    cpos = cp - cbuf;

	  if (cpos == 0)
	    cpos = 4;

	  c = cbuf[cpos];
	  cbuf[cpos] = '\0';
	  upper (cbuf);
	  p = lookup (cmdtab, cbuf);
	  cbuf[cpos] = c;

	  if (p != 0)
	    {
	      if (p->implemented == 0)
		{
		  nack (p->name);
		  longjmp (errcatch, 0);
		  /* NOTREACHED */
		}
	      state = p->state;
	      yylval.s = (char*) p->name;
	      return (p->token);
	    }
	  break;	/* Command not known.  */

	case SITECMD:
	  if (cbuf[cpos] == ' ')
	    {
	      cpos++;
	      return (SP);
	    }
	  cp = &cbuf[cpos];

	  cp2 = strpbrk (cp, " \n");
	  if (cp2)
	    cpos = cp2 - cbuf;

	  c = cbuf[cpos];
	  cbuf[cpos] = '\0';
	  upper (cp);
	  p = lookup (sitetab, cp);
	  cbuf[cpos] = c;

	  if (p != 0)
	    {
	      if (p->implemented == 0)
		{
		  state = CMD;
		  nack (p->name);
		  longjmp (errcatch, 0);
		  /* NOTREACHED */
		}

	      state = p->state;
	      yylval.s = (char*) p->name;
	      return (p->token);
	    }
	  state = CMD;
	  break;	/* Command not known.  */

	case OSTR:
	  if (cbuf[cpos] == '\n')
	    {
	      state = CMD;
	      return (CRLF);
	    }

	case STR1:
	case ZSTR1:
	dostr1:
	  if (cbuf[cpos] == ' ')
	    {
	      cpos++;
	      if (state == OSTR)
		state = STR2;
	      else
		++state;

	      return (SP);
	    }
	  /* Intentional continuation.  */

	case ZSTR2:
	  if (cbuf[cpos] == '\n')
	    {
	      state = CMD;
	      return (CRLF);
	    }

	case STR2:
	  cp = &cbuf[cpos];
	  n = strlen (cp);
	  cpos += n - 1;
	  /*
	   * Make sure the string is nonempty and newline terminated.
	   */
	  if (n > 1 && cbuf[cpos] == '\n')
	    {
	      cbuf[cpos] = '\0';
	      yylval.s = copy (cp);
	      cbuf[cpos] = '\n';
	      state = ARGS;
	      return (STRING);
	    }
	  break;	/* Empty string, missing NL.  */

	case NSTR:
	  if (cbuf[cpos] == ' ')
	    {
	      cpos++;
	      return (SP);
	    }
	  if (isdigit (cbuf[cpos]))
	    {
	      cp = &cbuf[cpos];
	      while (isdigit (cbuf[++cpos]))
		;

	      c = cbuf[cpos];
	      cbuf[cpos] = '\0';
	      yylval.i = atoi (cp);
	      cbuf[cpos] = c;
	      state = STR1;
	      return (NUMBER);
	    }
	  state = STR1;
	  goto dostr1;

	case DLIST:
	  /* Either numerical strings or
	   * address strings for IPv4 and IPv6.
	   * The consist of hexadecimal chars,
	   * colon and periods.  A period can
	   * not begin a valid address.  */
	  if (isxdigit (cbuf[cpos]) || cbuf[cpos] == ':')
	    {
	      int is_num = 1;	/* Only to turn off.  */

	      cp = &cbuf[cpos];
	      while (isxdigit (cbuf[cpos])
		     || cbuf[cpos] == ':'
		     || cbuf[cpos] == '.')
		{
		  if (!isdigit (cbuf[cpos]))
		    is_num = 0;
		  cpos++;
		}

	      c = cbuf[cpos];
	      cbuf[cpos] = '\0';
	      if (is_num)
		{
		  yylval.i = atoi (cp);
		  cbuf[cpos] = c;
		  return (NUMBER);
		}
	      else
		{
		  yylval.s = copy (cp);
		  cbuf[cpos] = c;
		  return (STRING);
		}
	    }

	  c = cbuf[cpos++];
	  switch (c)
	    {
	    case ' ':
	      return (SP);

	    case '\n':
	      state = CMD;
	      return (CRLF);

	    default:
	      yylval.i = c;
	      return (CHAR);
	    }
	  break;	/* Not reachable.  */

	case ARGS:
	  if (isdigit (cbuf[cpos]))
	    {
	      cp = &cbuf[cpos];
	      while (isdigit (cbuf[++cpos]))
		;

	      c = cbuf[cpos];
	      cbuf[cpos] = '\0';
	      yylval.i = strtoimax (cp, NULL, 10);	/* off_t */
	      cbuf[cpos] = c;
	      return (NUMBER);
	    }

	  switch (cbuf[cpos++])
	    {
	    case '\n':
	      state = CMD;
	      return (CRLF);

	    case ' ':
	      return (SP);

	    case ',':
	      return (COMMA);

	    case 'A':
	    case 'a':
	      return (A);

	    case 'B':
	    case 'b':
	      return (B);

	    case 'C':
	    case 'c':
	      return (C);

	    case 'E':
	    case 'e':
	      return (E);

	    case 'F':
	    case 'f':
	      return (F);

	    case 'I':
	    case 'i':
	      return (I);

	    case 'L':
	    case 'l':
	      return (L);

	    case 'N':
	    case 'n':
	      return (N);

	    case 'P':
	    case 'p':
	      return (P);

	    case 'R':
	    case 'r':
	      return (R);

	    case 'S':
	    case 's':
	      return (S);

	    case 'T':
	    case 't':
	      return (T);
	    }
	  break;	/* No number, not in [\n ,aAbBcCeEfFiIlLnNpPrRsSttT] */

	default:
	  fatal ("Unknown state in scanner.");
	}

      /*
       * Analysis: Cases when this point is reached.
       *
       *  CMD:      command not known
       *  SITECMD:  site command not known (state changed to CMD)
       *
       *  OSTR, STR1, ZSTR1, STR2, ZSTR2, NSTR:
       *            empty string or string without NL
       *
       *  ARGS:     not a number, not a special character
       */

      /*
       * Issue a new error message only if the parser has not
       * yet reported a complaint.  Without this precaution
       * two messages would be directed to the client, thus
       * upsetting all following exchange.
       */
      if (!yynerrs)
	yyerror ("command not recognized");

      state = CMD;
      longjmp (errcatch, 0);
    } /* for (;;) */
}

void
upper (char *s)
{
  while (*s != '\0')
    {
      if (islower (*s))
	*s = toupper (*s);
      s++;
    }
}

static char *
copy (char *s)
{
  char *p;

  p = malloc (strlen (s) + 1);
  if (p == NULL)
    fatal ("Ran out of memory.");

  strcpy (p, s);
  return (p);
}

static void
help (struct tab *ctab, char *s)
{
  struct tab *c;
  int width, NCMDS;
  const char *help_type;

  if (ctab == sitetab)
    help_type = "SITE ";
  else
    help_type = "";

  width = 0, NCMDS = 0;
  for (c = ctab; c->name != NULL; c++)
    {
      int len = strlen (c->name);

      if (len > width)
	width = len;

      NCMDS++;
    }

  width = (width + 8) &~ 7;

  if (s == 0)
    {
      int i, j, w;
      int columns, lines;

      lreply (214, "The following %scommands are recognized %s.",
	      help_type, "(* =>'s unimplemented)");

      columns = 76 / width;
      if (columns == 0)
	columns = 1;

      lines = (NCMDS + columns - 1) / columns;

      for (i = 0; i < lines; i++)
	{
	  printf ("   ");
	  for (j = 0; j < columns; j++)
	    {
	      c = ctab + j * lines + i;
	      printf ("%s%c", c->name, c->implemented ? ' ' : '*');

	      if (c + lines >= &ctab[NCMDS])
		break;

	      w = strlen (c->name) + 1;
	      while (w < width)
		{
		  putchar (' ');
		  w++;
		}
	    }
	  printf ("\r\n");
	}
      fflush (stdout);
      reply (214, "Direct comments to ftp-bugs@%s.", hostname);
      return;
    }

  upper (s);

  c = lookup (ctab, s);
  if (c == (struct tab *) 0)
    {
      reply (502, "Unknown command %s.", s);
      return;
    }

  if (c->implemented)
    reply (214, "Syntax: %s%s %s", help_type, c->name, c->help);
  else
    reply (214, "%s%-*s\t%s; unimplemented.", help_type,
	   width, c->name, c->help);
}

static void
sizecmd (char *filename)
{
  switch (type)
    {
    case TYPE_L:
    case TYPE_I:
      {
	struct stat stbuf;

	if (stat (filename, &stbuf) < 0 || !S_ISREG (stbuf.st_mode))
	  reply (550, "%s: not a plain file.", filename);
	else
	  reply (213, "%ju", (uintmax_t) stbuf.st_size);
	break;
      }

    case TYPE_A:
      {
	FILE *fin;
	int c;
	off_t count;
	struct stat stbuf;

	fin = fopen (filename, "r");
	if (fin == NULL)
	  {
	    perror_reply (550, filename);
	    return;
	  }

	if (fstat (fileno (fin), &stbuf) < 0 || !S_ISREG (stbuf.st_mode))
	  {
	    reply (550, "%s: not a plain file.", filename);
	    fclose (fin);
	    return;
	  }

	count = 0;
	while ((c = getc (fin)) != EOF)
	  {
	    if (c == '\n')	/* will get expanded to \r\n */
	      count++;
	    count++;
	  }
	fclose (fin);

	reply (213, "%jd", (intmax_t) count);
	break;
      }

    default:
      reply (504, "SIZE not implemented for Type %c.", "?AEIL"[type]);
    }
}

static void
yyerror (const char *s)
{
  char *cp;

  cp = strchr (cbuf, '\n');
  if (cp != NULL)
    *cp = '\0';

  reply (500, "'%s': %s", cbuf, (s ? s : "command not understood."));
}
