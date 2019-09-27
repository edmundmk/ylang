//
//  ast_print.cpp
//
//  Created by Edmund Kapusniak on 27/07/2014.
//  Copyright © 2014 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#include "ast.h"
#include "ast_visitor.h"

namespace yl
{

static const char* operator_name( ast_op_kind op )
{
    switch ( op )
    {
    case AST_OP_DECLARE:          return "[declare]";

    case AST_OP_ASSIGN:           return "=";
    case AST_OP_MULASSIGN:        return "*=";
    case AST_OP_DIVASSIGN:        return "/=";
    case AST_OP_MODASSIGN:        return "%=";
    case AST_OP_INTDIVASSIGN:     return "~=";
    case AST_OP_ADDASSIGN:        return "+=";
    case AST_OP_SUBASSIGN:        return "-=";
    case AST_OP_LSHIFTASSIGN:     return "<<=";
    case AST_OP_LRSHIFTASSIGN:    return ">>=";
    case AST_OP_ARSHIFTASSIGN:    return "~>>=";
    case AST_OP_BITANDASSIGN:     return "&=";
    case AST_OP_BITXORASSIGN:     return "^=";
    case AST_OP_BITORASSIGN:      return "|=";

    case AST_OP_PREINC:           return "++[preop]";
    case AST_OP_PREDEC:           return "--[preop]";

    case AST_OP_POSTINC:          return "[postop]++";
    case AST_OP_POSTDEC:          return "[postop]--";

    case AST_OP_POSITIVE:         return "+[unary]";
    case AST_OP_NEGATIVE:         return "-[unary]";
    case AST_OP_LOGICNOT:         return "![unary]";
    case AST_OP_BITNOT:           return "~[unary]";

    case AST_OP_MULTIPLY:         return "*";
    case AST_OP_DIVIDE:           return "/";
    case AST_OP_MODULUS:          return "%";
    case AST_OP_INTDIV:           return "~";
    case AST_OP_ADD:              return "+";
    case AST_OP_SUBTRACT:         return "-";
    case AST_OP_LSHIFT:           return "<<";
    case AST_OP_LRSHIFT:          return ">>";
    case AST_OP_ARSHIFT:          return "~>>";
    case AST_OP_BITAND:           return "&";
    case AST_OP_BITXOR:           return "^";
    case AST_OP_BITOR:            return "|";
    case AST_OP_CONCATENATE:      return "..";

    case AST_OP_EQUAL:            return "==";
    case AST_OP_NOTEQUAL:         return "!=";
    case AST_OP_LESS:             return "<";
    case AST_OP_GREATER:          return ">";
    case AST_OP_LESSEQUAL:        return "<=";
    case AST_OP_GREATEREQUAL:     return ">=";
    case AST_OP_IS:               return "is";
    case AST_OP_NOTIS:            return "!is";

    case AST_OP_LOGICAND:         return "&&";
    case AST_OP_LOGICXOR:         return "^^";
    case AST_OP_LOGICOR:          return "||";
    }

    return "??";
}


static const int INDENT = 4;

struct ast_printer : public ast_visitor< ast_printer, void, int >
{
    using ast_visitor< ast_printer, void, int >::visit;

    void print_func( ast_function* node );

