//
//  xec_lj_stmt.cpp
//
//  Created by Edmund Kapusniak on 10/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include <vector>
#include "xec_lj_stmt.h"
#include "xec_lj_builder.h"





xec_lj_stmtvisitor::xec_lj_stmtvisitor( xec_lj_builder* b )
    :   b( b )
{
}



void xec_lj_stmtvisitor::fallback( xec_ast_node* node, xec_lj_scope* scope, int indent )
{
    b->temporary( node, scope, indent );
}


void xec_lj_stmtvisitor::visit( xec_expr_preop* node, xec_lj_scope* scope, int indent )
{
    const char* op = node->opkind == XEC_ASTOP_PREINC ? "+" : "-";
    xec_lj_lvprol lvalue;
    b->lvprol( &lvalue, node->lvalue, scope, indent );
    lvalue.prologue();
    b->printf( "%*s", indent, "" );
    lvalue.value();
    b->printf( " = " );
    lvalue.value();
    b->printf( " %s 1\n", op );
}

void xec_lj_stmtvisitor::visit( xec_expr_postop* node, xec_lj_scope* scope, int indent )
{
    const char* op = node->opkind == XEC_ASTOP_PREINC ? "+" : "-";
    xec_lj_lvprol lvalue;
    b->lvprol( &lvalue, node->lvalue, scope, indent );
    lvalue.prologue();
    lvalue.value();
    b->printf( " = " );
    lvalue.value();
    b->printf( " %s 1\n", op );
}


void xec_lj_stmtvisitor::visit( xec_expr_call* node, xec_lj_scope* scope, int indent )
{
    b->printf( "%*s", indent, "" );
    xec_lj_value value;
    b->expr( &value, node, scope, indent );
    value.prologue();
    value.value();
    b->printf( "\n" );
}

void xec_lj_stmtvisitor::visit( xec_expr_yield* node, xec_lj_scope* scope, int indent )
{
    b->printf( "%*s", indent, "" );
    xec_lj_value value;
    b->expr( &value, node, scope, indent );
    value.prologue();
    value.value();
    b->printf( "\n" );
}

void xec_lj_stmtvisitor::visit( xec_expr_assign* node, xec_lj_scope* scope, int indent )
{
    if ( node->assignop == XEC_ASTOP_DECLARE )
    {
        assert( node->lvalue->kind == XEC_EXPR_LOCAL );
        xec_expr_local* local = (xec_expr_local*)node->lvalue;

        xec_lj_value rvalue;
        b->expr( &rvalue, node->rvalue, scope, indent );
        rvalue.prologue();
        b->printf( "%*slocal ", indent, "" );
        b->local( local->name->name );
        b->printf( " = " );
        rvalue.value();
        b->printf( "\n" );
    }
    else if ( node->assignop == XEC_ASTOP_ASSIGN )
    {
        xec_lj_value rvalue;
        b->expr( &rvalue, node->rvalue, scope, indent );
        if ( rvalue.has_prologue() )
        {
            xec_lj_lvprol lvalue;
            b->lvprol( &lvalue, node->lvalue, scope, indent );
            lvalue.prologue();
            rvalue.prologue();
            b->printf( "%*s", indent, "" );
            lvalue.value();
        }
        else
        {
            xec_lj_value lvalue;
            b->expr( &lvalue, node->lvalue, scope, indent );
            lvalue.prologue();
            b->printf( "%*s", indent, "" );
            lvalue.value();
        }

        b->printf( " = " );
        rvalue.value();
        b->printf( "\n" );
    }
    else
    {
        xec_lj_lvprol lvalue;
        b->lvprol( &lvalue, node->lvalue, scope, indent );
        lvalue.prologue();
        
        xec_lj_value rvalue;
        b->expr( &rvalue, node->rvalue, scope, indent );

        int lvval = -1;
        if ( rvalue.has_prologue() )
        {
            lvval = b->temporary( scope, indent );
            lvalue.value();
            b->printf( "\n" );
            rvalue.prologue();
        }
        
        b->printf( "%*s", indent, "" );
        lvalue.value();
        b->printf( " = " );
        auto op = b->op( node->assignop );
        if ( op.second == XEC_LJ_NONE )
        {
            b->printf( "%s( ", op.first );
            if ( lvval == -1 )
                lvalue.value();
            else
                b->printf( "__t%d", lvval );
            b->printf( ", " );
            rvalue.value();
            b->printf( " )\n" );
        }
        else
        {
            if ( lvval == -1 )
                lvalue.value();
            else
                b->printf( "__t%d", lvval );
            b->printf( " %s ", op.first );
            rvalue.value();
            b->printf( "\n" );
        }
    }
}

