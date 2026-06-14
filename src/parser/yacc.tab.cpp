/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "/home/yc_dada/db2026/src/parser/yacc.y"

#include "ast.h"
#include "yacc.tab.h"
#include <iostream>
#include <memory>
#include <stdexcept>

int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

void yyerror(YYLTYPE *locp, const char* s) {
    std::cerr << "Parser Error at line " << locp->first_line << " column " << locp->first_column << ": " << s << std::endl;
}

using namespace ast;

#line 87 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SHOW = 3,                       /* SHOW  */
  YYSYMBOL_TABLES = 4,                     /* TABLES  */
  YYSYMBOL_CREATE = 5,                     /* CREATE  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_DROP = 7,                       /* DROP  */
  YYSYMBOL_DESC = 8,                       /* DESC  */
  YYSYMBOL_INSERT = 9,                     /* INSERT  */
  YYSYMBOL_INTO = 10,                      /* INTO  */
  YYSYMBOL_VALUES = 11,                    /* VALUES  */
  YYSYMBOL_DELETE = 12,                    /* DELETE  */
  YYSYMBOL_FROM = 13,                      /* FROM  */
  YYSYMBOL_ASC = 14,                       /* ASC  */
  YYSYMBOL_ORDER = 15,                     /* ORDER  */
  YYSYMBOL_BY = 16,                        /* BY  */
  YYSYMBOL_UNION = 17,                     /* UNION  */
  YYSYMBOL_WHERE = 18,                     /* WHERE  */
  YYSYMBOL_UPDATE = 19,                    /* UPDATE  */
  YYSYMBOL_SET = 20,                       /* SET  */
  YYSYMBOL_SELECT = 21,                    /* SELECT  */
  YYSYMBOL_INT = 22,                       /* INT  */
  YYSYMBOL_CHAR = 23,                      /* CHAR  */
  YYSYMBOL_FLOAT = 24,                     /* FLOAT  */
  YYSYMBOL_INDEX = 25,                     /* INDEX  */
  YYSYMBOL_AND = 26,                       /* AND  */
  YYSYMBOL_JOIN = 27,                      /* JOIN  */
  YYSYMBOL_EXIT = 28,                      /* EXIT  */
  YYSYMBOL_HELP = 29,                      /* HELP  */
  YYSYMBOL_TXN_BEGIN = 30,                 /* TXN_BEGIN  */
  YYSYMBOL_TXN_COMMIT = 31,                /* TXN_COMMIT  */
  YYSYMBOL_TXN_ABORT = 32,                 /* TXN_ABORT  */
  YYSYMBOL_TXN_ROLLBACK = 33,              /* TXN_ROLLBACK  */
  YYSYMBOL_ORDER_BY = 34,                  /* ORDER_BY  */
  YYSYMBOL_ENABLE_NESTLOOP = 35,           /* ENABLE_NESTLOOP  */
  YYSYMBOL_ENABLE_SORTMERGE = 36,          /* ENABLE_SORTMERGE  */
  YYSYMBOL_EXPLAIN = 37,                   /* EXPLAIN  */
  YYSYMBOL_ANALYZE = 38,                   /* ANALYZE  */
  YYSYMBOL_AS = 39,                        /* AS  */
  YYSYMBOL_ON = 40,                        /* ON  */
  YYSYMBOL_GROUP = 41,                     /* GROUP  */
  YYSYMBOL_HAVING = 42,                    /* HAVING  */
  YYSYMBOL_LIMIT = 43,                     /* LIMIT  */
  YYSYMBOL_COUNT = 44,                     /* COUNT  */
  YYSYMBOL_MAX = 45,                       /* MAX  */
  YYSYMBOL_MIN = 46,                       /* MIN  */
  YYSYMBOL_SUM = 47,                       /* SUM  */
  YYSYMBOL_AVG = 48,                       /* AVG  */
  YYSYMBOL_LEQ = 49,                       /* LEQ  */
  YYSYMBOL_NEQ = 50,                       /* NEQ  */
  YYSYMBOL_GEQ = 51,                       /* GEQ  */
  YYSYMBOL_T_EOF = 52,                     /* T_EOF  */
  YYSYMBOL_IDENTIFIER = 53,                /* IDENTIFIER  */
  YYSYMBOL_VALUE_STRING = 54,              /* VALUE_STRING  */
  YYSYMBOL_VALUE_INT = 55,                 /* VALUE_INT  */
  YYSYMBOL_VALUE_FLOAT = 56,               /* VALUE_FLOAT  */
  YYSYMBOL_VALUE_BOOL = 57,                /* VALUE_BOOL  */
  YYSYMBOL_58_ = 58,                       /* ';'  */
  YYSYMBOL_59_ = 59,                       /* '='  */
  YYSYMBOL_60_ = 60,                       /* '('  */
  YYSYMBOL_61_ = 61,                       /* ')'  */
  YYSYMBOL_62_ = 62,                       /* ','  */
  YYSYMBOL_63_ = 63,                       /* '.'  */
  YYSYMBOL_64_ = 64,                       /* '<'  */
  YYSYMBOL_65_ = 65,                       /* '>'  */
  YYSYMBOL_66_ = 66,                       /* '*'  */
  YYSYMBOL_YYACCEPT = 67,                  /* $accept  */
  YYSYMBOL_start = 68,                     /* start  */
  YYSYMBOL_stmt = 69,                      /* stmt  */
  YYSYMBOL_txnStmt = 70,                   /* txnStmt  */
  YYSYMBOL_dbStmt = 71,                    /* dbStmt  */
  YYSYMBOL_setStmt = 72,                   /* setStmt  */
  YYSYMBOL_ddl = 73,                       /* ddl  */
  YYSYMBOL_dml = 74,                       /* dml  */
  YYSYMBOL_queryExpr = 75,                 /* queryExpr  */
  YYSYMBOL_selectStmt = 76,                /* selectStmt  */
  YYSYMBOL_fieldList = 77,                 /* fieldList  */
  YYSYMBOL_colNameList = 78,               /* colNameList  */
  YYSYMBOL_field = 79,                     /* field  */
  YYSYMBOL_type = 80,                      /* type  */
  YYSYMBOL_valueList = 81,                 /* valueList  */
  YYSYMBOL_value = 82,                     /* value  */
  YYSYMBOL_condition = 83,                 /* condition  */
  YYSYMBOL_optWhereClause = 84,            /* optWhereClause  */
  YYSYMBOL_whereClause = 85,               /* whereClause  */
  YYSYMBOL_col = 86,                       /* col  */
  YYSYMBOL_op = 87,                        /* op  */
  YYSYMBOL_expr = 88,                      /* expr  */
  YYSYMBOL_setClauses = 89,                /* setClauses  */
  YYSYMBOL_setClause = 90,                 /* setClause  */
  YYSYMBOL_selector = 91,                  /* selector  */
  YYSYMBOL_selectList = 92,                /* selectList  */
  YYSYMBOL_selectItem = 93,                /* selectItem  */
  YYSYMBOL_optAlias = 94,                  /* optAlias  */
  YYSYMBOL_aggregate = 95,                 /* aggregate  */
  YYSYMBOL_tableRef = 96,                  /* tableRef  */
  YYSYMBOL_fromClause = 97,                /* fromClause  */
  YYSYMBOL_opt_order_clause = 98,          /* opt_order_clause  */
  YYSYMBOL_order_clause = 99,              /* order_clause  */
  YYSYMBOL_opt_asc_desc = 100,             /* opt_asc_desc  */
  YYSYMBOL_opt_group_clause = 101,         /* opt_group_clause  */
  YYSYMBOL_group_clause = 102,             /* group_clause  */
  YYSYMBOL_opt_having_clause = 103,        /* opt_having_clause  */
  YYSYMBOL_havingClause = 104,             /* havingClause  */
  YYSYMBOL_havingCondition = 105,          /* havingCondition  */
  YYSYMBOL_opt_limit_clause = 106,         /* opt_limit_clause  */
  YYSYMBOL_set_knob_type = 107,            /* set_knob_type  */
  YYSYMBOL_tbName = 108,                   /* tbName  */
  YYSYMBOL_colName = 109                   /* colName  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
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
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  56
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   212

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  67
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  43
/* YYNRULES -- Number of rules.  */
#define YYNRULES  114
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  218

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   312


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      60,    61,    66,     2,    62,     2,    63,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    58,
      64,    59,    65,     2,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    69,    69,    74,    79,    84,    92,    93,    94,    95,
      96,   100,   104,   108,   112,   119,   123,   130,   137,   141,
     145,   149,   153,   160,   164,   168,   172,   176,   187,   191,
     203,   225,   229,   236,   240,   247,   254,   258,   262,   269,
     273,   280,   284,   288,   292,   299,   303,   308,   313,   321,
     322,   329,   333,   340,   344,   362,   366,   370,   374,   378,
     382,   389,   393,   400,   404,   411,   418,   422,   426,   430,
     437,   441,   448,   449,   450,   454,   458,   462,   466,   470,
     474,   497,   501,   505,   509,   513,   521,   526,   531,   537,
     545,   549,   553,   557,   565,   566,   567,   571,   572,   579,
     583,   590,   591,   598,   602,   609,   613,   617,   621,   628,
     629,   633,   634,   637,   639
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SHOW", "TABLES",
  "CREATE", "TABLE", "DROP", "DESC", "INSERT", "INTO", "VALUES", "DELETE",
  "FROM", "ASC", "ORDER", "BY", "UNION", "WHERE", "UPDATE", "SET",
  "SELECT", "INT", "CHAR", "FLOAT", "INDEX", "AND", "JOIN", "EXIT", "HELP",
  "TXN_BEGIN", "TXN_COMMIT", "TXN_ABORT", "TXN_ROLLBACK", "ORDER_BY",
  "ENABLE_NESTLOOP", "ENABLE_SORTMERGE", "EXPLAIN", "ANALYZE", "AS", "ON",
  "GROUP", "HAVING", "LIMIT", "COUNT", "MAX", "MIN", "SUM", "AVG", "LEQ",
  "NEQ", "GEQ", "T_EOF", "IDENTIFIER", "VALUE_STRING", "VALUE_INT",
  "VALUE_FLOAT", "VALUE_BOOL", "';'", "'='", "'('", "')'", "','", "'.'",
  "'<'", "'>'", "'*'", "$accept", "start", "stmt", "txnStmt", "dbStmt",
  "setStmt", "ddl", "dml", "queryExpr", "selectStmt", "fieldList",
  "colNameList", "field", "type", "valueList", "value", "condition",
  "optWhereClause", "whereClause", "col", "op", "expr", "setClauses",
  "setClause", "selector", "selectList", "selectItem", "optAlias",
  "aggregate", "tableRef", "fromClause", "opt_order_clause",
  "order_clause", "opt_asc_desc", "opt_group_clause", "group_clause",
  "opt_having_clause", "havingClause", "havingCondition",
  "opt_limit_clause", "set_knob_type", "tbName", "colName", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-140)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-114)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      78,     7,    10,    11,   -31,    36,    30,   -31,    35,    47,
    -140,  -140,  -140,  -140,  -140,  -140,    13,  -140,    48,     0,
    -140,  -140,  -140,  -140,  -140,    50,  -140,  -140,    60,   -31,
     -31,   -31,   -31,  -140,  -140,   -31,   -31,    62,  -140,  -140,
      25,    41,    45,    54,    56,    69,    86,  -140,   -14,    83,
      90,  -140,   -14,    95,  -140,   134,  -140,  -140,   134,   -31,
      99,   100,  -140,   101,   151,   145,   111,   108,   -40,   113,
     113,   113,   113,   115,  -140,  -140,   -16,    75,  -140,   111,
      50,  -140,  -140,   111,   111,   111,   107,    75,  -140,  -140,
     -12,  -140,   110,  -140,   112,   116,   117,   118,   119,   120,
    -140,   134,  -140,    -6,    -8,  -140,  -140,    15,  -140,   114,
      27,  -140,    63,    91,  -140,   146,    53,    53,   111,  -140,
      91,  -140,  -140,  -140,  -140,  -140,  -140,    -7,   -16,   -16,
     133,   122,  -140,  -140,   111,  -140,   123,  -140,  -140,  -140,
     111,  -140,  -140,  -140,  -140,  -140,    92,  -140,    75,  -140,
    -140,  -140,  -140,  -140,  -140,    87,    87,  -140,  -140,    -1,
     142,  -140,   168,   143,  -140,  -140,   131,  -140,  -140,    91,
    -140,  -140,  -140,  -140,  -140,  -140,  -140,   135,  -140,    75,
     113,    87,   174,   129,  -140,  -140,  -140,  -140,   136,    53,
      53,   169,  -140,   180,   154,  -140,   113,    87,    87,    87,
     113,   144,  -140,  -140,  -140,  -140,  -140,  -140,  -140,    33,
     138,  -140,  -140,  -140,  -140,   113,    33,  -140
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     3,    11,    12,    13,    14,     0,     5,     0,     0,
       9,     6,    10,     7,     8,    26,    28,    15,     0,     0,
       0,     0,     0,   113,    20,     0,     0,     0,   111,   112,
       0,     0,     0,     0,     0,     0,   114,    66,    72,     0,
      67,    68,    72,     0,    54,     0,     1,     2,     0,     0,
       0,     0,    19,     0,     0,    49,     0,     0,     0,     0,
       0,     0,     0,     0,    74,    70,     0,     0,    71,     0,
      27,    29,    16,     0,     0,     0,     0,     0,    24,   114,
      49,    63,     0,    17,     0,     0,     0,     0,     0,     0,
      73,     0,    86,    49,    81,    69,    53,     0,    31,     0,
       0,    33,     0,     0,    51,    50,     0,     0,     0,    25,
       0,    75,    76,    77,    78,    79,    80,     0,     0,     0,
      97,     0,    82,    18,     0,    36,     0,    38,    35,    21,
       0,    22,    43,    41,    42,    44,     0,    39,     0,    59,
      58,    60,    55,    56,    57,     0,     0,    64,    65,     0,
      89,    87,     0,   101,    83,    32,     0,    34,    23,     0,
      52,    61,    62,    45,    47,    46,    48,     0,    85,     0,
       0,     0,    91,     0,    40,    84,    88,    99,    98,     0,
       0,   102,   103,     0,   109,    37,     0,     0,     0,     0,
       0,     0,    30,   100,   105,   107,   106,   108,   104,    96,
      90,   110,    95,    94,    92,     0,    96,    93
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -140,  -140,  -140,  -140,  -140,  -140,  -140,  -140,   -52,   147,
    -140,   124,    67,  -140,  -140,  -105,  -139,   -48,  -140,    -9,
    -110,  -132,  -140,    84,  -140,  -140,   126,   152,    -5,    28,
    -140,  -140,  -140,    -4,  -140,  -140,  -140,  -140,     8,  -140,
    -140,    -2,   -65
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     107,   110,   108,   138,   146,   171,   114,    88,   115,   172,
     155,   189,    90,    91,    49,    50,    51,    75,   117,   102,
     103,   194,   210,   214,   163,   188,   182,   191,   192,   202,
      40,    53,    54
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      48,    92,    34,    80,    52,    37,    87,   156,   147,   170,
      58,    27,    87,    46,   106,   158,    29,    31,   109,   111,
     111,   128,    33,   173,   175,    73,    94,    60,    61,    62,
      63,   131,    28,    64,    65,    30,    32,    33,   177,    74,
     186,   212,   119,    36,   101,   132,    35,   213,    56,   127,
     118,    55,   178,    92,   159,   130,   129,    82,    57,    95,
      96,    97,    98,    99,   184,   204,   206,    58,    48,   109,
      38,    39,    52,    59,   104,   167,   133,   134,   116,   197,
     198,     1,    66,     2,    67,     3,     4,     5,   139,   140,
       6,    41,    42,    43,    44,    45,    76,     7,     8,     9,
      46,    68,   149,   150,   151,    69,    10,    11,    12,    13,
      14,    15,   152,    47,    70,    16,    71,   153,   154,    41,
      42,    43,    44,    45,   141,   140,   104,   104,    46,    72,
      17,    41,    42,    43,    44,    45,   135,   136,   137,   116,
      46,   142,   143,   144,   145,   142,   143,   144,   145,  -113,
     174,   176,    77,   168,   169,     9,   160,   161,    79,    83,
      84,    85,    86,    87,    89,    93,    46,   113,   100,   120,
     116,   187,   148,   121,   162,   164,   190,   122,   123,   124,
     125,   126,   179,   166,   180,   181,   183,   203,   185,   193,
     195,   209,   205,   207,   190,   199,   200,   201,   196,   211,
     215,   165,   157,   105,    78,    81,   216,   208,     0,   112,
       0,     0,   217
};

