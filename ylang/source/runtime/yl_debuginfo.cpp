//
//  yl_debuginfo.cpp
//
//  Created by Edmund Kapusniak on 17/11/2015.
//  Copyright © 2015 Edmund Kapusniak. All rights reserved.
//


#include "yl_debuginfo.h"



const char* yl_debuginfo::funcname()
{
    return _funcname.c_str();
}

const char* yl_debuginfo::filename()
{
    return _filename.c_str();
}




void yl_debuginfo::print()
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

