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
#include "errors.h"
#include <iostream>
#include <memory>
#include <stdexcept>

int yylex(YYSTYPE *yylval, YYLTYPE *yylloc);

void yyerror(YYLTYPE *locp, const char* s) {
    std::cerr << "Parser Error at line " << locp->first_line << " column " << locp->first_column << ": " << s << std::endl;
}

using namespace ast;

#line 88 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"

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
  YYSYMBOL_STATIC_CHECKPOINT = 18,         /* STATIC_CHECKPOINT  */
  YYSYMBOL_LOAD = 19,                      /* LOAD  */
  YYSYMBOL_OUTPUT_FILE = 20,               /* OUTPUT_FILE  */
  YYSYMBOL_OFF = 21,                       /* OFF  */
  YYSYMBOL_WHERE = 22,                     /* WHERE  */
  YYSYMBOL_UPDATE = 23,                    /* UPDATE  */
  YYSYMBOL_SET = 24,                       /* SET  */
  YYSYMBOL_SELECT = 25,                    /* SELECT  */
  YYSYMBOL_INT = 26,                       /* INT  */
  YYSYMBOL_CHAR = 27,                      /* CHAR  */
  YYSYMBOL_FLOAT = 28,                     /* FLOAT  */
  YYSYMBOL_INDEX = 29,                     /* INDEX  */
  YYSYMBOL_AND = 30,                       /* AND  */
  YYSYMBOL_JOIN = 31,                      /* JOIN  */
  YYSYMBOL_EXIT = 32,                      /* EXIT  */
  YYSYMBOL_HELP = 33,                      /* HELP  */
  YYSYMBOL_TXN_BEGIN = 34,                 /* TXN_BEGIN  */
  YYSYMBOL_TXN_COMMIT = 35,                /* TXN_COMMIT  */
  YYSYMBOL_TXN_ABORT = 36,                 /* TXN_ABORT  */
  YYSYMBOL_TXN_ROLLBACK = 37,              /* TXN_ROLLBACK  */
  YYSYMBOL_ORDER_BY = 38,                  /* ORDER_BY  */
  YYSYMBOL_ENABLE_NESTLOOP = 39,           /* ENABLE_NESTLOOP  */
  YYSYMBOL_ENABLE_SORTMERGE = 40,          /* ENABLE_SORTMERGE  */
  YYSYMBOL_EXPLAIN = 41,                   /* EXPLAIN  */
  YYSYMBOL_ANALYZE = 42,                   /* ANALYZE  */
  YYSYMBOL_AS = 43,                        /* AS  */
  YYSYMBOL_ON = 44,                        /* ON  */
  YYSYMBOL_GROUP = 45,                     /* GROUP  */
  YYSYMBOL_HAVING = 46,                    /* HAVING  */
  YYSYMBOL_LIMIT = 47,                     /* LIMIT  */
  YYSYMBOL_COUNT = 48,                     /* COUNT  */
  YYSYMBOL_MAX = 49,                       /* MAX  */
  YYSYMBOL_MIN = 50,                       /* MIN  */
  YYSYMBOL_SUM = 51,                       /* SUM  */
  YYSYMBOL_AVG = 52,                       /* AVG  */
  YYSYMBOL_TRANSACTION = 53,               /* TRANSACTION  */
  YYSYMBOL_ISOLATION = 54,                 /* ISOLATION  */
  YYSYMBOL_LEVEL = 55,                     /* LEVEL  */
  YYSYMBOL_SNAPSHOT = 56,                  /* SNAPSHOT  */
  YYSYMBOL_SERIALIZABLE = 57,              /* SERIALIZABLE  */
  YYSYMBOL_LEQ = 58,                       /* LEQ  */
  YYSYMBOL_NEQ = 59,                       /* NEQ  */
  YYSYMBOL_GEQ = 60,                       /* GEQ  */
  YYSYMBOL_T_EOF = 61,                     /* T_EOF  */
  YYSYMBOL_IDENTIFIER = 62,                /* IDENTIFIER  */
  YYSYMBOL_VALUE_STRING = 63,              /* VALUE_STRING  */
  YYSYMBOL_VALUE_INT = 64,                 /* VALUE_INT  */
  YYSYMBOL_VALUE_FLOAT = 65,               /* VALUE_FLOAT  */
  YYSYMBOL_VALUE_BOOL = 66,                /* VALUE_BOOL  */
  YYSYMBOL_67_ = 67,                       /* ';'  */
  YYSYMBOL_68_ = 68,                       /* '='  */
  YYSYMBOL_69_ = 69,                       /* '('  */
  YYSYMBOL_70_ = 70,                       /* ')'  */
  YYSYMBOL_71_ = 71,                       /* ','  */
  YYSYMBOL_72_ = 72,                       /* '+'  */
  YYSYMBOL_73_ = 73,                       /* '-'  */
  YYSYMBOL_74_ = 74,                       /* '.'  */
  YYSYMBOL_75_ = 75,                       /* '<'  */
  YYSYMBOL_76_ = 76,                       /* '>'  */
  YYSYMBOL_77_ = 77,                       /* '*'  */
  YYSYMBOL_YYACCEPT = 78,                  /* $accept  */
  YYSYMBOL_start = 79,                     /* start  */
  YYSYMBOL_stmt = 80,                      /* stmt  */
  YYSYMBOL_txnStmt = 81,                   /* txnStmt  */
  YYSYMBOL_dbStmt = 82,                    /* dbStmt  */
  YYSYMBOL_setStmt = 83,                   /* setStmt  */
  YYSYMBOL_ddl = 84,                       /* ddl  */
  YYSYMBOL_dml = 85,                       /* dml  */
  YYSYMBOL_queryExpr = 86,                 /* queryExpr  */
  YYSYMBOL_selectStmt = 87,                /* selectStmt  */
  YYSYMBOL_fieldList = 88,                 /* fieldList  */
  YYSYMBOL_colNameList = 89,               /* colNameList  */
  YYSYMBOL_field = 90,                     /* field  */
  YYSYMBOL_type = 91,                      /* type  */
  YYSYMBOL_valueList = 92,                 /* valueList  */
  YYSYMBOL_value = 93,                     /* value  */
  YYSYMBOL_condition = 94,                 /* condition  */
  YYSYMBOL_joinConds = 95,                 /* joinConds  */
  YYSYMBOL_optWhereClause = 96,            /* optWhereClause  */
  YYSYMBOL_whereClause = 97,               /* whereClause  */
  YYSYMBOL_col = 98,                       /* col  */
  YYSYMBOL_op = 99,                        /* op  */
  YYSYMBOL_expr = 100,                     /* expr  */
  YYSYMBOL_setClauses = 101,               /* setClauses  */
  YYSYMBOL_setClause = 102,                /* setClause  */
  YYSYMBOL_selector = 103,                 /* selector  */
  YYSYMBOL_selectList = 104,               /* selectList  */
  YYSYMBOL_selectItem = 105,               /* selectItem  */
  YYSYMBOL_optAlias = 106,                 /* optAlias  */
  YYSYMBOL_aggregate = 107,                /* aggregate  */
  YYSYMBOL_tableRef = 108,                 /* tableRef  */
  YYSYMBOL_fromClause = 109,               /* fromClause  */
  YYSYMBOL_opt_order_clause = 110,         /* opt_order_clause  */
  YYSYMBOL_order_clause = 111,             /* order_clause  */
  YYSYMBOL_opt_asc_desc = 112,             /* opt_asc_desc  */
  YYSYMBOL_opt_group_clause = 113,         /* opt_group_clause  */
  YYSYMBOL_group_clause = 114,             /* group_clause  */
  YYSYMBOL_opt_having_clause = 115,        /* opt_having_clause  */
  YYSYMBOL_havingClause = 116,             /* havingClause  */
  YYSYMBOL_havingCondition = 117,          /* havingCondition  */
  YYSYMBOL_opt_limit_clause = 118,         /* opt_limit_clause  */
  YYSYMBOL_set_knob_type = 119,            /* set_knob_type  */
  YYSYMBOL_isolation_level = 120,          /* isolation_level  */
  YYSYMBOL_tbName = 121,                   /* tbName  */
  YYSYMBOL_colName = 122,                  /* colName  */
  YYSYMBOL_fileName = 123                  /* fileName  */
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
#define YYFINAL  63
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   247

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  78
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  46
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  248

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   321


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
      69,    70,    77,    72,    71,    73,    74,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    67,
      75,    68,    76,     2,     2,     2,     2,     2,     2,     2,
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
      65,    66
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    72,    72,    77,    83,    88,    93,   101,   102,   103,
     104,   105,   109,   113,   117,   121,   128,   132,   139,   143,
     147,   155,   159,   163,   167,   171,   175,   182,   186,   190,
     194,   198,   202,   213,   217,   229,   251,   255,   262,   266,
     273,   280,   284,   288,   295,   299,   306,   310,   314,   318,
     322,   326,   330,   334,   341,   345,   350,   355,   363,   367,
     374,   375,   382,   386,   393,   397,   415,   419,   423,   427,
     431,   435,   442,   446,   453,   457,   464,   468,   473,   481,
     485,   489,   493,   500,   504,   511,   512,   513,   517,   521,
     525,   529,   533,   537,   560,   564,   568,   572,   576,   584,
     589,   594,   602,   610,   614,   618,   622,   630,   631,   632,
     636,   637,   644,   648,   655,   656,   663,   667,   674,   678,
     682,   686,   693,   694,   698,   699,   703,   704,   705,   708,
     710,   713,   714
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
  "FROM", "ASC", "ORDER", "BY", "UNION", "STATIC_CHECKPOINT", "LOAD",
  "OUTPUT_FILE", "OFF", "WHERE", "UPDATE", "SET", "SELECT", "INT", "CHAR",
  "FLOAT", "INDEX", "AND", "JOIN", "EXIT", "HELP", "TXN_BEGIN",
  "TXN_COMMIT", "TXN_ABORT", "TXN_ROLLBACK", "ORDER_BY", "ENABLE_NESTLOOP",
  "ENABLE_SORTMERGE", "EXPLAIN", "ANALYZE", "AS", "ON", "GROUP", "HAVING",
  "LIMIT", "COUNT", "MAX", "MIN", "SUM", "AVG", "TRANSACTION", "ISOLATION",
  "LEVEL", "SNAPSHOT", "SERIALIZABLE", "LEQ", "NEQ", "GEQ", "T_EOF",
  "IDENTIFIER", "VALUE_STRING", "VALUE_INT", "VALUE_FLOAT", "VALUE_BOOL",
  "';'", "'='", "'('", "')'", "','", "'+'", "'-'", "'.'", "'<'", "'>'",
  "'*'", "$accept", "start", "stmt", "txnStmt", "dbStmt", "setStmt", "ddl",
  "dml", "queryExpr", "selectStmt", "fieldList", "colNameList", "field",
  "type", "valueList", "value", "condition", "joinConds", "optWhereClause",
  "whereClause", "col", "op", "expr", "setClauses", "setClause",
  "selector", "selectList", "selectItem", "optAlias", "aggregate",
  "tableRef", "fromClause", "opt_order_clause", "order_clause",
  "opt_asc_desc", "opt_group_clause", "group_clause", "opt_having_clause",
  "havingClause", "havingCondition", "opt_limit_clause", "set_knob_type",
  "isolation_level", "tbName", "colName", "fileName", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-158)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-130)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      95,     5,    70,     4,   -34,    20,    43,   -42,   -34,    -4,
       2,  -158,  -158,  -158,  -158,  -158,  -158,     6,  -158,    63,
      -2,  -158,  -158,  -158,  -158,  -158,    84,  -158,  -158,    98,
     -34,  -158,   -34,   -34,   -34,  -158,  -158,   -34,   -34,  -158,
    -158,   107,    97,   105,  -158,  -158,    83,    81,    85,    88,
      94,   118,   119,    78,  -158,   -31,   140,    93,  -158,   -31,
      91,  -158,   166,  -158,  -158,   166,   -34,   123,   124,  -158,
     127,   186,   176,   -34,   137,   134,   147,   138,   -48,   141,
     141,   141,   141,   143,  -158,  -158,   -37,   122,  -158,   137,
      84,  -158,  -158,   137,   137,   137,   139,   122,  -158,  -158,
    -158,    -9,  -158,   142,    29,  -158,   136,   146,   148,   153,
     154,   155,  -158,   166,  -158,    -5,   -24,  -158,  -158,    21,
    -158,    82,    24,  -158,    35,   117,  -158,   177,    65,    65,
     137,  -158,   113,   157,  -158,  -158,  -158,  -158,  -158,  -158,
    -158,  -158,   -12,   -37,   -37,   164,   150,  -158,  -158,   137,
    -158,   144,  -158,  -158,  -158,   137,  -158,  -158,  -158,  -158,
    -158,    48,    51,    64,  -158,   122,  -158,  -158,  -158,  -158,
    -158,  -158,    96,    96,  -158,  -158,    66,  -158,  -158,   -16,
     173,  -158,   210,   181,  -158,  -158,   165,  -158,  -158,  -158,
    -158,  -158,  -158,   117,  -158,  -158,  -158,  -158,  -158,  -158,
     117,   117,   168,  -158,   122,   141,    96,   213,   161,  -158,
    -158,  -158,  -158,  -158,   202,  -158,   162,    65,    65,   204,
    -158,   220,   190,  -158,   122,   141,    96,    96,    96,   141,
     174,  -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,    47,
     169,  -158,  -158,  -158,  -158,   141,    47,  -158
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     5,     4,    12,    13,    14,    15,     0,     6,     0,
       0,    10,     7,    11,     8,     9,    30,    33,    16,     0,
       0,    26,     0,     0,     0,   129,    23,     0,     0,   131,
     132,     0,     0,     0,   124,   125,     0,     0,     0,     0,
       0,     0,     0,   130,    79,    85,     0,    80,    81,    85,
       0,    65,     0,     1,     2,     0,     0,     0,     0,    22,
       0,     0,    60,     0,     0,     3,     0,     0,     0,     0,
       0,     0,     0,     0,    87,    83,     0,     0,    84,     0,
      32,    34,    17,     0,     0,     0,     0,     0,    28,    31,
     130,    60,    74,     0,     0,    18,     0,     0,     0,     0,
       0,     0,    86,     0,    99,    60,    94,    82,    64,     0,
      36,     0,     0,    38,     0,     0,    62,    61,     0,     0,
       0,    29,     0,   127,   128,    19,    88,    89,    90,    91,
      92,    93,     0,     0,     0,   110,     0,    95,    21,     0,
      41,     0,    43,    40,    24,     0,    25,    52,    46,    49,
      53,     0,     0,     0,    44,     0,    70,    69,    71,    66,
      67,    68,     0,     0,    75,    72,    73,    76,   126,     0,
     102,   100,     0,   114,    96,    37,     0,    39,    47,    50,
      48,    51,    27,     0,    63,    73,    54,    56,    55,    57,
       0,     0,     0,    98,     0,     0,     0,   104,     0,    45,
      77,    78,    97,    58,   101,   112,   111,     0,     0,   115,
     116,     0,   122,    42,     0,     0,     0,     0,     0,     0,
       0,    35,    59,   113,   118,   120,   119,   121,   117,   109,
     103,   123,   108,   107,   105,     0,   109,   106
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -158,  -158,  -158,  -158,  -158,  -158,  -158,  -158,   -56,   178,
    -158,   149,    90,  -158,  -158,  -118,  -157,  -158,   -41,  -158,
     -10,  -128,  -130,  -158,   111,  -158,  -158,   158,   183,    -6,
      -1,  -158,  -158,  -158,     0,  -158,  -158,  -158,  -158,    19,
    -158,  -158,  -158,     7,   -71,  -158
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
     119,   122,   120,   153,   163,   175,   126,   214,    98,   127,
     195,   172,   217,   101,   102,    56,    57,    58,    85,   129,
     114,   115,   222,   240,   244,   183,   216,   207,   219,   220,
     231,    47,   135,    60,    61,    41
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      55,   173,   177,   103,    59,    65,    90,   164,   194,    28,
      33,    36,    83,    97,    53,    42,    43,    97,   118,   146,
      39,    40,   121,   123,   123,    35,   143,   202,    35,   106,
      37,    84,   113,    34,    29,    44,    45,    67,   147,    68,
      69,    70,   196,   198,    71,    72,   203,   213,    62,    46,
      48,    49,    50,    51,    52,   242,    38,   142,   179,   103,
     131,   243,   130,    63,    53,    64,   144,   232,   107,   108,
     109,   110,   111,    92,   145,   209,    30,    55,   121,    54,
      99,    59,   210,   211,   187,   133,   134,   128,    31,   226,
     227,   148,   149,   116,   154,   155,   234,   236,     1,    32,
       2,    65,     3,     4,     5,   156,   155,     6,   150,   151,
     152,    66,   188,   189,     7,   190,   191,    73,     8,     9,
      10,    74,   176,   166,   167,   168,    75,    11,    12,    13,
      14,    15,    16,   169,   192,   193,    17,    76,   200,   201,
     170,   171,   180,   181,    48,    49,    50,    51,    52,    77,
     116,   116,  -129,    86,    78,   128,    18,    79,    53,   157,
     158,   159,   160,    80,    87,    89,   197,   199,   161,   162,
      48,    49,    50,    51,    52,    53,   157,   158,   159,   160,
     157,   158,   159,   160,    53,   161,   162,    81,    82,   161,
     162,    10,    93,    94,   128,   215,    95,    96,    97,   100,
     218,   -20,   104,    53,   105,   112,   136,   165,   125,   182,
     132,   178,   184,   186,   128,   233,   137,   204,   138,   239,
     235,   237,   218,   139,   140,   141,   205,   206,   221,   208,
     212,   223,   224,   225,   228,   246,   229,   230,   241,   185,
     245,   174,    88,    91,   124,   117,   247,   238
};

