//
//  xec_ssa.h
//
//  Created by Edmund Kapusniak on 09/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_SSA_H
#define XEC_SSA_H


#include <assert.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <deque>
#include <vector>
#include <region.h>


class xec_script;
struct xec_ssa_opref;
struct xec_ssa_op;
struct xec_ssa_string;
struct xec_ssa_closure;
struct xec_ssa_phi;
struct xec_ssa_args;
struct xec_ssa_name;
struct xec_ssa;
struct xec_ssa_func;
struct xec_ssa_block;
struct xec_ssa_slice;




/*
    SSA ops are a fixed size, with several different packings depending on the
    arguments required by the opcode.  Every op contains the source location
    of the code that generated it (for debugging), and most ops have space
    for information about the live ranges of the generated value.  Live ranges
    have to be computed for every value before we can do register allocation
    and convert to low-level instructions, so inlining them makes sense.
    
    Ops are arranged into slices.  Each slice is individually growable without
    affecting other slices.  An op is indexed by the index of its slice, then
    the index of the op within the slice.  A global ordering of opcodes is
    achived by generating an ordering of slices.
    
    This scheme allows the SSA form to be relatively compact, while preserving
    the flexiblity required for optimization and compilation passes.
 
    Ops which require three arguments do not produce a result value - they are
    used to set object keys or values into an array or table).  Ops which
    require arguments lists (such as ɸ-ops, returns, yields, and calls) are
    followed by xarg (and xunpack) ops supplying the missing arguments.
    

    Op Packings:

    constants
        [   sloc   |   op/r   ] [  until   |  lnext   ] [        number       ]
        [   sloc   |   op/r   ] [  until   |  lnext   ] [        boolean      ]
        [   sloc   |   op/r   ] [  until   |  lnext   ] [        string       ]
    complex
        [   sloc   |   op/r   ] [  until   |  lnext   ] [        closure      ]
        [   sloc   |   op/r   ] [  until   |  lnext   ] [         phi         ]
        [   sloc   |   op/r   ] [  until   |  lnext   ] [         args        ]
    operands
        [   sloc   |   op/r   ] [  until   |  lnext   ] [ operanda | operandb ]
        [   sloc   |   op/r   ] [  until   |  lnext   ] [ operanda |  key/imm ]
    assignments
        [   sloc   |   op/r   ] [ operandv |          ] [ operanda | operandb ]
        [   sloc   |   op/r   ] [ operandv |          ] [ operanda |    key   ]


    Note that phi instructions should have one argument per predecessor block,
    in the correct order.
 

    xec has destructuring-assignment and multiple-return values, which means
    some SSA ops consume an unknown number of values or return multiple values.
    
    The following ops 'unpack' to an unknown number of values:
    
        vararg $-1          // whereas vararg $n selects vararg n
        unpack operanda $-1 // whereas unpack operanda $n selects element n
        call acount $-1     // unpacks to all return values
        ycall acount $-1    // similarly
        yield acount $-1    // unpacks to all values passed back to coroutine
 
 
    'unpacked' values can only be consumed by the 'unpacked' argument of an
    args-like op.  Typically an unpacked value is consumed immediately.  It
    is invalid to unpack another value before the first is consumed.
 
        call                // appends unpacked values to end of
        ycall               //      an argument list
        yield
        new
        return              // returns multiple values
        extend              // appends unpacked values to the end of an array
 
 
    The following ops can return multiple values.  Unlike unpacked values
    the number is fixed at compile time and encoded into the op.  Individual
    values are selected with a following 'select' op:
 
        next operanda $n    // next value from iterator, but returns a 'hidden'
                            //      'done' value, and tables return key, value
        call acount $n      // returns exactly n values
        ycall acount $n     // similarly
        yield acount $n     // the first n values passed back to coroutine
 
 
    Upvalues have explicit load and store instructions, as the alternative
    of forcing every instance of the upval into the same VM register defeats
    the purpose of SSA.  If we do alias/escape analysis then some of the loads
    or stores may be proved redundant.

*/



/*
    lists and maps.
*/

// TODO: replace std::deque with a container with a smaller chunk size

typedef std::deque< xec_ssa_func*,
    region_allocator< xec_ssa_func* > > xec_ssa_func_list;
typedef std::deque< xec_ssa_slice*,
    region_allocator< xec_ssa_slice* > > xec_ssa_slice_list;
typedef std::deque< xec_ssa_block*,
    region_allocator< xec_ssa_block* > > xec_ssa_block_list;
typedef std::deque< xec_ssa_op,
    region_allocator< xec_ssa_op > > xec_ssa_op_list;
typedef std::vector< xec_ssa_opref,
    region_allocator< xec_ssa_opref > > xec_ssa_opref_list;
