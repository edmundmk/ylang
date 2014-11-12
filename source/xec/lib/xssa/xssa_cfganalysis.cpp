//
//  xssa_cfganalysis.cpp
//  xob
//
//  Created by Edmund Kapusniak on 12/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xssa_cfganalysis.h"
#include "xssa.h"




static void visit_depth_first( xssa_block* block, int* index )
{
    // Ensure this node exists and hasn't been visited.
    if ( ! block || block->index != -1 )
    {
        return;
    }
    
    // Mark visited.
    block->index = 0;
    
    // Visit successors.
    if ( block->condition )
    {
        // Prefer true branch by visting it last.
        visit_depth_first( block->iffalse, index );
        visit_depth_first( block->iftrue, index );
    }
    else
    {
        visit_depth_first( block->next, index );
    }
    
    // Bless with final index.
    *index -= 1;
    assert( *index >= 0 );
    block->index = *index;
}


void xssa_order_depth_first( xssa_func* func )
{
    // Clear existing order.
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        xssa_block* block = func->blocks.at( i ).get();
        block->index = -1;
    }

    // Start at the initial block and visit recursively.
    int index = (int)func->blocks.size();
    visit_depth_first( func->blocks[ 0 ].get(), &index );
    
    // Reorder blocks.
    std::vector< xssa_block_p > blocks;
    blocks.resize( func->blocks.size() );
    for ( size_t i = 0; i < func->blocks.size(); ++i )
    {
        size_t index = func->blocks[ i ]->index;
        blocks[ index ] = std::move( func->blocks[ i ] );
    }
    
    std::swap( func->blocks, blocks );
}









