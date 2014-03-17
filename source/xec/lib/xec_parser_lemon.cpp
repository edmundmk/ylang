/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 7 "xec_parser_lemon.y"

    #include <assert.h>
    #include "xec_parser.h"
    #include "xec_token.h"
#line 13 "xec_parser_lemon.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    XecParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is XecParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    XecParseARG_SDECL     A static variable declaration for the %extra_argument
**    XecParseARG_PDECL     A parameter declaration for the %extra_argument
**    XecParseARG_STORE     Code to store %extra_argument into yypParser
**    XecParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 110
#define YYACTIONTYPE unsigned short int
#define XecParseTOKENTYPE void*
typedef union {
  int yyinit;
  XecParseTOKENTYPE yy0;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define XecParseARG_SDECL
#define XecParseARG_PDECL
#define XecParseARG_FETCH
#define XecParseARG_STORE
#define YYNSTATE 231
#define YYNRULE 113
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   345,  140,  103,   69,   70,   45,   44,  214,  164,   76,
 /*    10 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*    20 */   126,  127,   92,   55,  219,  196,  197,  108,   10,   81,
 /*    30 */    75,   59,   19,  214,  158,   76,   82,  181,  169,   72,
 /*    40 */    95,   78,  111,  112,  114,   68,  126,  127,   92,   55,
 /*    50 */   217,  196,  197,   81,   75,  106,  167,  214,    1,   76,
 /*    60 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*    70 */   126,  127,   92,  101,  209,  161,   81,   75,  104,  157,
 /*    80 */   214,  134,   76,   82,  181,  169,   72,   95,   78,  111,
 /*    90 */   112,  114,   68,  126,  127,   92,  101,  133,  161,   81,
 /*   100 */    75,  105,  154,  214,  149,   76,   82,  181,  169,   72,
 /*   110 */    95,   78,  111,  112,  114,   68,  126,  127,   92,  101,
 /*   120 */   167,  161,   81,   75,  136,   96,  214,  151,   76,   82,
 /*   130 */   181,  169,   72,   95,   78,  111,  112,  114,   68,  126,
 /*   140 */   127,   92,  101,  139,  161,   81,   75,  156,  152,  214,
 /*   150 */    11,   76,   82,  181,  169,   72,   95,   78,  111,  112,
 /*   160 */   114,   68,  126,  127,   92,  101,  159,  161,   81,   75,
 /*   170 */   131,  137,  214,  231,   76,   82,  181,  169,   72,   95,
 /*   180 */    78,  111,  112,  114,   68,  126,  127,   92,  101,  232,
 /*   190 */   161,   81,   75,  147,  177,  214,  221,   76,   82,  181,
 /*   200 */   169,   72,   95,   78,  111,  112,  114,   68,  126,  127,
 /*   210 */    92,  101,  206,  161,   81,   75,  107,  200,  214,  175,
 /*   220 */    76,   82,  181,  169,   72,   95,   78,  111,  112,  114,
 /*   230 */    68,  126,  127,   92,  101,  163,  161,   81,   75,   66,
 /*   240 */   174,  214,  138,   76,   82,  181,  169,   72,   95,   78,
 /*   250 */   111,  112,  114,   68,  126,  127,   92,  101,   40,  182,
 /*   260 */    81,   75,   39,   27,  229,   38,   76,   71,  181,  169,
 /*   270 */    72,   95,   78,  111,  112,  114,   68,  126,  127,   92,
 /*   280 */    18,  230,   81,   75,   25,   16,  109,   26,   76,   82,
 /*   290 */   181,  169,   72,   95,   78,  111,  112,  114,   68,  126,
 /*   300 */   127,   92,   81,   75,  155,    6,  148,   85,   76,   82,
 /*   310 */   181,  169,   72,   95,   78,  111,  112,  114,   68,  126,
 /*   320 */   127,   92,   81,   75,   57,    2,  130,   98,   76,   82,
 /*   330 */   181,  169,   72,   95,   78,  111,  112,  114,   68,  126,
 /*   340 */   127,   92,  218,   81,   75,  216,  211,  110,   62,   76,
 /*   350 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*   360 */   126,  127,   92,   81,   75,  213,  102,  144,  222,   76,
 /*   370 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*   380 */   126,  127,   92,   81,   75,  225,  168,  220,  172,   76,
 /*   390 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*   400 */   126,  127,   92,   81,   75,  227,   86,  141,  201,   76,
 /*   410 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*   420 */   126,  127,   92,   81,   75,  204,  210,  145,   64,   76,
 /*   430 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*   440 */   126,  127,   92,   81,   75,  224,   60,  146,  171,   76,
 /*   450 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*   460 */   126,  127,   92,   81,   75,  226,   99,  150,   65,   76,
 /*   470 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*   480 */   126,  127,   92,   81,   75,  203,  208,  142,  346,   76,
 /*   490 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*   500 */   126,  127,   92,   81,   75,  346,  346,  132,  346,   76,
 /*   510 */    82,  181,  169,   72,   95,   78,  111,  112,  114,   68,
 /*   520 */   126,  127,   92,  167,   61,    1,   43,   41,   42,   13,
 /*   530 */   346,  346,  346,  215,  212,  207,  205,  202,   50,   53,
 /*   540 */    51,   52,  167,  176,    1,  346,   56,  346,   13,  346,
 /*   550 */   346,   23,  215,  212,  207,  205,  202,   50,   53,   51,
 /*   560 */    52,  167,  180,    1,  135,   54,  346,   13,  346,  346,
 /*   570 */    14,  215,  212,  207,  205,  202,   50,   53,   51,   52,
 /*   580 */    81,   87,  346,  135,   54,  346,   88,   82,  181,  169,
 /*   590 */    72,   95,   78,  111,  112,  114,   68,  126,  129,  346,
 /*   600 */    91,    7,  135,   54,  346,  160,  167,   24,    1,  346,
 /*   610 */   346,  346,   13,  346,  346,  143,  215,  212,  207,  205,
 /*   620 */   202,   50,   53,   51,   52,  167,  226,    1,  346,  346,
 /*   630 */   346,   13,  346,  346,   15,  215,  212,  207,  205,  202,
 /*   640 */    50,   53,   51,   52,  346,   91,    3,  135,   54,  346,
 /*   650 */   346,   11,   12,  183,  184,  185,  186,  187,  188,  189,
 /*   660 */   190,  191,  192,  193,  194,  195,  135,   54,   81,   87,
 /*   670 */   346,  346,  346,  346,   88,   82,  181,  169,   72,   95,
 /*   680 */    78,  111,  112,  114,   68,  128,   81,   87,  228,  346,
 /*   690 */   346,  346,   88,   82,  181,  169,   72,   95,   78,  111,
 /*   700 */   112,  114,   67,   81,   87,   46,   49,   47,   48,   88,
 /*   710 */    82,  181,  169,   72,   95,   78,  111,  112,  116,   81,
 /*   720 */    87,  346,  346,  346,  346,   88,   82,  181,  169,   72,
 /*   730 */    95,   78,  111,  112,  119,   81,   87,  346,  346,  346,
 /*   740 */   346,   88,   82,  181,  169,   72,   95,   78,  111,  112,
 /*   750 */   124,   81,   87,  346,  346,  346,  346,   88,   82,  181,
 /*   760 */   169,   72,   95,   78,  111,  112,  123,   81,   87,  346,
 /*   770 */   346,  346,  346,   88,   82,  181,  169,   72,   95,   78,
 /*   780 */   111,  112,  118,   81,   87,  346,  346,  346,  346,   88,
 /*   790 */    82,  181,  169,   72,   95,   78,  111,  112,  122,   81,
 /*   800 */    87,  346,  346,  346,  346,   88,   82,  181,  169,   72,
 /*   810 */    95,   78,  111,  112,  125,   81,   87,  346,  346,  346,
 /*   820 */   346,   88,   82,  181,  169,   72,   95,   78,  111,  112,
 /*   830 */   121,   81,   87,  346,  346,  346,  346,   88,   82,  181,
 /*   840 */   169,   72,   95,   78,  111,  112,  117,   81,   87,  346,
 /*   850 */   346,  346,  346,   88,   82,  181,  169,   72,   95,   78,
 /*   860 */   111,  112,  120,  167,  346,    1,  346,  346,  346,  346,
 /*   870 */   346,  346,  346,  215,  212,  207,  205,  202,   50,   53,
 /*   880 */    51,   52,   81,   87,  346,  346,  346,  346,   88,   82,
 /*   890 */   181,  169,   72,   95,   78,  111,  115,   81,   87,  346,
 /*   900 */   346,  346,  346,   88,   82,  181,  169,   72,   95,   78,
 /*   910 */   113,   30,   31,   32,   33,   34,   35,   36,   37,   28,
 /*   920 */    29,   81,   87,  346,  346,  346,  346,   88,   82,  181,
 /*   930 */   169,   72,   95,   77,  346,   94,    8,   81,   87,  346,
 /*   940 */   346,  199,   20,   88,   82,  181,  169,   72,   90,   81,
 /*   950 */    87,  346,  346,  346,  346,   88,   82,  181,  169,   72,
 /*   960 */    89,   81,   87,  167,  346,    1,  346,   88,   82,  181,
 /*   970 */   169,   72,   93,  215,  212,  207,  205,  202,   81,   87,
 /*   980 */   346,  346,  346,  346,   88,   82,  181,  169,   74,   81,
 /*   990 */    87,  346,  346,  346,  346,   88,   82,  181,  169,   73,
 /*  1000 */    81,   87,  346,  346,   81,   87,   88,   82,  181,  166,
 /*  1010 */    88,   82,  181,  223,   81,   87,  346,  346,  100,    4,
 /*  1020 */    88,   82,  181,  165,  198,   21,  346,   81,   87,  346,
 /*  1030 */   346,   81,   87,   88,   82,  181,  162,   88,   82,  179,
 /*  1040 */    97,    9,  346,   58,  346,   22,  346,   17,  346,   94,
 /*  1050 */     8,  346,   63,   81,   87,  153,   20,   81,   87,   88,
 /*  1060 */    82,  173,  346,   88,   82,  170,  346,  346,   81,   87,
 /*  1070 */   346,   83,   79,  346,   88,   82,  178,   80,   84,  235,
 /*  1080 */   235,   97,    5,  244,  244,  346,  235,  346,   17,  346,
 /*  1090 */   244,  346,   91,    3,   94,    8,   97,    9,  346,   24,
 /*  1100 */   346,   20,  346,   17,  346,  100,    4,  346,  346,  346,
 /*  1110 */   346,  346,   21,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    80,   81,   82,   83,   84,   17,   18,   87,   11,   89,
 /*    10 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*    20 */   100,  101,  102,  103,    2,  105,  106,   81,  107,   83,
 /*    30 */    84,    6,   10,   87,    9,   89,   90,   91,   92,   93,
 /*    40 */    94,   95,   96,   97,   98,   99,  100,  101,  102,  103,
 /*    50 */    45,  105,  106,   83,   84,   85,    2,   87,    4,   89,
 /*    60 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*    70 */   100,  101,  102,  103,   45,  105,   83,   84,   85,   88,
 /*    80 */    87,   86,   89,   90,   91,   92,   93,   94,   95,   96,
 /*    90 */    97,   98,   99,  100,  101,  102,  103,   86,  105,   83,
 /*   100 */    84,   85,   88,   87,   86,   89,   90,   91,   92,   93,
 /*   110 */    94,   95,   96,   97,   98,   99,  100,  101,  102,  103,
 /*   120 */     2,  105,   83,   84,   85,   83,   87,   86,   89,   90,
 /*   130 */    91,   92,   93,   94,   95,   96,   97,   98,   99,  100,
 /*   140 */   101,  102,  103,   88,  105,   83,   84,   85,   88,   87,
 /*   150 */    45,   89,   90,   91,   92,   93,   94,   95,   96,   97,
 /*   160 */    98,   99,  100,  101,  102,  103,   88,  105,   83,   84,
 /*   170 */    85,   88,   87,    0,   89,   90,   91,   92,   93,   94,
 /*   180 */    95,   96,   97,   98,   99,  100,  101,  102,  103,    0,
 /*   190 */   105,   83,   84,   85,   11,   87,    5,   89,   90,   91,
 /*   200 */    92,   93,   94,   95,   96,   97,   98,   99,  100,  101,
 /*   210 */   102,  103,    5,  105,   83,   84,   85,    5,   87,    5,
 /*   220 */    89,   90,   91,   92,   93,   94,   95,   96,   97,   98,
 /*   230 */    99,  100,  101,  102,  103,    5,  105,   83,   84,    6,
 /*   240 */    11,   87,    9,   89,   90,   91,   92,   93,   94,   95,
 /*   250 */    96,   97,   98,   99,  100,  101,  102,  103,   27,  105,
 /*   260 */    83,   84,   28,   40,   87,   29,   89,   90,   91,   92,
 /*   270 */    93,   94,   95,   96,   97,   98,   99,  100,  101,  102,
 /*   280 */     8,  104,   83,   84,   42,   43,   87,   41,   89,   90,
 /*   290 */    91,   92,   93,   94,   95,   96,   97,   98,   99,  100,
 /*   300 */   101,  102,   83,   84,    3,    4,   87,    5,   89,   90,
 /*   310 */    91,   92,   93,   94,   95,   96,   97,   98,   99,  100,
 /*   320 */   101,  102,   83,   84,    6,    4,   87,    5,   89,   90,
 /*   330 */    91,   92,   93,   94,   95,   96,   97,   98,   99,  100,
 /*   340 */   101,  102,    7,   83,   84,    7,    7,   87,    6,   89,
 /*   350 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   360 */   100,  101,  102,   83,   84,    7,    1,   87,   11,   89,
 /*   370 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   380 */   100,  101,  102,   83,   84,   11,   11,   87,   11,   89,
 /*   390 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   400 */   100,  101,  102,   83,   84,   46,    5,   87,   11,   89,
 /*   410 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   420 */   100,  101,  102,   83,   84,    7,    7,   87,    6,   89,
 /*   430 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   440 */   100,  101,  102,   83,   84,    7,    6,   87,    7,   89,
 /*   450 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   460 */   100,  101,  102,   83,   84,    2,    5,   87,    6,   89,
 /*   470 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   480 */   100,  101,  102,   83,   84,    7,    7,   87,  109,   89,
 /*   490 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   500 */   100,  101,  102,   83,   84,  109,  109,   87,  109,   89,
 /*   510 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   99,
 /*   520 */   100,  101,  102,    2,    3,    4,   24,   25,   26,    8,
 /*   530 */   109,  109,  109,   12,   13,   14,   15,   16,   17,   18,
 /*   540 */    19,   20,    2,    2,    4,  109,    6,  109,    8,  109,
 /*   550 */   109,   10,   12,   13,   14,   15,   16,   17,   18,   19,
 /*   560 */    20,    2,    2,    4,   43,   44,  109,    8,  109,  109,
 /*   570 */    10,   12,   13,   14,   15,   16,   17,   18,   19,   20,
 /*   580 */    83,   84,  109,   43,   44,  109,   89,   90,   91,   92,
 /*   590 */    93,   94,   95,   96,   97,   98,   99,  100,  101,  109,
 /*   600 */     3,    4,   43,   44,  109,   46,    2,   10,    4,  109,
 /*   610 */   109,  109,    8,  109,  109,   11,   12,   13,   14,   15,
 /*   620 */    16,   17,   18,   19,   20,    2,    2,    4,  109,  109,
 /*   630 */   109,    8,  109,  109,   10,   12,   13,   14,   15,   16,
 /*   640 */    17,   18,   19,   20,  109,    3,    4,   43,   44,  109,
 /*   650 */   109,   45,   10,   47,   48,   49,   50,   51,   52,   53,
 /*   660 */    54,   55,   56,   57,   58,   59,   43,   44,   83,   84,
 /*   670 */   109,  109,  109,  109,   89,   90,   91,   92,   93,   94,
 /*   680 */    95,   96,   97,   98,   99,  100,   83,   84,   46,  109,
 /*   690 */   109,  109,   89,   90,   91,   92,   93,   94,   95,   96,
 /*   700 */    97,   98,   99,   83,   84,   20,   21,   22,   23,   89,
 /*   710 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   83,
 /*   720 */    84,  109,  109,  109,  109,   89,   90,   91,   92,   93,
 /*   730 */    94,   95,   96,   97,   98,   83,   84,  109,  109,  109,
 /*   740 */   109,   89,   90,   91,   92,   93,   94,   95,   96,   97,
 /*   750 */    98,   83,   84,  109,  109,  109,  109,   89,   90,   91,
 /*   760 */    92,   93,   94,   95,   96,   97,   98,   83,   84,  109,
 /*   770 */   109,  109,  109,   89,   90,   91,   92,   93,   94,   95,
 /*   780 */    96,   97,   98,   83,   84,  109,  109,  109,  109,   89,
 /*   790 */    90,   91,   92,   93,   94,   95,   96,   97,   98,   83,
 /*   800 */    84,  109,  109,  109,  109,   89,   90,   91,   92,   93,
 /*   810 */    94,   95,   96,   97,   98,   83,   84,  109,  109,  109,
 /*   820 */   109,   89,   90,   91,   92,   93,   94,   95,   96,   97,
 /*   830 */    98,   83,   84,  109,  109,  109,  109,   89,   90,   91,
 /*   840 */    92,   93,   94,   95,   96,   97,   98,   83,   84,  109,
 /*   850 */   109,  109,  109,   89,   90,   91,   92,   93,   94,   95,
 /*   860 */    96,   97,   98,    2,  109,    4,  109,  109,  109,  109,
 /*   870 */   109,  109,  109,   12,   13,   14,   15,   16,   17,   18,
 /*   880 */    19,   20,   83,   84,  109,  109,  109,  109,   89,   90,
 /*   890 */    91,   92,   93,   94,   95,   96,   97,   83,   84,  109,
 /*   900 */   109,  109,  109,   89,   90,   91,   92,   93,   94,   95,
 /*   910 */    96,   30,   31,   32,   33,   34,   35,   36,   37,   38,
 /*   920 */    39,   83,   84,  109,  109,  109,  109,   89,   90,   91,
 /*   930 */    92,   93,   94,   95,  109,    3,    4,   83,   84,  109,
 /*   940 */   109,    9,   10,   89,   90,   91,   92,   93,   94,   83,
 /*   950 */    84,  109,  109,  109,  109,   89,   90,   91,   92,   93,
 /*   960 */    94,   83,   84,    2,  109,    4,  109,   89,   90,   91,
 /*   970 */    92,   93,   94,   12,   13,   14,   15,   16,   83,   84,
 /*   980 */   109,  109,  109,  109,   89,   90,   91,   92,   93,   83,
 /*   990 */    84,  109,  109,  109,  109,   89,   90,   91,   92,   93,
 /*  1000 */    83,   84,  109,  109,   83,   84,   89,   90,   91,   92,
 /*  1010 */    89,   90,   91,   92,   83,   84,  109,  109,    3,    4,
 /*  1020 */    89,   90,   91,   92,    9,   10,  109,   83,   84,  109,
 /*  1030 */   109,   83,   84,   89,   90,   91,   92,   89,   90,   91,
 /*  1040 */     3,    4,  109,    6,  109,    8,  109,   10,  109,    3,
 /*  1050 */     4,  109,    6,   83,   84,    9,   10,   83,   84,   89,
 /*  1060 */    90,   91,  109,   89,   90,   91,  109,  109,   83,   84,
 /*  1070 */   109,   83,   84,  109,   89,   90,   91,   89,   90,    3,
 /*  1080 */     4,    3,    4,    3,    4,  109,   10,  109,   10,  109,
 /*  1090 */    10,  109,    3,    4,    3,    4,    3,    4,  109,   10,
 /*  1100 */   109,   10,  109,   10,  109,    3,    4,  109,  109,  109,
 /*  1110 */   109,  109,   10,
};
#define YY_SHIFT_USE_DFLT (-13)
#define YY_SHIFT_MAX 159
static const short yy_shift_ofst[] = {
 /*     0 */   521,  623,  623,  623,  623,  623,  623,  623,  623,  623,
 /*    10 */   623,  559,  604,  540,  623,  623,  623,  623,  623,  623,
 /*    20 */   623,  623,  623,  623,  623,  861,  861,  861,  861,  861,
 /*    30 */   861,  861,  861,  861,  861,  861,  861,  861,  861,  861,
 /*    40 */   861,  861,  861,  861,  861,  861,  861,  861,  861,  861,
 /*    50 */   961,  961,  961,  961,   54,  606,    5,    5,    5,   29,
 /*    60 */    29,  118,   29,   29,    5,   29,   29,  881,  881, 1037,
 /*    70 */  1046,  642,  685,  685,  685,  932, 1015,  502,  502, 1091,
 /*    80 */  1102, 1093, 1089, 1078,  597, 1076, 1080, 1091, 1102,  -12,
 /*    90 */   -12,   22,  242,  -12,  560,  -12,  301,  624,  233,   25,
 /*   100 */   541,  105,  173,  189,  191,  207,  212,  214,  230,  183,
 /*   110 */   229,  231,  234,  231,  236,  234,  236,  236,  236,  236,
 /*   120 */   236,  236,  236,  236,  236,  236,  223,  246,  223,  246,
 /*   130 */   272,  302,  318,  335,  338,  321,  322,  339,  342,  358,
 /*   140 */   365,  357,  374,  359,   -3,  375,  377,  401,  397,  418,
 /*   150 */   422,  419,  438,  440,  441,  463,  461,  478,  462,  479,
};
#define YY_REDUCE_USE_DFLT (-81)
#define YY_REDUCE_MAX 66
static const short yy_reduce_ofst[] = {
 /*     0 */   -80,  -54,   39,  131,  -30,   85,   62,  108,   16,   -7,
 /*    10 */   154,  177,  400,  420,  199,  219,  239,  280,  300,  320,
 /*    20 */   340,  360,  380,  260,  400,  497,  585,  603,  652,  716,
 /*    30 */   620,  748,  684,  636,  764,  732,  700,  668,  799,  814,
 /*    40 */   838,  854,  866,  878,  895,  906,  921,  931,  944,  917,
 /*    50 */   948,  970,  974,  985,  988,  -79,   -5,   11,   18,   -9,
 /*    60 */    14,   42,   55,   60,   41,   78,   83,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   344,  344,  339,  339,  339,  339,  339,  339,  339,  339,
 /*    10 */   344,  344,  344,  344,  344,  344,  344,  344,  344,  344,
 /*    20 */   344,  344,  344,  344,  344,  344,  344,  344,  344,  344,
 /*    30 */   344,  344,  344,  344,  344,  344,  344,  344,  344,  344,
 /*    40 */   344,  344,  344,  344,  344,  344,  344,  344,  344,  344,
 /*    50 */   344,  344,  344,  344,  344,  321,  344,  344,  344,  344,
 /*    60 */   344,  344,  344,  344,  344,  344,  344,  301,  300,  257,
 /*    70 */   259,  258,  276,  277,  278,  259,  260,  284,  283,  344,
 /*    80 */   344,  257,  258,  344,  344,  308,  309,  259,  260,  282,
 /*    90 */   281,  344,  306,  280,  344,  279,  344,  344,  344,  344,
 /*   100 */   344,  321,  344,  344,  344,  344,  344,  344,  344,  344,
 /*   110 */   344,  285,  287,  286,  289,  288,  290,  291,  292,  293,
 /*   120 */   294,  295,  296,  297,  298,  299,  302,  304,  303,  305,
 /*   130 */   344,  344,  344,  344,  344,  344,  344,  344,  344,  344,
 /*   140 */   344,  344,  344,  344,  344,  344,  344,  344,  344,  344,
 /*   150 */   344,  344,  344,  314,  344,  344,  344,  344,  344,  344,
 /*   160 */   320,  340,  274,  245,  247,  273,  272,  233,  250,  271,
 /*   170 */   270,  239,  253,  269,  252,  244,  251,  249,  268,  267,
 /*   180 */   248,  266,  324,  325,  326,  327,  328,  329,  330,  331,
 /*   190 */   332,  333,  334,  335,  336,  337,  323,  338,  315,  314,
 /*   200 */   243,  246,  265,  240,  236,  264,  242,  263,  241,  342,
 /*   210 */   237,  312,  262,  313,  316,  261,  311,  341,  310,  254,
 /*   220 */   307,  235,  255,  275,  238,  256,  234,  319,  318,  317,
 /*   230 */   322,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  XecParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void XecParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "SEMICOLON",     "IDENTIFIER",    "PERIOD",      
  "LPN",           "RPN",           "LBR",           "RBR",         
  "COLON",         "YIELD",         "LSQ",           "RSQ",         
  "NUMBER",        "STRING",        "TRUE",          "FALSE",       
  "NULL",          "PLUS",          "MINUS",         "XMARK",       
  "TILDE",         "ASTERISK",      "SOLIDUS",       "PERCENT",     
  "LSHIFT",        "RSHIFT",        "URSHIFT",       "AMPERSAND",   
  "CARET",         "VBAR",          "EQUAL",         "NOTEQUAL",    
  "LESS",          "GREATER",       "LESSEQUAL",     "GREATEREQUAL",
  "IN",            "NOTIN",         "IS",            "NOTIS",       
  "LOGICAND",      "LOGICXOR",      "LOGICOR",       "QMARK",       
  "NEW",           "COMMA",         "ELLIPSIS",      "ASSIGN",      
  "MULASSIGN",     "DIVASSIGN",     "MODASSIGN",     "INTDIVASSIGN",
  "ADDASSIGN",     "SUBASSIGN",     "LSHIFTASSIGN",  "RSHIFTASSIGN",
  "URSHIFTASSIGN",  "BITANDASSIGN",  "BITXORASSIGN",  "BITORASSIGN", 
  "BREAK",         "CASE",          "CATCH",         "CONTINUE",    
  "DEFAULT",       "DO",            "ELSE",          "FINALLY",     
  "FOR",           "IF",            "RETURN",        "SWITCH",      
  "THROW",         "TRY",           "USING",         "VAR",         
  "WHILE",         "INCREMENT",     "DECREMENT",     "error",       
  "blah",          "expr",          "decl",          "name",        
  "proto",         "arg_list",      "odecl_list",    "expr_value",  
  "stmt_list",     "expr_call",     "expr_postfix",  "expr_basic",  
  "expr_unary",    "expr_mul",      "expr_add",      "expr_shift",  
  "expr_bitand",   "expr_bitxor",   "expr_bitor",    "expr_compare",
  "expr_and",      "expr_xor",      "expr_or",       "expr_lbody",  
  "expr_final",    "expr_list",     "expr_assign",   "assign_op",   
  "tokens",      
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "blah ::= expr SEMICOLON",
 /*   1 */ "blah ::= decl",
 /*   2 */ "name ::= IDENTIFIER",
 /*   3 */ "name ::= name PERIOD IDENTIFIER",
 /*   4 */ "proto ::= name LPN arg_list RPN",
 /*   5 */ "decl ::= name LBR odecl_list RBR",
 /*   6 */ "decl ::= name COLON expr_value LBR odecl_list RBR",
 /*   7 */ "decl ::= proto LBR stmt_list RBR",
 /*   8 */ "decl ::= proto YIELD LBR stmt_list RBR",
 /*   9 */ "decl ::= PERIOD name LPN arg_list RPN LBR stmt_list RBR",
 /*  10 */ "decl ::= PERIOD name LPN arg_list RPN YIELD LBR stmt_list RBR",
 /*  11 */ "expr_call ::= proto LPN arg_list RPN",
 /*  12 */ "expr_call ::= expr_call LPN arg_list RPN",
 /*  13 */ "expr_call ::= expr_postfix LPN arg_list RPN",
 /*  14 */ "expr_postfix ::= LPN expr RPN",
 /*  15 */ "expr_postfix ::= name PERIOD LSQ expr_value RSQ",
 /*  16 */ "expr_postfix ::= name LSQ expr_value RSQ",
 /*  17 */ "expr_postfix ::= proto PERIOD IDENTIFIER",
 /*  18 */ "expr_postfix ::= proto PERIOD LSQ expr_value RSQ",
 /*  19 */ "expr_postfix ::= proto LSQ expr_value RSQ",
 /*  20 */ "expr_postfix ::= expr_call PERIOD IDENTIFIER",
 /*  21 */ "expr_postfix ::= expr_call PERIOD LSQ expr_value RSQ",
 /*  22 */ "expr_postfix ::= expr_call LSQ expr_value RSQ",
 /*  23 */ "expr_postfix ::= expr_postfix PERIOD IDENTIFIER",
 /*  24 */ "expr_postfix ::= expr_postfix PERIOD LSQ expr_value RSQ",
 /*  25 */ "expr_postfix ::= expr_postfix LSQ expr_value RSQ",
 /*  26 */ "expr_basic ::= name",
 /*  27 */ "expr_basic ::= expr_postfix",
 /*  28 */ "expr_basic ::= proto",
 /*  29 */ "expr_basic ::= expr_call",
 /*  30 */ "expr_basic ::= NUMBER",
 /*  31 */ "expr_basic ::= STRING",
 /*  32 */ "expr_basic ::= TRUE",
 /*  33 */ "expr_basic ::= FALSE",
 /*  34 */ "expr_basic ::= NULL",
 /*  35 */ "expr_unary ::= expr_basic",
 /*  36 */ "expr_unary ::= PLUS expr_basic",
 /*  37 */ "expr_unary ::= MINUS expr_basic",
 /*  38 */ "expr_unary ::= XMARK expr_basic",
 /*  39 */ "expr_unary ::= TILDE expr_basic",
 /*  40 */ "expr_mul ::= expr_unary",
 /*  41 */ "expr_mul ::= expr_mul ASTERISK expr_unary",
 /*  42 */ "expr_mul ::= expr_mul SOLIDUS expr_unary",
 /*  43 */ "expr_mul ::= expr_mul PERCENT expr_unary",
 /*  44 */ "expr_mul ::= expr_mul TILDE expr_unary",
 /*  45 */ "expr_add ::= expr_mul",
 /*  46 */ "expr_add ::= expr_add PLUS expr_mul",
 /*  47 */ "expr_add ::= expr_add MINUS expr_mul",
 /*  48 */ "expr_shift ::= expr_add",
 /*  49 */ "expr_shift ::= expr_shift LSHIFT expr_add",
 /*  50 */ "expr_shift ::= expr_shift RSHIFT expr_add",
 /*  51 */ "expr_shift ::= expr_shift URSHIFT expr_add",
 /*  52 */ "expr_bitand ::= expr_shift",
 /*  53 */ "expr_bitand ::= expr_bitand AMPERSAND expr_shift",
 /*  54 */ "expr_bitxor ::= expr_bitand",
 /*  55 */ "expr_bitxor ::= expr_bitxor CARET expr_bitand",
 /*  56 */ "expr_bitor ::= expr_bitxor",
 /*  57 */ "expr_bitor ::= expr_bitor VBAR expr_bitxor",
 /*  58 */ "expr_compare ::= expr_bitor",
 /*  59 */ "expr_compare ::= expr_compare EQUAL expr_bitor",
 /*  60 */ "expr_compare ::= expr_compare NOTEQUAL expr_bitor",
 /*  61 */ "expr_compare ::= expr_compare LESS expr_bitor",
 /*  62 */ "expr_compare ::= expr_compare GREATER expr_bitor",
 /*  63 */ "expr_compare ::= expr_compare LESSEQUAL expr_bitor",
 /*  64 */ "expr_compare ::= expr_compare GREATEREQUAL expr_bitor",
 /*  65 */ "expr_compare ::= expr_compare IN expr_bitor",
 /*  66 */ "expr_compare ::= expr_compare NOTIN expr_bitor",
 /*  67 */ "expr_compare ::= expr_compare IS expr_bitor",
 /*  68 */ "expr_compare ::= expr_compare NOTIS expr_bitor",
 /*  69 */ "expr_and ::= expr_compare",
 /*  70 */ "expr_and ::= expr_and LOGICAND expr_compare",
 /*  71 */ "expr_xor ::= expr_and",
 /*  72 */ "expr_xor ::= expr_xor LOGICXOR expr_and",
 /*  73 */ "expr_or ::= expr_xor",
 /*  74 */ "expr_or ::= expr_or LOGICOR expr_xor",
 /*  75 */ "expr_value ::= expr_or",
 /*  76 */ "expr_value ::= expr_or QMARK expr_value COLON expr_value",
 /*  77 */ "expr_value ::= NEW name LPN arg_list RPN",
 /*  78 */ "expr_value ::= NEW expr_postfix LPN arg_list RPN",
 /*  79 */ "expr_value ::= COLON expr_value LBR odecl_list RBR",
 /*  80 */ "expr_value ::= COLON LBR odecl_list RBR",
 /*  81 */ "expr_value ::= QMARK LPN arg_list RPN LBR stmt_list RBR",
 /*  82 */ "expr_value ::= QMARK LPN arg_list RPN YIELD LBR stmt_list RBR",
 /*  83 */ "expr_value ::= proto YIELD",
 /*  84 */ "expr_value ::= expr_call YIELD",
 /*  85 */ "expr_lbody ::= expr_value",
 /*  86 */ "expr_lbody ::= expr_lbody COMMA expr_value",
 /*  87 */ "expr_final ::= expr_postfix ELLIPSIS",
 /*  88 */ "expr_final ::= expr_postfix LSQ RSQ ELLIPSIS",
 /*  89 */ "expr_final ::= ELLIPSIS",
 /*  90 */ "expr_list ::= expr_lbody",
 /*  91 */ "expr_list ::= expr_lbody COMMA expr_final",
 /*  92 */ "expr_assign ::= expr_list",
 /*  93 */ "expr_assign ::= expr_lbody assign_op expr_list",
 /*  94 */ "assign_op ::= ASSIGN",
 /*  95 */ "assign_op ::= MULASSIGN",
 /*  96 */ "assign_op ::= DIVASSIGN",
 /*  97 */ "assign_op ::= MODASSIGN",
 /*  98 */ "assign_op ::= INTDIVASSIGN",
 /*  99 */ "assign_op ::= ADDASSIGN",
 /* 100 */ "assign_op ::= SUBASSIGN",
 /* 101 */ "assign_op ::= LSHIFTASSIGN",
 /* 102 */ "assign_op ::= RSHIFTASSIGN",
 /* 103 */ "assign_op ::= URSHIFTASSIGN",
 /* 104 */ "assign_op ::= BITANDASSIGN",
 /* 105 */ "assign_op ::= BITXORASSIGN",
 /* 106 */ "assign_op ::= BITORASSIGN",
 /* 107 */ "expr ::= expr_assign",
 /* 108 */ "arg_list ::=",
 /* 109 */ "arg_list ::= expr_list",
 /* 110 */ "odecl_list ::= COMMA",
 /* 111 */ "stmt_list ::= COMMA",
 /* 112 */ "tokens ::= BREAK CASE CATCH CONTINUE DEFAULT DO ELSE FALSE FINALLY FOR IF NEW NULL RETURN SWITCH THROW TRUE TRY USING VAR WHILE YIELD IDENTIFIER NUMBER STRING XMARK PERCENT AMPERSAND LPN RPN ASTERISK PLUS COMMA MINUS PERIOD SOLIDUS COLON SEMICOLON LESS ASSIGN GREATER QMARK LSQ RSQ CARET LBR VBAR RBR TILDE INCREMENT DECREMENT LSHIFT RSHIFT URSHIFT NOTEQUAL LESSEQUAL EQUAL GREATEREQUAL MODASSIGN BITANDASSIGN MULASSIGN ADDASSIGN SUBASSIGN DIVASSIGN BITXORASSIGN BITORASSIGN INTDIVASSIGN LSHIFTASSIGN RSHIFTASSIGN URSHIFTASSIGN LOGICAND LOGICXOR LOGICOR ELLIPSIS",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to XecParse and XecParseFree.
*/
void *XecParseAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  XecParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from XecParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void XecParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int XecParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_MAX ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_MAX );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_SZ_ACTTAB );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   XecParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   XecParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 80, 2 },
  { 80, 1 },
  { 83, 1 },
  { 83, 3 },
  { 84, 4 },
  { 82, 4 },
  { 82, 6 },
  { 82, 4 },
  { 82, 5 },
  { 82, 8 },
  { 82, 9 },
  { 89, 4 },
  { 89, 4 },
  { 89, 4 },
  { 90, 3 },
  { 90, 5 },
  { 90, 4 },
  { 90, 3 },
  { 90, 5 },
  { 90, 4 },
  { 90, 3 },
  { 90, 5 },
  { 90, 4 },
  { 90, 3 },
  { 90, 5 },
  { 90, 4 },
  { 91, 1 },
  { 91, 1 },
  { 91, 1 },
  { 91, 1 },
  { 91, 1 },
  { 91, 1 },
  { 91, 1 },
  { 91, 1 },
  { 91, 1 },
  { 92, 1 },
  { 92, 2 },
  { 92, 2 },
  { 92, 2 },
  { 92, 2 },
  { 93, 1 },
  { 93, 3 },
  { 93, 3 },
  { 93, 3 },
  { 93, 3 },
  { 94, 1 },
  { 94, 3 },
  { 94, 3 },
  { 95, 1 },
  { 95, 3 },
  { 95, 3 },
  { 95, 3 },
  { 96, 1 },
  { 96, 3 },
  { 97, 1 },
  { 97, 3 },
  { 98, 1 },
  { 98, 3 },
  { 99, 1 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 100, 1 },
  { 100, 3 },
  { 101, 1 },
  { 101, 3 },
  { 102, 1 },
  { 102, 3 },
  { 87, 1 },
  { 87, 5 },
  { 87, 5 },
  { 87, 5 },
  { 87, 5 },
  { 87, 4 },
  { 87, 7 },
  { 87, 8 },
  { 87, 2 },
  { 87, 2 },
  { 103, 1 },
  { 103, 3 },
  { 104, 2 },
  { 104, 4 },
  { 104, 1 },
  { 105, 1 },
  { 105, 3 },
  { 106, 1 },
  { 106, 3 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 81, 1 },
  { 85, 0 },
  { 85, 1 },
  { 86, 1 },
  { 88, 1 },
  { 108, 74 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  XecParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      default:
      /* (0) blah ::= expr SEMICOLON */ yytestcase(yyruleno==0);
      /* (1) blah ::= decl */ yytestcase(yyruleno==1);
      /* (2) name ::= IDENTIFIER */ yytestcase(yyruleno==2);
      /* (3) name ::= name PERIOD IDENTIFIER */ yytestcase(yyruleno==3);
      /* (4) proto ::= name LPN arg_list RPN */ yytestcase(yyruleno==4);
      /* (5) decl ::= name LBR odecl_list RBR */ yytestcase(yyruleno==5);
      /* (6) decl ::= name COLON expr_value LBR odecl_list RBR */ yytestcase(yyruleno==6);
      /* (7) decl ::= proto LBR stmt_list RBR */ yytestcase(yyruleno==7);
      /* (8) decl ::= proto YIELD LBR stmt_list RBR */ yytestcase(yyruleno==8);
      /* (9) decl ::= PERIOD name LPN arg_list RPN LBR stmt_list RBR */ yytestcase(yyruleno==9);
      /* (10) decl ::= PERIOD name LPN arg_list RPN YIELD LBR stmt_list RBR */ yytestcase(yyruleno==10);
      /* (11) expr_call ::= proto LPN arg_list RPN */ yytestcase(yyruleno==11);
      /* (12) expr_call ::= expr_call LPN arg_list RPN */ yytestcase(yyruleno==12);
      /* (13) expr_call ::= expr_postfix LPN arg_list RPN */ yytestcase(yyruleno==13);
      /* (14) expr_postfix ::= LPN expr RPN */ yytestcase(yyruleno==14);
      /* (15) expr_postfix ::= name PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==15);
      /* (16) expr_postfix ::= name LSQ expr_value RSQ */ yytestcase(yyruleno==16);
      /* (17) expr_postfix ::= proto PERIOD IDENTIFIER */ yytestcase(yyruleno==17);
      /* (18) expr_postfix ::= proto PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==18);
      /* (19) expr_postfix ::= proto LSQ expr_value RSQ */ yytestcase(yyruleno==19);
      /* (20) expr_postfix ::= expr_call PERIOD IDENTIFIER */ yytestcase(yyruleno==20);
      /* (21) expr_postfix ::= expr_call PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==21);
      /* (22) expr_postfix ::= expr_call LSQ expr_value RSQ */ yytestcase(yyruleno==22);
      /* (23) expr_postfix ::= expr_postfix PERIOD IDENTIFIER */ yytestcase(yyruleno==23);
      /* (24) expr_postfix ::= expr_postfix PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==24);
      /* (25) expr_postfix ::= expr_postfix LSQ expr_value RSQ */ yytestcase(yyruleno==25);
      /* (26) expr_basic ::= name */ yytestcase(yyruleno==26);
      /* (27) expr_basic ::= expr_postfix */ yytestcase(yyruleno==27);
      /* (28) expr_basic ::= proto */ yytestcase(yyruleno==28);
      /* (29) expr_basic ::= expr_call */ yytestcase(yyruleno==29);
      /* (30) expr_basic ::= NUMBER */ yytestcase(yyruleno==30);
      /* (31) expr_basic ::= STRING */ yytestcase(yyruleno==31);
      /* (32) expr_basic ::= TRUE */ yytestcase(yyruleno==32);
      /* (33) expr_basic ::= FALSE */ yytestcase(yyruleno==33);
      /* (34) expr_basic ::= NULL */ yytestcase(yyruleno==34);
      /* (35) expr_unary ::= expr_basic */ yytestcase(yyruleno==35);
      /* (36) expr_unary ::= PLUS expr_basic */ yytestcase(yyruleno==36);
      /* (37) expr_unary ::= MINUS expr_basic */ yytestcase(yyruleno==37);
      /* (38) expr_unary ::= XMARK expr_basic */ yytestcase(yyruleno==38);
      /* (39) expr_unary ::= TILDE expr_basic */ yytestcase(yyruleno==39);
      /* (40) expr_mul ::= expr_unary */ yytestcase(yyruleno==40);
      /* (41) expr_mul ::= expr_mul ASTERISK expr_unary */ yytestcase(yyruleno==41);
      /* (42) expr_mul ::= expr_mul SOLIDUS expr_unary */ yytestcase(yyruleno==42);
      /* (43) expr_mul ::= expr_mul PERCENT expr_unary */ yytestcase(yyruleno==43);
      /* (44) expr_mul ::= expr_mul TILDE expr_unary */ yytestcase(yyruleno==44);
      /* (45) expr_add ::= expr_mul */ yytestcase(yyruleno==45);
      /* (46) expr_add ::= expr_add PLUS expr_mul */ yytestcase(yyruleno==46);
      /* (47) expr_add ::= expr_add MINUS expr_mul */ yytestcase(yyruleno==47);
      /* (48) expr_shift ::= expr_add */ yytestcase(yyruleno==48);
      /* (49) expr_shift ::= expr_shift LSHIFT expr_add */ yytestcase(yyruleno==49);
      /* (50) expr_shift ::= expr_shift RSHIFT expr_add */ yytestcase(yyruleno==50);
      /* (51) expr_shift ::= expr_shift URSHIFT expr_add */ yytestcase(yyruleno==51);
      /* (52) expr_bitand ::= expr_shift */ yytestcase(yyruleno==52);
      /* (53) expr_bitand ::= expr_bitand AMPERSAND expr_shift */ yytestcase(yyruleno==53);
      /* (54) expr_bitxor ::= expr_bitand */ yytestcase(yyruleno==54);
      /* (55) expr_bitxor ::= expr_bitxor CARET expr_bitand */ yytestcase(yyruleno==55);
      /* (56) expr_bitor ::= expr_bitxor */ yytestcase(yyruleno==56);
      /* (57) expr_bitor ::= expr_bitor VBAR expr_bitxor */ yytestcase(yyruleno==57);
      /* (58) expr_compare ::= expr_bitor */ yytestcase(yyruleno==58);
      /* (59) expr_compare ::= expr_compare EQUAL expr_bitor */ yytestcase(yyruleno==59);
      /* (60) expr_compare ::= expr_compare NOTEQUAL expr_bitor */ yytestcase(yyruleno==60);
      /* (61) expr_compare ::= expr_compare LESS expr_bitor */ yytestcase(yyruleno==61);
      /* (62) expr_compare ::= expr_compare GREATER expr_bitor */ yytestcase(yyruleno==62);
      /* (63) expr_compare ::= expr_compare LESSEQUAL expr_bitor */ yytestcase(yyruleno==63);
      /* (64) expr_compare ::= expr_compare GREATEREQUAL expr_bitor */ yytestcase(yyruleno==64);
      /* (65) expr_compare ::= expr_compare IN expr_bitor */ yytestcase(yyruleno==65);
      /* (66) expr_compare ::= expr_compare NOTIN expr_bitor */ yytestcase(yyruleno==66);
      /* (67) expr_compare ::= expr_compare IS expr_bitor */ yytestcase(yyruleno==67);
      /* (68) expr_compare ::= expr_compare NOTIS expr_bitor */ yytestcase(yyruleno==68);
      /* (69) expr_and ::= expr_compare */ yytestcase(yyruleno==69);
      /* (70) expr_and ::= expr_and LOGICAND expr_compare */ yytestcase(yyruleno==70);
      /* (71) expr_xor ::= expr_and */ yytestcase(yyruleno==71);
      /* (72) expr_xor ::= expr_xor LOGICXOR expr_and */ yytestcase(yyruleno==72);
      /* (73) expr_or ::= expr_xor */ yytestcase(yyruleno==73);
      /* (74) expr_or ::= expr_or LOGICOR expr_xor */ yytestcase(yyruleno==74);
      /* (75) expr_value ::= expr_or */ yytestcase(yyruleno==75);
      /* (76) expr_value ::= expr_or QMARK expr_value COLON expr_value */ yytestcase(yyruleno==76);
      /* (77) expr_value ::= NEW name LPN arg_list RPN */ yytestcase(yyruleno==77);
      /* (78) expr_value ::= NEW expr_postfix LPN arg_list RPN */ yytestcase(yyruleno==78);
      /* (79) expr_value ::= COLON expr_value LBR odecl_list RBR */ yytestcase(yyruleno==79);
      /* (80) expr_value ::= COLON LBR odecl_list RBR */ yytestcase(yyruleno==80);
      /* (81) expr_value ::= QMARK LPN arg_list RPN LBR stmt_list RBR */ yytestcase(yyruleno==81);
      /* (82) expr_value ::= QMARK LPN arg_list RPN YIELD LBR stmt_list RBR */ yytestcase(yyruleno==82);
      /* (83) expr_value ::= proto YIELD */ yytestcase(yyruleno==83);
      /* (84) expr_value ::= expr_call YIELD */ yytestcase(yyruleno==84);
      /* (85) expr_lbody ::= expr_value */ yytestcase(yyruleno==85);
      /* (86) expr_lbody ::= expr_lbody COMMA expr_value */ yytestcase(yyruleno==86);
      /* (87) expr_final ::= expr_postfix ELLIPSIS */ yytestcase(yyruleno==87);
      /* (88) expr_final ::= expr_postfix LSQ RSQ ELLIPSIS */ yytestcase(yyruleno==88);
      /* (89) expr_final ::= ELLIPSIS */ yytestcase(yyruleno==89);
      /* (90) expr_list ::= expr_lbody */ yytestcase(yyruleno==90);
      /* (91) expr_list ::= expr_lbody COMMA expr_final */ yytestcase(yyruleno==91);
      /* (92) expr_assign ::= expr_list */ yytestcase(yyruleno==92);
      /* (93) expr_assign ::= expr_lbody assign_op expr_list */ yytestcase(yyruleno==93);
      /* (94) assign_op ::= ASSIGN */ yytestcase(yyruleno==94);
      /* (95) assign_op ::= MULASSIGN */ yytestcase(yyruleno==95);
      /* (96) assign_op ::= DIVASSIGN */ yytestcase(yyruleno==96);
      /* (97) assign_op ::= MODASSIGN */ yytestcase(yyruleno==97);
      /* (98) assign_op ::= INTDIVASSIGN */ yytestcase(yyruleno==98);
      /* (99) assign_op ::= ADDASSIGN */ yytestcase(yyruleno==99);
      /* (100) assign_op ::= SUBASSIGN */ yytestcase(yyruleno==100);
      /* (101) assign_op ::= LSHIFTASSIGN */ yytestcase(yyruleno==101);
      /* (102) assign_op ::= RSHIFTASSIGN */ yytestcase(yyruleno==102);
      /* (103) assign_op ::= URSHIFTASSIGN */ yytestcase(yyruleno==103);
      /* (104) assign_op ::= BITANDASSIGN */ yytestcase(yyruleno==104);
      /* (105) assign_op ::= BITXORASSIGN */ yytestcase(yyruleno==105);
      /* (106) assign_op ::= BITORASSIGN */ yytestcase(yyruleno==106);
      /* (107) expr ::= expr_assign */ yytestcase(yyruleno==107);
      /* (108) arg_list ::= */ yytestcase(yyruleno==108);
      /* (109) arg_list ::= expr_list */ yytestcase(yyruleno==109);
      /* (110) odecl_list ::= COMMA */ yytestcase(yyruleno==110);
      /* (111) stmt_list ::= COMMA */ yytestcase(yyruleno==111);
      /* (112) tokens ::= BREAK CASE CATCH CONTINUE DEFAULT DO ELSE FALSE FINALLY FOR IF NEW NULL RETURN SWITCH THROW TRUE TRY USING VAR WHILE YIELD IDENTIFIER NUMBER STRING XMARK PERCENT AMPERSAND LPN RPN ASTERISK PLUS COMMA MINUS PERIOD SOLIDUS COLON SEMICOLON LESS ASSIGN GREATER QMARK LSQ RSQ CARET LBR VBAR RBR TILDE INCREMENT DECREMENT LSHIFT RSHIFT URSHIFT NOTEQUAL LESSEQUAL EQUAL GREATEREQUAL MODASSIGN BITANDASSIGN MULASSIGN ADDASSIGN SUBASSIGN DIVASSIGN BITXORASSIGN BITORASSIGN INTDIVASSIGN LSHIFTASSIGN RSHIFTASSIGN URSHIFTASSIGN LOGICAND LOGICXOR LOGICOR ELLIPSIS */ yytestcase(yyruleno==112);
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  XecParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  XecParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  XecParseARG_FETCH;
#define TOKEN (yyminor.yy0)
  XecParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  XecParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  XecParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "XecParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void XecParse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  XecParseTOKENTYPE yyminor       /* The value for the token */
  XecParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  XecParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