static const yytype_uint8 yycheck[] =
{
      10,   129,   132,    74,    10,    17,    62,   125,   165,     4,
       6,     4,    43,    22,    62,     8,    20,    22,    89,    43,
      62,    63,    93,    94,    95,    62,    31,    43,    62,    77,
      10,    62,    69,    29,    29,    39,    40,    30,    62,    32,
      33,    34,   172,   173,    37,    38,    62,   204,    42,    53,
      48,    49,    50,    51,    52,     8,    13,   113,    70,   130,
     101,    14,    71,     0,    62,    67,    71,   224,    78,    79,
      80,    81,    82,    66,   115,   193,     6,    87,   149,    77,
      73,    87,   200,   201,   155,    56,    57,    97,    18,   217,
     218,    70,    71,    86,    70,    71,   226,   227,     3,    29,
       5,    17,     7,     8,     9,    70,    71,    12,    26,    27,
      28,    13,    64,    65,    19,    64,    65,    10,    23,    24,
      25,    24,   132,    58,    59,    60,    21,    32,    33,    34,
      35,    36,    37,    68,    70,    71,    41,    54,    72,    73,
      75,    76,   143,   144,    48,    49,    50,    51,    52,    68,
     143,   144,    74,    13,    69,   165,    61,    69,    62,    63,
      64,    65,    66,    69,    71,    74,   172,   173,    72,    73,
      48,    49,    50,    51,    52,    62,    63,    64,    65,    66,
      63,    64,    65,    66,    62,    72,    73,    69,    69,    72,
      73,    25,    69,    69,   204,   205,    69,    11,    22,    62,
     206,    67,    55,    62,    66,    62,    70,    30,    69,    45,
      68,    54,    62,    69,   224,   225,    70,    44,    70,   229,
     226,   227,   228,    70,    70,    70,    16,    46,    15,    64,
      62,    70,    30,    71,    30,   245,    16,    47,    64,   149,
      71,   130,    59,    65,    95,    87,   246,   228
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     7,     8,     9,    12,    19,    23,    24,
      25,    32,    33,    34,    35,    36,    37,    41,    61,    79,
      80,    81,    82,    83,    84,    85,    86,    87,     4,    29,
       6,    18,    29,     6,    29,    62,   121,    10,    13,    62,
      63,   123,   121,    20,    39,    40,    53,   119,    48,    49,
      50,    51,    52,    62,    77,    98,   103,   104,   105,   107,
     121,   122,    42,     0,    67,    17,    13,   121,   121,   121,
     121,   121,   121,    10,    24,    21,    54,    68,    69,    69,
      69,    69,    69,    43,    62,   106,    13,    71,   106,    74,
      86,    87,   121,    69,    69,    69,    11,    22,    96,   121,
      62,   101,   102,   122,    55,    66,    77,    98,    98,    98,
      98,    98,    62,    69,   108,   109,   121,   105,   122,    88,
      90,   122,    89,   122,    89,    69,    94,    97,    98,   107,
      71,    96,    68,    56,    57,   120,    70,    70,    70,    70,
      70,    70,    86,    31,    71,    96,    43,    62,    70,    71,
      26,    27,    28,    91,    70,    71,    70,    63,    64,    65,
      66,    72,    73,    92,    93,    30,    58,    59,    60,    68,
      75,    76,    99,    99,   102,    93,    98,   100,    54,    70,
     108,   108,    45,   113,    62,    90,    69,   122,    64,    65,
      64,    65,    70,    71,    94,    98,   100,   107,   100,   107,
      72,    73,    43,    62,    44,    16,    46,   115,    64,    93,
      93,    93,    62,    94,    95,    98,   114,   100,   107,   116,
     117,    15,   110,    70,    30,    71,    99,    99,    30,    16,
      47,   118,    94,    98,   100,   107,   100,   107,   117,    98,
     111,    64,     8,    14,   112,    71,    98,   112
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    78,    79,    79,    79,    79,    79,    80,    80,    80,
      80,    80,    81,    81,    81,    81,    82,    82,    83,    83,
      83,    84,    84,    84,    84,    84,    84,    85,    85,    85,
      85,    85,    85,    86,    86,    87,    88,    88,    89,    89,
      90,    91,    91,    91,    92,    92,    93,    93,    93,    93,
      93,    93,    93,    93,    94,    94,    94,    94,    95,    95,
      96,    96,    97,    97,    98,    98,    99,    99,    99,    99,
      99,    99,   100,   100,   101,   101,   102,   102,   102,   103,
     103,   104,   104,   105,   105,   106,   106,   106,   107,   107,
     107,   107,   107,   107,   108,   108,   108,   108,   108,   109,
     109,   109,   109,   110,   110,   111,   111,   112,   112,   112,
     113,   113,   114,   114,   115,   115,   116,   116,   117,   117,
     117,   117,   118,   118,   119,   119,   120,   120,   120,   121,
     122,   123,   123
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     4,     4,     5,
       3,     6,     3,     2,     6,     6,     2,     7,     4,     5,
       1,     4,     3,     1,     3,     9,     1,     3,     1,     3,
       2,     1,     4,     1,     1,     3,     1,     2,     2,     1,
       2,     2,     1,     1,     3,     3,     3,     3,     1,     3,
       0,     2,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     5,     5,     1,
       1,     1,     3,     2,     2,     0,     2,     1,     4,     4,
       4,     4,     4,     4,     1,     2,     3,     5,     4,     1,
       3,     5,     3,     3,     0,     2,     4,     1,     1,     0,
       0,     3,     1,     3,     0,     2,     1,     3,     3,     3,
       3,     3,     0,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1
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
#line 73 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        parse_tree = (yyvsp[-1].sv_node);
        YYACCEPT;
    }