void xec_lj_stmtvisitor::visit( xec_expr_assign_list* node, xec_lj_scope* scope, int indent )
{
    if ( node->assignop == XEC_ASTOP_DECLARE )
    {
        xec_lj_value rvalues;
        b->expr( &rvalues, node->rvalues, scope, indent );
        rvalues.prologue();
    
        b->printf( "%*slocal ", indent, "" );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            assert( node->lvalues.at( i )->kind == XEC_EXPR_LOCAL );
            xec_expr_local* local = (xec_expr_local*)node->lvalues.at( i );
            if ( i > 0 )
                b->printf( ", " );
            b->local( local->name->name );
        }
        b->printf( " = " );
        rvalues.values();
        b->printf( "\n" );
    }
    else if ( node->assignop == XEC_ASTOP_ASSIGN )
    {
        xec_lj_value rvalues;
        b->expr( &rvalues, node->rvalues, scope, indent );
        if ( rvalues.has_prologue() )
        {
            std::vector< xec_lj_lvprol > lvalues;
            lvalues.reserve( node->lvalues.size() );
            for ( size_t i = 0; i < node->lvalues.size(); ++i )
            {
                lvalues.emplace_back();
                b->lvprol( &lvalues.back(), node->lvalues.at( i ), scope, indent );
                lvalues.back().prologue();
            }
            
            rvalues.prologue();
            
            b->printf( "%*s", indent, "" );
            for ( size_t i = 0; i < node->lvalues.size(); ++i )
            {
                if ( i > 0 )
                    b->printf( ", " );
                lvalues.at( i ).value();
            }
        }
        else
        {
            std::vector< xec_lj_value > lvalues;
            lvalues.reserve( node->lvalues.size() );
            for ( size_t i = 0; i < node->lvalues.size(); ++i )
            {
                lvalues.emplace_back();
                b->expr( &lvalues.back(), node->lvalues.at( i ), scope, indent );
                lvalues.back().prologue();
            }
            
            b->printf( "%*s", indent, "" );
            for ( size_t i = 0; i < node->lvalues.size(); ++i )
            {
                if ( i > 0 )
                    b->printf( ", " );
                lvalues.at( i ).value();
            }
        }

        b->printf( " = " );
        rvalues.values();
        b->printf( "\n" );
    }
    else
    {
        std::vector< xec_lj_lvprol > lvalues;
        lvalues.reserve( node->lvalues.size() );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            lvalues.emplace_back();
            b->lvprol( &lvalues.back(), node->lvalues.at( i ), scope, indent );
            lvalues.back().prologue();
        }
        
        std::vector< int > lvvals;
        lvvals.reserve( node->lvalues.size() );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            lvvals.push_back( b->temporary( scope, indent ) );
            lvalues.at( i ).value();
            b->printf( "\n" );
        }
        
        xec_lj_value rvalues;
        b->expr( &rvalues, node->rvalues, scope, indent );
        rvalues.prologue();
        
        int declared = -1;
        std::vector< int > rvvals;
        rvvals.reserve( node->lvalues.size() );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            auto temporary = scope->temporary();
            rvvals.push_back( temporary.first );
            if ( temporary.second )
            {
                if ( declared != -1 )
                {
                    b->printf( ", __t%d", temporary.first );
                }
                else
                {
                    b->printf( "%*slocal __t%d", indent, "", temporary.first );
                    declared = (int)i;
                }
            }
        }
        if ( declared > 0 )
        {
            b->printf( "\n" );
        }
        if ( declared != 0 )
        {
            b->printf( "%*s", indent, "" );
            for ( size_t i = 0; i < node->lvalues.size(); ++i )
            {
                if ( i > 0 )
                    b->printf( ", __t%d", rvvals.at( i ) );
                else
                    b->printf( "__t%d", rvvals.at( i ) );
            }
        }
        b->printf( " = " );
        rvalues.values();
        b->printf( "\n" );
        
        auto op = b->op( node->assignop );
    
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            if ( i > 0 )
                b->printf( ", __t%d", rvvals.at( i ) );
            else
                b->printf( "__t%d", rvvals.at( i ) );
        }
        b->printf( " = " );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            int lvval = lvvals.at( i );
            int rvval = rvvals.at( i );
            if ( i > 0 )
                b->printf( ", " );
            if ( op.second == XEC_LJ_NONE )
                b->printf( "%s( __t%d, __t%d )", op.first, lvval, rvval );
            else
                b->printf( "__t%d %s __t%d", lvval, op.first, rvval );
        }
        b->printf( "\n" );

        b->printf( "%*s", indent, "" );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            if ( i > 0 )
                b->printf( ", " );
            lvalues.at( i ).value();
        }
        b->printf( " = " );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            if ( i > 0 )
                b->printf( ", __t%d", rvvals.at( i ) );
            else
                b->printf( "__t%d", rvvals.at( i ) );
        }
        b->printf( "\n" );
    }
}


