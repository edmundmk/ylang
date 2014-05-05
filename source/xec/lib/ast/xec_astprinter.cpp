//
//  xec_astprinter.cpp
//
//  Created by Edmund Kapusniak on 05/05/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_astprinter.h"


static const int INDENT = 4;



void xec_astprinter::print( xec_declaration* decl )
{
    visit( decl, 0 );
}

void xec_astprinter::print( xec_expression* expr )
{
    visit( expr, 0 );
}

void xec_astprinter::print( xec_statement* stmt )
{
    visit( stmt, 0 );
}



void xec_astprinter::visit( xec_declaration_var* decl, int indent )
{
    printf( "%*sdeclare-var\n", indent, "" );
    visit( decl->get_name_list(), indent + INDENT );
    if ( decl->get_expr_list() )
    {
        printf( "%*s=\n", indent, "" );
        visit( decl->get_expr_list(), indent + INDENT );
    }
}

void xec_astprinter::visit( xec_declaration_object* decl, int indent )
{
    printf( "%*sdeclare-object\n", indent, "" );
    visit( decl->get_name(), indent + INDENT );
    if ( decl->get_proto() )
        visit( decl->get_proto(), indent + INDENT );
    else
        printf( "%*s[object]\n", indent + INDENT, "" );
    for ( size_t i = 0; i < decl->get_member_count(); ++i )
        visit( decl->get_member( i ), indent + INDENT );
}

void xec_astprinter::visit( xec_declaration_prototype* decl, int indent )
{
    printf( "%*sdeclare-function-prototype", indent, "" );
    if ( decl->get_thiscall() )
        printf( " [thiscall]" );
    if ( decl->get_coroutine() )
        printf( " [coroutine]" );
    printf( "\n" );
    visit( decl->get_name(), indent + INDENT );
    visit( decl->get_parameters(), indent + INDENT );
}

void xec_astprinter::visit( xec_declaration_function* decl, int indent )
{
    printf( "%*sdeclare-function", indent, "" );
    if ( decl->get_thiscall() )
        printf( " [thiscall]" );
    if ( decl->get_coroutine() )
        printf( " [coroutine]" );
    printf( "\n" );
    visit( decl->get_name(), indent + INDENT );
    visit( decl->get_parameters(), indent + INDENT );
    visit( decl->get_body(), indent );
}


void xec_astprinter::visit( xec_expression_null* expr, int indent )
{
    printf( "%*snull\n", indent, "" );
}

void xec_astprinter::visit( xec_expression_bool* expr, int indent )
{
    printf( "%*s%s\n", indent, "", expr->get_value() ? "true" : "false" );
}

void xec_astprinter::visit( xec_expression_number* expr, int indent )
{
    printf( "%*s%g\n", indent, "", expr->get_value() );
}

void xec_astprinter::visit( xec_expression_string* expr, int indent )
{
    printf( "%*s\"", indent, "" );
    const char* s = expr->get_string();
    for ( size_t i = 0; i < expr->get_length(); ++i )
    {
        if ( s[ i ] >= 32 && s[ i ] <= 126 )
            printf( "%c", s[ i ] );
        else
            printf( "\\x%02X", (int)s[ i ] );
    }
    printf( "\"\n" );

//    printf( "%*s\"%s\"\n", indent, "", expr->get_value() );
}

void xec_astprinter::visit( xec_expression_identifier* expr, int indent )
{
    printf( "%*s%s\n", indent, "", expr->get_identifier() );
}

void xec_astprinter::visit( xec_expression_lookup* expr, int indent )
{
    printf( "%*slookup\n", indent, "" );
    visit( expr->get_expr(), indent + INDENT );
    printf( "%*s.\n", indent, "" );
    printf( "%*s%s\n", indent + INDENT, "", expr->get_identifier() );
}