#line 1774 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 3: /* start: SET OUTPUT_FILE OFF  */
#line 78 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        SetKnobType type = OutputFile;
        parse_tree = std::make_shared<SetStmt>(type, false);
        YYACCEPT;
    }
#line 1784 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 4: /* start: HELP  */
#line 84 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        parse_tree = std::make_shared<Help>();
        YYACCEPT;
    }
#line 1793 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 5: /* start: EXIT  */
#line 89 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1802 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 6: /* start: T_EOF  */
#line 94 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1811 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 12: /* txnStmt: TXN_BEGIN  */
#line 110 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnBegin>();
    }
#line 1819 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 13: /* txnStmt: TXN_COMMIT  */
#line 114 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnCommit>();
    }
#line 1827 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 14: /* txnStmt: TXN_ABORT  */
#line 118 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnAbort>();
    }
#line 1835 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 15: /* txnStmt: TXN_ROLLBACK  */
#line 122 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnRollback>();
    }
#line 1843 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 16: /* dbStmt: SHOW TABLES  */
#line 129 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowTables>();
    }
#line 1851 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 17: /* dbStmt: SHOW INDEX FROM tbName  */
#line 133 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowIndex>((yyvsp[0].sv_str));
    }
#line 1859 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 18: /* setStmt: SET set_knob_type '=' VALUE_BOOL  */
#line 140 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SetStmt>((yyvsp[-2].sv_setKnobType), (yyvsp[0].sv_bool));
    }