    void visit( ast_function* node, int indent );
    void visit( ast_expr_null* node, int indent );
    void visit( ast_expr_bool* node, int indent );
    void visit( ast_expr_number* node, int indent );
    void visit( ast_expr_string* node, int indent );
    void visit( ast_expr_local* node, int indent );
    void visit( ast_expr_global* node, int indent );
    void visit( ast_expr_upref* node, int indent );
    void visit( ast_expr_objref* node, int indent );
    void visit( ast_expr_superof* node, int indent );
    void visit( ast_expr_key* node, int indent );
    void visit( ast_expr_inkey* node, int indent );
    void visit( ast_expr_index* node, int indent );
    void visit( ast_expr_responds* node, int indent );
    void visit( ast_expr_inresponds* node, int indent );
    void visit( ast_expr_preop* node, int indent );
    void visit( ast_expr_postop* node, int indent );
    void visit( ast_expr_unary* node, int indent );
    void visit( ast_expr_binary* node, int indent );
    void visit( ast_expr_compare* node, int indent );
    void visit( ast_expr_logical* node, int indent );
    void visit( ast_expr_qmark* node, int indent );
    void visit( ast_make_new* node, int indent );
    void visit( ast_make_object* node, int indent );
    void visit( ast_make_array* node, int indent );
    void visit( ast_make_table* node, int indent );
    void visit( ast_expr_mono* node, int indent );
    void visit( ast_expr_call* node, int indent );
    void visit( ast_expr_yield* node, int indent );
    void visit( ast_expr_vararg* node, int indent );
    void visit( ast_expr_unpack* node, int indent );
    void visit( ast_expr_list* node, int indent );
    void visit( ast_expr_assign* node, int indent );
    void visit( ast_expr_assign_list* node, int indent );
    void visit( ast_stmt_block* node, int indent );
    void visit( ast_stmt_if* node, int indent );
    void visit( ast_stmt_switch* node, int indent );
    void visit( ast_stmt_while* node, int indent );
    void visit( ast_stmt_do* node, int indent );
    void visit( ast_stmt_foreach* node, int indent );
    void visit( ast_stmt_for* node, int indent );
    void visit( ast_stmt_using* node, int indent );
    void visit( ast_stmt_try* node, int indent );
    void visit( ast_stmt_catch* node, int indent );
    void visit( ast_stmt_delete* node, int indent );
    void visit( ast_stmt_case* node, int indent );
    void visit( ast_stmt_continue* node, int indent );
    void visit( ast_stmt_break* node, int indent );
    void visit( ast_stmt_return* node, int indent );
    void visit( ast_stmt_throw* node, int indent );
    void visit( ast_bind_name* node, int indent );
    void visit( ast_bind_qual* node, int indent );
    void visit( ast_bind_list* node, int indent );