void xec_astprinter::visit( xec_expression_indexkey* expr, int indent )
{
    printf( "%*sindexkey\n", indent, "" );
    visit( expr->get_expr(), indent + INDENT );
    printf( "%*s.[]\n", indent, "" );
    visit( expr->get_index(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_index* expr, int indent )
{
    printf( "%*sindex\n", indent, "" );
    visit( expr->get_expr(), indent + INDENT );
    printf( "%*s[]\n", indent, "" );
    visit( expr->get_index(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_yield* expr, int indent )
{
    printf( "%*syield()", indent, "" );
    if ( expr->get_unpack() )
        printf( " [...]" );
    printf( "\n" );
    visit( expr->get_arguments(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_call* expr, int indent )
{
    printf( "%*scall", indent, "" );
    if ( expr->get_yieldcall() )
        printf( " [yieldcall]" );
    if ( expr->get_unpack() )
        printf( " [...]" );
    printf( "\n" );
    visit( expr->get_expr(), indent + INDENT );
    visit( expr->get_arguments(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_preop* expr, int indent )
{
    printf( "%*spreop %s\n", indent, "", opname( expr->get_operator() ) );
    visit( expr->get_expr(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_postop* expr, int indent )
{
    printf( "%*spostop %s\n", indent, "", opname( expr->get_operator() ) );
    visit( expr->get_expr(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_unary* expr, int indent )
{
    printf( "%*sunary %s\n", indent, "", opname( expr->get_operator() ) );
    visit( expr->get_expr(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_binary* expr, int indent )
{
    printf( "%*sbinary\n", indent, "" );
    visit( expr->get_lhs(), indent + INDENT );
    printf( "%*s%s\n", indent, "", opname( expr->get_operator() ) );
    visit( expr->get_rhs(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_comparison* expr, int indent )
{
    printf( "%*scomparison\n", indent, "" );
    visit( expr->get_first_expr(), indent + INDENT );
    for ( size_t i = 0; i < expr->get_count(); ++i )
    {
        printf( "%*s%s\n", indent, "", opname( expr->get_operator( i ) ) );
        visit( expr->get_expr( i ), indent + INDENT );
    }
}

void xec_astprinter::visit( xec_expression_logical* expr, int indent )
{
    printf( "%*slogical\n", indent, "" );
    visit( expr->get_lhs(), indent + INDENT );
    printf( "%*s%s\n", indent, "", opname( expr->get_operator() ) );
    visit( expr->get_rhs(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_conditional* expr, int indent )
{
    printf( "%*sconditional\n", indent, "" );
    visit( expr->get_condition(), indent + INDENT );
    printf( "%*s?\n", indent, "" );
    visit( expr->get_iftrue(), indent + INDENT );
    printf( "%*s:\n", indent, "" );
    visit( expr->get_iffalse(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_varargs* expr, int indent )
{
    printf( "%*svarargs\n", indent, "" );
}

void xec_astprinter::visit( xec_expression_unpack* expr, int indent )
{
    printf( "%*sunpack\n", indent, "" );
    visit( expr->get_expr(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_list* expr, int indent )
{
    printf( "%*slist\n", indent, "" );
    for ( size_t i = 0; i < expr->get_count(); ++i )
        visit( expr->get_expr( i ), indent + INDENT );
    if ( expr->get_final() )
        visit( expr->get_final(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_assign* expr, int indent )
{
    printf( "%*sassign\n", indent, "" );
    visit( expr->get_lhs(), indent + INDENT );
    printf( "%*s%s=\n", indent, "", opname( expr->get_operator() ) );
    visit( expr->get_rhs(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_mono* expr, int indent )
{
    printf( "%*smono\n", indent, "" );
    visit( expr->get_expr(), indent + INDENT );
}

void xec_astprinter::visit( xec_expression_declare* expr, int indent )
{
    printf( "%*svar\n", indent, "" );
    visit( expr->get_name_list(), indent + INDENT );
    printf( "%*s=\n", indent, "" );
    visit( expr->get_expr_list(), indent + INDENT );
}

void xec_astprinter::visit( xec_constructor_new* expr, int indent )
{
    printf( "%*snew\n", indent, "" );
    visit( expr->get_proto(), indent + INDENT );
    visit( expr->get_arguments(), indent + INDENT );
}

void xec_astprinter::visit( xec_constructor_list* expr, int indent )
{
    printf( "%*slist\n", indent, "" );
    for ( size_t i = 0; i < expr->get_count(); ++i )
        visit( expr->get_expr( i ), indent + INDENT );
    if ( expr->get_final() )
        visit( expr->get_final(), indent + INDENT );
}

void xec_astprinter::visit( xec_constructor_table* expr, int indent )
{
    printf( "%*stable\n", indent, "" );
    for ( size_t i = 0; i < expr->get_count(); ++i )
    {
        printf( "%*skey\n", indent + INDENT, "" );
        visit( expr->get_key( i ), indent + INDENT + INDENT );
        printf( "%*svalue\n", indent + INDENT, "" );
        visit( expr->get_value( i ), indent + INDENT + INDENT );
    }
}

void xec_astprinter::visit( xec_constructor_object* expr, int indent )
{
    printf( "%*sobject", indent, "" );
    if ( expr->get_proto() )
        visit( expr->get_proto(), indent + INDENT );
    else
        printf( "%*s[object]\n", indent + INDENT, "" );
    for ( size_t i = 0; i < expr->get_member_count(); ++i )
        visit( expr->get_member( i ), indent + INDENT );
}

void xec_astprinter::visit( xec_constructor_function* expr, int indent )
{
    printf( "%*sfunction", indent, "" );
    if ( expr->get_thiscall() )
        printf( " [thiscall]" );
    if ( expr->get_coroutine() )
        printf( " [coroutine]" );
    printf( "\n" );
    visit( expr->get_parameters(), indent + INDENT );
    visit( expr->get_body(), indent + INDENT );
}


void xec_astprinter::visit( xec_statement_declaration* stmt, int indent )
{
    visit( stmt->get_decl(), indent );
}

void xec_astprinter::visit( xec_statement_expression* stmt, int indent )
{
    visit( stmt->get_expr(), indent );
}

void xec_astprinter::visit( xec_statement_compound* stmt, int indent )
{
    printf( "%*sblock\n", indent, "" );
    for ( size_t i = 0; i < stmt->get_count(); ++i )
        visit( stmt->get_stmt( i ), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_delete* stmt, int indent )
{
    printf( "%*sdelete\n", indent, "" );
    visit( stmt->get_expr_list(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_if* stmt, int indent )
{
    printf( "%*sif\n", indent, "" );
    visit( stmt->get_expr(), indent + INDENT );
    visit( stmt->get_iftrue(), indent + INDENT );
    if ( stmt->get_iffalse() )
        visit( stmt->get_iffalse(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_switch* stmt, int indent )
{
    printf( "%*sswitch\n", indent, "" );
    visit( stmt->get_expr(), indent + INDENT );
    visit( stmt->get_body(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_case* stmt, int indent )
{
    if ( stmt->get_expr() )
    {
        printf( "%*scase\n", indent, "" );
        visit( stmt->get_expr(), indent + INDENT );
    }
    else
    {
        printf( "%*sdefault\n", indent, "" );
    }
}

void xec_astprinter::visit( xec_statement_while* stmt, int indent )
{
    printf( "%*swhile\n", indent, "" );
    visit( stmt->get_expr(), indent + INDENT );
    visit( stmt->get_body(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_do* stmt, int indent )
{
    printf( "%*sdo\n", indent, "" );
    visit( stmt->get_body(), indent + INDENT );
    visit( stmt->get_expr(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_foreach* stmt, int indent )
{
    printf( "%*sforeach", indent, "" );
    if ( stmt->get_eachkey() )
        printf( " [eachkey]" );
    if ( stmt->get_declare() )
        printf( " [declare]" );
    printf( "\n" );
    visit( stmt->get_expr_list(), indent + INDENT );
    visit( stmt->get_expr(), indent + INDENT );
    visit( stmt->get_body(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_for* stmt, int indent )
{
    printf( "%*sfor\n", indent, "" );
    visit( stmt->get_init(), indent + INDENT );
    visit( stmt->get_expr(), indent + INDENT );
    visit( stmt->get_update(), indent + INDENT );
    visit( stmt->get_body(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_continue* stmt, int indent )
{
    printf( "%*scontinue\n", indent, "" );
}

void xec_astprinter::visit( xec_statement_break* stmt, int indent )
{
    printf( "%*sbreak\n", indent, "" );
}

void xec_astprinter::visit( xec_statement_return* stmt, int indent )
{
    printf( "%*sreturn\n", indent, "" );
    visit( stmt->get_expr_list(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_yield* stmt, int indent )
{
    printf( "%*syield\n", indent, "" );
    visit( stmt->get_expr_list(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_using* stmt, int indent )
{
    printf( "%*susing\n", indent, "" );
    visit( stmt->get_expr(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_usingscope* stmt, int indent )
{
    printf( "%*susingscope\n", indent, "" );
    visit( stmt->get_expr(), indent + INDENT );
    visit( stmt->get_body(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_try* stmt, int indent )
{
    printf( "%*stry\n", indent, "" );
    visit( stmt->get_body(), indent + INDENT );
    for ( size_t i = 0; i < stmt->get_catch_count(); ++i )
        visit( stmt->get_catch( i ), indent + INDENT );
    if ( stmt->get_final() )
        visit( stmt->get_final(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_catch* stmt, int indent )
{
    printf( "%*scatch\n", indent, "" );
    if ( stmt->get_lvalue() )
        visit( stmt->get_lvalue(), indent + INDENT );
    else
        printf( "%*s[anonymous]\n", indent + INDENT, "" );
    visit( stmt->get_proto(), indent + INDENT );
    visit( stmt->get_body(), indent + INDENT );
}

void xec_astprinter::visit( xec_statement_throw* stmt, int indent )
{
    printf( "%*sthrow\n", indent, "" );
    visit( stmt->get_expr(), indent + INDENT );
}




const char* xec_astprinter::opname( xec_operator_kind op )
{
    switch ( op )
    {
    case XEC_OPERATOR_NONE:         return "";
    case XEC_OPERATOR_INCREMENT:    return "++";
    case XEC_OPERATOR_DECREMENT:    return "--";
    case XEC_OPERATOR_POSITIVE:     return "+";
    case XEC_OPERATOR_NEGATIVE:     return "-";
    case XEC_OPERATOR_LOGICNOT:     return "!";
    case XEC_OPERATOR_BITNOT:       return "~";
    case XEC_OPERATOR_MULTIPLY:     return "*";
    case XEC_OPERATOR_DIVIDE:       return "/";
    case XEC_OPERATOR_MODULUS:      return "%";
    case XEC_OPERATOR_INTDIV:       return "~";
    case XEC_OPERATOR_ADD:          return "+";
    case XEC_OPERATOR_SUBTRACT:     return "-";
    case XEC_OPERATOR_LSHIFT:       return "<<";
    case XEC_OPERATOR_RSHIFT:       return ">>";
    case XEC_OPERATOR_URSHIFT:      return ">>>";
    case XEC_OPERATOR_BITAND:       return "&";
    case XEC_OPERATOR_BITXOR:       return "^";
    case XEC_OPERATOR_BITOR:        return "|";
    case XEC_OPERATOR_CONCATENATE:  return "..";
    case XEC_OPERATOR_EQUAL:        return "==";
    case XEC_OPERATOR_NOTEQUAL:     return "!=";
    case XEC_OPERATOR_LESS:         return "<";
    case XEC_OPERATOR_GREATER:      return ">";
    case XEC_OPERATOR_LEQUAL:       return "<=";
    case XEC_OPERATOR_GEQUAL:       return ">=";
    case XEC_OPERATOR_IN:           return "in";
    case XEC_OPERATOR_NOTIN:        return "!in";
    case XEC_OPERATOR_IS:           return "is";
    case XEC_OPERATOR_NOTIS:        return "!is";
    case XEC_OPERATOR_LOGICAND:     return "&&";
    case XEC_OPERATOR_LOGICXOR:     return "^^";
    case XEC_OPERATOR_LOGICOR:      return "||";
    }
}