#line 1867 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 19: /* setStmt: SET TRANSACTION ISOLATION LEVEL isolation_level  */
#line 144 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SetIsolationStmt>((yyvsp[0].sv_isolation_level));
    }
#line 1875 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 20: /* setStmt: SET OUTPUT_FILE OFF  */
#line 148 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        SetKnobType type = OutputFile;
        (yyval.sv_node) = std::make_shared<SetStmt>(type, false);
    }
#line 1884 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 21: /* ddl: CREATE TABLE tbName '(' fieldList ')'  */
#line 156 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateTable>((yyvsp[-3].sv_str), (yyvsp[-1].sv_fields));
    }
#line 1892 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 22: /* ddl: DROP TABLE tbName  */
#line 160 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropTable>((yyvsp[0].sv_str));
    }
#line 1900 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 23: /* ddl: DESC tbName  */
#line 164 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DescTable>((yyvsp[0].sv_str));
    }
#line 1908 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 24: /* ddl: CREATE INDEX tbName '(' colNameList ')'  */
#line 168 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1916 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 25: /* ddl: DROP INDEX tbName '(' colNameList ')'  */
#line 172 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1924 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 26: /* ddl: CREATE STATIC_CHECKPOINT  */
#line 176 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<StaticCheckpoint>();
    }
