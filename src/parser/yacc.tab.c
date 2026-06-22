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
#line 1 "src/parser/yacc.y"

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

#line 88 "src/parser/yacc.tab.c"

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
  YYSYMBOL_TRANSACTION = 49,               /* TRANSACTION  */
  YYSYMBOL_ISOLATION = 50,                 /* ISOLATION  */
  YYSYMBOL_LEVEL = 51,                     /* LEVEL  */
  YYSYMBOL_SNAPSHOT = 52,                  /* SNAPSHOT  */
  YYSYMBOL_SERIALIZABLE = 53,              /* SERIALIZABLE  */
  YYSYMBOL_LEQ = 54,                       /* LEQ  */
  YYSYMBOL_NEQ = 55,                       /* NEQ  */
  YYSYMBOL_GEQ = 56,                       /* GEQ  */
  YYSYMBOL_T_EOF = 57,                     /* T_EOF  */
  YYSYMBOL_IDENTIFIER = 58,                /* IDENTIFIER  */
  YYSYMBOL_VALUE_STRING = 59,              /* VALUE_STRING  */
  YYSYMBOL_VALUE_INT = 60,                 /* VALUE_INT  */
  YYSYMBOL_VALUE_FLOAT = 61,               /* VALUE_FLOAT  */
  YYSYMBOL_VALUE_BOOL = 62,                /* VALUE_BOOL  */
  YYSYMBOL_63_ = 63,                       /* ';'  */
  YYSYMBOL_64_ = 64,                       /* '='  */
  YYSYMBOL_65_ = 65,                       /* '('  */
  YYSYMBOL_66_ = 66,                       /* ')'  */
  YYSYMBOL_67_ = 67,                       /* ','  */
  YYSYMBOL_68_ = 68,                       /* '.'  */
  YYSYMBOL_69_ = 69,                       /* '<'  */
  YYSYMBOL_70_ = 70,                       /* '>'  */
  YYSYMBOL_71_ = 71,                       /* '*'  */
  YYSYMBOL_YYACCEPT = 72,                  /* $accept  */
  YYSYMBOL_start = 73,                     /* start  */
  YYSYMBOL_stmt = 74,                      /* stmt  */
  YYSYMBOL_txnStmt = 75,                   /* txnStmt  */
  YYSYMBOL_dbStmt = 76,                    /* dbStmt  */
  YYSYMBOL_setStmt = 77,                   /* setStmt  */
  YYSYMBOL_ddl = 78,                       /* ddl  */
  YYSYMBOL_dml = 79,                       /* dml  */
  YYSYMBOL_queryExpr = 80,                 /* queryExpr  */
  YYSYMBOL_selectStmt = 81,                /* selectStmt  */
  YYSYMBOL_fieldList = 82,                 /* fieldList  */
  YYSYMBOL_colNameList = 83,               /* colNameList  */
  YYSYMBOL_field = 84,                     /* field  */
  YYSYMBOL_type = 85,                      /* type  */
  YYSYMBOL_valueList = 86,                 /* valueList  */
  YYSYMBOL_value = 87,                     /* value  */
  YYSYMBOL_condition = 88,                 /* condition  */
  YYSYMBOL_joinConds = 89,                 /* joinConds  */
  YYSYMBOL_optWhereClause = 90,            /* optWhereClause  */
  YYSYMBOL_whereClause = 91,               /* whereClause  */
  YYSYMBOL_col = 92,                       /* col  */
  YYSYMBOL_op = 93,                        /* op  */
  YYSYMBOL_expr = 94,                      /* expr  */
  YYSYMBOL_setClauses = 95,                /* setClauses  */
  YYSYMBOL_setClause = 96,                 /* setClause  */
  YYSYMBOL_selector = 97,                  /* selector  */
  YYSYMBOL_selectList = 98,                /* selectList  */
  YYSYMBOL_selectItem = 99,                /* selectItem  */
  YYSYMBOL_optAlias = 100,                 /* optAlias  */
  YYSYMBOL_aggregate = 101,                /* aggregate  */
  YYSYMBOL_tableRef = 102,                 /* tableRef  */
  YYSYMBOL_fromClause = 103,               /* fromClause  */
  YYSYMBOL_opt_order_clause = 104,         /* opt_order_clause  */
  YYSYMBOL_order_clause = 105,             /* order_clause  */
  YYSYMBOL_opt_asc_desc = 106,             /* opt_asc_desc  */
  YYSYMBOL_opt_group_clause = 107,         /* opt_group_clause  */
  YYSYMBOL_group_clause = 108,             /* group_clause  */
  YYSYMBOL_opt_having_clause = 109,        /* opt_having_clause  */
  YYSYMBOL_havingClause = 110,             /* havingClause  */
  YYSYMBOL_havingCondition = 111,          /* havingCondition  */
  YYSYMBOL_opt_limit_clause = 112,         /* opt_limit_clause  */
  YYSYMBOL_set_knob_type = 113,            /* set_knob_type  */
  YYSYMBOL_isolation_level = 114,          /* isolation_level  */
  YYSYMBOL_tbName = 115,                   /* tbName  */
  YYSYMBOL_colName = 116                   /* colName  */
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
#define YYFINAL  57
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   218

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  120
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  228

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   317


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
      65,    66,    71,     2,    67,     2,    68,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    63,
      69,    64,    70,     2,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    71,    71,    76,    81,    86,    94,    95,    96,    97,
      98,   102,   106,   110,   114,   121,   125,   132,   136,   143,
     147,   151,   155,   159,   166,   170,   174,   178,   182,   193,
     197,   209,   231,   235,   242,   246,   253,   260,   264,   268,
     275,   279,   286,   290,   294,   298,   305,   309,   314,   319,
     327,   331,   338,   339,   346,   350,   357,   361,   379,   383,
     387,   391,   395,   399,   406,   410,   417,   421,   428,   435,
     439,   443,   447,   454,   458,   465,   466,   467,   471,   475,
     479,   483,   487,   491,   514,   518,   522,   526,   530,   538,
     543,   548,   556,   564,   568,   572,   576,   584,   585,   586,
     590,   591,   598,   602,   609,   610,   617,   621,   628,   632,
     636,   640,   647,   648,   652,   653,   657,   658,   659,   662,
     664
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
  "GROUP", "HAVING", "LIMIT", "COUNT", "MAX", "MIN", "SUM", "AVG",
  "TRANSACTION", "ISOLATION", "LEVEL", "SNAPSHOT", "SERIALIZABLE", "LEQ",
  "NEQ", "GEQ", "T_EOF", "IDENTIFIER", "VALUE_STRING", "VALUE_INT",
  "VALUE_FLOAT", "VALUE_BOOL", "';'", "'='", "'('", "')'", "','", "'.'",
  "'<'", "'>'", "'*'", "$accept", "start", "stmt", "txnStmt", "dbStmt",
  "setStmt", "ddl", "dml", "queryExpr", "selectStmt", "fieldList",
  "colNameList", "field", "type", "valueList", "value", "condition",
  "joinConds", "optWhereClause", "whereClause", "col", "op", "expr",
  "setClauses", "setClause", "selector", "selectList", "selectItem",
  "optAlias", "aggregate", "tableRef", "fromClause", "opt_order_clause",
  "order_clause", "opt_asc_desc", "opt_group_clause", "group_clause",
  "opt_having_clause", "havingClause", "havingCondition",
  "opt_limit_clause", "set_knob_type", "isolation_level", "tbName",
  "colName", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-152)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-120)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      85,    14,     6,     7,   -42,    39,    10,   -42,    42,    -3,
    -152,  -152,  -152,  -152,  -152,  -152,    18,  -152,    51,    12,
    -152,  -152,  -152,  -152,  -152,    67,  -152,  -152,    76,   -42,
     -42,   -42,   -42,  -152,  -152,   -42,   -42,    83,  -152,  -152,
      46,    47,    63,    71,    93,    94,    95,    96,  -152,   -20,
     140,    98,  -152,   -20,    99,  -152,    86,  -152,  -152,    86,
     -42,    97,   101,  -152,   103,   150,   145,   111,   119,   109,
      37,   114,   114,   114,   114,   115,  -152,  -152,   -43,    54,
    -152,   111,    67,  -152,  -152,   111,   111,   111,   110,    54,
    -152,  -152,   -10,  -152,   112,    -6,  -152,   108,   113,   116,
     118,   120,   121,  -152,    86,  -152,   -17,   -18,  -152,  -152,
      43,  -152,   128,    64,  -152,    77,    87,  -152,   154,    65,
      65,   111,  -152,    87,   131,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,     0,   -43,   -43,   144,   130,  -152,  -152,
     111,  -152,   124,  -152,  -152,  -152,   111,  -152,  -152,  -152,
    -152,  -152,    88,  -152,    54,  -152,  -152,  -152,  -152,  -152,
    -152,    79,    79,  -152,  -152,  -152,    13,   151,  -152,   174,
     152,  -152,  -152,   132,  -152,  -152,    87,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,   135,  -152,    54,   114,    79,   182,
     133,  -152,  -152,  -152,   172,  -152,   137,    65,    65,   179,
    -152,   190,   164,  -152,    54,   114,    79,    79,    79,   114,
     148,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,    40,
     142,  -152,  -152,  -152,  -152,   114,    40,  -152
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     3,    11,    12,    13,    14,     0,     5,     0,     0,
       9,     6,    10,     7,     8,    27,    29,    15,     0,     0,
       0,     0,     0,   119,    21,     0,     0,     0,   114,   115,
       0,     0,     0,     0,     0,     0,     0,   120,    69,    75,
       0,    70,    71,    75,     0,    57,     0,     1,     2,     0,
       0,     0,     0,    20,     0,     0,    52,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    77,    73,     0,     0,
      74,     0,    28,    30,    16,     0,     0,     0,     0,     0,
      25,   120,    52,    66,     0,     0,    17,     0,     0,     0,
       0,     0,     0,    76,     0,    89,    52,    84,    72,    56,
       0,    32,     0,     0,    34,     0,     0,    54,    53,     0,
       0,     0,    26,     0,   117,   118,    18,    78,    79,    80,
      81,    82,    83,     0,     0,     0,   100,     0,    85,    19,
       0,    37,     0,    39,    36,    22,     0,    23,    44,    42,
      43,    45,     0,    40,     0,    62,    61,    63,    58,    59,
      60,     0,     0,    67,    68,   116,     0,    92,    90,     0,
     104,    86,    33,     0,    35,    24,     0,    55,    64,    65,
      46,    48,    47,    49,     0,    88,     0,     0,     0,    94,
       0,    41,    87,    50,    91,   102,   101,     0,     0,   105,
     106,     0,   112,    38,     0,     0,     0,     0,     0,     0,
       0,    31,    51,   103,   108,   110,   109,   111,   107,    99,
      93,   113,    98,    97,    95,     0,    99,    96
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,   -45,   153,
    -152,   123,    73,  -152,  -152,  -109,  -151,  -152,   -23,  -152,
      -9,  -111,  -125,  -152,    90,  -152,  -152,   136,   161,    -5,
     -62,  -152,  -152,  -152,    -8,  -152,  -152,  -152,  -152,     9,
    -152,  -152,  -152,    -2,   -61
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     110,   113,   111,   144,   152,   178,   117,   194,    90,   118,
     179,   161,   197,    92,    93,    50,    51,    52,    77,   120,
     105,   106,   202,   220,   224,   170,   196,   189,   199,   200,
     211,    41,   126,    54,    55
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      49,    89,    34,   177,    53,    37,    94,   153,    89,   162,
     134,    82,    29,    31,   164,    33,    33,    59,    27,    75,
     109,   137,   104,    36,   112,   114,   114,    61,    62,    63,
      64,    30,    32,    65,    66,   193,   180,   182,    76,    28,
     138,    42,    43,    44,    45,    46,   124,   125,   222,    35,
     135,    57,   184,   212,   223,    47,    56,   121,    84,   133,
      94,    98,    99,   100,   101,   102,   166,   191,    48,   122,
      49,   185,   167,   168,    53,    58,   107,    38,    39,   112,
     119,   214,   216,   136,    59,   174,   206,   207,     1,    60,
       2,    40,     3,     4,     5,    47,    68,     6,    42,    43,
      44,    45,    46,    67,     7,     8,     9,     9,    97,   139,
     140,    69,    47,    10,    11,    12,    13,    14,    15,   155,
     156,   157,    16,    42,    43,    44,    45,    46,    70,   158,
     145,   146,   107,   107,   159,   160,    71,    47,   148,   149,
     150,   151,    17,   147,   146,   119,   148,   149,   150,   151,
     141,   142,   143,    78,   175,   176,   181,   183,    72,    73,
      74,    88,    85,    89,  -119,    79,    86,    81,    87,    91,
      95,    96,    47,   103,   127,   116,   123,   119,   195,   128,
     154,   165,   129,   198,   130,   169,   131,   132,   171,   173,
     187,   186,   190,   192,   188,   119,   213,   201,   204,   203,
     219,   215,   217,   198,   205,   208,   209,   210,   221,   225,
     115,   163,    83,   172,    80,   108,   226,   218,   227
};

