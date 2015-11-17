//
//  yl_debuginfo.h
//
//  Created by Edmund Kapusniak on 17/11/2015.
//  Copyright © 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YL_DEBUGINFO_H
#define YL_DEBUGINFO_H


#include <utility>
#include <string>
#include <vector>


/*
    Each program object has debug info.  The format may well change.
*/


struct yl_debuginfo
{
public:

    const char*             funcname();
    
    const char*             filename();
    std::pair< int, int >   line_column( unsigned ip );

    void                    variables( unsigned ip );

    void                    print();
    

private:

    friend class ygen_emit;
    
    struct sloc
    {
        unsigned    column : 12;
        unsigned    line   : 20;
    };
    
    struct variable
    {
        std::string name;       // name of variable.
        unsigned    r;          // register or upval index.
    };
    
    struct varspan
    {
        unsigned    varindex;   // index into variable list.
        unsigned    start;      // first instruction in span.
        unsigned    end;        // one past last instruction in span.
    };

    std::string             _funcname;
    std::string             _filename;
    std::vector< sloc >     _slocs;
    size_t                  _upcount;
    std::vector< variable > _variables;
    std::vector< varspan >  _varspans;

};





#endif