typedef std::unordered_multimap
        <
            xec_ssa_opref,
            xec_ssa_name*,
            std::hash< xec_ssa_opref >,
            std::equal_to< xec_ssa_opref >,
            region_allocator< std::pair< const xec_ssa_opref, xec_ssa_name* > >
        >
        xec_ssa_name_map;


/*
    opcodes
*/

enum xec_ssa_opcode
{
    XEC_SSA_NOP,        // no-op

    // constants
    XEC_SSA_NULL,       // null value
    XEC_SSA_NUMBER,     // number
    XEC_SSA_BOOL,       // boolean
    XEC_SSA_STRING,     // string

    // ops w/ operands.
    XEC_SSA_REF,        // reference another value

    XEC_SSA_POS,        // unary +
    XEC_SSA_NEG,        // unary -
    XEC_SSA_NOT,        // unary !
    XEC_SSA_BITNOT,     // unary ~

    XEC_SSA_MUL,        // *
    XEC_SSA_DIV,        // /
    XEC_SSA_MOD,        // %
    XEC_SSA_INTDIV,     // ~
    XEC_SSA_ADD,        // +
    XEC_SSA_SUB,        // -
    XEC_SSA_LSL,        // <<
    XEC_SSA_LSR,        // >>
    XEC_SSA_ASR,        // ~>>
    XEC_SSA_BITAND,     // &
    XEC_SSA_BITXOR,     // ^
    XEC_SSA_BITOR,      // |
    XEC_SSA_CONCAT,     // ..
    XEC_SSA_EQ,         // ==
    XEC_SSA_LT,         // <
    XEC_SSA_LE,         // <=
    XEC_SSA_IN,         // in
    XEC_SSA_IS,         // is
    XEC_SSA_XOR,        // ^^
    
    XEC_SSA_INKEY,      // object.[ key ]
    XEC_SSA_INDEX,      // container[ index ]
    XEC_SSA_DELINKEY,   // delete object.[ key ]
    XEC_SSA_OBJECT,     // create new object with prototype
    XEC_SSA_ITER,       // push iterator for a list
    XEC_SSA_ITERKEY,    // push iterator for object key
    XEC_SSA_POPITER,    // pop iterator
    XEC_SSA_APPEND,     // append to an array
    
    XEC_SSA_FIRST_REF   = XEC_SSA_REF,
    XEC_SSA_LAST_REF    = XEC_SSA_APPEND,

    // w/ key
    XEC_SSA_KEY,        // object.key
    XEC_SSA_DELKEY,     // delete object.key
    XEC_SSA_GLOBAL,     // global.key
    XEC_SSA_SETGLOBAL,  // global.key = value
    
    XEC_SSA_FIRST_KEY   = XEC_SSA_KEY,
    XEC_SSA_LAST_KEY    = XEC_SSA_SETGLOBAL,
    
    // w/ immediate
    XEC_SSA_PARAM,      // parameter number
    XEC_SSA_SELECT,     // select a call result
    XEC_SSA_NEWUP,      // create and initialize new upval
    XEC_SSA_SETUP,      // set upval
    XEC_SSA_REFUP,      // value of upval
    XEC_SSA_CLOSE,      // close upval
    XEC_SSA_ARRAY,      // new array
    XEC_SSA_TABLE,      // new table
    
    XEC_SSA_FIRST_IMM   = XEC_SSA_PARAM,
    XEC_SSA_LAST_IMM    = XEC_SSA_TABLE,

    // assignments
    XEC_SSA_SETINKEY,   // object.[ key ] = value
    XEC_SSA_SETINDEX,   // container[ index ] = value
    XEC_SSA_SETKEY,     // object.key = value

    XEC_SSA_FIRST_SET   = XEC_SSA_SETINKEY,
    XEC_SSA_LAST_SET    = XEC_SSA_SETKEY,
    
    // w/args
    XEC_SSA_VARARG,     // vararg, or unpack all
    XEC_SSA_UNPACK,     // array element, or unpack all
    XEC_SSA_CALL,       // function call
    XEC_SSA_YCALL,      // yieldable function call
    XEC_SSA_YIELD,      // yield
    XEC_SSA_EXTEND,     // extend an array with unpacked
    XEC_SSA_NEXT,       // produce values from an iterator
    XEC_SSA_RETURN,     // return    
    
    XEC_SSA_FIRST_ARG   = XEC_SSA_VARARG,
    XEC_SSA_LAST_ARG    = XEC_SSA_RETURN,

    // closures
    XEC_SSA_CLOSURE,    // create a function closure

    // phi
    XEC_SSA_PHI,        // SSA ɸ-function
    
