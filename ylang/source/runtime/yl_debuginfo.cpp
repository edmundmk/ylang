//
//  yl_debuginfo.cpp
//
//  Created by Edmund Kapusniak on 17/11/2015.
//  Copyright © 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_debuginfo.h"



const char* yl_debuginfo::funcname() const
{
    return _funcname.c_str();
}

const char* yl_debuginfo::filename() const
{
    return _filename.c_str();
}

std::pair< int, int > yl_debuginfo::line_column( unsigned ip ) const
{
    if ( _slocs.size() )
    {
        sloc sloc = _slocs.at( ip );
        return std::make_pair( (int)sloc.line, (int)sloc.column );
    }
    else
    {
        return std::make_pair( -1, -1 );
    }
}


void yl_debuginfo::print() const
{
    if ( _variables.size() )
    {
        printf( "    debugvars:\n" );
        for ( size_t varindex = 0; varindex < _variables.size(); ++varindex )
        {
            const variable& dv = _variables[ varindex ];
            printf
            (
                "        %s %s%u",
                dv.name.c_str(),
                varindex < _upcount ? "*" : "",
                dv.r
            );
            for ( size_t i = 0; i < _varspans.size(); ++i )
            {
                const varspan& ds = _varspans[ i ];

                if ( ds.varindex != varindex )
                    continue;
                
                printf( " %04X:%04X", ds.start, ds.end );
            }
            printf( "\n" );
        }
    }
}