    void print_scope( ast_scope* scope, int indent );
    void print_name( ast_name* name );

};

void ast_print( ast_script* ast )
{
    ast_printer printer;
    for ( size_t i = 0; i < ast->functions.size(); ++i )
    {
        printer.print_func( ast->functions.at( i ) );
    }
}

void ast_printer::print_func( ast_function* node )
{
    int indent = 0;

    printf( "%*sfunction %s %p:\n", indent, "", node->name, node );
    indent += INDENT;

    print_scope( node->scope, indent );

    if ( node->upvals.size() )
    {
        printf( "%*supvals:\n", indent, "" );
        indent += INDENT;
        int i = 0;
        for ( const ast_upval& upval : node->upvals )
        {
            printf( "%*s[ %d ] ", indent, "", i++ );
            switch ( upval.kind )
            {
            case AST_UPVAL_LOCAL:
                printf( "local %s %p \n", upval.local->name, upval.local );
                break;

            case AST_UPVAL_OBJECT:
                printf( "object %p\n", upval.object );
                break;

            case AST_UPVAL_UPVAL:
                printf( "upval %d\n", upval.upval );
                break;
            }
        }
        indent -= INDENT;
    }

    if ( node->member_of )
    {
        printf( "%*smemberof: %p\n", indent, "", node->member_of );
    }

    if ( node->this_name )
    {
        printf( "%*sthisname: %s %p\n", indent, "", node->this_name->name, node->this_name );
    }

    printf( "%*sparameters:\n", indent, "" );
    indent += INDENT;
    for ( ast_name* name : node->parameters )
    {
        printf( "%*s%s %p\n", indent, "", name->name, name );
    }
    if ( node->varargs )
    {
        printf( "%*s...\n", indent, "" );
    }
    indent -= INDENT;

    if ( node->generator )
    {
        printf( "%*sgenerator: true\n", indent, "" );
    }

    visit( node->block, indent );
}


void ast_printer::visit( ast_function* node, int indent )
{
    printf( "%*sfunction %s %p\n", indent, "", node->name, node );
}

void ast_printer::visit( ast_expr_null* node, int indent )
{
    printf( "%*snull\n", indent, "" );
}

void ast_printer::visit( ast_expr_bool* node, int indent )
{
    printf( "%*s%s\n", indent, "", node->value ? "true" : "false" );
}

void ast_printer::visit( ast_expr_number* node, int indent )
{
    printf( "%*s%g\n", indent, "", node->value );
}

void ast_printer::visit( ast_expr_string* node, int indent )
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

void ast_printer::visit( ast_expr_local* node, int indent )
{
    printf( "%*s%s %p\n", indent, "", node->name->name, node->name );
}

void ast_printer::visit( ast_expr_global* node, int indent )
{
    printf( "%*sglobal.%s\n", indent, "", node->name );
}

void ast_printer::visit( ast_expr_upref* node, int indent )
{
    printf( "%*supval %d\n", indent, "", node->index );
}

void ast_printer::visit( ast_expr_objref* node, int indent )
{
    printf( "%*sobject %p\n", indent, "", node->object );
}

void ast_printer::visit( ast_expr_superof* node, int indent )
{
    printf( "%*ssuperof:\n", indent, "" );
    visit( node->expr, indent + INDENT );
}

void ast_printer::visit( ast_expr_key* node, int indent )
{
    printf( "%*skey:\n", indent, "" );
    visit( node->object, indent + INDENT );
    printf( "%*s'%s'\n", indent + INDENT, "", node->key );
}

void ast_printer::visit( ast_expr_inkey* node, int indent )
{
    printf( "%*sinkey:\n", indent, "" );
    visit( node->object, indent + INDENT );
    visit( node->key, indent + INDENT );
}

void ast_printer::visit( ast_expr_index* node, int indent )
{
    printf( "%*sindex:\n", indent, "" );
    visit( node->object, indent + INDENT );
    visit( node->index, indent + INDENT );
}

void ast_printer::visit( ast_expr_responds* node, int indent )
{
    printf( "%*sresponds:\n", indent, "" );
    visit( node->object, indent + INDENT );
    printf( "%*s'%s'\n", indent + INDENT, "", node->key );
}

void ast_printer::visit( ast_expr_inresponds* node, int indent )
{
    printf( "%*sinresponds:\n", indent, "" );
    visit( node->object, indent + INDENT );
    visit( node->key, indent + INDENT );
}

void ast_printer::visit( ast_expr_preop* node, int indent )
{
    printf( "%*s%s\n", indent, "", operator_name( node->opkind ) );
    visit( node->lvalue, indent + INDENT );
}

void ast_printer::visit( ast_expr_postop* node, int indent )
{
    printf( "%*s%s\n", indent, "", operator_name( node->opkind ) );
    visit( node->lvalue, indent + INDENT );
}

void ast_printer::visit( ast_expr_unary* node, int indent )
{
    printf( "%*s%s\n", indent, "", operator_name( node->opkind ) );
    visit( node->operand, indent + INDENT );
}

void ast_printer::visit( ast_expr_binary* node, int indent )
{
    printf( "%*s%s\n", indent, "", operator_name( node->opkind ) );
    visit( node->lhs, indent + INDENT );
    visit( node->rhs, indent + INDENT );
}

void ast_printer::visit( ast_expr_compare* node, int indent )
{
    printf( "%*scompare:\n", indent, "" );
    indent += INDENT;

    visit( node->first, indent );
    for ( const ast_op_value& term : node->terms )
    {
        const char* opname = operator_name( term.op );
        printf( "%*s%s\n", indent, "", opname );
        visit( term.value, indent );
    }
}

void ast_printer::visit( ast_expr_logical* node, int indent )
{
    printf( "%*s%s\n", indent, "", operator_name( node->opkind ) );
    visit( node->lhs, indent + INDENT );
    visit( node->rhs, indent + INDENT );
}

void ast_printer::visit( ast_expr_qmark* node, int indent )
{
    printf( "%*s?:\n", indent, "" );
    visit( node->condition, indent + INDENT );
    visit( node->iftrue, indent + INDENT );
    visit( node->iffalse, indent + INDENT );
}

void ast_printer::visit( ast_make_new* node, int indent )
{
    printf( "%*snew:\n", indent, "" );
    visit( node->proto, indent + INDENT );
    visit( node->arguments, indent + INDENT );
}

void ast_printer::visit( ast_make_object* node, int indent )
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
    for ( ast_node* member : node->members )
        visit( member, indent + INDENT );
}

void ast_printer::visit( ast_make_array* node, int indent )
{
    printf( "%*slist:\n", indent, "" );
    for ( ast_node* value : node->values )
        visit( value, indent + INDENT );
    if ( node->final )
        visit( node->final, indent + INDENT );
}

void ast_printer::visit( ast_make_table* node, int indent )
{
    printf( "%*stable:\n", indent, "" );
    indent += INDENT;
    for ( const ast_key_value& element : node->elements )
    {
        printf( "%*selement:\n", indent, "" );
        visit( element.key, indent + INDENT );
        visit( element.value, indent + INDENT );
    }
}

void ast_printer::visit( ast_expr_mono* node, int indent )
{
    printf( "%*smono:\n", indent, "" );
    visit( node->expr, indent + INDENT );
}