#line 1932 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 27: /* dml: INSERT INTO tbName VALUES '(' valueList ')'  */
#line 183 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<InsertStmt>((yyvsp[-4].sv_str), (yyvsp[-1].sv_vals));
    }
#line 1940 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 28: /* dml: DELETE FROM tbName optWhereClause  */
#line 187 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DeleteStmt>((yyvsp[-1].sv_str), (yyvsp[0].sv_conds));
    }
#line 1948 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 29: /* dml: UPDATE tbName SET setClauses optWhereClause  */
#line 191 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<UpdateStmt>((yyvsp[-3].sv_str), (yyvsp[-1].sv_set_clauses), (yyvsp[0].sv_conds));
    }
#line 1956 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 30: /* dml: queryExpr  */
#line 195 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1964 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 31: /* dml: LOAD fileName INTO tbName  */
#line 199 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<LoadStmt>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 1972 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 32: /* dml: EXPLAIN ANALYZE queryExpr  */
#line 203 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        auto sel = std::dynamic_pointer_cast<SelectStmt>((yyvsp[0].sv_node));
        if (sel == nullptr) {
            throw RMDBError("failure");
        }
        (yyval.sv_node) = std::make_shared<ExplainStmt>(sel);
    }
#line 1984 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 33: /* queryExpr: selectStmt  */
#line 214 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1992 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 34: /* queryExpr: queryExpr UNION selectStmt  */
#line 218 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        if (auto u = std::dynamic_pointer_cast<UnionStmt>((yyvsp[-2].sv_node))) {
            u->append((yyvsp[0].sv_node));
            (yyval.sv_node) = u;
        } else {
            (yyval.sv_node) = std::make_shared<UnionStmt>((yyvsp[-2].sv_node), (yyvsp[0].sv_node));
        }
    }
