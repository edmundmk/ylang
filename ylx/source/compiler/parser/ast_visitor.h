//
//  ast_visitor.h
//
//  Created by Edmund Kapusniak on 27/07/2014.
//  Copyright © 2014 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ast.h"

namespace yl
{

/*
    All this template nonsense is to remove the overhead of virtual dispatch.
    It's the curiously recurring template pattern, see for example:
        https://eli.thegreenplace.net/2013/12/05/the-cost-of-dynamic-virtual-calls-vs-static-crtp-dispatch-in-c
*/

template < typename visitT, typename returnT, typename ... A >
struct ast_visitor
{
    returnT visit( ast_node* node, A ... arguments );

    returnT fallback( ast_node* node, A ... arguments )             { return returnT(); }

    returnT visit( ast_function* node, A ... arguments )            { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_null* node, A ... arguments )           { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_bool* node, A ... arguments )           { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_number* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_string* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_local* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_global* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_upref* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_objref* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_superof* node, A ... arguments )        { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_key* node, A ... arguments )            { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_inkey* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_index* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_responds* node, A ... arguments )       { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_inresponds* node, A ... arguments )     { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_preop* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_postop* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_unary* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_binary* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_compare* node, A ... arguments )        { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_logical* node, A ... arguments )        { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_qmark* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_make_new* node, A ... arguments )            { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_make_object* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_make_array* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_make_table* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_mono* node, A ... arguments )           { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_call* node, A ... arguments )           { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_yield* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_vararg* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_unpack* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_list* node, A ... arguments )           { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_assign* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_expr_assign_list* node, A ... arguments )    { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_block* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_if* node, A ... arguments )             { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_switch* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_while* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_do* node, A ... arguments )             { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_foreach* node, A ... arguments )        { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_for* node, A ... arguments )            { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_using* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_try* node, A ... arguments )            { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_catch* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_delete* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_case* node, A ... arguments )           { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_continue* node, A ... arguments )       { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_break* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_return* node, A ... arguments )         { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_stmt_throw* node, A ... arguments )          { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_bind_name* node, A ... arguments )           { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_bind_qual* node, A ... arguments )           { return ( (visitT*)this )->fallback( node, arguments ... ); }
    returnT visit( ast_bind_list* node, A ... arguments )           { return ( (visitT*)this )->fallback( node, arguments ... ); }

    template < typename nodeT >
    returnT dispatch( ast_node* node, A ... arguments );
};

template < typename visitT, typename returnT, typename ... A >
returnT ast_visitor< visitT, returnT, A ... >::visit( ast_node* node, A ... arguments )
{
    typedef returnT (ast_visitor::*dispatch_method)( ast_node*, A ... );
    static dispatch_method dispatch[] =
    {
        &visitT::template dispatch< ast_function >,
        &visitT::template dispatch< ast_expr_null >,
        &visitT::template dispatch< ast_expr_bool >,
        &visitT::template dispatch< ast_expr_number >,
        &visitT::template dispatch< ast_expr_string >,
        &visitT::template dispatch< ast_expr_local >,
        &visitT::template dispatch< ast_expr_global >,
        &visitT::template dispatch< ast_expr_upref >,
        &visitT::template dispatch< ast_expr_objref >,
        &visitT::template dispatch< ast_expr_superof >,
        &visitT::template dispatch< ast_expr_key >,
        &visitT::template dispatch< ast_expr_inkey >,
        &visitT::template dispatch< ast_expr_index >,
        &visitT::template dispatch< ast_expr_responds >,
        &visitT::template dispatch< ast_expr_inresponds >,
        &visitT::template dispatch< ast_expr_preop >,
        &visitT::template dispatch< ast_expr_postop >,
        &visitT::template dispatch< ast_expr_unary >,
        &visitT::template dispatch< ast_expr_binary >,
        &visitT::template dispatch< ast_expr_compare >,
        &visitT::template dispatch< ast_expr_logical >,
        &visitT::template dispatch< ast_expr_qmark >,
        &visitT::template dispatch< ast_make_new >,
        &visitT::template dispatch< ast_make_object >,
        &visitT::template dispatch< ast_make_array >,
        &visitT::template dispatch< ast_make_table >,
        &visitT::template dispatch< ast_expr_mono >,
        &visitT::template dispatch< ast_expr_call >,
        &visitT::template dispatch< ast_expr_yield >,
        &visitT::template dispatch< ast_expr_vararg >,
        &visitT::template dispatch< ast_expr_unpack >,
        &visitT::template dispatch< ast_expr_list >,
        &visitT::template dispatch< ast_expr_assign >,
        &visitT::template dispatch< ast_expr_assign_list >,
        &visitT::template dispatch< ast_stmt_block >,
        &visitT::template dispatch< ast_stmt_if >,
        &visitT::template dispatch< ast_stmt_switch >,
        &visitT::template dispatch< ast_stmt_while >,
        &visitT::template dispatch< ast_stmt_do >,
        &visitT::template dispatch< ast_stmt_foreach >,
        &visitT::template dispatch< ast_stmt_for >,
        &visitT::template dispatch< ast_stmt_using >,
        &visitT::template dispatch< ast_stmt_try >,
        &visitT::template dispatch< ast_stmt_catch >,
        &visitT::template dispatch< ast_stmt_delete >,
        &visitT::template dispatch< ast_stmt_case >,
        &visitT::template dispatch< ast_stmt_continue >,
        &visitT::template dispatch< ast_stmt_break >,
        &visitT::template dispatch< ast_stmt_return >,
        &visitT::template dispatch< ast_stmt_throw >,
        &visitT::template dispatch< ast_bind_name >,
        &visitT::template dispatch< ast_bind_qual >,
        &visitT::template dispatch< ast_bind_list >,
    };

    return ( this->*( dispatch[ node->kind ] ) )( node, arguments ... );
}

template < typename visitT, typename returnT, typename ... A >
template < typename nodeT >
returnT ast_visitor< visitT, returnT, A ... >::dispatch( ast_node* node, A ... arguments )
{
    return ( (visitT*)this )->visit( (nodeT*)node, arguments ... );
}

}

#endif