void ast_printer::visit( ast_expr_call* node, int indent )
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

void ast_printer::visit( ast_expr_yield* node, int indent )
{
    printf( "%*syield:\n", indent, "" );
    indent += INDENT;
    if ( node->unpack )
        printf( "%*sunpack: true\n", indent, "" );
    if ( node->arguments )
        visit( node->arguments, indent );
}

void ast_printer::visit( ast_expr_vararg* node, int indent )
{
    printf( "%*s...\n", indent, "" );
}

void ast_printer::visit( ast_expr_unpack* node, int indent )
{
    printf( "%*sunpack:\n", indent, "" );
    visit( node->array, indent + INDENT );
}

void ast_printer::visit( ast_expr_list* node, int indent )
{
    printf( "%*svalues:\n", indent, "" );
    for ( ast_node* value : node->values )
        visit( value, indent + INDENT );
    if ( node->final )
    {
        printf( "%*s  (final)\n", indent, "" );
        visit( node->final, indent + INDENT );
    }
}

void ast_printer::visit( ast_expr_assign* node, int indent )
{
    printf( "%*s%s\n", indent, "", operator_name( node->assignop ) );
    visit( node->lvalue, indent + INDENT );
    visit( node->rvalue, indent + INDENT );
}

void ast_printer::visit( ast_expr_assign_list* node, int indent )
{
    printf( "%*s%s\n", indent, "", operator_name( node->assignop ) );
    indent += INDENT;
    printf( "%*slvalues:\n", indent, "" );
    for ( ast_node* lvalue : node->lvalues )
        visit( lvalue, indent + INDENT );
    visit( node->rvalues, indent );
}

void ast_printer::visit( ast_stmt_block* node, int indent )
{
    printf( "%*sblock %p:\n", indent, "", node );
    indent += INDENT;

    if ( node->scope )
    {
        print_scope( node->scope, indent );
    }

    for ( ast_node* stmt : node->stmts )
    {
        visit( stmt, indent );
    }
}