static const yytype_uint8 yycheck[] =
{
       9,    18,     4,   154,     9,     7,    67,   116,    18,   120,
      27,    56,     6,     6,   123,    58,    58,    17,     4,    39,
      81,    39,    65,    13,    85,    86,    87,    29,    30,    31,
      32,    25,    25,    35,    36,   186,   161,   162,    58,    25,
      58,    44,    45,    46,    47,    48,    52,    53,     8,    10,
      67,     0,    39,   204,    14,    58,    38,    67,    60,   104,
     121,    70,    71,    72,    73,    74,    66,   176,    71,    92,
      79,    58,   134,   135,    79,    63,    78,    35,    36,   140,
      89,   206,   207,   106,    17,   146,   197,   198,     3,    13,
       5,    49,     7,     8,     9,    58,    50,    12,    44,    45,
      46,    47,    48,    20,    19,    20,    21,    21,    71,    66,
      67,    64,    58,    28,    29,    30,    31,    32,    33,    54,
      55,    56,    37,    44,    45,    46,    47,    48,    65,    64,
      66,    67,   134,   135,    69,    70,    65,    58,    59,    60,
      61,    62,    57,    66,    67,   154,    59,    60,    61,    62,
      22,    23,    24,    13,    66,    67,   161,   162,    65,    65,
      65,    11,    65,    18,    68,    67,    65,    68,    65,    58,
      51,    62,    58,    58,    66,    65,    64,   186,   187,    66,
      26,    50,    66,   188,    66,    41,    66,    66,    58,    65,
      16,    40,    60,    58,    42,   204,   205,    15,    26,    66,
     209,   206,   207,   208,    67,    26,    16,    43,    60,    67,
      87,   121,    59,   140,    53,    79,   225,   208,   226
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     7,     8,     9,    12,    19,    20,    21,
      28,    29,    30,    31,    32,    33,    37,    57,    73,    74,
      75,    76,    77,    78,    79,    80,    81,     4,    25,     6,
      25,     6,    25,    58,   115,    10,    13,   115,    35,    36,
      49,   113,    44,    45,    46,    47,    48,    58,    71,    92,
      97,    98,    99,   101,   115,   116,    38,     0,    63,    17,
      13,   115,   115,   115,   115,   115,   115,    20,    50,    64,
      65,    65,    65,    65,    65,    39,    58,   100,    13,    67,
     100,    68,    80,    81,   115,    65,    65,    65,    11,    18,
      90,    58,    95,    96,   116,    51,    62,    71,    92,    92,
      92,    92,    92,    58,    65,   102,   103,   115,    99,   116,
      82,    84,   116,    83,   116,    83,    65,    88,    91,    92,
     101,    67,    90,    64,    52,    53,   114,    66,    66,    66,
      66,    66,    66,    80,    27,    67,    90,    39,    58,    66,
      67,    22,    23,    24,    85,    66,    67,    66,    59,    60,
      61,    62,    86,    87,    26,    54,    55,    56,    64,    69,
      70,    93,    93,    96,    87,    50,    66,   102,   102,    41,
     107,    58,    84,    65,   116,    66,    67,    88,    87,    92,
      94,   101,    94,   101,    39,    58,    40,    16,    42,   109,
      60,    87,    58,    88,    89,    92,   108,    94,   101,   110,
     111,    15,   104,    66,    26,    67,    93,    93,    26,    16,
      43,   112,    88,    92,    94,   101,    94,   101,   111,    92,
     105,    60,     8,    14,   106,    67,    92,   106
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    72,    73,    73,    73,    73,    74,    74,    74,    74,
      74,    75,    75,    75,    75,    76,    76,    77,    77,    78,
      78,    78,    78,    78,    79,    79,    79,    79,    79,    80,
      80,    81,    82,    82,    83,    83,    84,    85,    85,    85,
      86,    86,    87,    87,    87,    87,    88,    88,    88,    88,
      89,    89,    90,    90,    91,    91,    92,    92,    93,    93,
      93,    93,    93,    93,    94,    94,    95,    95,    96,    97,
      97,    98,    98,    99,    99,   100,   100,   100,   101,   101,
     101,   101,   101,   101,   102,   102,   102,   102,   102,   103,
     103,   103,   103,   104,   104,   105,   105,   106,   106,   106,
     107,   107,   108,   108,   109,   109,   110,   110,   111,   111,
     111,   111,   112,   112,   113,   113,   114,   114,   114,   115,
     116
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     4,     4,     5,     6,
       3,     2,     6,     6,     7,     4,     5,     1,     3,     1,
       3,     9,     1,     3,     1,     3,     2,     1,     4,     1,
       1,     3,     1,     1,     1,     1,     3,     3,     3,     3,
       1,     3,     0,     2,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     1,
       1,     1,     3,     2,     2,     0,     2,     1,     4,     4,
       4,     4,     4,     4,     1,     2,     3,     5,     4,     1,
       3,     5,     3,     3,     0,     2,     4,     1,     1,     0,
       0,     3,     1,     3,     0,     2,     1,     3,     3,     3,
       3,     3,     0,     2,     1,     1,     2,     1,     1,     1,
       1
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
#line 72 "src/parser/yacc.y"
    {
        parse_tree = (yyvsp[-1].sv_node);
        YYACCEPT;
    }
#line 1751 "src/parser/yacc.tab.c"
    break;

  case 3: /* start: HELP  */
#line 77 "src/parser/yacc.y"
    {
        parse_tree = std::make_shared<Help>();
        YYACCEPT;
    }
#line 1760 "src/parser/yacc.tab.c"
    break;

  case 4: /* start: EXIT  */
#line 82 "src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1769 "src/parser/yacc.tab.c"
    break;

  case 5: /* start: T_EOF  */
#line 87 "src/parser/yacc.y"
    {
        parse_tree = nullptr;
        YYACCEPT;
    }
#line 1778 "src/parser/yacc.tab.c"
    break;

  case 11: /* txnStmt: TXN_BEGIN  */
#line 103 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnBegin>();
    }
