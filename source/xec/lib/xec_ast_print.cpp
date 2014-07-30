//
//  xec_ast_print.cpp
//
//  Created by Edmund Kapusniak on 27/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ast_print.h"
#include "xec_ast_visitor.h"



static const int INDENT = 4;



class xec_ast_printer : public xec_ast_visitor< xec_ast_printer, void, int >
{
public:


    using xec_ast_visitor< xec_ast_printer, void, int >::visit;
    

    void visit( xec_ast_func* node, int indent );
    void visit( xec_expr_null* node, int indent );
    void visit( xec_expr_bool* node, int indent );
    void visit( xec_expr_number* node, int indent );
    void visit( xec_expr_string* node, int indent );
    void visit( xec_expr_local* node, int indent );
    void visit( xec_expr_global* node, int indent );
    void visit( xec_expr_upref* node, int indent );
    void visit( xec_expr_objref* node, int indent );
    void visit( xec_expr_key* node, int indent );
    void visit( xec_expr_inkey* node, int indent );
    void visit( xec_expr_index* node, int indent );
    void visit( xec_expr_preop* node, int indent );
    void visit( xec_expr_postop* node, int indent );
    void visit( xec_expr_unary* node, int indent );
    void visit( xec_expr_binary* node, int indent );
    void visit( xec_expr_compare* node, int indent );
    void visit( xec_expr_logical* node, int indent );
    void visit( xec_expr_qmark* node, int indent );
    void visit( xec_new_new* node, int indent );
    void visit( xec_new_object* node, int indent );
    void visit( xec_new_list* node, int indent );
    void visit( xec_new_table* node, int indent );
    void visit( xec_expr_mono* node, int indent );
    void visit( xec_expr_call* node, int indent );
    void visit( xec_expr_yield* node, int indent );
    void visit( xec_expr_vararg* node, int indent );
    void visit( xec_expr_unpack* node, int indent );
    void visit( xec_expr_list* node, int indent );
    void visit( xec_expr_assign* node, int indent );
    void visit( xec_expr_assign_list* node, int indent );
    void visit( xec_stmt_block* node, int indent );
    void visit( xec_stmt_if* node, int indent );
    void visit( xec_stmt_switch* node, int indent );
    void visit( xec_stmt_while* node, int indent );
    void visit( xec_stmt_do* node, int indent );
    void visit( xec_stmt_foreach* node, int indent );
    void visit( xec_stmt_for* node, int indent );
    void visit( xec_stmt_using* node, int indent );
    void visit( xec_stmt_try* node, int indent );
    void visit( xec_stmt_catch* node, int indent );
    void visit( xec_stmt_delete* node, int indent );
    void visit( xec_stmt_case* node, int indent );
    void visit( xec_stmt_continue* node, int indent );
    void visit( xec_stmt_break* node, int indent );
    void visit( xec_stmt_return* node, int indent );
    void visit( xec_stmt_throw* node, int indent );
    void visit( xec_unqual_name* node, int indent );
    void visit( xec_unqual_qual* node, int indent );
    void visit( xec_unqual_list* node, int indent );
    void visit( xec_unqual_proto* node, int indent );

    void print_scope( xec_ast_scope* scope, int indent );
    void print_name( xec_ast_name* name );
    
};



void xec_ast_print( xec_ast_func* script )
{
    xec_ast_printer printer;
    printer.visit( script, 0 );
}