void ast_printer::visit( ast_stmt_if* node, int indent )
{
    printf( "%*sif %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->condition, indent );
    visit( node->iftrue, indent );
    if ( node->iffalse )
        visit( node->iffalse, indent );
}

void ast_printer::visit( ast_stmt_switch* node, int indent )
{
    printf( "%*sswitch %p:", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->value, indent );
    visit( node->body, indent );
}

void ast_printer::visit( ast_stmt_while* node, int indent )
{
    printf( "%*swhile %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->condition, indent );
    visit( node->body, indent );
}

void ast_printer::visit( ast_stmt_do* node, int indent )
{
    printf( "%*sdo %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->body, indent );
    visit( node->condition, indent );
}

void ast_printer::visit( ast_stmt_foreach* node, int indent )
{
    printf( "%*sforeach %p:\n", indent, "", node );
    indent += INDENT;

    print_scope( node->scope, indent );

    printf( "%*s%s:\n", indent, "", node->declare ? "var" : "lvalues" );
    indent += INDENT;
    for ( ast_node* lvalue : node->lvalues )
        visit( lvalue, indent );
    indent -= INDENT;

    printf( "%*s%s:\n", indent, "", node->eachkey ? "eachkey" : "eachindex" );
    visit( node->list, indent + INDENT );
    visit( node->body, indent );
}

void ast_printer::visit( ast_stmt_for* node, int indent )
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

void ast_printer::visit( ast_stmt_using* node, int indent )
{
    printf( "%*susing %p:\n", indent, "", node );
    indent += INDENT;
    print_scope( node->scope, indent );
    visit( node->uvalue, indent );
    visit( node->body, indent );
}

void ast_printer::visit( ast_stmt_try* node, int indent )
{
    printf( "%*stry %p:\n", indent, "", node );
    visit( node->tstmt, indent + INDENT );
    for ( ast_node* cstmt : node->clist )
    {
        visit( cstmt, indent );
    }
    if ( node->fstmt )
    {
        printf( "%*sfinally:\n", indent, "" );
        visit( node->fstmt, indent + INDENT );
    }
}

void ast_printer::visit( ast_stmt_catch* node, int indent )
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

void ast_printer::visit( ast_stmt_delete* node, int indent )
{
    printf( "%*sdelete:\n", indent, "" );
    for ( ast_node* delval : node->delvals )
        visit( delval, indent + INDENT );
}

void ast_printer::visit( ast_stmt_case* node, int indent )
{
    printf( "%*scase:\n", indent, "" );
    if ( node->value )
        visit( node->value, indent + INDENT );
    else
        printf( "%*s[default]\n", indent + INDENT, "" );
}

void ast_printer::visit( ast_stmt_continue* node, int indent )
{
    printf( "%*scontinue %p %p\n", indent, "", node->scope, node->target );
}

void ast_printer::visit( ast_stmt_break* node, int indent )
{
    printf( "%*sbreak %p %p\n", indent, "", node->scope, node->target );
}

void ast_printer::visit( ast_stmt_return* node, int indent )
{
    printf( "%*sreturn %p:\n", indent, "", node->scope );
    if ( node->values )
        visit( node->values, indent + INDENT );
    else
        printf( "%*s[null]\n", indent + INDENT, "" );
}

void ast_printer::visit( ast_stmt_throw* node, int indent )
{
    printf( "%*sthrow:\n", indent, "" );
    visit( node->value, indent + INDENT );
}

void ast_printer::visit( ast_bind_name* node, int indent )
{
    printf( "%*s[!!] '%s'\n", indent, "", node->name );
}

void ast_printer::visit( ast_bind_qual* node, int indent )
{
    printf( "%*s[!!] qual:\n", indent, "" );
    visit( node->scope, indent + INDENT );
    printf( "%*s'%s'\n", indent + INDENT, "", node->name );
}

void ast_printer::visit( ast_bind_list* node, int indent )
{
    printf( "%*s[!!] list:\n", indent, "" );
    for ( ast_bind_name* name : node->names )
        visit( name, indent + INDENT );
}


void ast_printer::print_scope( ast_scope* scope, int indent )
{
    const char* kind_name = "??";
    switch ( scope->kind )
    {
    case AST_SCOPE_SCRIPT:      kind_name = "SCRIPT";       break;
    case AST_SCOPE_BLOCK:       kind_name = "BLOCK";        break;
    case AST_SCOPE_SWITCH:      kind_name = "SWITCH";       break;
    case AST_SCOPE_IMPLICIT:    kind_name = "IMPLICIT";     break;
    case AST_SCOPE_OBJECT:      kind_name = "OBJECT";       break;
    case AST_SCOPE_IMPLIED:     kind_name = "IMPLIED";      break;
    }

    printf( "%*sscope %s %p:\n", indent, "", kind_name, scope );
    indent += INDENT;

    printf( "%*souter     : %p\n", indent, "", scope->outer );
    printf( "%*snode      : %p\n", indent, "", scope->node );
    printf( "%*sfunction  : %p\n", indent, "", scope->function );
    printf( "%*sblock     : %p\n", indent, "", scope->block );
    printf( "%*scontinued : %s\n", indent, "", scope->continued ? "true" : "false" );
    printf( "%*sdo_while  : %s\n", indent, "", scope->do_while ? "true" : "false" );

    if ( scope->implied.size() )
    {
        printf( "%*simplied:\n", indent, "" );
        indent += INDENT;
        for ( auto implied_entry : scope->implied )
        {
            std::string_view key = implied_entry.first;
            printf( "%*s%.*s ->", indent, "", (int)key.size(), key.data() );
            if ( implied_entry.second->node )
            {
                // Object scope, will appear later.
                printf( " scope %p\n", implied_entry.second );
            }
            else
            {
                // Implied scope, won't appear later.
                printf( "\n" );
                print_scope( implied_entry.second, indent + INDENT );
            }
        }
        indent -= INDENT;
    }

    if ( scope->names.size() )
    {
        printf( "%*snames:\n", indent, "" );
        indent += INDENT;
        for ( auto name_entry : scope->names )
        {
            std::string_view key = name_entry.first;
            printf( "%*s%.*s -> ", indent, "", (int)key.size(), key.data() );
            print_name( name_entry.second );
            printf( "\n" );
        }
        indent -= INDENT;
    }
}


void ast_printer::print_name( ast_name* name )
{
    printf( "%s %p", name->name, name );

    if ( name->prototype )
    {
        ast_prototype* proto = name->prototype;
        ast_bind_list* params = proto->parameters;
        printf( "(" );
        if ( params )
        {
            size_t i = 0;
            for ( ast_bind_name* unqual : params->names )
            {
                if ( i++ > 0 )
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

        if ( proto->generator )
        {
            printf( " generator" );
        }
    }

    if ( name->super_this )
    {
        printf( " super_this %p", name->super_this );
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

}

