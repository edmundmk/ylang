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
    printf( "%*svar\n", indent, "" );
    visit( decl->get_name_list(), indent + INDENT );
    if ( decl->get_expr_list() )
    {
        printf( "%*s=\n", indent, "" );
        visit( decl->get_expr_list(), indent + INDENT );
    }
}

void xec_astprinter::visit( xec_declaration_object* decl, int indent )
{
    printf( "%*sobject\n", indent, "" );
    visit( decl->get_name(), indent + INDENT );
    if ( decl->get_proto() )
    {
        printf( "%*sproto\n", indent, "" );
        visit( decl->get_proto(), indent + INDENT );
    }
    printf( "%*smembers\n", indent, "" );
    for ( size_t i = 0; i < decl->get_member_count(); ++i )
        visit( decl->get_member( i ), indent + INDENT );
}

void xec_astprinter::visit( xec_declaration_prototype* decl, int indent )
{
    printf( "%*sdecl-proto", indent, "" );
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
    visit( (xec_declaration_prototype*)decl, indent );
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
    printf( "%*s\"%s\"\n", indent, "", expr->get_value() );
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
    printf( "%*sconditional", indent, "" );
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
    printf( "%*svar-expression\n", indent, "" );
    visit( expr->get_name_list(), indent + INDENT );
    printf( "%*s=\n", indent, "" );
    visit( expr->get_expr_list(), indent + INDENT );
}

void xec_astprinter::visit( xec_constructor_new* expr, int indent )
{
}

void xec_astprinter::visit( xec_constructor_list* expr, int indent )
{
}

void xec_astprinter::visit( xec_constructor_table* expr, int indent )
{
}

void xec_astprinter::visit( xec_constructor_function* expr, int indent )
{
}

void xec_astprinter::visit( xec_constructor_object* expr, int indent )
{
}


void xec_astprinter::visit( xec_statement_declaration* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_expression* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_compound* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_delete* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_if* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_switch* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_case* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_while* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_do* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_foreach* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_for* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_continue* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_break* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_return* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_yield* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_using* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_usingscope* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_try* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_catch* decl, int indent )
{
}

void xec_astprinter::visit( xec_statement_throw* decl, int indent )
{
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