void xec_ast_printer::visit( xec_ast_func* node, int indent )
{
    printf( "%*sfunction %s %p:\n", indent, "", node->funcname, node );
    indent += INDENT;

    print_scope( node->scope, indent );

    if ( node->upvals.size() )
    {
        printf( "%*supvals:\n", indent, "" );
        indent += INDENT;
        for ( size_t i = 0; i < node->upvals.size(); ++i )
        {
            xec_ast_upval* upval = &node->upvals[ i ];
            printf( "%*s[ %d ] ", indent, "", (int)i );
            switch ( upval->kind )
            {
            case XEC_UPVAL_LOCAL:
                printf( "local %s %p \n", upval->local->name, upval->local );
                break;

            case XEC_UPVAL_OBJECT:
                printf( "object %p\n", upval->object );
                break;
            
            case XEC_UPVAL_UPVAL:
                printf( "upval %d\n", upval->upval );
                break;
            }
        }
        indent -= INDENT;
    }

    if ( node->memberof )
    {
        printf( "%*smemberof: %p\n", indent, "", node->memberof );
    }

    if ( node->thisname )
    {
        printf( "%*sthisname: %s %p\n", indent, "",
                        node->thisname->name, node->thisname );
    }

    printf( "%*sparameters:\n", indent, "" );
    indent += INDENT;
    for ( size_t i = 0; i < node->parameters.size(); ++i )
    {
        xec_ast_name* name = node->parameters[ i ];
        printf( "%*s%s %p\n", indent, "", name->name, name );
    }
    if ( node->varargs )
    {
        printf( "%*s...\n", indent, "" );
    }
    indent -= INDENT;
    
    if ( node->coroutine )
    {
        printf( "%*scoroutine: true\n", indent, "" );
    }
    
    visit( node->block, indent );
}

void xec_ast_printer::visit( xec_expr_null* node, int indent ) 
{
    printf( "%*snull\n", indent, "" );
}

void xec_ast_printer::visit( xec_expr_bool* node, int indent ) 
{
    printf( "%*s%s\n", indent, "", node->value ? "true" : "false" );
}

void xec_ast_printer::visit( xec_expr_number* node, int indent ) 
{
    printf( "%*s%g\n", indent, "", node->value );
}

void xec_ast_printer::visit( xec_expr_string* node, int indent ) 
{
    printf( "%*s\"%.*s\"\n", indent, "", (int)node->length, node->string );
}

void xec_ast_printer::visit( xec_expr_local* node, int indent ) 
{
    printf( "%*s%s %p\n", indent, "", node->name->name, node->name );
}

void xec_ast_printer::visit( xec_expr_global* node, int indent ) 
{
    printf( "%*sglobal.%s\n", indent, "", node->name );
}

void xec_ast_printer::visit( xec_expr_upref* node, int indent ) 
{
    printf( "%*supval %d\n", indent, "", node->index );
}

void xec_ast_printer::visit( xec_expr_objref* node, int indent ) 
{
    printf( "%*sobject %p\n", indent, "", node->object );
}

void xec_ast_printer::visit( xec_expr_key* node, int indent ) 
{
    printf( "%*skey:\n", indent, "" );
    visit( node->object, indent + INDENT );
    printf( "%*s'%s'\n", indent + INDENT, "", node->key );
}

