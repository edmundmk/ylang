//
//  xec_compile_js.cpp
//
//  Created by Edmund Kapusniak on 31/08/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_script.h"
#include "xec_js_astvisitor.h"
#include "xec_source_Writer.h"

void xec_compile_js( xec_ast* ast )
{
    xec_source_writer w;
    xec_js_astvisitor v( &w );
    v.visit( ast->function->block );
}