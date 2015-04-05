//
//  yssa_codegen.cpp
//
//  Created by Edmund Kapusniak on 04/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_codegen.h"
#include "yl_invoke.h"
#include "yssa.h"
#include "yl_string.h"
#include "yl_function.h"


struct ygen_module;
struct ygen_program;
struct ygen_string;


typedef std::unique_ptr< ygen_program > ygen_program_p;
typedef std::unique_ptr< ygen_string >  ygen_string_p;


/*
    Each function has an array of constant values used in the function.
    This includes lookup keys and program objects.
*/


enum ygen_value_kind
{
    YCV_KEY,
    YCV_STRING,
    YCV_NUMBER,
    YCV_PROGRAM,
};


struct ygen_value
{
    ygen_value_kind kind;
    union
    {
        ygen_string*    string;
        double          number;
        ygen_program*   program;
    };
};



/*
    We have to be very careful that the programs and strings we create are
    reachable by the garbage collector at all times.  The code generator
    constructs an unmanaged representation of the entire module before
    translating it to the final objects (ensuring that each heapobj is
    reachable from a yl_invoke that puts the main function in the root set).
*/


struct ygen_module
{
    std::unordered_map< symkey, ygen_string_p >  strings;
    std::unordered_map< yssa_function*, ygen_program_p > programs;
};


struct ygen_program
{
    yl_program*                 program;
    std::vector< ygen_value >   values;
    std::vector< yl_opinst >    ops;
    std::vector< yl_xframe >    xframes;
    std::vector< yl_varname >   varnames;
};


struct ygen_string
{
    const char*                 text;
    size_t                      size;
    bool                        iskey;
};




static void yssa_codegen_function( ygen_module* m, yssa_function* function );



yl_invoke yssa_codegen( yssa_module* module )
{
    ygen_module m;
    
    // Generate each function.
    for ( size_t i = 0; i < module->functions.size(); ++i )
    {
        yssa_function* function = module->functions.at( i ).get();
        yssa_codegen_function( &m, function );
    }
    
    // Construct final heap objects.

    

    return yl_invoke();
}


void yssa_codegen_function( ygen_module* m, yssa_function* function )
{
    
}















