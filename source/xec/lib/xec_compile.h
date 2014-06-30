//
//  xec_compile.h
//
//  Created by Edmund Kapusniak on 18/06/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_COMPILER_H
#define XEC_COMPILER_H


/*
    Compile an AST to xec code.
*/


class xec_parser;
struct xec_code;


void xec_compile( xec_parser* parser, xec_code* code );



#endif
