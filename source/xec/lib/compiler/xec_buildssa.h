//
//  xec_buildssa.h
//
//  Created by Edmund Kapusniak on 08/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_BUILDSSA_H
#define XEC_BUILDSSA_H


#include <unordered_set>
#include <unordered_map>
#include "xec_ssa.h"


class xec_parser;
struct xec_buildssa_block;


class xec_buildssa
{
public:

    xec_buildssa();

    xec_ssaname     declare( const char* text );
    void            retire( xec_ssaname name );

    void            write( xec_ssaname name, xec_ssavalue value );
    xec_ssavalue    read( xec_ssaname );

    void            loop_begin();
    void            loop_break();
    void            loop_continue();
    void            loop_end();
    void            if_true( xec_ssavalue condition );
    void            if_false( xec_ssavalue condition );
    void            if_else();
    void            if_end();
    
    xec_ssavalue    null();
    xec_ssavalue    literal( bool boolean );
    xec_ssavalue    literal( double number );
    xec_ssavalue    literal( const char* string, size_t length );
    xec_ssavalue    emit( int opcode, xec_ssavalue a );
    xec_ssavalue    emit( int opcode, xec_ssavalue a, xec_ssavalue b );
    void            parameter( xec_ssavalue a );
    xec_ssavalue    multiple( int opcode, int count );
    xec_ssavalue    result( xec_ssavalue v, int n );


private:

    xec_ssaname next_name;
    std::unordered_map< xec_ssaname, const char* > names;
    std::unordered_set< xec_ssaname > active_names;
    xec_buildssa_block* block;
    
};



#endif
