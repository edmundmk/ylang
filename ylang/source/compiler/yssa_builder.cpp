//
//  yssa_builder.cpp
//
//  Created by Edmund Kapusniak on 05/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_builder.h"
#include <make_unique.h>




yssa_builder::yssa_builder( yl_diagnostics* diagnostics )
    :   diagnostics( diagnostics )
    ,   module( std::make_unique< yssa_module >() )
{
}

yssa_builder::~yssa_builder()
{
}



bool yssa_builder::build( yl_ast* ast )
{
    // Construct each function.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        yl_ast_func* astf = ast->functions.at( i );
        yssa_function_p ssaf = std::make_unique< yssa_function >
        (
            astf->sloc,
            module->alloc.strdup( astf->funcname )
        );
        funcmap.emplace( std::make_pair( astf, ssaf.get() ) );
        module->functions.emplace_back( std::move( ssaf ) );
    }
    
    
    // Build SSA for each function.
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        yl_ast_func* astf = ast->functions.at( i );
        build( astf );
    }


    return diagnostics->error_count() == 0;
}





void yssa_builder::build( yl_ast_func* astf )
{
    function = funcmap.at( astf );
    
    // Create entry block.
    yssa_block_p entry_block = std::make_unique< yssa_block >( nullptr );
    block = entry_block.get();
    function->blocks.emplace_back( std::move( entry_block ) );
    
    
    // Add parameter ops.
    for ( size_t i = 0; i < astf->parameters.size(); ++i )
    {
        yl_ast_name* param = astf->parameters.at( i );
        yssa_opinst* param_op = op( param->sloc, YSSA_PARAM, 0, 1 );
        param_op->variable = variable( param );
        param_op->select = (int)i;
    }

    
    // Visit the block.
    visit( astf->block, 0 );

}




yssa_variable* yssa_builder::variable( yl_ast_name* name )
{
    // TODO.
    return nullptr;
}


yssa_opinst* yssa_builder::op(
        int sloc, uint8_t opcode, uint8_t operand_count, uint8_t result_count )
{
    void* p = module->alloc.malloc(
            sizeof( yssa_opinst ) + sizeof( yssa_opinst* ) * operand_count );
    yssa_opinst* op = new ( p ) yssa_opinst(
            sloc, opcode, operand_count, result_count );
    
    // TODO: add to current block.
    
    return op;
}