void xec_lj_stmtvisitor::visit( xec_stmt_block* node, xec_lj_scope* scope, int indent )
{
    b->printf( "%*sdo\n", indent, "" );
    xec_lj_scope local;
    for ( size_t i = 0; i < node->stmts.size(); ++i )
    {
        local.begin();
        visit( node->stmts.at( i ), &local, indent + INDENT );
    }
    b->printf( "%*send\n", indent, "" );
}

void xec_lj_stmtvisitor::visit( xec_stmt_if* node, xec_lj_scope* scope, int indent )
{
    int endcount = 0;

    xec_lj_scope local;
    xec_lj_value condition;
    b->expr( &condition, node->condition, &local, indent );
    if ( condition.has_prologue() )
    {
        b->printf( "%*sdo\n", indent, "" );
        condition.prologue();
        endcount += 1;
    }
    
    b->printf( "%*sif __test( ", indent, "" );
    condition.value();
    b->printf( " ) then\n" );
    body( node->iftrue, &local, indent + INDENT );
    endcount += 1;

    while ( node->iffalse && node->iffalse->kind == XEC_STMT_IF )
    {
        node = (xec_stmt_if*)node->iffalse;
        xec_lj_scope local;
        xec_lj_value condition;
        if ( condition.has_prologue() )
        {
            b->printf( "%*selse\n", indent, "" );
            condition.prologue();
            b->printf( "%*sif __test( ", indent, "" );
            condition.value();
            b->printf( " ) then\n" );
            endcount += 1;
        }
        else
        {
            b->printf( "%*selseif __test( ", indent, "" );
            condition.value();
            b->printf( " ) then\n" );
        }
        
        body( node->iftrue, &local, indent + INDENT );
    }

    if ( node->iffalse )
    {
        xec_lj_scope local;
        b->printf( "%*selse\n", indent, "" );
        body( node->iffalse, &local, indent + INDENT );
    }
    
    for ( int i = 0; i < endcount; ++i )
    {
        b->printf( "%*send\n", indent, "" );
    }
}