#line 1786 "src/parser/yacc.tab.c"
    break;

  case 12: /* txnStmt: TXN_COMMIT  */
#line 107 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnCommit>();
    }
#line 1794 "src/parser/yacc.tab.c"
    break;

  case 13: /* txnStmt: TXN_ABORT  */
#line 111 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnAbort>();
    }
#line 1802 "src/parser/yacc.tab.c"
    break;

  case 14: /* txnStmt: TXN_ROLLBACK  */
#line 115 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<TxnRollback>();
    }
#line 1810 "src/parser/yacc.tab.c"
    break;

  case 15: /* dbStmt: SHOW TABLES  */
#line 122 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowTables>();
    }
#line 1818 "src/parser/yacc.tab.c"
    break;

  case 16: /* dbStmt: SHOW INDEX FROM tbName  */
#line 126 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<ShowIndex>((yyvsp[0].sv_str));
    }
#line 1826 "src/parser/yacc.tab.c"
    break;

  case 17: /* setStmt: SET set_knob_type '=' VALUE_BOOL  */
#line 133 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SetStmt>((yyvsp[-2].sv_setKnobType), (yyvsp[0].sv_bool));
    }
#line 1834 "src/parser/yacc.tab.c"
    break;

  case 18: /* setStmt: SET TRANSACTION ISOLATION LEVEL isolation_level  */
