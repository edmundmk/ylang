//
//  xec_ast_print.cpp
//
//  Created by Edmund Kapusniak on 27/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_ast_visitor.h"



const char* xec_operator_name( xec_ast_opkind op )
{
    switch ( op )
    {
    case XEC_ASTOP_DECLARE:          return "[declare]";

    case XEC_ASTOP_ASSIGN:           return "=";
    case XEC_ASTOP_MULASSIGN:        return "*=";
    case XEC_ASTOP_DIVASSIGN:        return "/=";
    case XEC_ASTOP_MODASSIGN:        return "%=";
    case XEC_ASTOP_INTDIVASSIGN:     return "~=";
    case XEC_ASTOP_ADDASSIGN:        return "+=";
    case XEC_ASTOP_SUBASSIGN:        return "-=";
    case XEC_ASTOP_LSHIFTASSIGN:     return "<<=";
    case XEC_ASTOP_LRSHIFTASSIGN:    return ">>=";
    case XEC_ASTOP_ARSHIFTASSIGN:    return "~>>=";
    case XEC_ASTOP_BITANDASSIGN:     return "&=";
    case XEC_ASTOP_BITXORASSIGN:     return "^=";
    case XEC_ASTOP_BITORASSIGN:      return "|=";
    
    case XEC_ASTOP_PREINC:           return "++[preop]";
    case XEC_ASTOP_PREDEC:           return "--[preop]";
    
    case XEC_ASTOP_POSTINC:          return "[postop]++";
    case XEC_ASTOP_POSTDEC:          return "[postop]--";

    case XEC_ASTOP_POSITIVE:         return "+[unary]";
    case XEC_ASTOP_NEGATIVE:         return "-[unary]";
    case XEC_ASTOP_LOGICNOT:         return "![unary]";
    case XEC_ASTOP_BITNOT:           return "~[unary]";
    
    case XEC_ASTOP_MULTIPLY:         return "*";
    case XEC_ASTOP_DIVIDE:           return "/";
    case XEC_ASTOP_MODULUS:          return "%";
    case XEC_ASTOP_INTDIV:           return "~";
    case XEC_ASTOP_ADD:              return "+";
    case XEC_ASTOP_SUBTRACT:         return "-";
    case XEC_ASTOP_LSHIFT:           return "<<";
    case XEC_ASTOP_LRSHIFT:          return ">>";
    case XEC_ASTOP_ARSHIFT:          return "~>>";
    case XEC_ASTOP_BITAND:           return "&";
    case XEC_ASTOP_BITXOR:           return "^";
    case XEC_ASTOP_BITOR:            return "|";
    case XEC_ASTOP_CONCATENATE:      return "..";
    
    case XEC_ASTOP_EQUAL:            return "==";
    case XEC_ASTOP_NOTEQUAL:         return "!=";
    case XEC_ASTOP_LESS:             return "<";
    case XEC_ASTOP_GREATER:          return ">";
    case XEC_ASTOP_LESSEQUAL:        return "<=";
    case XEC_ASTOP_GREATEREQUAL:     return ">=";
    case XEC_ASTOP_IN:               return "in";
    case XEC_ASTOP_NOTIN:            return "!in";
    case XEC_ASTOP_IS:               return "is";
    case XEC_ASTOP_NOTIS:            return "!is";
    
    case XEC_ASTOP_LOGICAND:         return "&&";
    case XEC_ASTOP_LOGICXOR:         return "^^";
    case XEC_ASTOP_LOGICOR:          return "||";
    }
    
    return "??";
}





static const int INDENT = 4;



class xec_ast_printer : public xec_ast_visitor< xec_ast_printer, void, int >
{
public:


    using xec_ast_visitor< xec_ast_printer, void, int >::visit;
    
    void print_func( xec_ast_func* node );

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
    void visit( xec_new_array* node, int indent );
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
    void visit( xec_name_name* node, int indent );
    void visit( xec_name_qual* node, int indent );
    void visit( xec_name_list* node, int indent );

    void print_scope( xec_ast_scope* scope, int indent );
    void print_name( xec_ast_name* name );
    
};



void xec_ast_print( xec_ast* root )
{
    xec_ast_printer printer;
    for ( size_t i = 0; i < root->functions.size(); ++i )
    {
        printer.print_func( root->functions.at( i ) );
    }
}



void xec_ast_printer::print_func( xec_ast_func* node )
{
    int indent = 0;
    
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


void xec_ast_printer::visit( xec_ast_func* node, int indent )
{
    printf( "%*sfunction %s %p\n", indent, "", node->funcname, node );
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
    printf( "%*s\"", indent, "" );
    for ( size_t i = 0; i < node->length; ++i )
    {
        char c = node->string[ i ];
        if ( c >= 0x20 && c <= 0x7E )
        {
            printf( "%c", c );
        }
        else switch ( c )
        {
        case 0x07:  printf( "\\a" );        break;
        case 0x08:  printf( "\\b" );        break;
        case 0x09:  printf( "\\t" );        break;
        case 0x0A:  printf( "\\n" );        break;
        case 0x0B:  printf( "\\v" );        break;
        case 0x0C:  printf( "\\f" );        break;
        case 0x0D:  printf( "\\r" );        break;
        case '"':   printf( "\\\"" );       break;
        default:    printf( "\\x%02X", c ); break;
        }
    }
    printf( "\"\n" );
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
}

void xec_ast_printer::visit( xec_new_array* node, int indent )
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
    if ( node->arguments )
        visit( node->arguments, indent );
}

