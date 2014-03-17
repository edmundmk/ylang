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
#define YYNOCODE 90
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
#define YYNSTATE 111
#define YYNRULE 46
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
 /*     0 */    82,   19,   33,   78,    9,   71,   46,   64,   31,   85,
 /*    10 */   107,   89,  106,   97,   17,  110,   96,   87,   10,   67,
 /*    20 */    38,    1,    8,   69,   12,   14,   11,   20,    6,    2,
 /*    30 */    26,   24,    7,   82,   49,   34,   35,    9,   71,   33,
 /*    40 */    78,   13,   85,   27,    4,   32,   52,   95,  110,   96,
 /*    50 */    87,   10,  108,   25,    1,   33,   78,   12,  104,   11,
 /*    60 */    82,   32,    2,   91,    9,   71,   76,   49,  109,   85,
 /*    70 */    28,   62,    5,   73,   13,  110,   96,   87,   10,   17,
 /*    80 */    70,    1,   33,   78,   12,   38,   11,   82,   32,    2,
 /*    90 */    99,    9,   71,    6,   49,   22,   85,   21,   15,   76,
 /*   100 */    86,   13,  110,   96,   87,   10,   33,   78,    1,   81,
 /*   110 */    57,   12,   31,   11,  107,   89,    2,   84,   50,   94,
 /*   120 */    82,   49,   23,   55,    9,   71,   48,   47,   13,   85,
 /*   130 */    92,  103,   79,   72,   42,  110,   96,   87,   10,   33,
 /*   140 */    78,    1,  106,   44,   12,   31,   11,  107,   89,    2,
 /*   150 */   105,   37,   68,   83,   49,   18,   65,  110,   90,   36,
 /*   160 */   111,   13,   63,  158,   40,   41,   29,   39,   61,  112,
 /*   170 */    75,   60,   30,   58,  107,   89,   77,   53,   33,   78,
 /*   180 */    66,   45,   75,   82,   30,   43,  107,   89,   71,  100,
 /*   190 */    51,   80,   85,   98,  102,   93,   16,   26,  110,   96,
 /*   200 */    87,   33,   78,   35,    1,   54,  159,   31,  159,  107,
 /*   210 */    89,    4,  159,  159,  159,   33,   78,  159,  159,   88,
 /*   220 */   159,   31,  159,  107,   89,   33,   78,  159,  159,   56,
 /*   230 */    69,   31,  159,  107,   89,  159,  159,   26,  159,  159,
 /*   240 */   159,   33,   78,   35,  159,   59,  159,   31,  159,  107,
 /*   250 */    89,    3,   33,   78,  159,  159,  159,  159,   32,  159,
 /*   260 */   101,  159,  159,  159,  159,  159,  159,  159,  159,  159,
 /*   270 */   159,  159,  159,  159,  159,  159,  159,  159,  159,  159,
 /*   280 */   159,  159,   74,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     8,   29,   79,   80,   12,   13,   83,   35,   85,   17,
 /*    10 */    87,   88,   23,   48,   29,   23,   24,   25,   26,   22,
 /*    20 */    35,   29,   37,   22,   32,   29,   34,   35,   43,   37,
 /*    30 */    29,   46,   43,    8,   42,   30,   35,   12,   13,   79,
 /*    40 */    80,   49,   17,   46,   43,   85,   22,   87,   23,   24,
 /*    50 */    25,   26,   23,   46,   29,   79,   80,   32,   48,   34,
 /*    60 */     8,   85,   37,   87,   12,   13,   74,   42,   48,   17,
 /*    70 */    46,   22,   43,   74,   49,   23,   24,   25,   26,   29,
 /*    80 */    44,   29,   79,   80,   32,   35,   34,    8,   85,   37,
 /*    90 */    87,   12,   13,   43,   42,   46,   17,   46,   46,   74,
 /*   100 */    33,   49,   23,   24,   25,   26,   79,   80,   29,   44,
 /*   110 */    83,   32,   85,   34,   87,   88,   37,   48,   81,   33,
 /*   120 */     8,   42,   46,   44,   12,   13,   82,   82,   49,   17,
 /*   130 */    48,   48,   44,   48,   81,   23,   24,   25,   26,   79,
 /*   140 */    80,   29,   23,   83,   32,   85,   34,   87,   88,   37,
 /*   150 */    33,   30,   84,   48,   42,   46,   81,   23,   48,   79,
 /*   160 */     0,   49,   84,   76,   77,   78,   79,   80,   84,    0,
 /*   170 */    83,   82,   85,   82,   87,   88,   77,   84,   79,   80,
 /*   180 */    84,   81,   83,    8,   85,   86,   87,   88,   13,   30,
 /*   190 */    84,   30,   17,   44,   30,   48,   46,   29,   23,   24,
 /*   200 */    25,   79,   80,   35,   29,   83,   89,   85,   89,   87,
 /*   210 */    88,   43,   89,   89,   89,   79,   80,   89,   89,   83,
 /*   220 */    89,   85,   89,   87,   88,   79,   80,   89,   89,   83,
 /*   230 */    22,   85,   89,   87,   88,   89,   89,   29,   89,   89,
 /*   240 */    89,   79,   80,   35,   89,   83,   89,   85,   89,   87,
 /*   250 */    88,   43,   79,   80,   89,   89,   89,   89,   85,   89,
 /*   260 */    87,   89,   89,   89,   89,   89,   89,   89,   89,   89,
 /*   270 */    89,   89,   89,   89,   89,   89,   89,   89,   89,   89,
 /*   280 */    89,   89,   74,
};
#define YY_SHIFT_USE_DFLT (-36)
#define YY_SHIFT_MAX 68
static const short yy_shift_ofst[] = {
 /*     0 */    -8,   25,   52,   79,  112,  112,  112,  112,  112,  112,
 /*    10 */   175,  175,  175,  175,   67,   86,   86,   67,  117,   67,
 /*    20 */   134,  117,  117,   86,   86,  117,   67,  117,  117,  -15,
 /*    30 */   208,    1,  168,   50,   24,   29,  -28,   -3,  -11,   49,
 /*    40 */   160,  169,  159,  161,  149,  164,  150,  147,  -35,   -4,
 /*    50 */     5,   10,    7,   20,   36,   -1,   88,   65,   69,   76,
 /*    60 */    82,   83,   51,   85,  119,  121,  105,  109,  110,
};
#define YY_REDUCE_USE_DFLT (-78)
#define YY_REDUCE_MAX 28
static const short yy_reduce_ofst[] = {
 /*     0 */    87,   99,  -77,   60,   60,  122,  146,   27,  162,  136,
 /*    10 */   -40,    3,  173,  -24,   37,   44,   45,   53,   68,   75,
 /*    20 */    80,   78,   84,   89,   91,   93,  100,   96,  106,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   157,  157,  157,  157,  157,  157,  157,  157,  157,  157,
 /*    10 */   157,  157,  157,  157,  157,  157,  157,  157,  157,  157,
 /*    20 */   157,  157,  157,  157,  157,  157,  157,  157,  157,  130,
 /*    30 */   131,  131,  131,  130,  157,  157,  157,  157,  157,  125,
 /*    40 */   157,  157,  157,  157,  157,  157,  157,  157,  157,  157,
 /*    50 */   157,  157,  157,  157,  157,  157,  157,  157,  157,  157,
 /*    60 */   157,  157,  157,  157,  157,  157,  157,  157,  157,  148,
 /*    70 */   127,  136,  119,  151,  150,  149,  152,  153,  125,  124,
 /*    80 */   122,  123,  135,  120,  116,  134,  154,  133,  143,  142,
 /*    90 */   121,  141,  117,  144,  155,  140,  132,  145,  128,  139,
 /*   100 */   115,  138,  129,  118,  146,  156,  114,  137,  126,  147,
 /*   110 */   113,
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
  "$",             "BREAK",         "CASE",          "CATCH",       
  "CONTINUE",      "DEFAULT",       "DO",            "ELSE",        
  "FALSE",         "FINALLY",       "FOR",           "IF",          
  "NEW",           "NULL",          "RETURN",        "SWITCH",      
  "THROW",         "TRUE",          "TRY",           "USING",       
  "VAR",           "WHILE",         "YIELD",         "IDENTIFIER",  
  "NUMBER",        "STRING",        "XMARK",         "PERCENT",     
  "AMPERSAND",     "LPN",           "RPN",           "ASTERISK",    
  "PLUS",          "COMMA",         "MINUS",         "PERIOD",      
  "SOLIDUS",       "COLON",         "SEMICOLON",     "LESS",        
  "ASSIGN",        "GREATER",       "QMARK",         "LSQ",         
  "RSQ",           "CARET",         "LBR",           "VBAR",        
  "RBR",           "TILDE",         "INCREMENT",     "DECREMENT",   
  "LSHIFT",        "RSHIFT",        "URSHIFT",       "NOTEQUAL",    
  "LESSEQUAL",     "EQUAL",         "GREATEREQUAL",  "MODASSIGN",   
  "BITANDASSIGN",  "MULASSIGN",     "ADDASSIGN",     "SUBASSIGN",   
  "DIVASSIGN",     "BITXORASSIGN",  "BITORASSIGN",   "INTDIVASSIGN",
  "LSHIFTASSIGN",  "RSHIFTASSIGN",  "URSHIFTASSIGN",  "LOGICAND",    
  "LOGICXOR",      "LOGICOR",       "ELLIPSIS",      "error",       
  "blah",          "expr_final",    "decl",          "name",        
  "proto",         "arg_list",      "odecl_list",    "expr_value",  
  "stmt_list",     "expr_postfix",  "expr",          "expr_basic",  
  "expr_unary",  
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "blah ::= expr_final",
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
 /*  11 */ "expr_postfix ::= LPN expr RPN",
 /*  12 */ "expr_postfix ::= name PERIOD LSQ expr_value RSQ",
 /*  13 */ "expr_postfix ::= name LSQ expr_value RSQ",
 /*  14 */ "expr_postfix ::= proto",
 /*  15 */ "expr_postfix ::= expr_postfix PERIOD IDENTIFIER",
 /*  16 */ "expr_postfix ::= expr_postfix PERIOD LSQ expr_value RSQ",
 /*  17 */ "expr_postfix ::= expr_postfix LSQ expr_value RSQ",
 /*  18 */ "expr_postfix ::= expr_postfix LPN arg_list RPN",
 /*  19 */ "expr_basic ::= name",
 /*  20 */ "expr_basic ::= expr_postfix",
 /*  21 */ "expr_basic ::= NUMBER",
 /*  22 */ "expr_basic ::= STRING",
 /*  23 */ "expr_basic ::= TRUE",
 /*  24 */ "expr_basic ::= FALSE",
 /*  25 */ "expr_basic ::= NULL",
 /*  26 */ "expr_unary ::= expr_basic",
 /*  27 */ "expr_unary ::= PLUS expr_basic",
 /*  28 */ "expr_unary ::= MINUS expr_basic",
 /*  29 */ "expr_unary ::= XMARK expr_basic",
 /*  30 */ "expr_unary ::= TILDE expr_basic",
 /*  31 */ "expr_value ::= expr_unary",
 /*  32 */ "expr_value ::= NEW expr_value",
 /*  33 */ "expr_value ::= COLON expr_value LBR odecl_list RBR",
 /*  34 */ "expr_value ::= COLON LBR odecl_list RBR",
 /*  35 */ "expr_value ::= QMARK LPN arg_list RPN LBR stmt_list RBR",
 /*  36 */ "expr_value ::= QMARK LPN arg_list RPN YIELD LBR stmt_list RBR",
 /*  37 */ "expr_value ::= expr_postfix YIELD",
 /*  38 */ "expr_final ::= expr_value",
 /*  39 */ "expr_final ::= expr_postfix ELLIPSIS",
 /*  40 */ "expr_final ::= expr_postfix LSQ RSQ ELLIPSIS",
 /*  41 */ "expr_final ::= ELLIPSIS",
 /*  42 */ "expr ::= expr_final",
 /*  43 */ "arg_list ::= COMMA",
 /*  44 */ "odecl_list ::= COMMA",
 /*  45 */ "stmt_list ::= COMMA",
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
  { 76, 1 },
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
  { 85, 3 },
  { 85, 5 },
  { 85, 4 },
  { 85, 1 },
  { 85, 3 },
  { 85, 5 },
  { 85, 4 },
  { 85, 4 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 87, 1 },
  { 88, 1 },
  { 88, 2 },
  { 88, 2 },
  { 88, 2 },
  { 88, 2 },
  { 83, 1 },
  { 83, 2 },
  { 83, 5 },
  { 83, 4 },
  { 83, 7 },
  { 83, 8 },
  { 83, 2 },
  { 77, 1 },
  { 77, 2 },
  { 77, 4 },
  { 77, 1 },
  { 86, 1 },
  { 81, 1 },
  { 82, 1 },
  { 84, 1 },
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
      /* (0) blah ::= expr_final */ yytestcase(yyruleno==0);
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
      /* (11) expr_postfix ::= LPN expr RPN */ yytestcase(yyruleno==11);
      /* (12) expr_postfix ::= name PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==12);
      /* (13) expr_postfix ::= name LSQ expr_value RSQ */ yytestcase(yyruleno==13);
      /* (14) expr_postfix ::= proto */ yytestcase(yyruleno==14);
      /* (15) expr_postfix ::= expr_postfix PERIOD IDENTIFIER */ yytestcase(yyruleno==15);
      /* (16) expr_postfix ::= expr_postfix PERIOD LSQ expr_value RSQ */ yytestcase(yyruleno==16);
      /* (17) expr_postfix ::= expr_postfix LSQ expr_value RSQ */ yytestcase(yyruleno==17);
      /* (18) expr_postfix ::= expr_postfix LPN arg_list RPN */ yytestcase(yyruleno==18);
      /* (19) expr_basic ::= name */ yytestcase(yyruleno==19);
      /* (20) expr_basic ::= expr_postfix */ yytestcase(yyruleno==20);
      /* (21) expr_basic ::= NUMBER */ yytestcase(yyruleno==21);
      /* (22) expr_basic ::= STRING */ yytestcase(yyruleno==22);
      /* (23) expr_basic ::= TRUE */ yytestcase(yyruleno==23);
      /* (24) expr_basic ::= FALSE */ yytestcase(yyruleno==24);
      /* (25) expr_basic ::= NULL */ yytestcase(yyruleno==25);
      /* (26) expr_unary ::= expr_basic */ yytestcase(yyruleno==26);
      /* (27) expr_unary ::= PLUS expr_basic */ yytestcase(yyruleno==27);
      /* (28) expr_unary ::= MINUS expr_basic */ yytestcase(yyruleno==28);
      /* (29) expr_unary ::= XMARK expr_basic */ yytestcase(yyruleno==29);
      /* (30) expr_unary ::= TILDE expr_basic */ yytestcase(yyruleno==30);
      /* (31) expr_value ::= expr_unary */ yytestcase(yyruleno==31);
      /* (32) expr_value ::= NEW expr_value */ yytestcase(yyruleno==32);
      /* (33) expr_value ::= COLON expr_value LBR odecl_list RBR */ yytestcase(yyruleno==33);
      /* (34) expr_value ::= COLON LBR odecl_list RBR */ yytestcase(yyruleno==34);
      /* (35) expr_value ::= QMARK LPN arg_list RPN LBR stmt_list RBR */ yytestcase(yyruleno==35);
      /* (36) expr_value ::= QMARK LPN arg_list RPN YIELD LBR stmt_list RBR */ yytestcase(yyruleno==36);
      /* (37) expr_value ::= expr_postfix YIELD */ yytestcase(yyruleno==37);
      /* (38) expr_final ::= expr_value */ yytestcase(yyruleno==38);
      /* (39) expr_final ::= expr_postfix ELLIPSIS */ yytestcase(yyruleno==39);
      /* (40) expr_final ::= expr_postfix LSQ RSQ ELLIPSIS */ yytestcase(yyruleno==40);
      /* (41) expr_final ::= ELLIPSIS */ yytestcase(yyruleno==41);
      /* (42) expr ::= expr_final */ yytestcase(yyruleno==42);
      /* (43) arg_list ::= COMMA */ yytestcase(yyruleno==43);
      /* (44) odecl_list ::= COMMA */ yytestcase(yyruleno==44);
      /* (45) stmt_list ::= COMMA */ yytestcase(yyruleno==45);
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