#line 2005 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 35: /* selectStmt: SELECT selector FROM fromClause optWhereClause opt_group_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 230 "/home/yc_dada/db2026/src/parser/yacc.y"
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
#line 2028 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 36: /* fieldList: field  */
#line 252 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_fields) = std::vector<std::shared_ptr<Field>>{(yyvsp[0].sv_field)};
    }
#line 2036 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 37: /* fieldList: fieldList ',' field  */
#line 256 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_fields).push_back((yyvsp[0].sv_field));
    }
#line 2044 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 38: /* colNameList: colName  */
#line 263 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_strs) = std::vector<std::string>{(yyvsp[0].sv_str)};
    }
#line 2052 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 39: /* colNameList: colNameList ',' colName  */
#line 267 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_strs).push_back((yyvsp[0].sv_str));
    }
#line 2060 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 40: /* field: colName type  */
#line 274 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_field) = std::make_shared<ColDef>((yyvsp[-1].sv_str), (yyvsp[0].sv_type_len));
    }
#line 2068 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 41: /* type: INT  */
#line 281 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_INT, sizeof(int));
    }
#line 2076 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 42: /* type: CHAR '(' VALUE_INT ')'  */
#line 285 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_STRING, (yyvsp[-1].sv_int));
    }
#line 2084 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 43: /* type: FLOAT  */
#line 289 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_FLOAT, sizeof(float));
    }
#line 2092 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 44: /* valueList: value  */
#line 296 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_vals) = std::vector<std::shared_ptr<Value>>{(yyvsp[0].sv_val)};
    }
#line 2100 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 45: /* valueList: valueList ',' value  */
#line 300 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_vals).push_back((yyvsp[0].sv_val));
    }
#line 2108 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 46: /* value: VALUE_INT  */
#line 307 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<IntLit>((yyvsp[0].sv_int));
    }
#line 2116 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 47: /* value: '+' VALUE_INT  */
#line 311 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<IntLit>((yyvsp[0].sv_int));
    }
#line 2124 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 48: /* value: '-' VALUE_INT  */
#line 315 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<IntLit>(-(yyvsp[0].sv_int));
    }
#line 2132 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 49: /* value: VALUE_FLOAT  */
#line 319 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<FloatLit>((yyvsp[0].sv_float));
    }
#line 2140 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 50: /* value: '+' VALUE_FLOAT  */
#line 323 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<FloatLit>((yyvsp[0].sv_float));
    }
#line 2148 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 51: /* value: '-' VALUE_FLOAT  */
#line 327 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<FloatLit>(-(yyvsp[0].sv_float));
    }
#line 2156 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 52: /* value: VALUE_STRING  */
#line 331 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<StringLit>((yyvsp[0].sv_str));
    }
#line 2164 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 53: /* value: VALUE_BOOL  */
#line 335 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<BoolLit>((yyvsp[0].sv_bool));
    }
#line 2172 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 54: /* condition: col op expr  */
#line 342 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>((yyvsp[-2].sv_col), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2180 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 55: /* condition: aggregate op expr  */
#line 346 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), (yyvsp[-1].sv_comp_op),
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
#line 2189 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 56: /* condition: col op aggregate  */
#line 351 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), (yyvsp[-1].sv_comp_op),
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
#line 2198 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 57: /* condition: aggregate op aggregate  */
#line 356 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), (yyvsp[-1].sv_comp_op),
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
#line 2207 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 58: /* joinConds: condition  */
#line 364 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)};
    }
#line 2215 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 59: /* joinConds: joinConds AND condition  */
#line 368 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_conds).push_back((yyvsp[0].sv_cond));
    }
#line 2223 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 60: /* optWhereClause: %empty  */
#line 374 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { /* ignore*/ }
#line 2229 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 61: /* optWhereClause: WHERE whereClause  */
#line 376 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_conds) = (yyvsp[0].sv_conds);
    }
#line 2237 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 62: /* whereClause: condition  */
#line 383 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)};
    }
#line 2245 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 63: /* whereClause: whereClause AND condition  */
#line 387 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_conds).push_back((yyvsp[0].sv_cond));
    }
#line 2253 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 64: /* col: tbName '.' colName  */
#line 394 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2261 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 65: /* col: colName  */
#line 398 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>("", (yyvsp[0].sv_str));
    }
#line 2269 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 66: /* op: '='  */
#line 416 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_EQ;
    }
#line 2277 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 67: /* op: '<'  */
#line 420 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LT;
    }
#line 2285 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 68: /* op: '>'  */
#line 424 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GT;
    }
#line 2293 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 69: /* op: NEQ  */
#line 428 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_NE;
    }
#line 2301 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 70: /* op: LEQ  */
#line 432 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LE;
    }
#line 2309 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 71: /* op: GEQ  */
#line 436 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GE;
    }