static const yytype_int16 yycheck[] =
{
       9,    66,     4,    55,     9,     7,    18,   117,   113,   148,
      17,     4,    18,    53,    79,   120,     6,     6,    83,    84,
      85,    27,    53,   155,   156,    39,    66,    29,    30,    31,
      32,    39,    25,    35,    36,    25,    25,    53,    39,    53,
     179,     8,    90,    13,    60,    53,    10,    14,     0,   101,
      62,    38,    53,   118,    61,   103,    62,    59,    58,    68,
      69,    70,    71,    72,   169,   197,   198,    17,    77,   134,
      35,    36,    77,    13,    76,   140,    61,    62,    87,   189,
     190,     3,    20,     5,    59,     7,     8,     9,    61,    62,
      12,    44,    45,    46,    47,    48,    13,    19,    20,    21,
      53,    60,    49,    50,    51,    60,    28,    29,    30,    31,
      32,    33,    59,    66,    60,    37,    60,    64,    65,    44,
      45,    46,    47,    48,    61,    62,   128,   129,    53,    60,
      52,    44,    45,    46,    47,    48,    22,    23,    24,   148,
      53,    54,    55,    56,    57,    54,    55,    56,    57,    63,
     155,   156,    62,    61,    62,    21,   128,   129,    63,    60,
      60,    60,    11,    18,    53,    57,    53,    60,    53,    59,
     179,   180,    26,    61,    41,    53,   181,    61,    61,    61,
      61,    61,    40,    60,    16,    42,    55,   196,    53,    15,
      61,   200,   197,   198,   199,    26,    16,    43,    62,    55,
      62,   134,   118,    77,    52,    58,   215,   199,    -1,    85,
      -1,    -1,   216
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     7,     8,     9,    12,    19,    20,    21,
      28,    29,    30,    31,    32,    33,    37,    52,    68,    69,
      70,    71,    72,    73,    74,    75,    76,     4,    25,     6,
      25,     6,    25,    53,   108,    10,    13,   108,    35,    36,
     107,    44,    45,    46,    47,    48,    53,    66,    86,    91,
      92,    93,    95,   108,   109,    38,     0,    58,    17,    13,
     108,   108,   108,   108,   108,   108,    20,    59,    60,    60,
      60,    60,    60,    39,    53,    94,    13,    62,    94,    63,
      75,    76,   108,    60,    60,    60,    11,    18,    84,    53,
      89,    90,   109,    57,    66,    86,    86,    86,    86,    86,
      53,    60,    96,    97,   108,    93,   109,    77,    79,   109,
      78,   109,    78,    60,    83,    85,    86,    95,    62,    84,
      59,    61,    61,    61,    61,    61,    61,    75,    27,    62,
      84,    39,    53,    61,    62,    22,    23,    24,    80,    61,
      62,    61,    54,    55,    56,    57,    81,    82,    26,    49,
      50,    51,    59,    64,    65,    87,    87,    90,    82,    61,
      96,    96,    41,   101,    53,    79,    60,   109,    61,    62,
      83,    82,    86,    88,    95,    88,    95,    39,    53,    40,
      16,    42,   103,    55,    82,    53,    83,    86,   102,    88,
      95,   104,   105,    15,    98,    61,    62,    87,    87,    26,
      16,    43,   106,    86,    88,    95,    88,    95,   105,    86,
      99,    55,     8,    14,   100,    62,    86,   100
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    67,    68,    68,    68,    68,    69,    69,    69,    69,
      69,    70,    70,    70,    70,    71,    71,    72,    73,    73,
      73,    73,    73,    74,    74,    74,    74,    74,    75,    75,
      76,    77,    77,    78,    78,    79,    80,    80,    80,    81,
      81,    82,    82,    82,    82,    83,    83,    83,    83,    84,
      84,    85,    85,    86,    86,    87,    87,    87,    87,    87,
      87,    88,    88,    89,    89,    90,    91,    91,    92,    92,
      93,    93,    94,    94,    94,    95,    95,    95,    95,    95,
      95,    96,    96,    96,    96,    96,    97,    97,    97,    97,
      98,    98,    99,    99,   100,   100,   100,   101,   101,   102,
     102,   103,   103,   104,   104,   105,   105,   105,   105,   106,
     106,   107,   107,   108,   109
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     4,     4,     6,     3,
       2,     6,     6,     7,     4,     5,     1,     3,     1,     3,
       9,     1,     3,     1,     3,     2,     1,     4,     1,     1,
       3,     1,     1,     1,     1,     3,     3,     3,     3,     0,
       2,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     1,     1,     1,     3,
       2,     2,     0,     2,     1,     4,     4,     4,     4,     4,
       4,     1,     2,     3,     5,     4,     1,     3,     5,     3,
       3,     0,     2,     4,     1,     1,     0,     0,     3,     1,
       3,     0,     2,     1,     3,     3,     3,     3,     3,     0,
       2,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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
        yyerror (&yylloc, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
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
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
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
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
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
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* start: stmt ';'  */
#line 70 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        parse_tree = (yyvsp[-1].sv_node);
        YYACCEPT;
    }
#line 1735 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 3: /* start: HELP  */
#line 75 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        parse_tree = std::make_shared<Help>();
        YYACCEPT;
    }