#line 137 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<SetIsolationStmt>((yyvsp[0].sv_isolation_level));
    }
#line 1842 "src/parser/yacc.tab.c"
    break;

  case 19: /* ddl: CREATE TABLE tbName '(' fieldList ')'  */
#line 144 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateTable>((yyvsp[-3].sv_str), (yyvsp[-1].sv_fields));
    }
#line 1850 "src/parser/yacc.tab.c"
    break;

  case 20: /* ddl: DROP TABLE tbName  */
#line 148 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropTable>((yyvsp[0].sv_str));
    }
#line 1858 "src/parser/yacc.tab.c"
    break;

  case 21: /* ddl: DESC tbName  */
#line 152 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DescTable>((yyvsp[0].sv_str));
    }
#line 1866 "src/parser/yacc.tab.c"
    break;

  case 22: /* ddl: CREATE INDEX tbName '(' colNameList ')'  */
#line 156 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<CreateIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1874 "src/parser/yacc.tab.c"
    break;

  case 23: /* ddl: DROP INDEX tbName '(' colNameList ')'  */
#line 160 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DropIndex>((yyvsp[-3].sv_str), (yyvsp[-1].sv_strs));
    }
#line 1882 "src/parser/yacc.tab.c"
    break;

  case 24: /* dml: INSERT INTO tbName VALUES '(' valueList ')'  */
#line 167 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<InsertStmt>((yyvsp[-4].sv_str), (yyvsp[-1].sv_vals));
    }
#line 1890 "src/parser/yacc.tab.c"
    break;

  case 25: /* dml: DELETE FROM tbName optWhereClause  */
#line 171 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<DeleteStmt>((yyvsp[-1].sv_str), (yyvsp[0].sv_conds));
    }
#line 1898 "src/parser/yacc.tab.c"
    break;

  case 26: /* dml: UPDATE tbName SET setClauses optWhereClause  */
#line 175 "src/parser/yacc.y"
    {
        (yyval.sv_node) = std::make_shared<UpdateStmt>((yyvsp[-3].sv_str), (yyvsp[-1].sv_set_clauses), (yyvsp[0].sv_conds));
    }
#line 1906 "src/parser/yacc.tab.c"
    break;

  case 27: /* dml: queryExpr  */
#line 179 "src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1914 "src/parser/yacc.tab.c"
    break;

  case 28: /* dml: EXPLAIN ANALYZE queryExpr  */