void xec_ast_printer::visit( xec_expr_yield* node, int indent ) 
{
    printf( "%*syield:\n", indent, "" );
    indent += INDENT;
    if ( node->unpack )
        printf( "%*sunpack: true\n", indent, "" );
    if ( node->arguments )
        visit( node->arguments, indent );
}

void xec_ast_printer::visit( xec_expr_vararg* node, int indent ) 
{
    printf( "%*s...\n", indent, "" );
}

void xec_ast_printer::visit( xec_expr_unpack* node, int indent ) 
{
    printf( "%*sunpack:\n", indent, "" );
    visit( node->array, indent + INDENT );
}

void xec_ast_printer::visit( xec_expr_list* node, int indent ) 
{
    printf( "%*svalues:\n", indent, "" );
    for ( size_t i = 0; i < node->values.size(); ++i )
        visit( node->values[ i ], indent + INDENT );
    if ( node->final )
    {
        printf( "%*s  (final)\n", indent, "" );
        visit( node->final, indent + INDENT );
    }
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
}

void xec_ast_printer::visit( xec_stmt_switch* node, int indent ) 
{
    printf( "%*sswitch %p:", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->value, indent );
    visit( node->body, indent );
}

void xec_ast_printer::visit( xec_stmt_while* node, int indent ) 
{
    printf( "%*swhile %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->condition, indent );
    visit( node->body, indent );
}

void xec_ast_printer::visit( xec_stmt_do* node, int indent ) 
{
    printf( "%*sdo %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->body, indent );
    visit( node->condition, indent );
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
}

void xec_ast_printer::visit( xec_stmt_using* node, int indent ) 
{
    printf( "%*susing %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->uvalue, indent );
    visit( node->body, indent );
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
    printf( "%*scontinue %p %p\n", indent, "", node->scope, node->target );
}

void xec_ast_printer::visit( xec_stmt_break* node, int indent ) 
{
    printf( "%*sbreak %p %p\n", indent, "", node->scope, node->target );
}

void xec_ast_printer::visit( xec_stmt_return* node, int indent ) 
{
    printf( "%*sreturn %p:\n", indent, "", node->scope );
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

void xec_ast_printer::visit( xec_name_name* node, int indent )
{
    printf( "%*s[!!] '%s'\n", indent, "", node->name );
}

void xec_ast_printer::visit( xec_name_qual* node, int indent )
{
    printf( "%*s[!!] qual:\n", indent, "" );
    visit( node->scope, indent + INDENT );
    printf( "%*s'%s'\n", indent + INDENT, "", node->name );
}

void xec_ast_printer::visit( xec_name_list* node, int indent )
{
    printf( "%*s[!!] list:\n", indent, "" );
    for ( size_t i = 0; i < node->names.size(); ++i )
        visit( node->names[ i ], indent + INDENT );
}





void xec_ast_printer::print_scope( xec_ast_scope* scope, int indent )
{
    const char* kind_name = "??";
    switch ( scope->kind )
    {
    case XEC_SCOPE_SCRIPT:      kind_name = "SCRIPT";       break;
    case XEC_SCOPE_BLOCK:       kind_name = "BLOCK";        break;
    case XEC_SCOPE_SWITCH:      kind_name = "SWITCH";       break;
    case XEC_SCOPE_IMPLICIT:    kind_name = "IMPLICIT";     break;
    case XEC_SCOPE_OBJECT:      kind_name = "OBJECT";       break;
    case XEC_SCOPE_IMPLIED:     kind_name = "IMPLIED";      break;
    }

    printf( "%*sscope %s %p:\n", indent, "", kind_name, scope );
    indent += INDENT;
    
    printf( "%*souter     : %p\n", indent, "", scope->outer );
    printf( "%*snode      : %p\n", indent, "", scope->node );
    printf( "%*sfunc      : %p\n", indent, "", scope->func );
    printf( "%*sblock     : %p\n", indent, "", scope->block );
    printf( "%*scontinued : %s\n", indent, "",
                    scope->continued ? "true" : "false" );
    printf( "%*sdowhile   : %s\n", indent, "",
                    scope->dowhile ? "true" : "false" );
    
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
    }
}


void xec_ast_printer::print_name( xec_ast_name* name )
{
    printf( "%s %p", name->name, name );

    if ( name->prototype )
    {
        xec_ast_prototype* proto = name->prototype;
        xec_name_list* params = proto->parameters;
        printf( "(" );
        if ( params )
        {
            for ( size_t i = 0; i < params->names.size(); ++i )
            {
                xec_name_name* unqual = params->names[ i ];
                if ( i > 0 )
                    printf( "," );
                printf( " %s", unqual->name );
            }
            if ( params->varargs )
            {
                if ( params->names.size() )
                    printf( "," );
                printf( " ..." );
            }
            if ( params->names.size() || params->varargs )
                printf( " " );
        }
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
    
    if ( name->continued )
    {
        printf( " continued" );
    }
}




