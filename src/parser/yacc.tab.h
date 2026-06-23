/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_YC_DADA_DB2026_SRC_PARSER_YACC_TAB_H_INCLUDED
# define YY_YY_HOME_YC_DADA_DB2026_SRC_PARSER_YACC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SHOW = 258,                    /* SHOW  */
    TABLES = 259,                  /* TABLES  */
    CREATE = 260,                  /* CREATE  */
    TABLE = 261,                   /* TABLE  */
    DROP = 262,                    /* DROP  */
    DESC = 263,                    /* DESC  */
    INSERT = 264,                  /* INSERT  */
    INTO = 265,                    /* INTO  */
    VALUES = 266,                  /* VALUES  */
    DELETE = 267,                  /* DELETE  */
    FROM = 268,                    /* FROM  */
    ASC = 269,                     /* ASC  */
    ORDER = 270,                   /* ORDER  */
    BY = 271,                      /* BY  */
    UNION = 272,                   /* UNION  */
    STATIC_CHECKPOINT = 273,       /* STATIC_CHECKPOINT  */
    LOAD = 274,                    /* LOAD  */
    OUTPUT_FILE = 275,             /* OUTPUT_FILE  */
    OFF = 276,                     /* OFF  */
    WHERE = 277,                   /* WHERE  */
    UPDATE = 278,                  /* UPDATE  */
    SET = 279,                     /* SET  */
    SELECT = 280,                  /* SELECT  */
    INT = 281,                     /* INT  */
    CHAR = 282,                    /* CHAR  */
    FLOAT = 283,                   /* FLOAT  */
    INDEX = 284,                   /* INDEX  */
    AND = 285,                     /* AND  */
    JOIN = 286,                    /* JOIN  */
    EXIT = 287,                    /* EXIT  */
    HELP = 288,                    /* HELP  */
    TXN_BEGIN = 289,               /* TXN_BEGIN  */
    TXN_COMMIT = 290,              /* TXN_COMMIT  */
    TXN_ABORT = 291,               /* TXN_ABORT  */
    TXN_ROLLBACK = 292,            /* TXN_ROLLBACK  */
    ORDER_BY = 293,                /* ORDER_BY  */
    ENABLE_NESTLOOP = 294,         /* ENABLE_NESTLOOP  */
    ENABLE_SORTMERGE = 295,        /* ENABLE_SORTMERGE  */
    EXPLAIN = 296,                 /* EXPLAIN  */
    ANALYZE = 297,                 /* ANALYZE  */
    AS = 298,                      /* AS  */
    ON = 299,                      /* ON  */
    GROUP = 300,                   /* GROUP  */
    HAVING = 301,                  /* HAVING  */
    LIMIT = 302,                   /* LIMIT  */
    COUNT = 303,                   /* COUNT  */
    MAX = 304,                     /* MAX  */
    MIN = 305,                     /* MIN  */
    SUM = 306,                     /* SUM  */
    AVG = 307,                     /* AVG  */
    TRANSACTION = 308,             /* TRANSACTION  */
    ISOLATION = 309,               /* ISOLATION  */
    LEVEL = 310,                   /* LEVEL  */
    SNAPSHOT = 311,                /* SNAPSHOT  */
    SERIALIZABLE = 312,            /* SERIALIZABLE  */
    LEQ = 313,                     /* LEQ  */
    NEQ = 314,                     /* NEQ  */
    GEQ = 315,                     /* GEQ  */
    T_EOF = 316,                   /* T_EOF  */
    IDENTIFIER = 317,              /* IDENTIFIER  */
    VALUE_STRING = 318,            /* VALUE_STRING  */
    VALUE_INT = 319,               /* VALUE_INT  */
    VALUE_FLOAT = 320,             /* VALUE_FLOAT  */
    VALUE_BOOL = 321               /* VALUE_BOOL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef ast::SemValue YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int yyparse (void);


#endif /* !YY_YY_HOME_YC_DADA_DB2026_SRC_PARSER_YACC_TAB_H_INCLUDED  */