#line 1744 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 4: /* start: EXIT  */
#line 80 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1753 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 5: /* start: T_EOF  */
#line 85 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1762 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 11: /* txnStmt: TXN_BEGIN  */
#line 101 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnBegin>();
    }
#line 1770 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 12: /* txnStmt: TXN_COMMIT  */
#line 105 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnCommit>();
    }
#line 1778 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 13: /* txnStmt: TXN_ABORT  */
#line 109 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnAbort>();
    }
#line 1786 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 14: /* txnStmt: TXN_ROLLBACK  */
#line 113 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnRollback>();
    }
#line 1794 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 15: /* dbStmt: SHOW TABLES  */
#line 120 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowTables>();
    }
#line 1802 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 16: /* dbStmt: SHOW INDEX FROM tbName  */
#line 124 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowIndex>((yyvsp[0].sv_str));
    }
#line 1810 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 17: /* setStmt: SET set_knob_type '=' VALUE_BOOL  */
#line 131 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SetStmt>((yyvsp[-2].sv_setKnobType), (yyvsp[0].sv_bool));
    }
#line 1818 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 18: /* ddl: CREATE TABLE tbName '(' fieldList ')'  */
#line 138 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateTable>((yyvsp[-3].sv_str), (yyvsp[-1].sv_fields));
    }