#line 2317 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 72: /* expr: value  */
#line 443 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_val));
    }
#line 2325 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 73: /* expr: col  */
#line 447 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_col));
    }
#line 2333 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 74: /* setClauses: setClause  */
#line 454 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses) = std::vector<std::shared_ptr<SetClause>>{(yyvsp[0].sv_set_clause)};
    }
#line 2341 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 75: /* setClauses: setClauses ',' setClause  */
#line 458 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_set_clauses).push_back((yyvsp[0].sv_set_clause));
    }
#line 2349 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 76: /* setClause: colName '=' expr  */
#line 465 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_set_clause) = std::make_shared<SetClause>((yyvsp[-2].sv_str), (yyvsp[0].sv_expr));
    }
#line 2357 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 77: /* setClause: colName '=' col '+' value  */
#line 469 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_set_clause) = std::make_shared<SetClause>(
            (yyvsp[-4].sv_str), std::static_pointer_cast<Expr>(std::make_shared<ArithmeticExpr>((yyvsp[-2].sv_col), '+', (yyvsp[0].sv_val))));
    }
#line 2366 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 78: /* setClause: colName '=' col '-' value  */
#line 474 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_set_clause) = std::make_shared<SetClause>(
            (yyvsp[-4].sv_str), std::static_pointer_cast<Expr>(std::make_shared<ArithmeticExpr>((yyvsp[-2].sv_col), '-', (yyvsp[0].sv_val))));
    }
#line 2375 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 79: /* selector: '*'  */
#line 482 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_items) = {};
    }
#line 2383 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 81: /* selectList: selectItem  */
#line 490 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_items) = std::vector<std::shared_ptr<SelectItem>>{(yyvsp[0].sv_select_item)};
    }
#line 2391 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 82: /* selectList: selectList ',' selectItem  */
#line 494 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_items).push_back((yyvsp[0].sv_select_item));
    }
#line 2399 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 83: /* selectItem: col optAlias  */
#line 501 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_item) = std::make_shared<SelectItem>(std::static_pointer_cast<Expr>((yyvsp[-1].sv_col)), (yyvsp[0].sv_str));
    }
#line 2407 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 84: /* selectItem: aggregate optAlias  */
#line 505 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_select_item) = std::make_shared<SelectItem>(std::static_pointer_cast<Expr>((yyvsp[-1].sv_agg_expr)), (yyvsp[0].sv_str));
    }
#line 2415 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 85: /* optAlias: %empty  */
#line 511 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_str) = ""; }
#line 2421 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 86: /* optAlias: AS IDENTIFIER  */
#line 512 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_str) = (yyvsp[0].sv_str); }
#line 2427 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 87: /* optAlias: IDENTIFIER  */
#line 513 "/home/yc_dada/db2026/src/parser/yacc.y"
                   { (yyval.sv_str) = (yyvsp[0].sv_str); }
#line 2433 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 88: /* aggregate: COUNT '(' '*' ')'  */
#line 518 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_COUNT, nullptr, true);
    }
#line 2441 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 89: /* aggregate: COUNT '(' col ')'  */
#line 522 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_COUNT, (yyvsp[-1].sv_col), false);
    }
#line 2449 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 90: /* aggregate: MAX '(' col ')'  */
#line 526 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_MAX, (yyvsp[-1].sv_col), false);
    }
#line 2457 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 91: /* aggregate: MIN '(' col ')'  */
#line 530 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_MIN, (yyvsp[-1].sv_col), false);
    }
#line 2465 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 92: /* aggregate: SUM '(' col ')'  */
#line 534 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_SUM, (yyvsp[-1].sv_col), false);
    }
#line 2473 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 93: /* aggregate: AVG '(' col ')'  */
#line 538 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_AVG, (yyvsp[-1].sv_col), false);
    }
#line 2481 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 94: /* tableRef: tbName  */
#line 561 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[0].sv_str));
    }
#line 2489 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 95: /* tableRef: tbName IDENTIFIER  */
#line 565 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-1].sv_str), (yyvsp[0].sv_str));
    }
#line 2497 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 96: /* tableRef: tbName AS IDENTIFIER  */
#line 569 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2505 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 97: /* tableRef: '(' queryExpr ')' AS IDENTIFIER  */
#line 573 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-3].sv_node), (yyvsp[0].sv_str));
    }
#line 2513 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 98: /* tableRef: '(' queryExpr ')' IDENTIFIER  */
#line 577 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-2].sv_node), (yyvsp[0].sv_str));
    }
#line 2521 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 99: /* fromClause: tableRef  */
#line 585 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = std::make_shared<FromClause>();
        (yyval.sv_from_clause)->refs.push_back((yyvsp[0].sv_table_ref));
    }
#line 2530 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 100: /* fromClause: fromClause ',' tableRef  */
#line 590 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = (yyvsp[-2].sv_from_clause);
        (yyval.sv_from_clause)->refs.push_back((yyvsp[0].sv_table_ref));
    }
#line 2539 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 101: /* fromClause: fromClause JOIN tableRef ON joinConds  */
#line 595 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = (yyvsp[-4].sv_from_clause);
        (yyval.sv_from_clause)->refs.push_back((yyvsp[-2].sv_table_ref));
        for (auto &cond : (yyvsp[0].sv_conds)) {
            (yyval.sv_from_clause)->conds.push_back(cond);
        }
    }
#line 2551 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 102: /* fromClause: fromClause JOIN tableRef  */
#line 603 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = (yyvsp[-2].sv_from_clause);
        (yyval.sv_from_clause)->refs.push_back((yyvsp[0].sv_table_ref));
    }
