//
//  xec_semantics.h
//
//  Created by Edmund Kapusniak on 05/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_DECLARE_H
#define XEC_DECLARE_H


/*
    We make a pass through the AST, declaring declarations into the
    appropriate scopes and resolving identifiers.  The scope in which
    an identifier is found is important:
    
        -   If it's the global scope, it's a global reference.
        -   If it's an object scope, it's an implicit-this reference.
        -   If it's a block outside of the current function, then it's an
                upvalue.  In this case the upvalue itself must be marked
                as an upval, and imported into each function constructor
                or declaration down the scope chain.

    We also check other semantic constraints:
    
        -   Lvalues must be assignable.
        -   yield calls are only allowable inside coroutine functions.
 
*/


class xec_parser;

void xec_semantics( xec_parser* p );



#endif