#line 183 "src/parser/yacc.y"
    {
        auto sel = std::dynamic_pointer_cast<SelectStmt>((yyvsp[0].sv_node));
        if (sel == nullptr) {
            throw RMDBError("failure");
        }
        (yyval.sv_node) = std::make_shared<ExplainStmt>(sel);
    }
#line 1926 "src/parser/yacc.tab.c"
    break;

  case 29: /* queryExpr: selectStmt  */
#line 194 "src/parser/yacc.y"
    {
        (yyval.sv_node) = (yyvsp[0].sv_node);
    }
#line 1934 "src/parser/yacc.tab.c"
    break;

  case 30: /* queryExpr: queryExpr UNION selectStmt  */
#line 198 "src/parser/yacc.y"
    {
        if (auto u = std::dynamic_pointer_cast<UnionStmt>((yyvsp[-2].sv_node))) {
            u->append((yyvsp[0].sv_node));
            (yyval.sv_node) = u;
        } else {
            (yyval.sv_node) = std::make_shared<UnionStmt>((yyvsp[-2].sv_node), (yyvsp[0].sv_node));
        }
    }
#line 1947 "src/parser/yacc.tab.c"
    break;

  case 31: /* selectStmt: SELECT selector FROM fromClause optWhereClause opt_group_clause opt_having_clause opt_order_clause opt_limit_clause  */
#line 210 "src/parser/yacc.y"
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
#line 1970 "src/parser/yacc.tab.c"
    break;

  case 32: /* fieldList: field  */
#line 232 "src/parser/yacc.y"
    {
        (yyval.sv_fields) = std::vector<std::shared_ptr<Field>>{(yyvsp[0].sv_field)};
    }
#line 1978 "src/parser/yacc.tab.c"
    break;

  case 33: /* fieldList: fieldList ',' field  */
#line 236 "src/parser/yacc.y"
    {
        (yyval.sv_fields).push_back((yyvsp[0].sv_field));
    }
#line 1986 "src/parser/yacc.tab.c"
    break;

  case 34: /* colNameList: colName  */
#line 243 "src/parser/yacc.y"
    {
        (yyval.sv_strs) = std::vector<std::string>{(yyvsp[0].sv_str)};
    }
#line 1994 "src/parser/yacc.tab.c"
    break;

  case 35: /* colNameList: colNameList ',' colName  */
#line 247 "src/parser/yacc.y"
    {
        (yyval.sv_strs).push_back((yyvsp[0].sv_str));
    }
#line 2002 "src/parser/yacc.tab.c"
    break;

  case 36: /* field: colName type  */
#line 254 "src/parser/yacc.y"
    {
        (yyval.sv_field) = std::make_shared<ColDef>((yyvsp[-1].sv_str), (yyvsp[0].sv_type_len));
    }
#line 2010 "src/parser/yacc.tab.c"
    break;

  case 37: /* type: INT  */
#line 261 "src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_INT, sizeof(int));
    }
#line 2018 "src/parser/yacc.tab.c"
    break;

  case 38: /* type: CHAR '(' VALUE_INT ')'  */
#line 265 "src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_STRING, (yyvsp[-1].sv_int));
    }
#line 2026 "src/parser/yacc.tab.c"
    break;

  case 39: /* type: FLOAT  */
#line 269 "src/parser/yacc.y"
    {
        (yyval.sv_type_len) = std::make_shared<TypeLen>(SV_TYPE_FLOAT, sizeof(float));
    }
#line 2034 "src/parser/yacc.tab.c"
    break;

  case 40: /* valueList: value  */
#line 276 "src/parser/yacc.y"
    {
        (yyval.sv_vals) = std::vector<std::shared_ptr<Value>>{(yyvsp[0].sv_val)};
    }
#line 2042 "src/parser/yacc.tab.c"
    break;

  case 41: /* valueList: valueList ',' value  */
#line 280 "src/parser/yacc.y"
    {
        (yyval.sv_vals).push_back((yyvsp[0].sv_val));
    }
#line 2050 "src/parser/yacc.tab.c"
    break;

  case 42: /* value: VALUE_INT  */
#line 287 "src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<IntLit>((yyvsp[0].sv_int));
    }
#line 2058 "src/parser/yacc.tab.c"
    break;

  case 43: /* value: VALUE_FLOAT  */
#line 291 "src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<FloatLit>((yyvsp[0].sv_float));
    }
#line 2066 "src/parser/yacc.tab.c"
    break;

  case 44: /* value: VALUE_STRING  */
#line 295 "src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<StringLit>((yyvsp[0].sv_str));
    }
#line 2074 "src/parser/yacc.tab.c"
    break;

  case 45: /* value: VALUE_BOOL  */
#line 299 "src/parser/yacc.y"
    {
        (yyval.sv_val) = std::make_shared<BoolLit>((yyvsp[0].sv_bool));
    }
#line 2082 "src/parser/yacc.tab.c"
    break;

  case 46: /* condition: col op expr  */
#line 306 "src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>((yyvsp[-2].sv_col), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2090 "src/parser/yacc.tab.c"
    break;

  case 47: /* condition: aggregate op expr  */
#line 310 "src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), (yyvsp[-1].sv_comp_op),
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
#line 2099 "src/parser/yacc.tab.c"
    break;

  case 48: /* condition: col op aggregate  */
#line 315 "src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), (yyvsp[-1].sv_comp_op),
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
#line 2108 "src/parser/yacc.tab.c"
    break;

  case 49: /* condition: aggregate op aggregate  */
#line 320 "src/parser/yacc.y"
    {
        (yyval.sv_cond) = std::make_shared<BinaryExpr>(std::make_shared<Col>("", "__agg_in_where__"), (yyvsp[-1].sv_comp_op),
                                          std::static_pointer_cast<Expr>(std::make_shared<IntLit>(0)));
    }