#line 1826 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 19: /* ddl: DROP TABLE tbName  */
#line 142 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropTable>((yyvsp[0].sv_str));
    }
#line 1834 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 20: /* ddl: DESC tbName  */
#line 146 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DescTable>((yyvsp[0].sv_str));
    }
#line 1842 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 21: /* ddl: CREATE INDEX tbName '(' colNameList ')'  */
#line 150 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1850 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 22: /* ddl: DROP INDEX tbName '(' colNameList ')'  */
#line 154 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1858 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 23: /* dml: INSERT INTO tbName VALUES '(' valueList ')'  */
#line 161 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<InsertStmt>((yyvsp[-4].sv_str), (yyvsp[-1].sv_vals));
    }
#line 1866 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 24: /* dml: DELETE FROM tbName optWhereClause  */
#line 165 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DeleteStmt>((yyvsp[-1].sv_str), (yyvsp[0].sv_conds));
    }
#line 1874 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 25: /* dml: UPDATE tbName SET setClauses optWhereClause  */
#line 169 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<UpdateStmt>((yyvsp[-3].sv_str), (yyvsp[-1].sv_set_clauses), (yyvsp[0].sv_conds));
    }
#line 1882 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 26: /* dml: queryExpr  */
#line 173 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1890 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 27: /* dml: EXPLAIN ANALYZE queryExpr  */
#line 177 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        auto sel = std::dynamic_pointer_cast<SelectStmt>((yyvsp[0].sv_node));
        if (sel == nullptr) {
            throw std::runtime_error("EXPLAIN ANALYZE only supports SELECT");
        }
        (yyval.sv_node) = std::make_shared<ExplainStmt>(sel);
    }