#line 2560 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 103: /* opt_order_clause: ORDER BY order_clause  */
#line 611 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_orderby) = (yyvsp[0].sv_orderby);
    }
#line 2568 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 104: /* opt_order_clause: %empty  */
#line 614 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { /* ignore*/ }
#line 2574 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 105: /* order_clause: col opt_asc_desc  */
#line 619 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_orderby) = std::make_shared<OrderBy>((yyvsp[-1].sv_col), (yyvsp[0].sv_orderby_dir));
    }
#line 2582 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 106: /* order_clause: order_clause ',' col opt_asc_desc  */
#line 623 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_orderby) = (yyvsp[-3].sv_orderby);
        (yyval.sv_orderby)->append((yyvsp[-1].sv_col), (yyvsp[0].sv_orderby_dir));
    }
#line 2591 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 107: /* opt_asc_desc: ASC  */
#line 630 "/home/yc_dada/db2026/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_ASC;     }
#line 2597 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 108: /* opt_asc_desc: DESC  */
#line 631 "/home/yc_dada/db2026/src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_DESC;    }
#line 2603 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 109: /* opt_asc_desc: %empty  */
#line 632 "/home/yc_dada/db2026/src/parser/yacc.y"
            { (yyval.sv_orderby_dir) = OrderBy_DEFAULT; }
#line 2609 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 110: /* opt_group_clause: %empty  */
#line 636 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_cols) = {}; }
#line 2615 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 111: /* opt_group_clause: GROUP BY group_clause  */
#line 638 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cols) = (yyvsp[0].sv_cols);
    }
#line 2623 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 112: /* group_clause: col  */
#line 645 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cols) = std::vector<std::shared_ptr<Col>>{(yyvsp[0].sv_col)};
    }
#line 2631 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 113: /* group_clause: group_clause ',' col  */
#line 649 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_cols).push_back((yyvsp[0].sv_col));
    }
#line 2639 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 114: /* opt_having_clause: %empty  */
#line 655 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_having_conds) = {}; }
#line 2645 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 115: /* opt_having_clause: HAVING havingClause  */
#line 657 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_conds) = (yyvsp[0].sv_having_conds);
    }
#line 2653 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 116: /* havingClause: havingCondition  */
#line 664 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_conds) = std::vector<std::shared_ptr<HavingExpr>>{(yyvsp[0].sv_having_cond)};
    }
#line 2661 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 117: /* havingClause: havingClause AND havingCondition  */
#line 668 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_conds).push_back((yyvsp[0].sv_having_cond));
    }
#line 2669 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 118: /* havingCondition: expr op expr  */
#line 675 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>((yyvsp[-2].sv_expr), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2677 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 119: /* havingCondition: aggregate op expr  */
#line 679 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_agg_expr)), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2685 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 120: /* havingCondition: expr op aggregate  */
#line 683 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>((yyvsp[-2].sv_expr), (yyvsp[-1].sv_comp_op), std::static_pointer_cast<Expr>((yyvsp[0].sv_agg_expr)));
    }
#line 2693 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 121: /* havingCondition: aggregate op aggregate  */
#line 687 "/home/yc_dada/db2026/src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_agg_expr)), (yyvsp[-1].sv_comp_op), std::static_pointer_cast<Expr>((yyvsp[0].sv_agg_expr)));
    }
#line 2701 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 122: /* opt_limit_clause: %empty  */
#line 693 "/home/yc_dada/db2026/src/parser/yacc.y"
                      { (yyval.sv_int) = -1; }
#line 2707 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 123: /* opt_limit_clause: LIMIT VALUE_INT  */
#line 694 "/home/yc_dada/db2026/src/parser/yacc.y"
                        { (yyval.sv_int) = (yyvsp[0].sv_int); }
#line 2713 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 124: /* set_knob_type: ENABLE_NESTLOOP  */
#line 698 "/home/yc_dada/db2026/src/parser/yacc.y"
                    { (yyval.sv_setKnobType) = EnableNestLoop; }
#line 2719 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 125: /* set_knob_type: ENABLE_SORTMERGE  */
#line 699 "/home/yc_dada/db2026/src/parser/yacc.y"
                         { (yyval.sv_setKnobType) = EnableSortMerge; }
#line 2725 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 126: /* isolation_level: SNAPSHOT ISOLATION  */
#line 703 "/home/yc_dada/db2026/src/parser/yacc.y"
                           { (yyval.sv_isolation_level) = SnapshotIsolation; }
#line 2731 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 127: /* isolation_level: SNAPSHOT  */
#line 704 "/home/yc_dada/db2026/src/parser/yacc.y"
                 { (yyval.sv_isolation_level) = SnapshotIsolation; }
#line 2737 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 128: /* isolation_level: SERIALIZABLE  */
#line 705 "/home/yc_dada/db2026/src/parser/yacc.y"
                     { (yyval.sv_isolation_level) = Serializable; }
#line 2743 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 131: /* fileName: IDENTIFIER  */
#line 713 "/home/yc_dada/db2026/src/parser/yacc.y"
                   { (yyval.sv_str) = (yyvsp[0].sv_str); }
#line 2749 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;

  case 132: /* fileName: VALUE_STRING  */
#line 714 "/home/yc_dada/db2026/src/parser/yacc.y"
                     { (yyval.sv_str) = (yyvsp[0].sv_str); }
#line 2755 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"
    break;


#line 2759 "/home/yc_dada/db2026/src/parser/yacc.tab.cpp"

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

#line 716 "/home/yc_dada/db2026/src/parser/yacc.y"