#line 2117 "src/parser/yacc.tab.c"
    break;

  case 50: /* joinConds: condition  */
#line 328 "src/parser/yacc.y"
    {
        (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)};
    }
#line 2125 "src/parser/yacc.tab.c"
    break;

  case 51: /* joinConds: joinConds AND condition  */
#line 332 "src/parser/yacc.y"
    {
        (yyval.sv_conds).push_back((yyvsp[0].sv_cond));
    }
#line 2133 "src/parser/yacc.tab.c"
    break;

  case 52: /* optWhereClause: %empty  */
#line 338 "src/parser/yacc.y"
                      { /* ignore*/ }
#line 2139 "src/parser/yacc.tab.c"
    break;

  case 53: /* optWhereClause: WHERE whereClause  */
#line 340 "src/parser/yacc.y"
    {
        (yyval.sv_conds) = (yyvsp[0].sv_conds);
    }
#line 2147 "src/parser/yacc.tab.c"
    break;

  case 54: /* whereClause: condition  */
#line 347 "src/parser/yacc.y"
    {
        (yyval.sv_conds) = std::vector<std::shared_ptr<BinaryExpr>>{(yyvsp[0].sv_cond)};
    }
#line 2155 "src/parser/yacc.tab.c"
    break;

  case 55: /* whereClause: whereClause AND condition  */
#line 351 "src/parser/yacc.y"
    {
        (yyval.sv_conds).push_back((yyvsp[0].sv_cond));
    }
#line 2163 "src/parser/yacc.tab.c"
    break;

  case 56: /* col: tbName '.' colName  */
#line 358 "src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2171 "src/parser/yacc.tab.c"
    break;

  case 57: /* col: colName  */
#line 362 "src/parser/yacc.y"
    {
        (yyval.sv_col) = std::make_shared<Col>("", (yyvsp[0].sv_str));
    }
#line 2179 "src/parser/yacc.tab.c"
    break;

  case 58: /* op: '='  */
#line 380 "src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_EQ;
    }
#line 2187 "src/parser/yacc.tab.c"
    break;

  case 59: /* op: '<'  */
#line 384 "src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LT;
    }
#line 2195 "src/parser/yacc.tab.c"
    break;

  case 60: /* op: '>'  */
#line 388 "src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GT;
    }
#line 2203 "src/parser/yacc.tab.c"
    break;

  case 61: /* op: NEQ  */
#line 392 "src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_NE;
    }
#line 2211 "src/parser/yacc.tab.c"
    break;

  case 62: /* op: LEQ  */
#line 396 "src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_LE;
    }
#line 2219 "src/parser/yacc.tab.c"
    break;

  case 63: /* op: GEQ  */
#line 400 "src/parser/yacc.y"
    {
        (yyval.sv_comp_op) = SV_OP_GE;
    }
#line 2227 "src/parser/yacc.tab.c"
    break;

  case 64: /* expr: value  */
#line 407 "src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_val));
    }
#line 2235 "src/parser/yacc.tab.c"
    break;

  case 65: /* expr: col  */
#line 411 "src/parser/yacc.y"
    {
        (yyval.sv_expr) = std::static_pointer_cast<Expr>((yyvsp[0].sv_col));
    }
#line 2243 "src/parser/yacc.tab.c"
    break;

  case 66: /* setClauses: setClause  */
#line 418 "src/parser/yacc.y"
    {
        (yyval.sv_set_clauses) = std::vector<std::shared_ptr<SetClause>>{(yyvsp[0].sv_set_clause)};
    }
#line 2251 "src/parser/yacc.tab.c"
    break;

  case 67: /* setClauses: setClauses ',' setClause  */
#line 422 "src/parser/yacc.y"
    {
        (yyval.sv_set_clauses).push_back((yyvsp[0].sv_set_clause));
    }
#line 2259 "src/parser/yacc.tab.c"
    break;

  case 68: /* setClause: colName '=' value  */
#line 429 "src/parser/yacc.y"
    {
        (yyval.sv_set_clause) = std::make_shared<SetClause>((yyvsp[-2].sv_str), (yyvsp[0].sv_val));
    }
#line 2267 "src/parser/yacc.tab.c"
    break;

  case 69: /* selector: '*'  */
#line 436 "src/parser/yacc.y"
    {
        (yyval.sv_select_items) = {};
    }
#line 2275 "src/parser/yacc.tab.c"
    break;

  case 71: /* selectList: selectItem  */
#line 444 "src/parser/yacc.y"
    {
        (yyval.sv_select_items) = std::vector<std::shared_ptr<SelectItem>>{(yyvsp[0].sv_select_item)};
    }
#line 2283 "src/parser/yacc.tab.c"
    break;

  case 72: /* selectList: selectList ',' selectItem  */
#line 448 "src/parser/yacc.y"
    {
        (yyval.sv_select_items).push_back((yyvsp[0].sv_select_item));
    }
#line 2291 "src/parser/yacc.tab.c"
    break;

  case 73: /* selectItem: col optAlias  */
#line 455 "src/parser/yacc.y"
    {
        (yyval.sv_select_item) = std::make_shared<SelectItem>(std::static_pointer_cast<Expr>((yyvsp[-1].sv_col)), (yyvsp[0].sv_str));
    }
#line 2299 "src/parser/yacc.tab.c"
    break;

  case 74: /* selectItem: aggregate optAlias  */
#line 459 "src/parser/yacc.y"
    {
        (yyval.sv_select_item) = std::make_shared<SelectItem>(std::static_pointer_cast<Expr>((yyvsp[-1].sv_agg_expr)), (yyvsp[0].sv_str));
    }
#line 2307 "src/parser/yacc.tab.c"
    break;

  case 75: /* optAlias: %empty  */
#line 465 "src/parser/yacc.y"
                      { (yyval.sv_str) = ""; }