#line 1902 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 28: /* queryExpr: selectStmt  */
#line 188 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1910 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 29: /* queryExpr: queryExpr UNION selectStmt  */
#line 192 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        if (auto u = std::dynamic_pointer_cast<UnionStmt>((yyvsp[-2].sv_node))) {
            u->append((yyvsp[0].sv_node));
            (yyval.sv_node) = u;
        } else {
            (yyval.sv_node) = std::make_shared<UnionStmt>((yyvsp[-2].sv_node), (yyvsp[0].sv_node));
        }
    }
#line 1923 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 30: /* selectStmt: SELECT selector FROM fromClause optWhereClause opt_group_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 204 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        std::vector<std::shared_ptr<Col>> legacy_cols;
        for (auto &item : (yyvsp[-7].sv_select_items)) {
            if (auto c = std::dynamic_pointer_cast<Col>(item->expr)) {
                legacy_cols.push_back(c);
            }
        }
        auto sel = std::make_shared<SelectStmt>(legacy_cols, (yyvsp[-7].sv_select_items), std::vector<std::string>{}, (yyvsp[-4].sv_conds), (yyvsp[-1].sv_orderby), (yyvsp[-3].sv_cols), (yyvsp[-2].sv_having_conds), (yyvsp[0].sv_int));
        sel->from_refs = (yyvsp[-5].sv_from_clause)->refs;
        for (auto &ref : (yyvsp[-5].sv_from_clause)->refs) {
            sel->tabs.push_back(ref->tab_name);
            sel->tab_alias.push_back(ref->alias);
        }
        for (auto &c : (yyvsp[-5].sv_from_clause)->conds) {
            sel->conds.push_back(c);
        }
        (yyval.sv_node) = sel;
    }