    // liveness information.
    XEC_SSA_LIVE,       // additional live range for value.
    
};



/*
    oprefs uniquely identify an opcode by slice index and index within the
    slice.
*/

struct xec_ssa_opref
{
    static const int SLICE_BITS = 20;
    static const int INDEX_BITS = 12;

    explicit operator bool() const;

    union
    {
        struct
        {
    unsigned    slice : SLICE_BITS;   // maximum of 1mi slices per function
    unsigned    index : INDEX_BITS;   // actual number of ops per slice is likely small
        };
    unsigned    value;
    };
};

namespace std
{
template <> struct hash< xec_ssa_opref >
{
    size_t operator () ( const xec_ssa_opref& o ) const;
};
}

bool operator == ( const xec_ssa_opref& a, const xec_ssa_opref& b );
bool operator != ( const xec_ssa_opref& a, const xec_ssa_opref& b );

static const xec_ssa_opref XEC_SSA_INVALID = { 0xFFFFF, 0xFFF };
static const xec_ssa_opref XEC_SSA_FOREVER = { 0xFFFFF, 0xFFE };
static const xec_ssa_opref XEC_SSA_UNDEF   = { 0xFFFFF, 0xFFD };
static const xec_ssa_opref XEC_SSA_LOOP    = { 0xFFFFF, 0xFFC };
static const xec_ssa_opref XEC_SSA_SELF    = { 0xFFFFF, 0xFFB };



/*
    ops have a ridiculously overcomplicated set of packings based on opcode.
*/

struct xec_ssa_op
{
    xec_ssa_op( int sloc, xec_ssa_opcode opcode );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode, double number );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode, bool boolean );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode, xec_ssa_string* string );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode, xec_ssa_closure* closure );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode, xec_ssa_phi* phi );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode, xec_ssa_args* args );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode, int immkey );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode, xec_ssa_opref operanda );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode,
                    xec_ssa_opref operanda, xec_ssa_opref operandb );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode,
                    xec_ssa_opref operanda, int immkey );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode,
                    xec_ssa_opref a, xec_ssa_opref b, xec_ssa_opref v );
    xec_ssa_op( int sloc, xec_ssa_opcode opcode,
                    xec_ssa_opref a, int key, xec_ssa_opref v );

    
    void get_operands( std::vector< xec_ssa_opref >* operands );


    // [   sloc   |   op/r   ]

    int             sloc;
    xec_ssa_opcode  opcode  : 10;
    int             r       : 22;
    

    // [  until   |  lnext   ]
    // [ operandv |          ]
    union
    {
        struct
        {
    xec_ssa_opref   until;
    xec_ssa_opref   lnext;
        };
    xec_ssa_opref   operandv;
    };
    
    
    union
    {

    // [ operanda | operandb ]
    // [ operanda |  key/imm ]
        struct
        {
    xec_ssa_opref   operanda;
            union
            {
    xec_ssa_opref   operandb;
    int             immkey;
            };
        };

    // [        number       ]
    // [        boolean      ]
    // [        string       ]
    // [        closure      ]
    // [         phi         ]
    // [         args        ]
    double          number;
    bool            boolean;
    xec_ssa_string* string;
    xec_ssa_closure* closure;
    xec_ssa_phi*    phi;
    xec_ssa_args*   args;

    };


};




/*
    extended opcode types.
*/

struct xec_ssa_string
{
    xec_ssa_string( const char* string, size_t length );

    const char*         string;
    size_t              length;
};


struct xec_ssa_closure
{
    explicit xec_ssa_closure( xec_ssa_func* function );

    xec_ssa_func*       function;
    xec_ssa_opref_list  upvals;
};


struct xec_ssa_phi
{
    xec_ssa_phi();

    xec_ssa_opref_list  definitions;
};


struct xec_ssa_args
{
    explicit xec_ssa_args( int rcount );

    int                 rcount;
    int                 stacktop;
    xec_ssa_opref_list  args;
    xec_ssa_opref       unpacked;
};







/*
    names are for debugging and map ops to source code identifiers
*/

struct xec_ssa_name
{
    xec_ssa_name( int sloc, const char* name );

    int                 sloc;
    const char*         name;
};




/*
    SSA structures.
*/

struct xec_ssa
{
    xec_ssa();
    ~xec_ssa();

    region              alloc;
    xec_script*         script;
    xec_ssa_func*       function;
    std::deque< xec_ssa_func* > functions;
    std::deque< const char* > keys;
};


struct xec_ssa_func
{
    xec_ssa_func( int sloc, const char* funcname );
    
    xec_ssa_opref   operand( xec_ssa_opref opref );
    xec_ssa_op*     getop( xec_ssa_opref opref );