#line 2313 "src/parser/yacc.tab.c"
    break;

  case 76: /* optAlias: AS IDENTIFIER  */
#line 466 "src/parser/yacc.y"
                      { (yyval.sv_str) = (yyvsp[0].sv_str); }
#line 2319 "src/parser/yacc.tab.c"
    break;

  case 77: /* optAlias: IDENTIFIER  */
#line 467 "src/parser/yacc.y"
                   { (yyval.sv_str) = (yyvsp[0].sv_str); }
#line 2325 "src/parser/yacc.tab.c"
    break;

  case 78: /* aggregate: COUNT '(' '*' ')'  */
#line 472 "src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_COUNT, nullptr, true);
    }
#line 2333 "src/parser/yacc.tab.c"
    break;

  case 79: /* aggregate: COUNT '(' col ')'  */
#line 476 "src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_COUNT, (yyvsp[-1].sv_col), false);
    }
#line 2341 "src/parser/yacc.tab.c"
    break;

  case 80: /* aggregate: MAX '(' col ')'  */
#line 480 "src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_MAX, (yyvsp[-1].sv_col), false);
    }
#line 2349 "src/parser/yacc.tab.c"
    break;

  case 81: /* aggregate: MIN '(' col ')'  */
#line 484 "src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_MIN, (yyvsp[-1].sv_col), false);
    }
#line 2357 "src/parser/yacc.tab.c"
    break;

  case 82: /* aggregate: SUM '(' col ')'  */
#line 488 "src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_SUM, (yyvsp[-1].sv_col), false);
    }
#line 2365 "src/parser/yacc.tab.c"
    break;

  case 83: /* aggregate: AVG '(' col ')'  */
#line 492 "src/parser/yacc.y"
    {
        (yyval.sv_agg_expr) = std::make_shared<AggExpr>(Agg_AVG, (yyvsp[-1].sv_col), false);
    }
#line 2373 "src/parser/yacc.tab.c"
    break;

  case 84: /* tableRef: tbName  */
#line 515 "src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[0].sv_str));
    }
#line 2381 "src/parser/yacc.tab.c"
    break;

  case 85: /* tableRef: tbName IDENTIFIER  */
#line 519 "src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-1].sv_str), (yyvsp[0].sv_str));
    }
#line 2389 "src/parser/yacc.tab.c"
    break;

  case 86: /* tableRef: tbName AS IDENTIFIER  */
#line 523 "src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-2].sv_str), (yyvsp[0].sv_str));
    }
#line 2397 "src/parser/yacc.tab.c"
    break;

  case 87: /* tableRef: '(' queryExpr ')' AS IDENTIFIER  */
#line 527 "src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-3].sv_node), (yyvsp[0].sv_str));
    }
#line 2405 "src/parser/yacc.tab.c"
    break;

  case 88: /* tableRef: '(' queryExpr ')' IDENTIFIER  */
#line 531 "src/parser/yacc.y"
    {
        (yyval.sv_table_ref) = std::make_shared<TableRef>((yyvsp[-2].sv_node), (yyvsp[0].sv_str));
    }
#line 2413 "src/parser/yacc.tab.c"
    break;

  case 89: /* fromClause: tableRef  */
#line 539 "src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = std::make_shared<FromClause>();
        (yyval.sv_from_clause)->refs.push_back((yyvsp[0].sv_table_ref));
    }
#line 2422 "src/parser/yacc.tab.c"
    break;

  case 90: /* fromClause: fromClause ',' tableRef  */
#line 544 "src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = (yyvsp[-2].sv_from_clause);
        (yyval.sv_from_clause)->refs.push_back((yyvsp[0].sv_table_ref));
    }
#line 2431 "src/parser/yacc.tab.c"
    break;

  case 91: /* fromClause: fromClause JOIN tableRef ON joinConds  */
#line 549 "src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = (yyvsp[-4].sv_from_clause);
        (yyval.sv_from_clause)->refs.push_back((yyvsp[-2].sv_table_ref));
        for (auto &cond : (yyvsp[0].sv_conds)) {
            (yyval.sv_from_clause)->conds.push_back(cond);
        }
    }
#line 2443 "src/parser/yacc.tab.c"
    break;

  case 92: /* fromClause: fromClause JOIN tableRef  */
#line 557 "src/parser/yacc.y"
    {
        (yyval.sv_from_clause) = (yyvsp[-2].sv_from_clause);
        (yyval.sv_from_clause)->refs.push_back((yyvsp[0].sv_table_ref));
    }
#line 2452 "src/parser/yacc.tab.c"
    break;

  case 93: /* opt_order_clause: ORDER BY order_clause  */
#line 565 "src/parser/yacc.y"
    {
        (yyval.sv_orderby) = (yyvsp[0].sv_orderby);
    }
#line 2460 "src/parser/yacc.tab.c"
    break;

  case 94: /* opt_order_clause: %empty  */
#line 568 "src/parser/yacc.y"
                      { /* ignore*/ }
#line 2466 "src/parser/yacc.tab.c"
    break;

  case 95: /* order_clause: col opt_asc_desc  */
#line 573 "src/parser/yacc.y"
    {
        (yyval.sv_orderby) = std::make_shared<OrderBy>((yyvsp[-1].sv_col), (yyvsp[0].sv_orderby_dir));
    }
#line 2474 "src/parser/yacc.tab.c"
    break;

  case 96: /* order_clause: order_clause ',' col opt_asc_desc  */
#line 577 "src/parser/yacc.y"
    {
        (yyval.sv_orderby) = (yyvsp[-3].sv_orderby);
        (yyval.sv_orderby)->append((yyvsp[-1].sv_col), (yyvsp[0].sv_orderby_dir));
    }
#line 2483 "src/parser/yacc.tab.c"
    break;

  case 97: /* opt_asc_desc: ASC  */
