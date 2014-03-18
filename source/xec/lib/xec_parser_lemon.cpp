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
#define YYNOCODE 122
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
#define YYNSTATE 260
#define YYNRULE 153
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
 /*     0 */   414,    3,    4,    6,  196,  239,  249,   62,  139,   63,
 /*    10 */   237,  195,   25,   70,   71,  249,  193,  179,  163,   96,
 /*    20 */   106,   98,  117,  118,  120,   94,  132,  133,  108,  242,
 /*    30 */   286,  244,  186,  171,  243,  286,   67,  194,  156,  234,
 /*    40 */     2,  286,  213,  196,  178,   80,   62,   81,   63,  237,
 /*    50 */   195,    1,   70,   71,   85,   86,  179,  163,   96,  106,
 /*    60 */    98,  117,  118,  120,   94,  132,  133,  108,  242,    6,
 /*    70 */    47,   48,  171,    6,  111,   67,  194,  156,  234,  196,
 /*    80 */    14,    1,   62,  157,   63,  229,  195,   87,   70,   71,
 /*    90 */    28,   19,  179,  163,   96,  106,   98,  117,  118,  120,
 /*   100 */    94,  132,  133,  108,  242,   46,   45,   44,  171,  180,
 /*   110 */     1,   67,  194,  156,  210,  209,   84,  228,  247,  249,
 /*   120 */    66,   72,   91,   69,  187,  195,    8,   70,   71,  151,
 /*   130 */   170,  179,  163,   96,  106,   98,  117,  118,  120,   94,
 /*   140 */   132,  133,  108,  259,  141,  244,   68,  176,   72,    4,
 /*   150 */    69,   57,  195,  249,   70,   71,  101,  170,  179,  163,
 /*   160 */    96,  106,   98,  117,  118,  120,   94,  132,  133,  108,
 /*   170 */   259,  255,  167,  146,  176,  178,   72,  184,   69,  204,
 /*   180 */   195,   41,   70,   71,  139,  170,  179,  163,   96,  106,
 /*   190 */    98,  117,  118,  120,   94,  132,  133,  108,  259,    6,
 /*   200 */    30,  146,  176,  202,  107,   72,  213,   69,  239,  195,
 /*   210 */    24,   70,   71,  100,  170,  179,  163,   96,  106,   98,
 /*   220 */   117,  118,  120,   94,  132,  133,  108,  259,  138,   29,
 /*   230 */   147,  149,  214,  248,  253,   21,   72,  212,   69,    7,
 /*   240 */   195,   23,   70,   71,   17,  170,  179,  163,   96,  106,
 /*   250 */    98,  117,  118,  120,   94,  132,  133,  108,  259,    9,
 /*   260 */   113,  146,  176,  240,   72,  249,   69,  200,  195,  207,
 /*   270 */    70,   71,   43,  170,  179,  163,   96,  106,   98,  117,
 /*   280 */   118,  120,   94,  132,  133,  108,  259,    6,  177,  146,
 /*   290 */   176,  238,  104,   72,  172,   69,  235,  195,   26,   70,
 /*   300 */    77,  232,  112,  179,  163,   96,  106,   98,  117,  118,
 /*   310 */   120,   94,  132,  133,  108,  259,   72,  143,   69,   42,
 /*   320 */   195,  144,   70,   71,  230,  173,  179,  163,   96,  106,
 /*   330 */    98,  117,  118,  120,   94,  132,  133,  108,  259,  245,
 /*   340 */   249,   66,  175,  203,    4,  187,    1,  251,  249,   65,
 /*   350 */    20,   58,  211,  231,  208,    5,   16,  192,  185,  183,
 /*   360 */   182,  181,   53,   54,   55,   56,    4,  272,    1,  165,
 /*   370 */   249,   65,  272,   58,  199,  198,  197,    5,  272,  192,
 /*   380 */   185,  183,  182,  181,   53,   54,   55,   56,   92,   61,
 /*   390 */   174,   72,  201,   73,  161,  195,   13,   74,   77,   15,
 /*   400 */    12,  179,  163,   96,  106,   98,  117,  118,  120,   93,
 /*   410 */    92,   61,  174,  261,    4,    6,    1,   57,  249,   65,
 /*   420 */   107,   58,   60,  231,   18,    5,   24,  192,  185,  183,
 /*   430 */   182,  181,   53,   54,   55,   56,   72,   86,   73,  252,
 /*   440 */   195,  140,   74,   77,   83,  246,  179,  163,   96,  106,
 /*   450 */    98,  117,  118,  120,   94,  132,  135,   57,   92,   61,
 /*   460 */   174,   72,  415,   69,  415,  195,  415,   70,   71,  415,
 /*   470 */   256,  179,  163,   96,  106,   98,  117,  118,  120,   94,
 /*   480 */   132,  133,  108,  259,  415,  415,  415,  160,  260,    4,
 /*   490 */     6,    1,   57,  249,   65,  139,   58,   60,  198,  415,
 /*   500 */     5,  415,  192,  185,  183,  182,  181,   53,   54,   55,
 /*   510 */    56,   72,  415,   73,  415,  195,  415,   74,   77,  415,
 /*   520 */   415,  179,  163,   96,  106,   98,  117,  118,  120,   94,
 /*   530 */   134,  415,  415,   92,   61,  174,   34,   31,   36,   32,
 /*   540 */    33,   35,   40,   39,   38,   37,   72,  415,   69,  415,
 /*   550 */   195,  415,   70,   71,  415,  173,  179,  163,   96,  106,
 /*   560 */    98,  117,  118,  120,   94,  132,  133,  108,  259,   72,
 /*   570 */   415,   69,  148,  195,  415,   70,   77,  415,  164,  179,
 /*   580 */   163,   96,  106,   98,  117,  118,  120,   94,  132,  133,
 /*   590 */   108,  259,   72,  415,   69,  415,  195,  415,   70,   77,
 /*   600 */   415,  153,  179,  163,   96,  106,   98,  117,  118,  120,
 /*   610 */    94,  132,  133,  108,  259,  415,   59,   64,  102,   82,
 /*   620 */   188,   72,  190,   69,  415,  195,  415,   70,   77,  415,
 /*   630 */   116,  179,  163,   96,  106,   98,  117,  118,  120,   94,
 /*   640 */   132,  133,  108,  259,   72,  415,   69,  415,  195,  415,
 /*   650 */    70,   77,  415,  154,  179,  163,   96,  106,   98,  117,
 /*   660 */   118,  120,   94,  132,  133,  108,  259,   72,  415,   69,
 /*   670 */   415,  195,  415,   70,   77,  415,  155,  179,  163,   96,
 /*   680 */   106,   98,  117,  118,  120,   94,  132,  133,  108,  259,
 /*   690 */    72,  415,   69,  415,  195,  415,   70,   77,  415,  114,
 /*   700 */   179,  163,   96,  106,   98,  117,  118,  120,   94,  132,
 /*   710 */   133,  108,  259,    4,  241,   10,  415,  249,  142,  415,
 /*   720 */    58,   12,  415,  415,    5,  215,  192,  185,  183,  182,
 /*   730 */   181,   53,   54,   55,   56,    4,  415,   10,  415,  249,
 /*   740 */   142,  415,   58,  415,  415,  415,    5,  250,  192,  185,
 /*   750 */   183,  182,  181,   53,   54,   55,   56,   92,   61,  174,
 /*   760 */   415,  415,  216,  217,  218,  219,  220,  221,  222,  223,
 /*   770 */   224,  225,  226,  227,   52,   49,   50,   51,  415,   92,
 /*   780 */    61,  174,   72,  415,   69,  415,  195,  415,   70,   77,
 /*   790 */   415,  115,  179,  163,   96,  106,   98,  117,  118,  120,
 /*   800 */    94,  132,  133,  108,  259,   72,  415,   69,  415,  195,
 /*   810 */   415,   70,   77,  415,  145,  179,  163,   96,  106,   98,
 /*   820 */   117,  118,  120,   94,  132,  133,  108,  259,   72,  415,
 /*   830 */    69,  415,  195,  415,   70,   77,  415,  152,  179,  163,
 /*   840 */    96,  106,   98,  117,  118,  120,   94,  132,  133,  108,
 /*   850 */   259,   72,  415,   73,  415,  195,  415,   74,   77,  415,
 /*   860 */   415,  179,  163,   96,  106,   98,  119,   72,  415,   69,
 /*   870 */   415,  195,  415,   70,   77,  415,  254,  179,  163,   96,
 /*   880 */   106,   98,  117,  118,  120,   94,  132,  133,  108,  259,
 /*   890 */    72,  415,   69,  415,  195,  415,   70,   77,  415,  137,
 /*   900 */   179,  163,   96,  106,   98,  117,  118,  120,   94,  132,
 /*   910 */   133,  108,  259,   72,  415,   69,  415,  195,  415,   70,
 /*   920 */    77,  415,  136,  179,  163,   96,  106,   98,  117,  118,
 /*   930 */   120,   94,  132,  133,  108,  259,   72,  415,   69,  415,
 /*   940 */   195,  415,   70,   77,  415,  169,  179,  163,   96,  106,
 /*   950 */    98,  117,  118,  120,   94,  132,  133,  108,  259,    4,
 /*   960 */   415,   10,  415,  249,  142,  415,   58,   11,  415,  415,
 /*   970 */     5,  215,  192,  185,  183,  182,  181,   53,   54,   55,
 /*   980 */    56,    4,  415,   10,  415,  249,  142,  415,   58,  415,
 /*   990 */   415,  415,    5,  150,  192,  185,  183,  182,  181,   53,
 /*  1000 */    54,   55,   56,   92,   61,  174,  415,  415,  216,  217,
 /*  1010 */   218,  219,  220,  221,  222,  223,  224,  225,  226,  227,
 /*  1020 */   415,  415,  415,  415,  415,   92,   61,    4,  415,   10,
 /*  1030 */   159,  249,  142,  415,   58,  415,  415,  415,    5,  415,
 /*  1040 */   192,  185,  183,  182,  181,   53,   54,   55,   56,    4,
 /*  1050 */     6,   10,    1,  249,  142,  104,   58,  415,   90,  415,
 /*  1060 */     5,   26,  192,  185,  183,  182,  181,   53,   54,   55,
 /*  1070 */    56,   92,   61,   72,  415,   73,  415,  195,  415,   74,
 /*  1080 */    77,  415,  415,  179,  163,   96,  106,   98,  117,  118,
 /*  1090 */   130,  415,  415,   92,   61,  415,   72,  415,   73,  415,
 /*  1100 */   195,  415,   74,   77,  415,  415,  179,  163,   96,  106,
 /*  1110 */    98,  117,  118,  123,   72,  415,   73,  415,  195,  415,
 /*  1120 */    74,   77,  415,  415,  179,  163,   96,  106,   98,  117,
 /*  1130 */   118,  127,   72,  415,   73,  415,  195,  415,   74,   77,
 /*  1140 */   415,  415,  179,  163,   96,  106,   98,  117,  118,  124,
 /*  1150 */    72,  415,   73,  415,  195,  415,   74,   77,  415,  415,
 /*  1160 */   179,  163,   96,  106,   98,  117,  118,  131,   72,  415,
 /*  1170 */    73,  415,  195,  415,   74,   77,  415,  415,  179,  163,
 /*  1180 */    96,  106,   98,  117,  118,  122,   72,  415,   73,  415,
 /*  1190 */   195,  415,   74,   77,  415,  415,  179,  163,   96,  106,
 /*  1200 */    98,  117,  118,  129,   72,  415,   73,  415,  195,  415,
 /*  1210 */    74,   77,  415,  415,  179,  163,   96,  106,   98,  117,
 /*  1220 */   118,  125,   72,  415,   73,  415,  195,  415,   74,   77,
 /*  1230 */   415,  415,  179,  163,   96,  106,   98,  117,  118,  128,
 /*  1240 */    72,  415,   73,  415,  195,  415,   74,   77,  415,  415,
 /*  1250 */   179,  163,   96,  106,   98,  117,  118,  126,   72,  415,
 /*  1260 */    73,  415,  195,  415,   74,   77,  415,  415,  179,  163,
 /*  1270 */    96,  106,   98,  117,  121,    4,   72,  415,   73,  249,
 /*  1280 */   195,  415,   74,   77,  415,  415,  179,  258,  192,  185,
 /*  1290 */   183,  182,  181,   53,   54,   55,   56,  415,   72,  415,
 /*  1300 */    73,  415,  195,  415,   74,   77,  415,  415,  179,  163,
 /*  1310 */    96,  106,   99,  415,   72,  415,   73,  415,  195,  415,
 /*  1320 */    74,   77,  415,  415,  179,  163,   96,  110,   72,  415,
 /*  1330 */    73,  415,  195,  415,   74,   77,  415,  415,  179,  163,
 /*  1340 */    96,  103,   72,  415,   73,  415,  195,  415,   74,   77,
 /*  1350 */   415,  415,  179,  163,   96,  105,   72,    4,   73,  415,
 /*  1360 */   195,  249,   74,   77,    6,  415,  179,  163,   95,  109,
 /*  1370 */   192,  185,  183,  182,  181,   22,   72,  415,   73,  415,
 /*  1380 */   195,  415,   74,   77,    6,  415,  179,  163,   97,  111,
 /*  1390 */    72,  415,   73,  415,  195,   27,   74,   77,  415,  415,
 /*  1400 */   179,  158,   72,  415,   73,  415,  195,  415,   74,   77,
 /*  1410 */   415,  415,  179,  162,   72,  415,   73,  415,  195,  415,
 /*  1420 */    74,   77,  415,  415,  179,  257,   72,  415,   73,  415,
 /*  1430 */   195,  415,   74,   77,  415,  415,  206,   72,  415,   73,
 /*  1440 */   415,  195,  415,   74,   77,  415,   72,  205,   73,  415,
 /*  1450 */   195,  415,   74,   77,  415,   72,  168,   73,  415,  195,
 /*  1460 */   415,   74,   77,  415,  191,  166,   72,  415,   73,  415,
 /*  1470 */    88,  415,   74,   77,   64,  102,   82,  188,   72,  189,
 /*  1480 */    73,    6,   89,  415,   74,   77,  104,  415,   75,  236,
 /*  1490 */    76,    6,   26,  415,   78,   79,  109,  415,  415,  233,
 /*  1500 */   415,  415,   22,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    80,   81,    1,    1,   84,    5,    5,   87,    6,   89,
 /*    10 */    90,   91,   12,   93,   94,    5,   85,   97,   98,   99,
 /*    20 */   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,
 /*    30 */     1,   82,   84,  113,   85,    6,  116,  117,  118,  119,
 /*    40 */    81,   12,   82,   84,   84,   87,   87,   89,   89,   90,
 /*    50 */    91,    3,   93,   94,   82,   45,   97,   98,   99,  100,
 /*    60 */   101,  102,  103,  104,  105,  106,  107,  108,  109,    1,
 /*    70 */    19,   20,  113,    1,    6,  116,  117,  118,  119,   84,
 /*    80 */    12,    3,   87,   84,   89,   90,   91,    9,   93,   94,
 /*    90 */    44,   45,   97,   98,   99,  100,  101,  102,  103,  104,
 /*   100 */   105,  106,  107,  108,  109,   26,   27,   28,  113,   84,
 /*   110 */     3,  116,  117,  118,  119,   47,    9,   83,    4,    5,
 /*   120 */     6,   87,    7,   89,   10,   91,   11,   93,   94,   95,
 /*   130 */    96,   97,   98,   99,  100,  101,  102,  103,  104,  105,
 /*   140 */   106,  107,  108,  109,   83,   82,  112,  113,   87,    1,
 /*   150 */    89,    3,   91,    5,   93,   94,   82,   96,   97,   98,
 /*   160 */    99,  100,  101,  102,  103,  104,  105,  106,  107,  108,
 /*   170 */   109,   84,   83,  112,  113,   84,   87,   84,   89,   82,
 /*   180 */    91,   31,   93,   94,    6,   96,   97,   98,   99,  100,
 /*   190 */   101,  102,  103,  104,  105,  106,  107,  108,  109,    1,
 /*   200 */    42,  112,  113,   82,    6,   87,   82,   89,    5,   91,
 /*   210 */    12,   93,   94,   82,   96,   97,   98,   99,  100,  101,
 /*   220 */   102,  103,  104,  105,  106,  107,  108,  109,  110,   43,
 /*   230 */   112,  113,   83,   13,   13,    8,   87,    5,   89,  114,
 /*   240 */    91,    8,   93,   94,   12,   96,   97,   98,   99,  100,
 /*   250 */   101,  102,  103,  104,  105,  106,  107,  108,  109,  114,
 /*   260 */    83,  112,  113,    2,   87,    5,   89,   13,   91,   10,
 /*   270 */    93,   94,   29,   96,   97,   98,   99,  100,  101,  102,
 /*   280 */   103,  104,  105,  106,  107,  108,  109,    1,   13,  112,
 /*   290 */   113,   13,    6,   87,   13,   89,   13,   91,   12,   93,
 /*   300 */    94,   13,   96,   97,   98,   99,  100,  101,  102,  103,
 /*   310 */   104,  105,  106,  107,  108,  109,   87,  111,   89,   30,
 /*   320 */    91,  115,   93,   94,   13,   96,   97,   98,   99,  100,
 /*   330 */   101,  102,  103,  104,  105,  106,  107,  108,  109,    4,
 /*   340 */     5,    6,  113,    5,    1,   10,    3,    4,    5,    6,
 /*   350 */    12,    8,    2,   10,   47,   12,    8,   14,   15,   16,
 /*   360 */    17,   18,   19,   20,   21,   22,    1,    1,    3,    4,
 /*   370 */     5,    6,    6,    8,    7,   10,    7,   12,   12,   14,
 /*   380 */    15,   16,   17,   18,   19,   20,   21,   22,   45,   46,
 /*   390 */    47,   87,    5,   89,    4,   91,    7,   93,   94,   12,
 /*   400 */     7,   97,   98,   99,  100,  101,  102,  103,  104,  105,
 /*   410 */    45,   46,   47,    0,    1,    1,    3,    3,    5,    6,
 /*   420 */     6,    8,    8,   10,    7,   12,   12,   14,   15,   16,
 /*   430 */    17,   18,   19,   20,   21,   22,   87,   45,   89,   10,
 /*   440 */    91,   87,   93,   94,   82,   85,   97,   98,   99,  100,
 /*   450 */   101,  102,  103,  104,  105,  106,  107,    3,   45,   46,
 /*   460 */    47,   87,  121,   89,  121,   91,  121,   93,   94,  121,
 /*   470 */    96,   97,   98,   99,  100,  101,  102,  103,  104,  105,
 /*   480 */   106,  107,  108,  109,  121,  121,  121,  113,    0,    1,
 /*   490 */     1,    3,    3,    5,    6,    6,    8,    8,   10,  121,
 /*   500 */    12,  121,   14,   15,   16,   17,   18,   19,   20,   21,
 /*   510 */    22,   87,  121,   89,  121,   91,  121,   93,   94,  121,
 /*   520 */   121,   97,   98,   99,  100,  101,  102,  103,  104,  105,
 /*   530 */   106,  121,  121,   45,   46,   47,   32,   33,   34,   35,
 /*   540 */    36,   37,   38,   39,   40,   41,   87,  121,   89,  121,
 /*   550 */    91,  121,   93,   94,  121,   96,   97,   98,   99,  100,
 /*   560 */   101,  102,  103,  104,  105,  106,  107,  108,  109,   87,
 /*   570 */   121,   89,  113,   91,  121,   93,   94,  121,   96,   97,
 /*   580 */    98,   99,  100,  101,  102,  103,  104,  105,  106,  107,
 /*   590 */   108,  109,   87,  121,   89,  121,   91,  121,   93,   94,
 /*   600 */   121,   96,   97,   98,   99,  100,  101,  102,  103,  104,
 /*   610 */   105,  106,  107,  108,  109,  121,   86,   87,   88,   89,
 /*   620 */    90,   87,   92,   89,  121,   91,  121,   93,   94,  121,
 /*   630 */    96,   97,   98,   99,  100,  101,  102,  103,  104,  105,
 /*   640 */   106,  107,  108,  109,   87,  121,   89,  121,   91,  121,
 /*   650 */    93,   94,  121,   96,   97,   98,   99,  100,  101,  102,
 /*   660 */   103,  104,  105,  106,  107,  108,  109,   87,  121,   89,
 /*   670 */   121,   91,  121,   93,   94,  121,   96,   97,   98,   99,
 /*   680 */   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,
 /*   690 */    87,  121,   89,  121,   91,  121,   93,   94,  121,   96,
 /*   700 */    97,   98,   99,  100,  101,  102,  103,  104,  105,  106,
 /*   710 */   107,  108,  109,    1,    2,    3,  121,    5,    6,  121,
 /*   720 */     8,    7,  121,  121,   12,   11,   14,   15,   16,   17,
 /*   730 */    18,   19,   20,   21,   22,    1,  121,    3,  121,    5,
 /*   740 */     6,  121,    8,  121,  121,  121,   12,   13,   14,   15,
 /*   750 */    16,   17,   18,   19,   20,   21,   22,   45,   46,   47,
 /*   760 */   121,  121,   48,   49,   50,   51,   52,   53,   54,   55,
 /*   770 */    56,   57,   58,   59,   22,   23,   24,   25,  121,   45,
 /*   780 */    46,   47,   87,  121,   89,  121,   91,  121,   93,   94,
 /*   790 */   121,   96,   97,   98,   99,  100,  101,  102,  103,  104,
 /*   800 */   105,  106,  107,  108,  109,   87,  121,   89,  121,   91,
 /*   810 */   121,   93,   94,  121,   96,   97,   98,   99,  100,  101,
 /*   820 */   102,  103,  104,  105,  106,  107,  108,  109,   87,  121,
 /*   830 */    89,  121,   91,  121,   93,   94,  121,   96,   97,   98,
 /*   840 */    99,  100,  101,  102,  103,  104,  105,  106,  107,  108,
 /*   850 */   109,   87,  121,   89,  121,   91,  121,   93,   94,  121,
 /*   860 */   121,   97,   98,   99,  100,  101,  102,   87,  121,   89,
 /*   870 */   121,   91,  121,   93,   94,  121,   96,   97,   98,   99,
 /*   880 */   100,  101,  102,  103,  104,  105,  106,  107,  108,  109,
 /*   890 */    87,  121,   89,  121,   91,  121,   93,   94,  121,   96,
 /*   900 */    97,   98,   99,  100,  101,  102,  103,  104,  105,  106,
 /*   910 */   107,  108,  109,   87,  121,   89,  121,   91,  121,   93,
 /*   920 */    94,  121,   96,   97,   98,   99,  100,  101,  102,  103,
 /*   930 */   104,  105,  106,  107,  108,  109,   87,  121,   89,  121,
 /*   940 */    91,  121,   93,   94,  121,   96,   97,   98,   99,  100,
 /*   950 */   101,  102,  103,  104,  105,  106,  107,  108,  109,    1,
 /*   960 */   121,    3,  121,    5,    6,  121,    8,    7,  121,  121,
 /*   970 */    12,   11,   14,   15,   16,   17,   18,   19,   20,   21,
 /*   980 */    22,    1,  121,    3,  121,    5,    6,  121,    8,  121,
 /*   990 */   121,  121,   12,   13,   14,   15,   16,   17,   18,   19,
 /*  1000 */    20,   21,   22,   45,   46,   47,  121,  121,   48,   49,
 /*  1010 */    50,   51,   52,   53,   54,   55,   56,   57,   58,   59,
 /*  1020 */   121,  121,  121,  121,  121,   45,   46,    1,  121,    3,
 /*  1030 */     4,    5,    6,  121,    8,  121,  121,  121,   12,  121,
 /*  1040 */    14,   15,   16,   17,   18,   19,   20,   21,   22,    1,
 /*  1050 */     1,    3,    3,    5,    6,    6,    8,  121,    9,  121,
 /*  1060 */    12,   12,   14,   15,   16,   17,   18,   19,   20,   21,
 /*  1070 */    22,   45,   46,   87,  121,   89,  121,   91,  121,   93,
 /*  1080 */    94,  121,  121,   97,   98,   99,  100,  101,  102,  103,
 /*  1090 */   104,  121,  121,   45,   46,  121,   87,  121,   89,  121,
 /*  1100 */    91,  121,   93,   94,  121,  121,   97,   98,   99,  100,
 /*  1110 */   101,  102,  103,  104,   87,  121,   89,  121,   91,  121,
 /*  1120 */    93,   94,  121,  121,   97,   98,   99,  100,  101,  102,
 /*  1130 */   103,  104,   87,  121,   89,  121,   91,  121,   93,   94,
 /*  1140 */   121,  121,   97,   98,   99,  100,  101,  102,  103,  104,
 /*  1150 */    87,  121,   89,  121,   91,  121,   93,   94,  121,  121,
 /*  1160 */    97,   98,   99,  100,  101,  102,  103,  104,   87,  121,
 /*  1170 */    89,  121,   91,  121,   93,   94,  121,  121,   97,   98,
 /*  1180 */    99,  100,  101,  102,  103,  104,   87,  121,   89,  121,
 /*  1190 */    91,  121,   93,   94,  121,  121,   97,   98,   99,  100,
 /*  1200 */   101,  102,  103,  104,   87,  121,   89,  121,   91,  121,
 /*  1210 */    93,   94,  121,  121,   97,   98,   99,  100,  101,  102,
 /*  1220 */   103,  104,   87,  121,   89,  121,   91,  121,   93,   94,
 /*  1230 */   121,  121,   97,   98,   99,  100,  101,  102,  103,  104,
 /*  1240 */    87,  121,   89,  121,   91,  121,   93,   94,  121,  121,
 /*  1250 */    97,   98,   99,  100,  101,  102,  103,  104,   87,  121,
 /*  1260 */    89,  121,   91,  121,   93,   94,  121,  121,   97,   98,
 /*  1270 */    99,  100,  101,  102,  103,    1,   87,  121,   89,    5,
 /*  1280 */    91,  121,   93,   94,  121,  121,   97,   98,   14,   15,
 /*  1290 */    16,   17,   18,   19,   20,   21,   22,  121,   87,  121,
 /*  1300 */    89,  121,   91,  121,   93,   94,  121,  121,   97,   98,
 /*  1310 */    99,  100,  101,  121,   87,  121,   89,  121,   91,  121,
 /*  1320 */    93,   94,  121,  121,   97,   98,   99,  100,   87,  121,
 /*  1330 */    89,  121,   91,  121,   93,   94,  121,  121,   97,   98,
 /*  1340 */    99,  100,   87,  121,   89,  121,   91,  121,   93,   94,
 /*  1350 */   121,  121,   97,   98,   99,  100,   87,    1,   89,  121,
 /*  1360 */    91,    5,   93,   94,    1,  121,   97,   98,   99,    6,
 /*  1370 */    14,   15,   16,   17,   18,   12,   87,  121,   89,  121,
 /*  1380 */    91,  121,   93,   94,    1,  121,   97,   98,   99,    6,
 /*  1390 */    87,  121,   89,  121,   91,   12,   93,   94,  121,  121,
 /*  1400 */    97,   98,   87,  121,   89,  121,   91,  121,   93,   94,
 /*  1410 */   121,  121,   97,   98,   87,  121,   89,  121,   91,  121,
 /*  1420 */    93,   94,  121,  121,   97,   98,   87,  121,   89,  121,
 /*  1430 */    91,  121,   93,   94,  121,  121,   97,   87,  121,   89,
 /*  1440 */   121,   91,  121,   93,   94,  121,   87,   97,   89,  121,
 /*  1450 */    91,  121,   93,   94,  121,   87,   97,   89,  121,   91,
 /*  1460 */   121,   93,   94,  121,   85,   97,   87,  121,   89,  121,
 /*  1470 */    91,  121,   93,   94,   87,   88,   89,   90,   87,   92,
 /*  1480 */    89,    1,   91,  121,   93,   94,    6,  121,   87,    9,
 /*  1490 */    89,    1,   12,  121,   93,   94,    6,  121,  121,    9,
 /*  1500 */   121,  121,   12,
};
#define YY_SHIFT_USE_DFLT (-1)
#define YY_SHIFT_MAX 156
static const short yy_shift_ofst[] = {
 /*     0 */   488,  365,  343,  413,  958,  734,  712,  958,  958,  958,
 /*    10 */  1026,  958,  958,  958,  980, 1048, 1048, 1048, 1048, 1048,
 /*    20 */  1048, 1048, 1048, 1048, 1048, 1048, 1048, 1048, 1274, 1274,
 /*    30 */  1274, 1274, 1274, 1274, 1274, 1274, 1274, 1274, 1274, 1274,
 /*    40 */  1274, 1274, 1274, 1274, 1274, 1274, 1274, 1274, 1274, 1274,
 /*    50 */  1274, 1274, 1274, 1356, 1356, 1356, 1356,  114,  148,  335,
 /*    60 */     1,    1,  414, 1049,  489,   10,  260,  960,  714, 1480,
 /*    70 */  1490,   68,  198,  286, 1363,  198,  286, 1383, 1363, 1383,
 /*    80 */     2,   78,  107,   48,   48,   48,   72,   48,  454,  454,
 /*    90 */    48,  260,   72,  504,  504,  752,  752,  752,   79,   79,
 /*   100 */   366,   29,  115,   51,  232,   51,   51,    0,   46,  338,
 /*   110 */    51,  387,  233,  261,  254,  275,  281,  243,  289,  243,
 /*   120 */   150,  289,  150,  150,  150,  150,  150,  150,  150,  150,
 /*   130 */   150,  150,  158,  186,  158,  186,  227,  221,  220,  203,
 /*   140 */   178,  429,  392,  390,  417,  348,  393,  389,  369,  367,
 /*   150 */   307,  350,  311,  288,  283,  278,  259,
};
#define YY_REDUCE_USE_DFLT (-81)
#define YY_REDUCE_MAX 92
static const short yy_reduce_ofst[] = {
 /*     0 */   -80,  -41,   -5,   -5,   34,  118,  177,  149,   61,   89,
 /*    10 */   206,  374,  229,  459,  603,  741,  849,  557,  718,  826,
 /*    20 */   505,  780,  534,  482,  803,  580,  695,  603,  349,  424,
 /*    30 */   304, 1009, 1117, 1153, 1081, 1027, 1045, 1063,  986, 1099,
 /*    40 */  1135, 1171,  764, 1211, 1255, 1241, 1227, 1289, 1269, 1315,
 /*    50 */  1303, 1189, 1327, 1339, 1350, 1359, 1368,  530, 1379, 1387,
 /*    60 */  1391, 1401,  -51,  -40,  -51,  -42,  -42,  145,  125,  124,
 /*    70 */    97,  121,   63,  124,   97,  131,  124,  121,   97,   74,
 /*    80 */    63,   93,   91,   87,   25,   -1,  -28,  -52,  -69,  360,
 /*    90 */    25,  354,  362,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   413,  413,  413,  413,  413,  413,  413,  413,  413,  413,
 /*    10 */   413,  413,  413,  390,  413,  413,  413,  413,  396,  413,
 /*    20 */   413,  413,  413,  413,  413,  413,  413,  413,  413,  413,
 /*    30 */   413,  413,  413,  413,  413,  413,  413,  413,  413,  413,
 /*    40 */   413,  413,  413,  413,  413,  413,  413,  413,  413,  413,
 /*    50 */   413,  413,  413,  413,  413,  413,  413,  413,  413,  413,
 /*    60 */   413,  413,  299,  301,  270,  413,  413,  400,  370,  301,
 /*    70 */   302,  300,  299,  301,  302,  413,  413,  300,  413,  413,
 /*    80 */   413,  413,  413,  413,  413,  413,  413,  413,  413,  413,
 /*    90 */   359,  413,  413,  344,  343,  321,  319,  320,  326,  327,
 /*   100 */   351,  352,  413,  324,  413,  325,  322,  413,  349,  413,
 /*   110 */   323,  413,  413,  413,  413,  413,  413,  328,  330,  329,
 /*   120 */   332,  331,  333,  334,  335,  336,  337,  338,  339,  340,
 /*   130 */   341,  342,  345,  347,  346,  348,  413,  413,  413,  413,
 /*   140 */   271,  413,  413,  413,  395,  413,  370,  389,  391,  387,
 /*   150 */   413,  413,  413,  413,  413,  413,  413,  358,  316,  362,
 /*   160 */   401,  363,  315,  314,  393,  264,  313,  403,  312,  394,
 /*   170 */   364,  399,  295,  365,  368,  371,  369,  292,  275,  309,
 /*   180 */   276,  308,  307,  306,  277,  305,  278,  279,  280,  283,
 /*   190 */   282,  355,  304,  356,  402,  303,  404,  392,  408,  388,
 /*   200 */   298,  296,  286,  293,  285,  311,  310,  405,  367,  366,
 /*   210 */   409,  287,  290,  284,  373,  374,  375,  376,  377,  378,
 /*   220 */   379,  380,  381,  382,  383,  384,  385,  386,  372,  410,
 /*   230 */   297,  411,  294,  360,  406,  291,  359,  407,  288,  269,
 /*   240 */   263,  262,  397,  273,  272,  267,  274,  266,  354,  268,
 /*   250 */   353,  265,  281,  289,  350,  357,  398,  318,  317,  361,
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
  "$",             "LPN",           "RPN",           "LBR",         
  "RBR",           "IDENTIFIER",    "PERIOD",        "COMMA",       
  "COLON",         "YIELD",         "SEMICOLON",     "ASSIGN",      
  "LSQ",           "RSQ",           "NUMBER",        "STRING",      
  "TRUE",          "FALSE",         "NULL",          "PLUS",        
  "MINUS",         "XMARK",         "TILDE",         "ASTERISK",    
  "SOLIDUS",       "PERCENT",       "LSHIFT",        "RSHIFT",      
  "URSHIFT",       "AMPERSAND",     "CARET",         "VBAR",        
  "EQUAL",         "NOTEQUAL",      "LESS",          "GREATER",     
  "LESSEQUAL",     "GREATEREQUAL",  "IN",            "NOTIN",       
  "IS",            "NOTIS",         "LOGICAND",      "LOGICXOR",    
  "LOGICOR",       "QMARK",         "NEW",           "ELLIPSIS",    
  "MULASSIGN",     "DIVASSIGN",     "MODASSIGN",     "INTDIVASSIGN",
  "ADDASSIGN",     "SUBASSIGN",     "LSHIFTASSIGN",  "RSHIFTASSIGN",
  "URSHIFTASSIGN",  "BITANDASSIGN",  "BITXORASSIGN",  "BITORASSIGN", 
  "BREAK",         "CASE",          "CATCH",         "CONTINUE",    
  "DEFAULT",       "DO",            "ELSE",          "FINALLY",     
  "FOR",           "IF",            "RETURN",        "SWITCH",      
  "THROW",         "TRY",           "USING",         "VAR",         
  "WHILE",         "INCREMENT",     "DECREMENT",     "error",       
  "script",        "stmt_list",     "expr_paren",    "expr_list",   
  "stmt_brace",    "odecl_brace",   "odecl_list",    "name",        
  "name_list",     "proto",         "decl",          "expr_simple", 
  "odecl",         "expr_call",     "expr_postfix",  "expr_assign", 
  "expr_value",    "expr_basic",    "expr_unary",    "expr_mul",    
  "expr_add",      "expr_shift",    "expr_bitand",   "expr_bitxor", 
  "expr_bitor",    "expr_compare",  "expr_and",      "expr_xor",    
  "expr_or",       "expr_nolbr",    "value_list",    "keyval_list", 
  "expr_lbody",    "expr_final",    "assign_op",     "keyval_lbody",
  "sexpr_lbody",   "sexpr_list",    "sexpr_assign",  "stmt",        
  "tokens",      
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "script ::=",
 /*   1 */ "script ::= stmt_list",
 /*   2 */ "expr_paren ::= LPN RPN",
 /*   3 */ "expr_paren ::= LPN expr_list RPN",
 /*   4 */ "stmt_brace ::= LBR RBR",
 /*   5 */ "stmt_brace ::= LBR stmt_list RBR",
 /*   6 */ "odecl_brace ::= LBR RBR",
 /*   7 */ "odecl_brace ::= LBR odecl_list RBR",
 /*   8 */ "name ::= IDENTIFIER",
 /*   9 */ "name ::= name PERIOD IDENTIFIER",
 /*  10 */ "name_list ::= name",
 /*  11 */ "name_list ::= name_list COMMA name",
 /*  12 */ "proto ::= name expr_paren",
 /*  13 */ "decl ::= name odecl_brace",
 /*  14 */ "decl ::= name COLON expr_simple odecl_brace",
 /*  15 */ "decl ::= proto stmt_brace",
 /*  16 */ "decl ::= proto YIELD stmt_brace",
 /*  17 */ "decl ::= PERIOD proto stmt_brace",
 /*  18 */ "decl ::= PERIOD proto YIELD stmt_brace",
 /*  19 */ "odecl ::= SEMICOLON",
 /*  20 */ "odecl ::= decl",
 /*  21 */ "odecl ::= name_list ASSIGN expr_list SEMICOLON",
 /*  22 */ "odecl_list ::= odecl",
 /*  23 */ "odecl_list ::= odecl_list odecl",
 /*  24 */ "expr_call ::= proto expr_paren",
 /*  25 */ "expr_call ::= expr_call expr_paren",
 /*  26 */ "expr_call ::= expr_postfix expr_paren",
 /*  27 */ "expr_postfix ::= LPN expr_assign RPN",
 /*  28 */ "expr_postfix ::= name PERIOD LSQ expr_value RSQ",
 /*  29 */ "expr_postfix ::= name LSQ expr_value RSQ",
 /*  30 */ "expr_postfix ::= proto PERIOD IDENTIFIER",
 /*  31 */ "expr_postfix ::= proto PERIOD LSQ expr_value RSQ",
 /*  32 */ "expr_postfix ::= proto LSQ expr_value RSQ",
 /*  33 */ "expr_postfix ::= expr_call PERIOD IDENTIFIER",
 /*  34 */ "expr_postfix ::= expr_call PERIOD LSQ expr_value RSQ",
 /*  35 */ "expr_postfix ::= expr_call LSQ expr_value RSQ",
 /*  36 */ "expr_postfix ::= expr_postfix PERIOD IDENTIFIER",
 /*  37 */ "expr_postfix ::= expr_postfix PERIOD LSQ expr_value RSQ",
 /*  38 */ "expr_postfix ::= expr_postfix LSQ expr_value RSQ",
 /*  39 */ "expr_simple ::= name",
 /*  40 */ "expr_simple ::= expr_postfix",
 /*  41 */ "expr_simple ::= proto",
 /*  42 */ "expr_simple ::= expr_call",
 /*  43 */ "expr_basic ::= expr_simple",
 /*  44 */ "expr_basic ::= NUMBER",
 /*  45 */ "expr_basic ::= STRING",
 /*  46 */ "expr_basic ::= TRUE",
 /*  47 */ "expr_basic ::= FALSE",
 /*  48 */ "expr_basic ::= NULL",
 /*  49 */ "expr_unary ::= expr_basic",
 /*  50 */ "expr_unary ::= PLUS expr_basic",
 /*  51 */ "expr_unary ::= MINUS expr_basic",
 /*  52 */ "expr_unary ::= XMARK expr_basic",
 /*  53 */ "expr_unary ::= TILDE expr_basic",
 /*  54 */ "expr_mul ::= expr_unary",
 /*  55 */ "expr_mul ::= expr_mul ASTERISK expr_unary",
 /*  56 */ "expr_mul ::= expr_mul SOLIDUS expr_unary",
 /*  57 */ "expr_mul ::= expr_mul PERCENT expr_unary",
 /*  58 */ "expr_mul ::= expr_mul TILDE expr_unary",
 /*  59 */ "expr_add ::= expr_mul",
 /*  60 */ "expr_add ::= expr_add PLUS expr_mul",
 /*  61 */ "expr_add ::= expr_add MINUS expr_mul",
 /*  62 */ "expr_shift ::= expr_add",
 /*  63 */ "expr_shift ::= expr_shift LSHIFT expr_add",
 /*  64 */ "expr_shift ::= expr_shift RSHIFT expr_add",
 /*  65 */ "expr_shift ::= expr_shift URSHIFT expr_add",
 /*  66 */ "expr_bitand ::= expr_shift",
 /*  67 */ "expr_bitand ::= expr_bitand AMPERSAND expr_shift",
 /*  68 */ "expr_bitxor ::= expr_bitand",
 /*  69 */ "expr_bitxor ::= expr_bitxor CARET expr_bitand",
 /*  70 */ "expr_bitor ::= expr_bitxor",
 /*  71 */ "expr_bitor ::= expr_bitor VBAR expr_bitxor",
 /*  72 */ "expr_compare ::= expr_bitor",
 /*  73 */ "expr_compare ::= expr_compare EQUAL expr_bitor",
 /*  74 */ "expr_compare ::= expr_compare NOTEQUAL expr_bitor",
 /*  75 */ "expr_compare ::= expr_compare LESS expr_bitor",
 /*  76 */ "expr_compare ::= expr_compare GREATER expr_bitor",
 /*  77 */ "expr_compare ::= expr_compare LESSEQUAL expr_bitor",
 /*  78 */ "expr_compare ::= expr_compare GREATEREQUAL expr_bitor",
 /*  79 */ "expr_compare ::= expr_compare IN expr_bitor",
 /*  80 */ "expr_compare ::= expr_compare NOTIN expr_bitor",
 /*  81 */ "expr_compare ::= expr_compare IS expr_bitor",
 /*  82 */ "expr_compare ::= expr_compare NOTIS expr_bitor",
 /*  83 */ "expr_and ::= expr_compare",
 /*  84 */ "expr_and ::= expr_and LOGICAND expr_compare",
 /*  85 */ "expr_xor ::= expr_and",
 /*  86 */ "expr_xor ::= expr_xor LOGICXOR expr_and",
 /*  87 */ "expr_or ::= expr_xor",
 /*  88 */ "expr_or ::= expr_or LOGICOR expr_xor",
 /*  89 */ "expr_nolbr ::= expr_or",
 /*  90 */ "expr_nolbr ::= expr_or QMARK expr_value COLON expr_value",
 /*  91 */ "expr_nolbr ::= NEW name expr_paren",
 /*  92 */ "expr_nolbr ::= NEW expr_postfix expr_paren",
 /*  93 */ "expr_nolbr ::= LSQ RSQ",
 /*  94 */ "expr_nolbr ::= LSQ value_list RSQ",
 /*  95 */ "expr_nolbr ::= COLON odecl_brace",
 /*  96 */ "expr_nolbr ::= COLON expr_simple odecl_brace",
 /*  97 */ "expr_nolbr ::= QMARK expr_paren stmt_brace",
 /*  98 */ "expr_nolbr ::= PERIOD QMARK expr_paren stmt_brace",
 /*  99 */ "expr_nolbr ::= proto YIELD",
 /* 100 */ "expr_nolbr ::= expr_call YIELD",
 /* 101 */ "expr_value ::= expr_nolbr",
 /* 102 */ "expr_value ::= LBR RBR",
 /* 103 */ "expr_value ::= LBR keyval_list RBR",
 /* 104 */ "expr_lbody ::= expr_value",
 /* 105 */ "expr_lbody ::= expr_lbody COMMA expr_value",
 /* 106 */ "expr_final ::= expr_postfix ELLIPSIS",
 /* 107 */ "expr_final ::= expr_postfix LSQ RSQ ELLIPSIS",
 /* 108 */ "expr_final ::= ELLIPSIS",
 /* 109 */ "expr_list ::= expr_final",
 /* 110 */ "expr_list ::= expr_lbody",
 /* 111 */ "expr_list ::= expr_lbody COMMA expr_final",
 /* 112 */ "expr_assign ::= expr_list",
 /* 113 */ "expr_assign ::= expr_lbody assign_op expr_list",
 /* 114 */ "assign_op ::= ASSIGN",
 /* 115 */ "assign_op ::= MULASSIGN",
 /* 116 */ "assign_op ::= DIVASSIGN",
 /* 117 */ "assign_op ::= MODASSIGN",
 /* 118 */ "assign_op ::= INTDIVASSIGN",
 /* 119 */ "assign_op ::= ADDASSIGN",
 /* 120 */ "assign_op ::= SUBASSIGN",
 /* 121 */ "assign_op ::= LSHIFTASSIGN",
 /* 122 */ "assign_op ::= RSHIFTASSIGN",
 /* 123 */ "assign_op ::= URSHIFTASSIGN",
 /* 124 */ "assign_op ::= BITANDASSIGN",
 /* 125 */ "assign_op ::= BITXORASSIGN",
 /* 126 */ "assign_op ::= BITORASSIGN",
 /* 127 */ "value_list ::= expr_final",
 /* 128 */ "value_list ::= expr_final COMMA",
 /* 129 */ "value_list ::= expr_lbody",
 /* 130 */ "value_list ::= expr_lbody COMMA",
 /* 131 */ "value_list ::= expr_lbody COMMA expr_final",
 /* 132 */ "value_list ::= expr_lbody COMMA expr_final COMMA",
 /* 133 */ "keyval_lbody ::= expr_value COLON expr_value",
 /* 134 */ "keyval_lbody ::= keyval_lbody COMMA expr_value COLON expr_value",
 /* 135 */ "keyval_list ::= keyval_lbody",
 /* 136 */ "keyval_list ::= keyval_lbody COMMA",
 /* 137 */ "sexpr_lbody ::= expr_nolbr",
 /* 138 */ "sexpr_lbody ::= sexpr_lbody COMMA expr_value",
 /* 139 */ "sexpr_list ::= expr_final",
 /* 140 */ "sexpr_list ::= sexpr_lbody",
 /* 141 */ "sexpr_list ::= sexpr_lbody COMMA expr_final",
 /* 142 */ "sexpr_assign ::= sexpr_list",
 /* 143 */ "sexpr_assign ::= sexpr_lbody assign_op expr_list",
 /* 144 */ "stmt ::= stmt_brace",
 /* 145 */ "stmt ::= sexpr_assign SEMICOLON",
 /* 146 */ "stmt_list ::= stmt",
 /* 147 */ "stmt_list ::= decl",
 /* 148 */ "stmt_list ::= SEMICOLON",
 /* 149 */ "stmt_list ::= stmt_list stmt",
 /* 150 */ "stmt_list ::= stmt_list decl",
 /* 151 */ "stmt_list ::= stmt_list SEMICOLON",
 /* 152 */ "tokens ::= BREAK CASE CATCH CONTINUE DEFAULT DO ELSE FALSE FINALLY FOR IF NEW NULL RETURN SWITCH THROW TRUE TRY USING VAR WHILE YIELD IDENTIFIER NUMBER STRING XMARK PERCENT AMPERSAND LPN RPN ASTERISK PLUS COMMA MINUS PERIOD SOLIDUS COLON SEMICOLON LESS ASSIGN GREATER QMARK LSQ RSQ CARET LBR VBAR RBR TILDE INCREMENT DECREMENT LSHIFT RSHIFT URSHIFT NOTEQUAL LESSEQUAL EQUAL GREATEREQUAL MODASSIGN BITANDASSIGN MULASSIGN ADDASSIGN SUBASSIGN DIVASSIGN BITXORASSIGN BITORASSIGN INTDIVASSIGN LSHIFTASSIGN RSHIFTASSIGN URSHIFTASSIGN LOGICAND LOGICXOR LOGICOR ELLIPSIS",
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
  { 80, 0 },
  { 80, 1 },
  { 82, 2 },
  { 82, 3 },
  { 84, 2 },
  { 84, 3 },
  { 85, 2 },
  { 85, 3 },
  { 87, 1 },
  { 87, 3 },
  { 88, 1 },
  { 88, 3 },
  { 89, 2 },
  { 90, 2 },
  { 90, 4 },
  { 90, 2 },
  { 90, 3 },
  { 90, 3 },
  { 90, 4 },
  { 92, 1 },
  { 92, 1 },
  { 92, 4 },
  { 86, 1 },
  { 86, 2 },
  { 93, 2 },
  { 93, 2 },
  { 93, 2 },
  { 94, 3 },
  { 94, 5 },
  { 94, 4 },
  { 94, 3 },
  { 94, 5 },
  { 94, 4 },
  { 94, 3 },
  { 94, 5 },
  { 94, 4 },
  { 94, 3 },
  { 94, 5 },
  { 94, 4 },
  { 91, 1 },
  { 91, 1 },
  { 91, 1 },
  { 91, 1 },
  { 97, 1 },
  { 97, 1 },
  { 97, 1 },
  { 97, 1 },
  { 97, 1 },
  { 97, 1 },
  { 98, 1 },
  { 98, 2 },
  { 98, 2 },
  { 98, 2 },
  { 98, 2 },
  { 99, 1 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 99, 3 },
  { 100, 1 },
  { 100, 3 },
  { 100, 3 },
  { 101, 1 },
  { 101, 3 },
  { 101, 3 },
  { 101, 3 },
  { 102, 1 },
  { 102, 3 },
  { 103, 1 },
  { 103, 3 },
  { 104, 1 },
  { 104, 3 },
  { 105, 1 },
  { 105, 3 },
  { 105, 3 },
  { 105, 3 },
  { 105, 3 },
  { 105, 3 },
  { 105, 3 },
  { 105, 3 },
  { 105, 3 },
  { 105, 3 },
  { 105, 3 },
  { 106, 1 },
  { 106, 3 },
  { 107, 1 },
  { 107, 3 },
  { 108, 1 },
  { 108, 3 },
  { 109, 1 },
  { 109, 5 },
  { 109, 3 },
  { 109, 3 },
  { 109, 2 },
  { 109, 3 },
  { 109, 2 },
  { 109, 3 },
  { 109, 3 },
  { 109, 4 },
  { 109, 2 },
  { 109, 2 },
  { 96, 1 },
  { 96, 2 },
  { 96, 3 },
  { 112, 1 },
  { 112, 3 },
  { 113, 2 },
  { 113, 4 },
  { 113, 1 },
  { 83, 1 },
  { 83, 1 },
  { 83, 3 },
  { 95, 1 },
  { 95, 3 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 114, 1 },
  { 110, 1 },
  { 110, 2 },
  { 110, 1 },
  { 110, 2 },
  { 110, 3 },
  { 110, 4 },
  { 115, 3 },
  { 115, 5 },
  { 111, 1 },
  { 111, 2 },
  { 116, 1 },
  { 116, 3 },
  { 117, 1 },
  { 117, 1 },
  { 117, 3 },
  { 118, 1 },
  { 118, 3 },
  { 119, 1 },
  { 119, 2 },
  { 81, 1 },
  { 81, 1 },
  { 81, 1 },
  { 81, 2 },
  { 81, 2 },
  { 81, 2 },
  { 120, 74 },
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
      /* (0) script ::= */ yytestcase(yyruleno==0);
      /* (1) script ::= stmt_list */ yytestcase(yyruleno==1);
      /* (2) expr_paren ::= LPN RPN */ yytestcase(yyruleno==2);
      /* (3) expr_paren ::= LPN expr_list RPN */ yytestcase(yyruleno==3);
      /* (4) stmt_brace ::= LBR RBR */ yytestcase(yyruleno==4);
      /* (5) stmt_brace ::= LBR stmt_list RBR */ yytestcase(yyruleno==5);
      /* (6) odecl_brace ::= LBR RBR */ yytestcase(yyruleno==6);
      /* (7) odecl_brace ::= LBR odecl_list RBR */ yytestcase(yyruleno==7);
      /* (8) name ::= IDENTIFIER */ yytestcase(yyruleno==8);
      /* (9) name ::= name PERIOD IDENTIFIER */ yytestcase(yyruleno==9);
      /* (10) name_list ::= name */ yytestcase(yyruleno==10);
      /* (11) name_list ::= name_list COMMA name */ yytestcase(yyruleno==11);
      /* (12) proto ::= name expr_paren */ yytestcase(yyruleno==12);
      /* (13) decl ::= name odecl_brace */ yytestcase(yyruleno==13);
      /* (14) decl ::= name COLON expr_simple odecl_brace */ yytestcase(yyruleno==14);
      /* (15) decl ::= proto stmt_brace */ yytestcase(yyruleno==15);
      /* (16) decl ::= proto YIELD stmt_brace */ yytestcase(yyruleno==16);
      /* (17) decl ::= PERIOD proto stmt_brace */ yytestcase(yyruleno==17);
      /* (18) decl ::= PERIOD proto YIELD stmt_brace */ yytestcase(yyruleno==18);
      /* (19) odecl ::= SEMICOLON */ yytestcase(yyruleno==19);
      /* (20) odecl ::= decl */ yytestcase(yyruleno==20);
      /* (21) odecl ::= name_list ASSIGN expr_list SEMICOLON */ yytestcase(yyruleno==21);
      /* (22) odecl_list ::= odecl */ yytestcase(yyruleno==22);
      /* (23) odecl_list ::= odecl_list odecl */ yytestcase(yyruleno==23);
      /* (24) expr_call ::= proto expr_paren */ yytestcase(yyruleno==24);
      /* (25) expr_call ::= expr_call expr_paren */ yytestcase(yyruleno==25);
      /* (26) expr_call ::= expr_postfix expr_paren */ yytestcase(yyruleno==26);
      /* (27) expr_postfix ::= LPN expr_assign RPN */ yytestcase(yyruleno==27);
      /* (28) expr_postfix ::= name PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==28);
      /* (29) expr_postfix ::= name LSQ expr_value RSQ */ yytestcase(yyruleno==29);
      /* (30) expr_postfix ::= proto PERIOD IDENTIFIER */ yytestcase(yyruleno==30);
      /* (31) expr_postfix ::= proto PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==31);
      /* (32) expr_postfix ::= proto LSQ expr_value RSQ */ yytestcase(yyruleno==32);
      /* (33) expr_postfix ::= expr_call PERIOD IDENTIFIER */ yytestcase(yyruleno==33);
      /* (34) expr_postfix ::= expr_call PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==34);
      /* (35) expr_postfix ::= expr_call LSQ expr_value RSQ */ yytestcase(yyruleno==35);
      /* (36) expr_postfix ::= expr_postfix PERIOD IDENTIFIER */ yytestcase(yyruleno==36);
      /* (37) expr_postfix ::= expr_postfix PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==37);
      /* (38) expr_postfix ::= expr_postfix LSQ expr_value RSQ */ yytestcase(yyruleno==38);
      /* (39) expr_simple ::= name */ yytestcase(yyruleno==39);
      /* (40) expr_simple ::= expr_postfix */ yytestcase(yyruleno==40);
      /* (41) expr_simple ::= proto */ yytestcase(yyruleno==41);
      /* (42) expr_simple ::= expr_call */ yytestcase(yyruleno==42);
      /* (43) expr_basic ::= expr_simple */ yytestcase(yyruleno==43);
      /* (44) expr_basic ::= NUMBER */ yytestcase(yyruleno==44);
      /* (45) expr_basic ::= STRING */ yytestcase(yyruleno==45);
      /* (46) expr_basic ::= TRUE */ yytestcase(yyruleno==46);
      /* (47) expr_basic ::= FALSE */ yytestcase(yyruleno==47);
      /* (48) expr_basic ::= NULL */ yytestcase(yyruleno==48);
      /* (49) expr_unary ::= expr_basic */ yytestcase(yyruleno==49);
      /* (50) expr_unary ::= PLUS expr_basic */ yytestcase(yyruleno==50);
      /* (51) expr_unary ::= MINUS expr_basic */ yytestcase(yyruleno==51);
      /* (52) expr_unary ::= XMARK expr_basic */ yytestcase(yyruleno==52);
      /* (53) expr_unary ::= TILDE expr_basic */ yytestcase(yyruleno==53);
      /* (54) expr_mul ::= expr_unary */ yytestcase(yyruleno==54);
      /* (55) expr_mul ::= expr_mul ASTERISK expr_unary */ yytestcase(yyruleno==55);
      /* (56) expr_mul ::= expr_mul SOLIDUS expr_unary */ yytestcase(yyruleno==56);
      /* (57) expr_mul ::= expr_mul PERCENT expr_unary */ yytestcase(yyruleno==57);
      /* (58) expr_mul ::= expr_mul TILDE expr_unary */ yytestcase(yyruleno==58);
      /* (59) expr_add ::= expr_mul */ yytestcase(yyruleno==59);
      /* (60) expr_add ::= expr_add PLUS expr_mul */ yytestcase(yyruleno==60);
      /* (61) expr_add ::= expr_add MINUS expr_mul */ yytestcase(yyruleno==61);
      /* (62) expr_shift ::= expr_add */ yytestcase(yyruleno==62);
      /* (63) expr_shift ::= expr_shift LSHIFT expr_add */ yytestcase(yyruleno==63);
      /* (64) expr_shift ::= expr_shift RSHIFT expr_add */ yytestcase(yyruleno==64);
      /* (65) expr_shift ::= expr_shift URSHIFT expr_add */ yytestcase(yyruleno==65);
      /* (66) expr_bitand ::= expr_shift */ yytestcase(yyruleno==66);
      /* (67) expr_bitand ::= expr_bitand AMPERSAND expr_shift */ yytestcase(yyruleno==67);
      /* (68) expr_bitxor ::= expr_bitand */ yytestcase(yyruleno==68);
      /* (69) expr_bitxor ::= expr_bitxor CARET expr_bitand */ yytestcase(yyruleno==69);
      /* (70) expr_bitor ::= expr_bitxor */ yytestcase(yyruleno==70);
      /* (71) expr_bitor ::= expr_bitor VBAR expr_bitxor */ yytestcase(yyruleno==71);
      /* (72) expr_compare ::= expr_bitor */ yytestcase(yyruleno==72);
      /* (73) expr_compare ::= expr_compare EQUAL expr_bitor */ yytestcase(yyruleno==73);
      /* (74) expr_compare ::= expr_compare NOTEQUAL expr_bitor */ yytestcase(yyruleno==74);
      /* (75) expr_compare ::= expr_compare LESS expr_bitor */ yytestcase(yyruleno==75);
      /* (76) expr_compare ::= expr_compare GREATER expr_bitor */ yytestcase(yyruleno==76);
      /* (77) expr_compare ::= expr_compare LESSEQUAL expr_bitor */ yytestcase(yyruleno==77);
      /* (78) expr_compare ::= expr_compare GREATEREQUAL expr_bitor */ yytestcase(yyruleno==78);
      /* (79) expr_compare ::= expr_compare IN expr_bitor */ yytestcase(yyruleno==79);
      /* (80) expr_compare ::= expr_compare NOTIN expr_bitor */ yytestcase(yyruleno==80);
      /* (81) expr_compare ::= expr_compare IS expr_bitor */ yytestcase(yyruleno==81);
      /* (82) expr_compare ::= expr_compare NOTIS expr_bitor */ yytestcase(yyruleno==82);
      /* (83) expr_and ::= expr_compare */ yytestcase(yyruleno==83);
      /* (84) expr_and ::= expr_and LOGICAND expr_compare */ yytestcase(yyruleno==84);
      /* (85) expr_xor ::= expr_and */ yytestcase(yyruleno==85);
      /* (86) expr_xor ::= expr_xor LOGICXOR expr_and */ yytestcase(yyruleno==86);
      /* (87) expr_or ::= expr_xor */ yytestcase(yyruleno==87);
      /* (88) expr_or ::= expr_or LOGICOR expr_xor */ yytestcase(yyruleno==88);
      /* (89) expr_nolbr ::= expr_or */ yytestcase(yyruleno==89);
      /* (90) expr_nolbr ::= expr_or QMARK expr_value COLON expr_value */ yytestcase(yyruleno==90);
      /* (91) expr_nolbr ::= NEW name expr_paren */ yytestcase(yyruleno==91);
      /* (92) expr_nolbr ::= NEW expr_postfix expr_paren */ yytestcase(yyruleno==92);
      /* (93) expr_nolbr ::= LSQ RSQ */ yytestcase(yyruleno==93);
      /* (94) expr_nolbr ::= LSQ value_list RSQ */ yytestcase(yyruleno==94);
      /* (95) expr_nolbr ::= COLON odecl_brace */ yytestcase(yyruleno==95);
      /* (96) expr_nolbr ::= COLON expr_simple odecl_brace */ yytestcase(yyruleno==96);
      /* (97) expr_nolbr ::= QMARK expr_paren stmt_brace */ yytestcase(yyruleno==97);
      /* (98) expr_nolbr ::= PERIOD QMARK expr_paren stmt_brace */ yytestcase(yyruleno==98);
      /* (99) expr_nolbr ::= proto YIELD */ yytestcase(yyruleno==99);
      /* (100) expr_nolbr ::= expr_call YIELD */ yytestcase(yyruleno==100);
      /* (101) expr_value ::= expr_nolbr */ yytestcase(yyruleno==101);
      /* (102) expr_value ::= LBR RBR */ yytestcase(yyruleno==102);
      /* (103) expr_value ::= LBR keyval_list RBR */ yytestcase(yyruleno==103);
      /* (104) expr_lbody ::= expr_value */ yytestcase(yyruleno==104);
      /* (105) expr_lbody ::= expr_lbody COMMA expr_value */ yytestcase(yyruleno==105);
      /* (106) expr_final ::= expr_postfix ELLIPSIS */ yytestcase(yyruleno==106);
      /* (107) expr_final ::= expr_postfix LSQ RSQ ELLIPSIS */ yytestcase(yyruleno==107);
      /* (108) expr_final ::= ELLIPSIS */ yytestcase(yyruleno==108);
      /* (109) expr_list ::= expr_final */ yytestcase(yyruleno==109);
      /* (110) expr_list ::= expr_lbody */ yytestcase(yyruleno==110);
      /* (111) expr_list ::= expr_lbody COMMA expr_final */ yytestcase(yyruleno==111);
      /* (112) expr_assign ::= expr_list */ yytestcase(yyruleno==112);
      /* (113) expr_assign ::= expr_lbody assign_op expr_list */ yytestcase(yyruleno==113);
      /* (114) assign_op ::= ASSIGN */ yytestcase(yyruleno==114);
      /* (115) assign_op ::= MULASSIGN */ yytestcase(yyruleno==115);
      /* (116) assign_op ::= DIVASSIGN */ yytestcase(yyruleno==116);
      /* (117) assign_op ::= MODASSIGN */ yytestcase(yyruleno==117);
      /* (118) assign_op ::= INTDIVASSIGN */ yytestcase(yyruleno==118);
      /* (119) assign_op ::= ADDASSIGN */ yytestcase(yyruleno==119);
      /* (120) assign_op ::= SUBASSIGN */ yytestcase(yyruleno==120);
      /* (121) assign_op ::= LSHIFTASSIGN */ yytestcase(yyruleno==121);
      /* (122) assign_op ::= RSHIFTASSIGN */ yytestcase(yyruleno==122);
      /* (123) assign_op ::= URSHIFTASSIGN */ yytestcase(yyruleno==123);
      /* (124) assign_op ::= BITANDASSIGN */ yytestcase(yyruleno==124);
      /* (125) assign_op ::= BITXORASSIGN */ yytestcase(yyruleno==125);
      /* (126) assign_op ::= BITORASSIGN */ yytestcase(yyruleno==126);
      /* (127) value_list ::= expr_final */ yytestcase(yyruleno==127);
      /* (128) value_list ::= expr_final COMMA */ yytestcase(yyruleno==128);
      /* (129) value_list ::= expr_lbody */ yytestcase(yyruleno==129);
      /* (130) value_list ::= expr_lbody COMMA */ yytestcase(yyruleno==130);
      /* (131) value_list ::= expr_lbody COMMA expr_final */ yytestcase(yyruleno==131);
      /* (132) value_list ::= expr_lbody COMMA expr_final COMMA */ yytestcase(yyruleno==132);
      /* (133) keyval_lbody ::= expr_value COLON expr_value */ yytestcase(yyruleno==133);
      /* (134) keyval_lbody ::= keyval_lbody COMMA expr_value COLON expr_value */ yytestcase(yyruleno==134);
      /* (135) keyval_list ::= keyval_lbody */ yytestcase(yyruleno==135);
      /* (136) keyval_list ::= keyval_lbody COMMA */ yytestcase(yyruleno==136);
      /* (137) sexpr_lbody ::= expr_nolbr */ yytestcase(yyruleno==137);
      /* (138) sexpr_lbody ::= sexpr_lbody COMMA expr_value */ yytestcase(yyruleno==138);
      /* (139) sexpr_list ::= expr_final */ yytestcase(yyruleno==139);
      /* (140) sexpr_list ::= sexpr_lbody */ yytestcase(yyruleno==140);
      /* (141) sexpr_list ::= sexpr_lbody COMMA expr_final */ yytestcase(yyruleno==141);
      /* (142) sexpr_assign ::= sexpr_list */ yytestcase(yyruleno==142);
      /* (143) sexpr_assign ::= sexpr_lbody assign_op expr_list */ yytestcase(yyruleno==143);
      /* (144) stmt ::= stmt_brace */ yytestcase(yyruleno==144);
      /* (145) stmt ::= sexpr_assign SEMICOLON */ yytestcase(yyruleno==145);
      /* (146) stmt_list ::= stmt */ yytestcase(yyruleno==146);
      /* (147) stmt_list ::= decl */ yytestcase(yyruleno==147);
      /* (148) stmt_list ::= SEMICOLON */ yytestcase(yyruleno==148);
      /* (149) stmt_list ::= stmt_list stmt */ yytestcase(yyruleno==149);
      /* (150) stmt_list ::= stmt_list decl */ yytestcase(yyruleno==150);
      /* (151) stmt_list ::= stmt_list SEMICOLON */ yytestcase(yyruleno==151);
      /* (152) tokens ::= BREAK CASE CATCH CONTINUE DEFAULT DO ELSE FALSE FINALLY FOR IF NEW NULL RETURN SWITCH THROW TRUE TRY USING VAR WHILE YIELD IDENTIFIER NUMBER STRING XMARK PERCENT AMPERSAND LPN RPN ASTERISK PLUS COMMA MINUS PERIOD SOLIDUS COLON SEMICOLON LESS ASSIGN GREATER QMARK LSQ RSQ CARET LBR VBAR RBR TILDE INCREMENT DECREMENT LSHIFT RSHIFT URSHIFT NOTEQUAL LESSEQUAL EQUAL GREATEREQUAL MODASSIGN BITANDASSIGN MULASSIGN ADDASSIGN SUBASSIGN DIVASSIGN BITXORASSIGN BITORASSIGN INTDIVASSIGN LSHIFTASSIGN RSHIFTASSIGN URSHIFTASSIGN LOGICAND LOGICXOR LOGICOR ELLIPSIS */ yytestcase(yyruleno==152);
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