void xec_lj_stmtvisitor::visit( xec_stmt_switch* node, xec_lj_scope* scope, int indent )
{
    b->printf( "%*sdo\n", indent, "" );
    
    xec_lj_scope local;
    xec_lj_value value;
    b->expr( &value, node->value, &local, indent );
    value.prologue();
    b->printf( "%*slocal __v = ", indent, "" );
    value.value();
    b->printf( "\n" );
    
    int caseindex = 0;
    bool hasdefault = false;
    for ( size_t i = 0; i < node->body->stmts.size(); ++i )
    {
        xec_ast_node* stmt = node->body->stmts.at( i );
        if ( stmt->kind == XEC_STMT_CASE )
        {
            xec_stmt_case* casestmt = (xec_stmt_case*)stmt;
            if ( ! casestmt->value )
            {
                hasdefault = true;
                continue;
            }
            
            local.begin();
            xec_lj_value value;
            b->expr( &value, node->value, &local, indent );
            value.prologue();
            b->printf( "%*sif __v == ", indent, "" );
            value.value();
            b->printf( " then goto ::__case%d:: end\n", caseindex );
            caseindex += 1;
        }
    }
    
    const char* defaultlabel = hasdefault ? "__default" : "__break";
    b->printf( "%*sgoto ::%s::", indent, "", defaultlabel );
    
    caseindex = 0;
    bool openblock = false;
    for ( size_t i = 0; i < node->body->stmts.size(); ++i )
    {
        xec_ast_node* stmt = node->body->stmts.at( i );
        if ( stmt->kind == XEC_STMT_CASE )
        {
            if ( openblock )
            {
                b->printf( "%*send\n", indent, "" );
                openblock = false;
            }
        
            xec_stmt_case* casestmt = (xec_stmt_case*)stmt;
            if ( casestmt->value )
            {
                b->printf( "%*s::__case%d::", indent, "", caseindex );
                caseindex += 1;
            }
            else
            {
                b->printf( "%*s::__default::", indent, "" );
            }
        }
        else
        {
            if ( ! openblock )
            {
                b->printf( "%*sdo\n", indent, "" );
                local.reset();
            }
            
            local.begin();
            visit( stmt, &local, indent + INDENT );
        }
    }
    
    if ( openblock )
    {
        b->printf( "%*send\n", indent, "" );
    }
    
    b->printf( "%*s::__break::\n", indent, "" );
    b->printf( "%*send\n", indent, "" );
    
}

void xec_lj_stmtvisitor::visit( xec_stmt_while* node, xec_lj_scope* scope, int indent )
{
    xec_lj_scope local;
    xec_lj_value condition;
    b->expr( &condition, node->condition, &local, indent );
    if ( condition.has_prologue() )
    {
        b->printf( "%*swhile true do\n", indent, "" );
        condition.prologue();
        b->printf( "%*sif not __test( ", indent, "" );
        condition.value();
        b->printf( " ) then break end\n" );
    }
    else
    {
        b->printf( "%*swhile __test( ", indent, "" );
        condition.value();
        b->printf( " ) do\n" );
    }
    
    body( node->body, &local, indent + INDENT );

    if ( node->scope->continued )
    {
        b->printf( "%*s::__continue::\n", indent, "" );
    }
    b->printf( "%*send\n", indent, "" );
}

void xec_lj_stmtvisitor::visit( xec_stmt_do* node, xec_lj_scope* scope, int indent )
{
    b->printf( "%*srepeat\n", indent, "" );
    
    xec_lj_scope local;
    body( node, &local, indent + INDENT );
    
    if ( node->scope->continued )
    {
        b->printf( "%*s::__continue::\n", indent, "" );
    }
    
    local.begin();
    xec_lj_value condition;
    b->expr( &condition, node->condition, &local, indent );
    condition.prologue();
    b->printf( "%*suntil not __test( ", indent, "" );
    condition.value();
    b->printf( " )\n" );
}