    int                 sloc;
    const char*         funcname;
    xec_ssa_name_map    names;
    xec_ssa_block*      block;
    xec_ssa_block_list  blocks;
    xec_ssa_slice_list  slices;
    int                 upvalcount;
    int                 localupcount;
    int                 paramcount;
    bool                varargs;
    bool                coroutine;
};


struct xec_ssa_block
{
    xec_ssa_block();

    xec_ssa_block_list  previous;
    xec_ssa_slice*      live;
    xec_ssa_slice*      phi;
    xec_ssa_slice*      ops;
    int                 index;
    xec_ssa_opref       condition;
    union
    {
    xec_ssa_block*      next;
        struct
        {
    xec_ssa_block*      iftrue;
    xec_ssa_block*      iffalse;
        };
    };
};


struct xec_ssa_slice
{
    explicit xec_ssa_slice( int index, xec_ssa_block* block );
    
    unsigned            index;
    xec_ssa_block*      block;
    xec_ssa_op_list     ops;
};






/*
    opref
*/

inline xec_ssa_opref::operator bool() const
{
    return value != 0xFFFFFFFF;
}

inline size_t std::hash< xec_ssa_opref >::operator() ( const xec_ssa_opref& o ) const
{
    return std::hash< unsigned >()( o.value );
}

inline bool operator == ( const xec_ssa_opref& a, const xec_ssa_opref& b )
{
    return a.value == b.value;
}

inline bool operator != ( const xec_ssa_opref& a, const xec_ssa_opref& b )
{
    return a.value != b.value;
}



/*
    op
*/

inline xec_ssa_op::xec_ssa_op( int sloc, xec_ssa_opcode opcode )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   operanda( XEC_SSA_INVALID )
    ,   operandb( XEC_SSA_INVALID )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc, xec_ssa_opcode opcode, double number )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   number( number )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc, xec_ssa_opcode opcode, bool boolean )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   boolean( boolean )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc,
            xec_ssa_opcode opcode, xec_ssa_string* string )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   string( string )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc,
            xec_ssa_opcode opcode, xec_ssa_closure* closure )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   closure( closure )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc,
            xec_ssa_opcode opcode, xec_ssa_phi* phi )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   phi( phi )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc,
            xec_ssa_opcode opcode, xec_ssa_args* args )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   args( args )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc, xec_ssa_opcode opcode, int immkey )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   operanda( XEC_SSA_INVALID )
    ,   immkey( immkey )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc,
                        xec_ssa_opcode opcode, xec_ssa_opref operanda )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   operanda( operanda )
    ,   operandb( XEC_SSA_INVALID )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc, xec_ssa_opcode opcode,
                        xec_ssa_opref operanda, xec_ssa_opref operandb )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   operanda( operanda )
    ,   operandb( operandb )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc, xec_ssa_opcode opcode,
                        xec_ssa_opref operanda, int immkey )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   until( XEC_SSA_INVALID )
    ,   lnext( XEC_SSA_INVALID )
    ,   operanda( operanda )
    ,   immkey( immkey )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc, xec_ssa_opcode opcode,
                        xec_ssa_opref a, xec_ssa_opref b, xec_ssa_opref v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   operandv( v )
    ,   operanda( a )
    ,   operandb( b )
{
}

inline xec_ssa_op::xec_ssa_op( int sloc, xec_ssa_opcode opcode,
                        xec_ssa_opref a, int key, xec_ssa_opref v )
    :   sloc( sloc )
    ,   opcode( opcode )
    ,   r( -1 )
    ,   operandv( v )
    ,   operanda( a )
    ,   immkey( key )
{
}


inline xec_ssa_string::xec_ssa_string( const char* string, size_t length )
    :   string( string )
    ,   length( length )
{
}

inline xec_ssa_closure::xec_ssa_closure( xec_ssa_func* function )
    :   function( function )
{
}

inline xec_ssa_phi::xec_ssa_phi()
{
}

inline xec_ssa_args::xec_ssa_args( int rcount )
    :   rcount( rcount )
    ,   stacktop( -1 )
    ,   unpacked( XEC_SSA_INVALID )
{
}



/*
    inline
*/

inline xec_ssa_opref xec_ssa_func::operand( xec_ssa_opref opref )
{
    while ( true )
    {
        xec_ssa_op* op = getop( opref );
        if ( op && op->opcode == XEC_SSA_REF )
            opref = op->operanda;
        else
            return opref;
    }

}


inline xec_ssa_op* xec_ssa_func::getop( xec_ssa_opref opref )
{
    if ( opref )
        return &slices.at( opref.slice )->ops.at( opref.index );
    else
        return NULL;
}



#endif