void xec_ast_printer::visit( xec_expr_inkey* node, int indent ) 
{
    printf( "%*sinkey:\n", indent, "" );
    visit( node->object, indent + INDENT );
    visit( node->key, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_index* node, int indent ) 
{
    printf( "%*sindex:\n", indent, "" );
    visit( node->object, indent + INDENT );
    visit( node->index, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_preop* node, int indent ) 
{
    printf( "%*s%s\n", indent, "", xec_operator_name( node->opkind ) );
    visit( node->lvalue, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_postop* node, int indent ) 
{
    printf( "%*s%s\n", indent, "", xec_operator_name( node->opkind ) );
    visit( node->lvalue, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_unary* node, int indent ) 
{
    printf( "%*s%s\n", indent, "", xec_operator_name( node->opkind ) );
    visit( node->operand, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_binary* node, int indent ) 
{
    printf( "%*s%s\n", indent, "", xec_operator_name( node->opkind ) );
    visit( node->lhs, indent + INDENT );
    visit( node->rhs, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_compare* node, int indent ) 
{
    printf( "%*scompare:\n", indent, "" );
    indent += INDENT;
    
    visit( node->first, indent );
    assert( node->opkinds.size() == node->terms.size() );
    for ( size_t i = 0; i < node->terms.size(); ++i )
    {
        const char* opname = xec_operator_name( node->opkinds[ i ] );
        printf( "%*s%s\n", indent, "", opname );
        visit( node->terms[ i ], indent );
    }
    
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_expr_logical* node, int indent ) 
{
    printf( "%*s%s\n", indent, "", xec_operator_name( node->opkind ) );
    visit( node->lhs, indent + INDENT );
    visit( node->rhs, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_qmark* node, int indent ) 
{
    printf( "%*s?:\n", indent, "" );
    visit( node->condition, indent + INDENT );
    visit( node->iftrue, indent + INDENT );
    visit( node->iffalse, indent + INDENT );
}

void xec_ast_printer::visit( xec_new_new* node, int indent ) 
{
    printf( "%*snew:\n", indent, "" );
    visit( node->proto, indent + INDENT );
    visit( node->arguments, indent + INDENT );
}

void xec_ast_printer::visit( xec_new_object* node, int indent ) 
{
    printf( "%*sobject %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    if ( node->upval )
        printf( "%*supval: true\n", indent, "" );
    if ( node->proto )
        visit( node->proto, indent );
    else
        printf( "%*s[no prototype]\n", indent, "" );
    printf( "%*smembers:\n", indent, "" );
    for ( size_t i = 0; i < node->members.size(); ++i )
        visit( node->members[ i ], indent + INDENT );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_new_list* node, int indent ) 
{
    printf( "%*slist:\n", indent, "" );
    for ( size_t i = 0; i < node->values.size(); ++i )
        visit( node->values[ i ], indent + INDENT );
    if ( node->final )
        visit( node->final, indent + INDENT );
}

void xec_ast_printer::visit( xec_new_table* node, int indent ) 
{
    printf( "%*stable:\n", indent, "" );
    indent += INDENT;
    for ( size_t i = 0; i < node->elements.size(); ++i )
    {
        printf( "%*selement:\n", indent, "" );
        visit( node->elements[ i ].key, indent + INDENT );
        visit( node->elements[ i ].value, indent + INDENT );
    }
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_expr_mono* node, int indent ) 
{
    printf( "%*smono:\n", indent, "" );
    visit( node->expr, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_call* node, int indent ) 
{
    printf( "%*scall:\n", indent, "" );
    indent += INDENT;
    if ( node->yieldcall )
        printf( "%*syieldcall: true\n", indent, "" );
    if ( node->unpack )
        printf( "%*sunpack: true\n", indent, "" );
    visit( node->function, indent );
    visit( node->arguments, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_expr_yield* node, int indent ) 
{
    printf( "%*syield:\n", indent, "" );
    indent += INDENT;
    if ( node->unpack )
        printf( "%*sunpack: true\n", indent, "" );
    visit( node->arguments, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_expr_vararg* node, int indent ) 
{
    printf( "%*s...\n", indent, "" );
}

void xec_ast_printer::visit( xec_expr_unpack* node, int indent ) 
{
    printf( "%*sunpack:\n", indent, "" );
    visit( node->list, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_list* node, int indent ) 
{
    printf( "%*svalues:\n", indent, "" );
    for ( size_t i = 0; i < node->values.size(); ++i )
        visit( node->values[ i ], indent + INDENT );
    if ( node->final )
        visit( node->final, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_assign* node, int indent ) 
{
    printf( "%*s%s\n", indent, "", xec_operator_name( node->assignop ) );
    visit( node->lvalue, indent + INDENT );
    visit( node->rvalue, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_assign_list* node, int indent ) 
{
    printf( "%*s%s\n", indent, "", xec_operator_name( node->assignop ) );
    indent += INDENT;
    printf( "%*slvalues:\n", indent, "" );
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
        visit( node->lvalues[ i ], indent + INDENT );
    visit( node->rvalues, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_block* node, int indent ) 
{
    printf( "%*sblock %p:\n", indent, "", node );
    indent += INDENT;
    
    if ( node->scope )
    {
        print_scope( node->scope, indent );
    }
    
    for ( size_t i = 0; i < node->stmts.size(); ++i )
    {
        visit( node->stmts[ i ], indent );
    }
    
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_if* node, int indent ) 
{
    printf( "%*sif %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->condition, indent );
    visit( node->iftrue, indent );
    if ( node->iffalse )
        visit( node->iffalse, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_switch* node, int indent ) 
{
    printf( "%*sswitch %p:", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->value, indent );
    visit( node->body, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_while* node, int indent ) 
{
    printf( "%*swhile %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->condition, indent );
    visit( node->body, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_do* node, int indent ) 
{
    printf( "%*sdo %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->body, indent );
    visit( node->condition, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_foreach* node, int indent ) 
{
    printf( "%*sforeach %p:\n", indent, "", node );
    indent += INDENT;
    
    print_scope( node->scope, indent );

    printf( "%*s%s:\n", indent, "", node->declare ? "var" : "lvalues" );
    indent += INDENT;
    for ( size_t i = 0; i < node->lvalues.size(); ++i )
        visit( node->lvalues[ i ], indent );
    indent -= INDENT;
    
    printf( "%*s%s:\n", indent, "", node->eachkey ? "eachkey" : "eachindex" );
    visit( node->list, indent + INDENT );
    visit( node->body, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_for* node, int indent ) 
{
    printf( "%*sfor %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    if ( node->init )
        visit( node->init, indent );
    else
        printf( "%*s[no init]\n", indent, "" );
    if ( node->condition )
        visit( node->condition, indent );
    else
        printf( "%*s[no condition]\n", indent, "" );
    if ( node->update )
        visit( node->update, indent );
    else
        printf( "%*s[no update]\n", indent, "" );
    visit( node->body, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_using* node, int indent ) 
{
    printf( "%*susing %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->uvalue, indent );
    visit( node->body, indent );
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_try* node, int indent ) 
{
    printf( "%*stry %p:\n", indent, "", node );
    visit( node->tstmt, indent + INDENT );
    for ( size_t i = 0; i < node->clist.size(); ++i )
    {
        visit( node->clist[ i ], indent );
    }
    if ( node->fstmt )
    {
        printf( "%*sfinally:\n", indent, "" );
        visit( node->fstmt, indent + INDENT );
    }
}

void xec_ast_printer::visit( xec_stmt_catch* node, int indent ) 
{
    printf( "%*scatch %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    
    printf( "%*s%s:\n", indent, "", node->declare ? "var" : "lvalue" );
    indent += INDENT;
    visit( node->lvalue, indent );
    if ( node->proto )
        visit( node->proto, indent );
    else
        printf( "%*s[no prototype]\n", indent, "" );
    indent -= INDENT;
    visit( node->body, indent );
    
    indent -= INDENT;
}

void xec_ast_printer::visit( xec_stmt_delete* node, int indent ) 
{
    printf( "%*sdelete:\n", indent, "" );
    for ( size_t i = 0; i < node->delvals.size(); ++i )
        visit( node->delvals[ i ], indent + INDENT );
}

void xec_ast_printer::visit( xec_stmt_case* node, int indent ) 
{
    printf( "%*scase:\n", indent, "" );
    if ( node->value )
        visit( node->value, indent + INDENT );
    else
        printf( "%*s[default]\n", indent + INDENT, "" );
}

void xec_ast_printer::visit( xec_stmt_continue* node, int indent ) 
{
    printf( "%*scontinue %p\n", indent, "", node->target );
}

void xec_ast_printer::visit( xec_stmt_break* node, int indent ) 
{
    printf( "%*sbreak %p\n", indent, "", node->target );
}

void xec_ast_printer::visit( xec_stmt_return* node, int indent ) 
{
    printf( "%*sreturn:\n", indent, "" );
    if ( node->values )
        visit( node->values, indent + INDENT );
    else
        printf( "%*s[null]\n", indent + INDENT, "" );
}

void xec_ast_printer::visit( xec_stmt_throw* node, int indent ) 
{
    printf( "%*sthrow:\n", indent, "" );
    visit( node->value, indent + INDENT );
}

void xec_ast_printer::visit( xec_unqual_name* node, int indent ) 
{
    printf( "%*s[!!] '%s'\n", indent, "", node->name );
}

void xec_ast_printer::visit( xec_unqual_qual* node, int indent ) 
{
    printf( "%*s[!!] qual:\n", indent, "" );
    visit( node->scope, indent + INDENT );
    printf( "%*s'%s'\n", indent + INDENT, "", node->name );
}

void xec_ast_printer::visit( xec_unqual_list* node, int indent ) 
{
    printf( "%*s[!!] list:\n", indent, "" );
    for ( size_t i = 0; i < node->names.size(); ++i )
        visit( node->names[ i ], indent + INDENT );
}

void xec_ast_printer::visit( xec_unqual_proto* node, int indent ) 
{
    printf( "%*s[!!] proto:\n", indent, "" );
    visit( node->name, indent + INDENT );
    visit( node->params, indent + INDENT );
    if ( node->coroutine )
        printf( "%*scoroutine: true\n", indent + INDENT, "" );
}




void xec_ast_printer::print_scope( xec_ast_scope* scope, int indent )
{
    const char* kind_name = "??";
    switch ( scope->kind )
    {
    case XEC_SCOPE_SCRIPT:  kind_name = "SCRIPT";   break;
    case XEC_SCOPE_BLOCK:   kind_name = "BLOCK";    break;
    case XEC_SCOPE_OBJECT:  kind_name = "OBJECT";   break;
    case XEC_SCOPE_IMPLIED: kind_name = "IMPLIED";  break;
    }

    printf( "%*sscope %s %p:\n", indent, "", kind_name, scope );
    indent += INDENT;
    
    printf( "%*souter: %p\n", indent, "", scope->outer );
    printf( "%*snode : %p\n", indent, "", scope->node );
    printf( "%*sfunc : %p\n", indent, "", scope->func );
    printf( "%*sblock: %p\n", indent, "", scope->block );
    
    if ( scope->implied.size() )
    {
        printf( "%*simplied:\n", indent, "" );
        indent += INDENT;
        for ( auto i = scope->implied.begin(); i != scope->implied.end(); ++i )
        {
            printf( "%*s%s ->", indent, "", i->first.c_str() );
            if ( i->second->node )
            {
                // Object scope, will appear later.
                printf( " scope %p\n", i->second );
            }
            else
            {
                // Implied scope, won't appear later.
                printf( "\n" );
                print_scope( i->second, indent + INDENT );
            }
        }
        indent -= INDENT;
    }
    
    if ( scope->names.size() )
    {
        printf( "%*snames:\n", indent, "" );
        indent += INDENT;
        for ( auto i = scope->names.begin(); i != scope->names.end(); ++i )
        {
            printf( "%*s%s -> ", indent, "",  i->first.c_str() );
            print_name( i->second );
            printf( "\n" );
        }
        indent -= INDENT;
    }
    
    if ( scope->decls.size() )
    {
        printf( "%*sdecls:\n", indent, "" );
        indent += INDENT;
        for ( size_t i = 0; i < scope->decls.size(); ++i )
        {
            printf( "%*s", indent, "" );
            print_name( scope->decls[ i ] );
            printf( "\n" );
        }
        indent -= INDENT;
    }
}


void xec_ast_printer::print_name( xec_ast_name* name )
{
    printf( "%s %p", name->name, name );

    if ( name->prototype )
    {
        xec_ast_prototype* proto = name->prototype;
        printf( "(" );
        for ( size_t i = 0; i < proto->parameters.size(); ++i )
        {
            xec_unqual_name* unqual = proto->parameters[ i ];
            if ( i > 0 )
                printf( "," );
            printf( " %s", unqual->name );
        }
        if ( proto->varargs )
        {
            if ( proto->parameters.size() )
                printf( "," );
            printf( " ..." );
        }
        if ( proto->parameters.size() || proto->varargs )
            printf( " " );
        printf( ")" );
        
        if ( proto->coroutine )
        {
            printf( " coroutine" );
        }
    }
    
    if ( name->superthis )
    {
        printf( " superthis %p", name->superthis );
    }
    
    if ( name->upval )
    {
        printf( " upval" );
    }
}