void xec_lj_stmtvisitor::visit( xec_stmt_foreach* node, xec_lj_scope* scope, int indent )
{
    xec_lj_scope local;
    xec_lj_value list;
    b->expr( &list, node->list, &local, indent );
    if ( list.has_prologue() )
    {
        b->printf( "%*sdo\n", indent, "" );
        list.prologue();
    }

    if ( node->declare )
    {
        b->printf( "%*sfor __fi", indent, "" );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            xec_ast_node* lvalue = node->lvalues.at( i );
            assert( lvalue->kind == XEC_EXPR_LOCAL );
            xec_expr_local* local = (xec_expr_local*)lvalue;
            b->printf( ", %s", local->name->name );
        }
        if ( node->eachkey )
        {
            b->printf( " in __eachkey( " );
            list.value();
            b->printf( " ) do\n" );
        }
        else
        {
            b->printf( " in " );
            list.value();
            b->printf( ":__each() do\n" );
        }
    }
    else
    {
        b->printf( "%*sfor __fi", indent, "" );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            b->printf( ", __f%d", (int)i );
        }
        if ( node->eachkey )
        {
            b->printf( " in __eachkey( " );
            list.value();
            b->printf( " ) do\n" );
        }
        else
        {
            b->printf( " in " );
            list.value();
            b->printf( ":__each() do\n" );
        }
        
        std::vector< xec_lj_value > lvalues;
        lvalues.reserve( node->lvalues.size() );
        
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            lvalues.emplace_back();
            b->expr( &lvalues.back(), node->lvalues.at( i ), &local, indent );
            lvalues.back().prologue();
        }
        
        b->printf( "%*s", indent, "" );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            if ( i > 0 )
            {
                b->printf( ", " );
            }
            
            lvalues.at( i ).value();
        }
        b->printf( " = " );
        for ( size_t i = 0; i < node->lvalues.size(); ++i )
        {
            if ( i > 0 )
            {
                b->printf( ", " );
            }
        
            b->printf( "__f%d", (int)i );
        }
    }
    
    body( node->body, &local, indent + INDENT );
    
    if ( node->scope->continued )
    {
        b->printf( "%*s::__continue::\n", indent, "" );
    }
    
    b->printf( "%*send\n", indent, "" );
    if ( list.has_prologue() )
    {
        b->printf( "%*send\n", indent, "" );
    }
}

void xec_lj_stmtvisitor::visit( xec_stmt_for* node, xec_lj_scope* scope, int indent )
{
    b->printf( "%*sdo\n", indent, "" );

    xec_lj_scope local;
    visit( node->init, &local, indent );
    
    local.begin();
    xec_lj_value condition;
    b->expr( &condition, node->condition, &local, indent );
    if ( condition.has_prologue() )
    {
        b->printf( "%*swhile true do\n", indent, "" );
        condition.prologue();
        b->printf( "%*sif not __test( ", indent, "" );
        condition.value();
        b->printf( " ) then break end\n" );
    }
    else
    {
        b->printf( "%*swhile __test( ", indent, "" );
        condition.value();
        b->printf( " ) do\n" );
    }
    
    body( node->body, &local, indent + INDENT );
    
    if ( node->scope->continued )
    {
        b->printf( "%*s::__continue::\n", indent, "" );
    }
    
    local.begin();
    visit( node->update, &local, indent );
    
    b->printf( "%*send\n", indent, "" );
    b->printf( "%*send\n", indent, "" );
}

void xec_lj_stmtvisitor::visit( xec_stmt_using* node, xec_lj_scope* scope, int indent )
{
    

}