#line 1946 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 31: /* fieldList: field  */
#line 226 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_fields) = std::vector<std::shared_ptr<Field>>{(yyvsp[0].sv_field)};
    }
#line 1954 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 32: /* fieldList: fieldList ',' field  */
#line 230 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_fields).push_back((yyvsp[0].sv_field));
    }
#line 1962 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 33: /* colNameList: colName  */
#line 237 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_strs) = std::vector<std::string>{(yyvsp[0].sv_str)};
    }
#line 1970 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 34: /* colNameList: colNameList ',' colName  */
#line 241 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_strs).push_back((yyvsp[0].sv_str));
    }
#line 1978 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 35: /* field: colName type  */
#line 248 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_field) = std::make_shared<ColDef>((yyvsp[-1].sv_str), (yyvsp[0].sv_type_len));
    }
#line 1986 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 36: /* type: INT  */
#line 255 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_INT, sizeof(int));
    }
#line 1994 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 37: /* type: CHAR '(' VALUE_INT ')'  */
#line 259 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_STRING, (yyvsp[-1].sv_int));
    }
#line 2002 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 38: /* type: FLOAT  */
#line 263 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_FLOAT, sizeof(float));
    }
#line 2010 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 39: /* valueList: value  */
#line 270 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_vals) = std::vector<std::shared_ptr<Value>>{(yyvsp[0].sv_val)};
    }
#line 2018 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 40: /* valueList: valueList ',' value  */
#line 274 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_vals).push_back((yyvsp[0].sv_val));
    }
#line 2026 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 41: /* value: VALUE_INT  */
#line 281 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<IntLit>((yyvsp[0].sv_int));
    }
#line 2034 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 42: /* value: VALUE_FLOAT  */
#line 285 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<FloatLit>((yyvsp[0].sv_float));
    }
#line 2042 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 43: /* value: VALUE_STRING  */
#line 289 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<StringLit>((yyvsp[0].sv_str));
    }
#line 2050 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 44: /* value: VALUE_BOOL  */
#line 293 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<BoolLit>((yyvsp[0].sv_bool));
    }
#line 2058 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 45: /* condition: col op expr  */
#line 300 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>((yyvsp[-2].sv_col), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2066 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 46: /* condition: aggregate op expr  */
#line 304 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), (yyvsp[-1].sv_comp_op),
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
#line 2075 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 47: /* condition: col op aggregate  */
#line 309 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), (yyvsp[-1].sv_comp_op),
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
#line 2084 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 48: /* condition: aggregate op aggregate  */
#line 314 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), (yyvsp[-1].sv_comp_op),
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
#line 2093 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 49: /* optWhereClause: %empty  */
#line 321 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { /* ignore*/ }
#line 2099 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 50: /* optWhereClause: WHERE whereClause  */
#line 323 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_conds) = (yyvsp[0].sv_conds);
    }
#line 2107 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 51: /* whereClause: condition  */
#line 330 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)};
    }
#line 2115 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 52: /* whereClause: whereClause AND condition  */
#line 334 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_conds).push_back((yyvsp[0].sv_cond));
    }