#line 584 "src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_ASC;     }
#line 2489 "src/parser/yacc.tab.c"
    break;

  case 98: /* opt_asc_desc: DESC  */
#line 585 "src/parser/yacc.y"
                 { (yyval.sv_orderby_dir) = OrderBy_DESC;    }
#line 2495 "src/parser/yacc.tab.c"
    break;

  case 99: /* opt_asc_desc: %empty  */
#line 586 "src/parser/yacc.y"
            { (yyval.sv_orderby_dir) = OrderBy_DEFAULT; }
#line 2501 "src/parser/yacc.tab.c"
    break;

  case 100: /* opt_group_clause: %empty  */
#line 590 "src/parser/yacc.y"
                      { (yyval.sv_cols) = {}; }
#line 2507 "src/parser/yacc.tab.c"
    break;

  case 101: /* opt_group_clause: GROUP BY group_clause  */
#line 592 "src/parser/yacc.y"
    {
        (yyval.sv_cols) = (yyvsp[0].sv_cols);
    }
#line 2515 "src/parser/yacc.tab.c"
    break;

  case 102: /* group_clause: col  */
#line 599 "src/parser/yacc.y"
    {
        (yyval.sv_cols) = std::vector<std::shared_ptr<Col>>{(yyvsp[0].sv_col)};
    }
#line 2523 "src/parser/yacc.tab.c"
    break;

  case 103: /* group_clause: group_clause ',' col  */
#line 603 "src/parser/yacc.y"
    {
        (yyval.sv_cols).push_back((yyvsp[0].sv_col));
    }
#line 2531 "src/parser/yacc.tab.c"
    break;

  case 104: /* opt_having_clause: %empty  */
#line 609 "src/parser/yacc.y"
                      { (yyval.sv_having_conds) = {}; }
#line 2537 "src/parser/yacc.tab.c"
    break;

  case 105: /* opt_having_clause: HAVING havingClause  */
#line 611 "src/parser/yacc.y"
    {
        (yyval.sv_having_conds) = (yyvsp[0].sv_having_conds);
    }
#line 2545 "src/parser/yacc.tab.c"
    break;

  case 106: /* havingClause: havingCondition  */
#line 618 "src/parser/yacc.y"
    {
        (yyval.sv_having_conds) = std::vector<std::shared_ptr<HavingExpr>>{(yyvsp[0].sv_having_cond)};
    }
#line 2553 "src/parser/yacc.tab.c"
    break;

  case 107: /* havingClause: havingClause AND havingCondition  */
#line 622 "src/parser/yacc.y"
    {
        (yyval.sv_having_conds).push_back((yyvsp[0].sv_having_cond));
    }
#line 2561 "src/parser/yacc.tab.c"
    break;

  case 108: /* havingCondition: expr op expr  */
#line 629 "src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>((yyvsp[-2].sv_expr), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2569 "src/parser/yacc.tab.c"
    break;

  case 109: /* havingCondition: aggregate op expr  */
#line 633 "src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_agg_expr)), (yyvsp[-1].sv_comp_op), (yyvsp[0].sv_expr));
    }
#line 2577 "src/parser/yacc.tab.c"
    break;

  case 110: /* havingCondition: expr op aggregate  */
#line 637 "src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>((yyvsp[-2].sv_expr), (yyvsp[-1].sv_comp_op), std::static_pointer_cast<Expr>((yyvsp[0].sv_agg_expr)));
    }
#line 2585 "src/parser/yacc.tab.c"
    break;

  case 111: /* havingCondition: aggregate op aggregate  */
#line 641 "src/parser/yacc.y"
    {
        (yyval.sv_having_cond) = std::make_shared<HavingExpr>(std::static_pointer_cast<Expr>((yyvsp[-2].sv_agg_expr)), (yyvsp[-1].sv_comp_op), std::static_pointer_cast<Expr>((yyvsp[0].sv_agg_expr)));
    }
#line 2593 "src/parser/yacc.tab.c"
    break;

  case 112: /* opt_limit_clause: %empty  */
#line 647 "src/parser/yacc.y"
                      { (yyval.sv_int) = -1; }
#line 2599 "src/parser/yacc.tab.c"
    break;

  case 113: /* opt_limit_clause: LIMIT VALUE_INT  */
#line 648 "src/parser/yacc.y"
                        { (yyval.sv_int) = (yyvsp[0].sv_int); }
#line 2605 "src/parser/yacc.tab.c"
    break;

  case 114: /* set_knob_type: ENABLE_NESTLOOP  */
#line 652 "src/parser/yacc.y"
                    { (yyval.sv_setKnobType) = EnableNestLoop; }
#line 2611 "src/parser/yacc.tab.c"
    break;

  case 115: /* set_knob_type: ENABLE_SORTMERGE  */
#line 653 "src/parser/yacc.y"
                         { (yyval.sv_setKnobType) = EnableSortMerge; }
#line 2617 "src/parser/yacc.tab.c"
    break;

  case 116: /* isolation_level: SNAPSHOT ISOLATION  */
#line 657 "src/parser/yacc.y"
                           { (yyval.sv_isolation_level) = SnapshotIsolation; }
#line 2623 "src/parser/yacc.tab.c"
    break;

  case 117: /* isolation_level: SNAPSHOT  */
#line 658 "src/parser/yacc.y"
                 { (yyval.sv_isolation_level) = SnapshotIsolation; }
#line 2629 "src/parser/yacc.tab.c"
    break;

  case 118: /* isolation_level: SERIALIZABLE  */
#line 659 "src/parser/yacc.y"
                     { (yyval.sv_isolation_level) = Serializable; }
#line 2635 "src/parser/yacc.tab.c"
    break;


#line 2639 "src/parser/yacc.tab.c"

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

#line 665 "src/parser/yacc.y"