void xec_lj_stmtvisitor::visit( xec_stmt_try* node, xec_lj_scope* scope, int indent )
{
    b->printf( "%*sdo local __ok, __e = __pcall( function()\n", indent, "" );
    
    xec_lj_scope local;
    visit( node->tstmt, &local, indent );
    
    b->printf( "%*send )\n", indent, "" );
    
    if ( node->clist.size() )
    {
        b->printf( "%*sif not __ok then\n", indent, "" );

        for ( size_t i = 0; i < node->clist.size(); ++i )
        {
            xec_ast_node* cnode = node->clist.at( i );
            assert( cnode->kind == XEC_STMT_CATCH );
            xec_stmt_catch* cstmt = (xec_stmt_catch*)cnode;
            
            const char* elseif = i > 0 ? "elseif" : "if";
            if ( cstmt->proto )
            {
                local.begin();
                xec_lj_value proto;
                b->expr( &proto, cstmt->proto, &local, indent );
                proto.prologue();
                b->printf( "%*s%s __is( __e, ", indent, "", elseif );
                proto.value();
                b->printf( " ) then\n" );
            }
            else
            {
                b->printf( "%*s%s true then\n", indent, "", elseif );
            }

            b->printf( "%*s__ok = true\n", indent, "" );

            xec_lj_scope local;
            if ( cstmt->declare )
            {
                assert( cstmt->lvalue->kind == XEC_EXPR_LOCAL );
                xec_expr_local* local = (xec_expr_local*)cstmt->lvalue;
                b->printf( "%*slocal %s = __e\n", indent, "", local->name->name );
            }
            else
            {
                xec_lj_value lvalue;
                b->expr( &lvalue, cstmt->lvalue, &local, indent );
                lvalue.prologue();
                lvalue.value();
                b->printf( " = __e\n" );
            }
            
            body( cstmt->body, &local, indent + INDENT );
        }

        b->printf( "%*send\n", indent, "" );
        b->printf( "%*send\n", indent, "" );
    }
    
    if ( node->fstmt )
    {
        visit( node->fstmt, &local, indent );
    }
    
    b->printf( "%*sif not __ok then __error( __e ) end\n", indent, "" );
    b->printf( "%*send\n", indent, "" );
}

void xec_lj_stmtvisitor::visit( xec_stmt_catch* node, xec_lj_scope* scope, int indent )
{
    assert( ! "unexpected catch" );
}

void xec_lj_stmtvisitor::visit( xec_stmt_delete* node, xec_lj_scope* scope, int indent )
{
//  do deletions happen all at once or one by one?
//    delete a.x, a.[ a.x ];
}

void xec_lj_stmtvisitor::visit( xec_stmt_case* node, xec_lj_scope* scope, int indent )
{
    assert( ! "unexpected case" );
}

void xec_lj_stmtvisitor::visit( xec_stmt_continue* node, xec_lj_scope* scope, int indent )
{
    b->printf( "%*sgoto __continue\n", indent, "" );
}

void xec_lj_stmtvisitor::visit( xec_stmt_break* node, xec_lj_scope* scope, int indent )
{
    if ( node->target->kind == XEC_SCOPE_SWITCH )
    {
        b->printf( "%*sgoto __break\n", indent, "" );
    }
    else
    {
        b->printf( "%*sdo break end\n", indent, "" );
    }
}

void xec_lj_stmtvisitor::visit( xec_stmt_return* node, xec_lj_scope* scope, int indent )
{
    xec_lj_value values;
    b->expr( &values, node->values, scope, indent );
    values.prologue();
    b->printf( "%*sdo return ", indent, "" );
    values.values();
    b->printf( " end\n" );
}

void xec_lj_stmtvisitor::visit( xec_stmt_throw* node, xec_lj_scope* scope, int indent )
{
    xec_lj_value value;
    b->expr( &value, node->value, scope, indent );
    value.prologue();
    b->printf( "%*s__error( ", indent, "" );
    value.value();
    b->printf( " )\n" );
}




void xec_lj_stmtvisitor::body( xec_ast_node* node, xec_lj_scope* scope, int indent )
{
    if ( node->kind == XEC_STMT_BLOCK )
    {
        xec_stmt_block* block = (xec_stmt_block*)node;
        for ( size_t i = 0; i < block->stmts.size(); ++i )
        {
            scope->begin();
            visit( block->stmts.at( i ), scope, indent );
        }
    }
    else
    {
        scope->begin();
        visit( node, scope, indent );
    }
}