#line 2123 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 53: /* col: tbName '.' colName  */
#line 341 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2131 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 54: /* col: colName  */
#line 345 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>("", (yyvsp[0].sv_str));
    }
#line 2139 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 55: /* op: '='  */
#line 363 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_EQ;
    }
#line 2147 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 56: /* op: '<'  */
#line 367 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LT;
    }
#line 2155 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 57: /* op: '>'  */
#line 371 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GT;
    }
#line 2163 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 58: /* op: NEQ  */
#line 375 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_NE;
    }
#line 2171 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 59: /* op: LEQ  */
#line 379 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LE;
    }
#line 2179 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 60: /* op: GEQ  */
#line 383 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GE;
    }
#line 2187 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 61: /* expr: value  */
#line 390 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_val));
    }
#line 2195 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 62: /* expr: col  */
#line 394 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_col));
    }
#line 2203 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 63: /* setClauses: setClause  */
#line 401 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses) = std::vector<std::shared_ptr<SetClause>>{(yyvsp[0].sv_set_clause)};
    }
#line 2211 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 64: /* setClauses: setClauses ',' setClause  */
#line 405 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses).push_back((yyvsp[0].sv_set_clause));
    }
#line 2219 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 65: /* setClause: colName '=' value  */
#line 412 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_set_clause) = std::make_shared<SetClause>((yyvsp[-2].sv_str), (yyvsp[0].sv_val));
    }
#line 2227 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 66: /* selector: '*'  */
#line 419 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_items) = {};
    }
#line 2235 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 68: /* selectList: selectItem  */
#line 427 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_items) = std::vector<std::shared_ptr<SelectItem>>{(yyvsp[0].sv_select_item)};
    }
#line 2243 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 69: /* selectList: selectList ',' selectItem  */
#line 431 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_items).push_back((yyvsp[0].sv_select_item));
    }
#line 2251 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 70: /* selectItem: col optAlias  */
#line 438 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_item) = std::make_shared<SelectItem>(std::static_pointer_cast<Expr>((yyvsp[-1].sv_col)), (yyvsp[0].sv_str));
    }
#line 2259 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 71: /* selectItem: aggregate optAlias  */
#line 442 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_item) = std::make_shared<SelectItem>(std::static_pointer_cast<Expr>((yyvsp[-1].sv_agg_expr)), (yyvsp[0].sv_str));
    }
#line 2267 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 72: /* optAlias: %empty  */
#line 448 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_str) = ""; }
#line 2273 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 73: /* optAlias: AS IDENTIFIER  */
#line 449 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_str) = (yyvsp[0].sv_str); }
#line 2279 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 74: /* optAlias: IDENTIFIER  */
#line 450 "/home/yc_dada/db2026/src/parser/yacc.y"
                   { (yyval.sv_str) = (yyvsp[0].sv_str); }
#line 2285 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 75: /* aggregate: COUNT '(' '*' ')'  */
#line 455 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_COUNT, nullptr, true);
    }
#line 2293 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 76: /* aggregate: COUNT '(' col ')'  */
#line 459 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_COUNT, (yyvsp[-1].sv_col), false);
    }
#line 2301 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 77: /* aggregate: MAX '(' col ')'  */
#line 463 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_MAX, (yyvsp[-1].sv_col), false);
    }
#line 2309 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 78: /* aggregate: MIN '(' col ')'  */
#line 467 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_MIN, (yyvsp[-1].sv_col), false);
    }
#line 2317 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 79: /* aggregate: SUM '(' col ')'  */
#line 471 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_SUM, (yyvsp[-1].sv_col), false);
    }
#line 2325 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 80: /* aggregate: AVG '(' col ')'  */
#line 475 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_AVG, (yyvsp[-1].sv_col), false);
    }
#line 2333 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 81: /* tableRef: tbName  */
#line 498 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[0].sv_str));
    }
#line 2341 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 82: /* tableRef: tbName IDENTIFIER  */
#line 502 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-1].sv_str), (yyvsp[0].sv_str));
    }
#line 2349 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 83: /* tableRef: tbName AS IDENTIFIER  */
#line 506 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2357 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 84: /* tableRef: '(' queryExpr ')' AS IDENTIFIER  */
#line 510 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-3].sv_node), (yyvsp[0].sv_str));
    }
#line 2365 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 85: /* tableRef: '(' queryExpr ')' IDENTIFIER  */
#line 514 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-2].sv_node), (yyvsp[0].sv_str));
    }
#line 2373 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 86: /* fromClause: tableRef  */
#line 522 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = std::make_shared<FromClause>();
        (yyval.sv_from_clause)->refs.push_back((yyvsp[0].sv_table_ref));
    }
#line 2382 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 87: /* fromClause: fromClause ',' tableRef  */
#line 527 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = (yyvsp[-2].sv_from_clause);
        (yyval.sv_from_clause)->refs.push_back((yyvsp[0].sv_table_ref));
    }
