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
#define YYNOCODE 92
#define YYACTIONTYPE unsigned char
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
#define YYNSTATE 146
#define YYNRULE 58
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
 /*     0 */   107,   24,    1,   92,   22,   33,    3,  107,   59,    1,
 /*    10 */   117,  141,  136,  135,  133,   14,   16,   15,   13,   17,
 /*    20 */    74,  103,  107,   29,    1,  126,   95,  128,    3,   72,
 /*    30 */    70,   69,  117,  141,  136,  135,  133,   14,   16,   15,
 /*    40 */    13,   17,   74,  103,  107,  130,    1,  139,   35,   96,
 /*    50 */     3,   93,   63,   11,  117,  141,  136,  135,  133,   14,
 /*    60 */    16,   15,   13,   17,   74,  107,  107,    1,   56,   64,
 /*    70 */    91,    3,   65,   75,   79,  117,  141,  136,  135,  133,
 /*    80 */    14,   16,   15,   13,   17,   74,  107,  100,    1,   62,
 /*    90 */    89,   94,    3,   88,   86,   10,  117,  141,  136,  135,
 /*   100 */   133,   14,   16,   15,   13,   17,   74,   52,   20,  205,
 /*   110 */    77,   61,   37,   38,    8,  113,  102,  140,   41,   39,
 /*   120 */    78,  129,  119,    5,  104,    6,   47,   40,   76,  107,
 /*   130 */   102,    1,   41,   39,   80,  129,  119,   73,   32,  117,
 /*   140 */   141,  136,  135,  133,   47,   40,  146,  147,   68,  124,
 /*   150 */    41,   46,  125,  129,  119,   47,   40,  142,   51,   81,
 /*   160 */   123,   41,   46,  115,  129,  119,   47,   40,  105,  137,
 /*   170 */    66,   18,   41,   46,   43,  129,  119,   47,   40,  127,
 /*   180 */   131,   71,   34,   41,   46,   55,  129,  119,  138,   47,
 /*   190 */    40,   60,  143,   87,   98,   41,   46,  101,  129,  119,
 /*   200 */   106,   47,   40,  120,  108,   83,  111,   41,   46,  114,
 /*   210 */   129,  119,   47,   40,  116,   30,   82,  134,   41,   46,
 /*   220 */   145,  129,  119,   47,   40,   26,  100,   67,  110,   41,
 /*   230 */    46,   57,  129,  119,   47,   40,  118,   21,   84,  132,
 /*   240 */    41,   46,  206,  129,  119,   47,   40,  206,  206,   85,
 /*   250 */   206,   41,   46,  206,  129,  119,   47,   45,  206,  206,
 /*   260 */   206,  206,   50,   46,  206,   97,   53,   23,  206,   27,
 /*   270 */    47,   45,   90,    7,  206,  206,   50,   46,  206,  121,
 /*   280 */   206,   47,   45,  206,  206,   47,   45,   50,   46,  206,
 /*   290 */   122,   50,   46,  206,  144,   52,   36,  206,   31,  206,
 /*   300 */     4,  206,    8,  206,   58,   25,  206,   44,   42,  206,
 /*   310 */   109,   12,  206,   49,   48,   54,   28,   53,   23,  150,
 /*   320 */   150,  206,    2,  112,    7,  206,  150,   53,   23,  206,
 /*   330 */    58,   25,   54,   28,    7,   99,  206,   12,  206,    9,
 /*   340 */   206,   52,   36,   54,   19,  159,  159,  206,    8,  206,
 /*   350 */     9,  206,  159,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     2,    3,    4,    3,    4,    6,    8,    2,    9,    4,
 /*    10 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*    20 */    22,   23,    2,    6,    4,   24,    9,   24,    8,   82,
 /*    30 */    81,   81,   12,   13,   14,   15,   16,   17,   18,   19,
 /*    40 */    20,   21,   22,   23,    2,    2,    4,   24,    6,   84,
 /*    50 */     8,   81,   81,   10,   12,   13,   14,   15,   16,   17,
 /*    60 */    18,   19,   20,   21,   22,    2,    2,    4,   79,   81,
 /*    70 */    84,    8,   81,   81,   11,   12,   13,   14,   15,   16,
 /*    80 */    17,   18,   19,   20,   21,   22,    2,    2,    4,   81,
 /*    90 */    84,   84,    8,   82,   82,   10,   12,   13,   14,   15,
 /*   100 */    16,   17,   18,   19,   20,   21,   22,    3,    4,   76,
 /*   110 */    77,   78,   79,   80,   10,    2,   83,    2,   85,   86,
 /*   120 */    84,   88,   89,   10,   77,   10,   79,   80,   84,    2,
 /*   130 */    83,    4,   85,   86,   87,   88,   89,   82,    6,   12,
 /*   140 */    13,   14,   15,   16,   79,   80,    0,    0,   83,   11,
 /*   150 */    85,   86,   11,   88,   89,   79,   80,    5,    5,   83,
 /*   160 */    11,   85,   86,    5,   88,   89,   79,   80,    5,    5,
 /*   170 */    83,    6,   85,   86,    5,   88,   89,   79,   80,    7,
 /*   180 */     7,   83,    4,   85,   86,    5,   88,   89,    7,   79,
 /*   190 */    80,    1,    7,   83,   23,   85,   86,    5,   88,   89,
 /*   200 */    11,   79,   80,    7,   11,   83,   11,   85,   86,   11,
 /*   210 */    88,   89,   79,   80,   11,    6,   83,    7,   85,   86,
 /*   220 */     7,   88,   89,   79,   80,    6,    2,   83,    7,   85,
 /*   230 */    86,    5,   88,   89,   79,   80,    7,    6,   83,    7,
 /*   240 */    85,   86,   91,   88,   89,   79,   80,   91,   91,   83,
 /*   250 */    91,   85,   86,   91,   88,   89,   79,   80,   91,   91,
 /*   260 */    91,   91,   85,   86,   91,   88,    3,    4,   91,    6,
 /*   270 */    79,   80,    9,   10,   91,   91,   85,   86,   91,   88,
 /*   280 */    91,   79,   80,   91,   91,   79,   80,   85,   86,   91,
 /*   290 */    88,   85,   86,   91,   88,    3,    4,   91,    6,   91,
 /*   300 */     8,   91,   10,   91,    3,    4,   91,   79,   80,   91,
 /*   310 */     9,   10,   91,   85,   86,    3,    4,    3,    4,    3,
 /*   320 */     4,   91,   10,    9,   10,   91,   10,    3,    4,   91,
 /*   330 */     3,    4,    3,    4,   10,   23,   91,   10,   91,   10,
 /*   340 */    91,    3,    4,    3,    4,    3,    4,   91,   10,   91,
 /*   350 */    10,   91,   10,
};
#define YY_SHIFT_USE_DFLT (-3)
#define YY_SHIFT_MAX 96
static const short yy_shift_ofst[] = {
 /*     0 */    -2,   20,   63,   42,   84,   84,   84,   84,   84,   84,
 /*    10 */    84,   84,   84,  127,  127,  127,  127,    5,    3,    1,
 /*    20 */     1,   23,    1,    1,   64,    1,   23,   23,    1,   23,
 /*    30 */     3,    3,   23,   23,    1,    3,    1,  292,  263,  312,
 /*    40 */   314,  301,  324,  342,  104,  324,  329,  338,  340,  327,
 /*    50 */   327,  316,   85,  113,   43,   -1,    0,   17,  115,  132,
 /*    60 */   146,  147,  152,  158,  163,  164,  138,  141,  149,  153,
 /*    70 */   169,  165,  172,  173,  178,  180,  181,  190,  185,  171,
 /*    80 */   192,  189,  193,  195,  198,  203,  196,  209,  210,  213,
 /*    90 */   219,  221,  224,  226,  229,  231,  232,
};
#define YY_REDUCE_USE_DFLT (-54)
#define YY_REDUCE_MAX 36
static const short yy_reduce_ofst[] = {
 /*     0 */    33,   47,   87,   98,  110,  122,  133,  144,  155,   87,
 /*    10 */   166,   76,   65,  191,  177,  202,  206,  228,  -53,  -51,
 /*    20 */   -50,  -35,  -30,  -29,  -11,  -12,  -14,    6,   -9,    7,
 /*    30 */    11,   12,   36,   44,   -8,   55,    8,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   204,  204,  204,  204,  204,  204,  204,  204,  204,  204,
 /*    10 */   204,  204,  204,  204,  204,  204,  204,  204,  204,  204,
 /*    20 */   204,  204,  204,  204,  204,  204,  204,  204,  204,  204,
 /*    30 */   204,  204,  204,  204,  204,  204,  204,  172,  174,  173,
 /*    40 */   174,  175,  204,  188,  204,  174,  173,  172,  204,  204,
 /*    50 */   175,  187,  204,  204,  204,  204,  204,  204,  204,  204,
 /*    60 */   204,  204,  204,  204,  204,  204,  204,  204,  204,  204,
 /*    70 */   204,  204,  204,  204,  204,  204,  204,  204,  204,  204,
 /*    80 */   204,  204,  204,  204,  204,  204,  204,  204,  204,  204,
 /*    90 */   193,  204,  204,  204,  204,  204,  204,  182,  197,  196,
 /*   100 */   149,  160,  195,  198,  199,  158,  170,  148,  167,  194,
 /*   110 */   154,  164,  193,  163,  162,  157,  161,  176,  155,  186,
 /*   120 */   151,  185,  184,  168,  171,  165,  200,  189,  201,  181,
 /*   130 */   169,  190,  156,  180,  152,  179,  178,  159,  191,  202,
 /*   140 */   166,  177,  150,  192,  183,  153,
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
  "TILDE",         "NEW",           "QMARK",         "ELLIPSIS",    
  "COMMA",         "BREAK",         "CASE",          "CATCH",       
  "CONTINUE",      "DEFAULT",       "DO",            "ELSE",        
  "FINALLY",       "FOR",           "IF",            "RETURN",      
  "SWITCH",        "THROW",         "TRY",           "USING",       
  "VAR",           "WHILE",         "PERCENT",       "AMPERSAND",   
  "ASTERISK",      "SOLIDUS",       "LESS",          "ASSIGN",      
  "GREATER",       "CARET",         "VBAR",          "INCREMENT",   
  "DECREMENT",     "LSHIFT",        "RSHIFT",        "URSHIFT",     
  "NOTEQUAL",      "LESSEQUAL",     "EQUAL",         "GREATEREQUAL",
  "MODASSIGN",     "BITANDASSIGN",  "MULASSIGN",     "ADDASSIGN",   
  "SUBASSIGN",     "DIVASSIGN",     "BITXORASSIGN",  "BITORASSIGN", 
  "INTDIVASSIGN",  "LSHIFTASSIGN",  "RSHIFTASSIGN",  "URSHIFTASSIGN",
  "LOGICAND",      "LOGICXOR",      "LOGICOR",       "error",       
  "blah",          "expr_final",    "decl",          "name",        
  "proto",         "arg_list",      "odecl_list",    "expr_value",  
  "stmt_list",     "expr_call",     "expr_postfix",  "expr",        
  "expr_basic",    "expr_unary",    "tokens",      
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "blah ::= expr_final SEMICOLON",
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
 /*  40 */ "expr_value ::= expr_unary",
 /*  41 */ "expr_value ::= NEW name LPN arg_list RPN",
 /*  42 */ "expr_value ::= NEW expr_postfix LPN arg_list RPN",
 /*  43 */ "expr_value ::= COLON expr_value LBR odecl_list RBR",
 /*  44 */ "expr_value ::= COLON LBR odecl_list RBR",
 /*  45 */ "expr_value ::= QMARK LPN arg_list RPN LBR stmt_list RBR",
 /*  46 */ "expr_value ::= QMARK LPN arg_list RPN YIELD LBR stmt_list RBR",
 /*  47 */ "expr_value ::= proto YIELD",
 /*  48 */ "expr_value ::= expr_call YIELD",
 /*  49 */ "expr_final ::= expr_value",
 /*  50 */ "expr_final ::= expr_postfix ELLIPSIS",
 /*  51 */ "expr_final ::= expr_postfix LSQ RSQ ELLIPSIS",
 /*  52 */ "expr_final ::= ELLIPSIS",
 /*  53 */ "expr ::= expr_final",
 /*  54 */ "arg_list ::= COMMA",
 /*  55 */ "odecl_list ::= COMMA",
 /*  56 */ "stmt_list ::= COMMA",
 /*  57 */ "tokens ::= BREAK CASE CATCH CONTINUE DEFAULT DO ELSE FALSE FINALLY FOR IF NEW NULL RETURN SWITCH THROW TRUE TRY USING VAR WHILE YIELD IDENTIFIER NUMBER STRING XMARK PERCENT AMPERSAND LPN RPN ASTERISK PLUS COMMA MINUS PERIOD SOLIDUS COLON SEMICOLON LESS ASSIGN GREATER QMARK LSQ RSQ CARET LBR VBAR RBR TILDE INCREMENT DECREMENT LSHIFT RSHIFT URSHIFT NOTEQUAL LESSEQUAL EQUAL GREATEREQUAL MODASSIGN BITANDASSIGN MULASSIGN ADDASSIGN SUBASSIGN DIVASSIGN BITXORASSIGN BITORASSIGN INTDIVASSIGN LSHIFTASSIGN RSHIFTASSIGN URSHIFTASSIGN LOGICAND LOGICXOR LOGICOR ELLIPSIS",
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
  { 76, 2 },
  { 76, 1 },
  { 79, 1 },
  { 79, 3 },
  { 80, 4 },
  { 78, 4 },
  { 78, 6 },
  { 78, 4 },
  { 78, 5 },
  { 78, 8 },
  { 78, 9 },
  { 85, 4 },
  { 85, 4 },
  { 85, 4 },
  { 86, 3 },
  { 86, 5 },
  { 86, 4 },
  { 86, 3 },
  { 86, 5 },
  { 86, 4 },
  { 86, 3 },
  { 86, 5 },
  { 86, 4 },
  { 86, 3 },
  { 86, 5 },
  { 86, 4 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 88, 1 },
  { 89, 1 },
  { 89, 2 },
  { 89, 2 },
  { 89, 2 },
  { 89, 2 },
  { 83, 1 },
  { 83, 5 },
  { 83, 5 },
  { 83, 5 },
  { 83, 4 },
  { 83, 7 },
  { 83, 8 },
  { 83, 2 },
  { 83, 2 },
  { 77, 1 },
  { 77, 2 },
  { 77, 4 },
  { 77, 1 },
  { 87, 1 },
  { 81, 1 },
  { 82, 1 },
  { 84, 1 },
  { 90, 74 },
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
      /* (0) blah ::= expr_final SEMICOLON */ yytestcase(yyruleno==0);
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
      /* (40) expr_value ::= expr_unary */ yytestcase(yyruleno==40);
      /* (41) expr_value ::= NEW name LPN arg_list RPN */ yytestcase(yyruleno==41);
      /* (42) expr_value ::= NEW expr_postfix LPN arg_list RPN */ yytestcase(yyruleno==42);
      /* (43) expr_value ::= COLON expr_value LBR odecl_list RBR */ yytestcase(yyruleno==43);
      /* (44) expr_value ::= COLON LBR odecl_list RBR */ yytestcase(yyruleno==44);
      /* (45) expr_value ::= QMARK LPN arg_list RPN LBR stmt_list RBR */ yytestcase(yyruleno==45);
      /* (46) expr_value ::= QMARK LPN arg_list RPN YIELD LBR stmt_list RBR */ yytestcase(yyruleno==46);
      /* (47) expr_value ::= proto YIELD */ yytestcase(yyruleno==47);
      /* (48) expr_value ::= expr_call YIELD */ yytestcase(yyruleno==48);
      /* (49) expr_final ::= expr_value */ yytestcase(yyruleno==49);
      /* (50) expr_final ::= expr_postfix ELLIPSIS */ yytestcase(yyruleno==50);
      /* (51) expr_final ::= expr_postfix LSQ RSQ ELLIPSIS */ yytestcase(yyruleno==51);
      /* (52) expr_final ::= ELLIPSIS */ yytestcase(yyruleno==52);
      /* (53) expr ::= expr_final */ yytestcase(yyruleno==53);
      /* (54) arg_list ::= COMMA */ yytestcase(yyruleno==54);
      /* (55) odecl_list ::= COMMA */ yytestcase(yyruleno==55);
      /* (56) stmt_list ::= COMMA */ yytestcase(yyruleno==56);
      /* (57) tokens ::= BREAK CASE CATCH CONTINUE DEFAULT DO ELSE FALSE FINALLY FOR IF NEW NULL RETURN SWITCH THROW TRUE TRY USING VAR WHILE YIELD IDENTIFIER NUMBER STRING XMARK PERCENT AMPERSAND LPN RPN ASTERISK PLUS COMMA MINUS PERIOD SOLIDUS COLON SEMICOLON LESS ASSIGN GREATER QMARK LSQ RSQ CARET LBR VBAR RBR TILDE INCREMENT DECREMENT LSHIFT RSHIFT URSHIFT NOTEQUAL LESSEQUAL EQUAL GREATEREQUAL MODASSIGN BITANDASSIGN MULASSIGN ADDASSIGN SUBASSIGN DIVASSIGN BITXORASSIGN BITORASSIGN INTDIVASSIGN LSHIFTASSIGN RSHIFTASSIGN URSHIFTASSIGN LOGICAND LOGICXOR LOGICOR ELLIPSIS */ yytestcase(yyruleno==57);
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
