//
//  xec_ast_visitor.h
//
//  Created by Edmund Kapusniak on 27/07/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_AST_VISITOR_H
#define XEC_AST_VISITOR_H


#include "xec_ast.h"


template < typename visitor_t, typename return_t, typename ... arguments_t >
class xec_ast_visitor
{
public:

    return_t visit( xec_ast_node* node, arguments_t ... arguments );

    return_t fallback( xec_ast_node* node, arguments_t ... arguments )
        { return return_t(); }

    return_t visit( xec_ast_func* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_null* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_bool* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_number* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_string* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_local* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_global* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_upref* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_objref* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_key* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_inkey* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_index* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_preop* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_postop* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_unary* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_binary* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_compare* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_logical* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_qmark* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_new_new* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_new_object* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_new_list* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_new_table* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_mono* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_call* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_yield* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_vararg* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_unpack* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_list* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_assign* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_expr_assign_list* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_block* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_if* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_switch* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_while* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_do* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_foreach* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_for* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_using* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_try* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_catch* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_delete* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_case* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_continue* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_break* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_return* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_stmt_throw* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_unqual_name* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_unqual_qual* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_unqual_list* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }
    return_t visit( xec_unqual_proto* node, arguments_t ... arguments )
        { return ( (visitor_t*)this )->fallback( node, arguments ... ); }


    template < typename node_t >
    return_t dispatch( xec_ast_node* node, arguments_t ... arguments );

};



template < typename visitor_t, typename return_t, typename ... arguments_t >
return_t xec_ast_visitor< visitor_t, return_t, arguments_t ... >::
                visit( xec_ast_node* node, arguments_t ... arguments )
{
    typedef return_t (xec_ast_visitor::*visit_t)(
                    xec_ast_node*, arguments_t ... );

    static visit_t dispatch[] =
    {
        &visitor_t::template dispatch< xec_ast_func >,
        &visitor_t::template dispatch< xec_expr_null >,
        &visitor_t::template dispatch< xec_expr_bool >,
        &visitor_t::template dispatch< xec_expr_number >,
        &visitor_t::template dispatch< xec_expr_string >,
        &visitor_t::template dispatch< xec_expr_local >,
        &visitor_t::template dispatch< xec_expr_global >,
        &visitor_t::template dispatch< xec_expr_upref >,
        &visitor_t::template dispatch< xec_expr_objref >,
        &visitor_t::template dispatch< xec_expr_key >,
        &visitor_t::template dispatch< xec_expr_inkey >,
        &visitor_t::template dispatch< xec_expr_index >,
        &visitor_t::template dispatch< xec_expr_preop >,
        &visitor_t::template dispatch< xec_expr_postop >,
        &visitor_t::template dispatch< xec_expr_unary >,
        &visitor_t::template dispatch< xec_expr_binary >,
        &visitor_t::template dispatch< xec_expr_compare >,
        &visitor_t::template dispatch< xec_expr_logical >,
        &visitor_t::template dispatch< xec_expr_qmark >,
        &visitor_t::template dispatch< xec_new_new >,
        &visitor_t::template dispatch< xec_new_object >,
        &visitor_t::template dispatch< xec_new_list >,
        &visitor_t::template dispatch< xec_new_table >,
        &visitor_t::template dispatch< xec_expr_mono >,
        &visitor_t::template dispatch< xec_expr_call >,
        &visitor_t::template dispatch< xec_expr_yield >,
        &visitor_t::template dispatch< xec_expr_vararg >,
        &visitor_t::template dispatch< xec_expr_unpack >,
        &visitor_t::template dispatch< xec_expr_list >,
        &visitor_t::template dispatch< xec_expr_assign >,
        &visitor_t::template dispatch< xec_expr_assign_list >,
        &visitor_t::template dispatch< xec_stmt_block >,
        &visitor_t::template dispatch< xec_stmt_if >,
        &visitor_t::template dispatch< xec_stmt_switch >,
        &visitor_t::template dispatch< xec_stmt_while >,
        &visitor_t::template dispatch< xec_stmt_do >,
        &visitor_t::template dispatch< xec_stmt_foreach >,
        &visitor_t::template dispatch< xec_stmt_for >,
        &visitor_t::template dispatch< xec_stmt_using >,
        &visitor_t::template dispatch< xec_stmt_try >,
        &visitor_t::template dispatch< xec_stmt_catch >,
        &visitor_t::template dispatch< xec_stmt_delete >,
        &visitor_t::template dispatch< xec_stmt_case >,
        &visitor_t::template dispatch< xec_stmt_continue >,
        &visitor_t::template dispatch< xec_stmt_break >,
        &visitor_t::template dispatch< xec_stmt_return >,
        &visitor_t::template dispatch< xec_stmt_throw >,
        &visitor_t::template dispatch< xec_unqual_name >,
        &visitor_t::template dispatch< xec_unqual_qual >,
        &visitor_t::template dispatch< xec_unqual_list >,
        &visitor_t::template dispatch< xec_unqual_proto >
    };
    
    return ( this->*( dispatch[ node->kind ] ) )( node, arguments ... );

}


template < typename visitor_t, typename return_t, typename ... arguments_t >
template < typename node_t >
return_t xec_ast_visitor< visitor_t, return_t, arguments_t ... >::dispatch(
                xec_ast_node* node, arguments_t ... arguments )
{
    return ( (visitor_t*)this )->visit( (node_t*)node, arguments ... );
}





#endif