#line 2391 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 88: /* fromClause: fromClause JOIN tableRef ON condition  */
#line 532 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = (yyvsp[-4].sv_from_clause);
        (yyval.sv_from_clause)->refs.push_back((yyvsp[-2].sv_table_ref));
        (yyval.sv_from_clause)->conds.push_back((yyvsp[0].sv_cond));
    }
#line 2401 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 89: /* fromClause: fromClause JOIN tableRef  */
#line 538 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = (yyvsp[-2].sv_from_clause);
        (yyval.sv_from_clause)->refs.push_back((yyvsp[0].sv_table_ref));
    }
#line 2410 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 90: /* opt_order_clause: ORDER BY order_clause  */
#line 546 "/home/yc_dada/db2026/src/parser/yacc.y"
    { 
        (yyval.sv_orderby) = (yyvsp[0].sv_orderby); 
    }
#line 2418 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 91: /* opt_order_clause: %empty  */
#line 549 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { /* ignore*/ }
#line 2424 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 92: /* order_clause: col opt_asc_desc  */
#line 554 "/home/yc_dada/db2026/src/parser/yacc.y"
    { 
        (yyval.sv_orderby) = std::make_shared<OrderBy>((yyvsp[-1].sv_col), (yyvsp[0].sv_orderby_dir));
    }
#line 2432 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 93: /* order_clause: order_clause ',' col opt_asc_desc  */
#line 558 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_orderby) = (yyvsp[-3].sv_orderby);
        (yyval.sv_orderby)->append((yyvsp[-1].sv_col), (yyvsp[0].sv_orderby_dir));
    }
#line 2441 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 94: /* opt_asc_desc: ASC  */
#line 565 "/home/yc_dada/db2026/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_ASC;     }
#line 2447 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 95: /* opt_asc_desc: DESC  */
#line 566 "/home/yc_dada/db2026/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_DESC;    }
#line 2453 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 96: /* opt_asc_desc: %empty  */
#line 567 "/home/yc_dada/db2026/src/parser/yacc.y"
            { (yyval.sv_orderby_dir) = OrderBy_DEFAULT; }
#line 2459 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 97: /* opt_group_clause: %empty  */
#line 571 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_cols) = {}; }
#line 2465 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 98: /* opt_group_clause: GROUP BY group_clause  */
#line 573 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cols) = (yyvsp[0].sv_cols);
    }
#line 2473 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 99: /* group_clause: col  */
#line 580 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cols) = std::vector<std::shared_ptr<Col>>{(yyvsp[0].sv_col)};
    }
#line 2481 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 100: /* group_clause: group_clause ',' col  */
#line 584 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cols).push_back((yyvsp[0].sv_col));
    }
#line 2489 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 101: /* opt_having_clause: %empty  */
#line 590 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_having_conds) = {}; }
#line 2495 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 102: /* opt_having_clause: HAVING havingClause  */
#line 592 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_conds) = (yyvsp[0].sv_having_conds);
    }
#line 2503 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 103: /* havingClause: havingCondition  */
#line 599 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_conds) = std::vector<std::shared_ptr<HavingExpr>>{(yyvsp[0].sv_having_cond)};
    }
#line 2511 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 104: /* havingClause: havingClause AND havingCondition  */
#line 603 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_conds).push_back((yyvsp[0].sv_having_cond));
    }
#line 2519 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 105: /* havingCondition: expr op expr  */
#line 610 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>((yyvsp[-2].sv_expr), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2527 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 106: /* havingCondition: aggregate op expr  */
#line 614 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_agg_expr)), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2535 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 107: /* havingCondition: expr op aggregate  */
#line 618 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>((yyvsp[-2].sv_expr), (yyvsp[-1].sv_comp_op), std::static_pointer_cast<Expr>((yyvsp[0].sv_agg_expr)));
    }
#line 2543 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 108: /* havingCondition: aggregate op aggregate  */
#line 622 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_agg_expr)), (yyvsp[-1].sv_comp_op), std::static_pointer_cast<Expr>((yyvsp[0].sv_agg_expr)));
    }
#line 2551 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 109: /* opt_limit_clause: %empty  */
#line 628 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_int) = -1; }
#line 2557 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 110: /* opt_limit_clause: LIMIT VALUE_INT  */
#line 629 "/home/yc_dada/db2026/src/parser/yacc.y"
                        { (yyval.sv_int) = (yyvsp[0].sv_int); }
#line 2563 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 111: /* set_knob_type: ENABLE_NESTLOOP  */
#line 633 "/home/yc_dada/db2026/src/parser/yacc.y"
                    { (yyval.sv_setKnobType) = EnableNestLoop; }
#line 2569 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 112: /* set_knob_type: ENABLE_SORTMERGE  */
#line 634 "/home/yc_dada/db2026/src/parser/yacc.y"
                         { (yyval.sv_setKnobType) = EnableSortMerge; }
#line 2575 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;


#line 2579 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
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
                      yytoken, &yylval, &yylloc);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 640 "/home/yc_dada/db2026/src/parser/yacc.y"

