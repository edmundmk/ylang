//
//  yssa_codegen.cpp
//
//  Created by Edmund Kapusniak on 04/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "yssa_codegen.h"
#include <make_unique.h>
#include "yl_invoke.h"
#include "yssa.h"
#include "yl_string.h"
#include "yl_function.h"


struct ygen_module;
struct ygen_program;
struct ygen_string;


typedef std::unique_ptr< ygen_program > ygen_program_p;
typedef std::unique_ptr< ygen_string >  ygen_string_p;


/*
    Each function has an array of constant values used in the function.
    This includes lookup keys and program objects.
*/


enum ygen_value_kind
{
    YGEN_KEY,
    YGEN_STRING,
    YGEN_NUMBER,
    YGEN_PROGRAM,
};


struct ygen_value
{
    ygen_value_kind kind;
    union
    {
        ygen_string*    string;
        double          number;
        ygen_program*   program;
    };
};



/*
    We have to be very careful that the programs and strings we create are
    reachable by the garbage collector at all times.  The code generator
    constructs an unmanaged representation of the entire module before
    translating it to the final objects (ensuring that each heapobj is
    reachable from a yl_invoke that puts the main function in the root set).
*/


struct ygen_module
{
    std::unordered_map< symkey, ygen_string_p >  strings;
    std::unordered_map< yssa_function*, ygen_program_p > programs;
};


struct ygen_program
{
    yl_program*                 program;

    std::vector< ygen_value >   values;
    std::vector< yl_opinst >    ops;
    std::vector< yl_xframe >    xframes;
    std::vector< yl_varname >   varnames;

    std::unordered_map< symkey, size_t > strvals;
    std::unordered_map< double, size_t > numvals;
    std::unordered_map< yssa_function*, size_t > funvals;
};


struct ygen_string
{
    const char*                 text;
    size_t                      size;
    bool                        iskey;
};




/*
    A register transfer graph represents moves that happen in parallel.
    It has nodes representing registers and edges representing moves.
    Each node has at most a single incoming edge, but may have many
    outgoing edges.  We can separate the graph into:

      - Cycles.
      - Paths which start at a node with no outgoing edges and follow
            edges backwards to either a node with no incoming edges, or
            a node which is part of a cycle.

    We can perform the register transfers by following this algorithm:

      - Count the number of outgoing edges for each node.
      - While there is at least one node with a zero count:
          - Perform move to this node A from its source node B.
          - Decrement outgoing edge count of source node A.
          - Remove node A from set.
      - The remaining nodes form loops.  While nodes remain:
          - Swap values between a random node A and its source node B.
          - Remove edge between nodes A and B.
          - Create a new edge between node A and the target of node B.
          - Remove node A from set.

    Or, a version of the loop-swapping algorithm that doesn't require us to
    traverse the graph forwards along edges:

      - Pick a random node C.
      - Find its source node B and B's source node A.
      - If A == C then swap the two nodes, and remove both from set.
      - Otherwise swap A and B, and
          - remove B, and
          - change the source node of C to be A.
*/


struct ygen_movgraph
{
    explicit operator bool ();
    void move( unsigned target, unsigned source );
    void emit( ygen_program* p );
    
    std::unordered_map< unsigned, unsigned > moves;
};


ygen_movgraph::operator bool()
{
    return ! moves.empty();
}

void ygen_movgraph::move( unsigned target, unsigned source )
{
    if ( target != source )
    {
        moves[ target ] = source;
    }
}

void ygen_movgraph::emit( ygen_program* p )
{
    // Count outgoing edges.
    std::unordered_map< unsigned, unsigned > outcount;
    for ( auto move : moves )
    {
        unsigned source = move.second;
        outcount[ source ] += 1;
    }
    
    // Build list of nodes with zero outcount.
    std::unordered_set< unsigned > zero;
    std::unordered_set< unsigned > nonzero;
    for ( auto move : moves )
    {
        unsigned target = move.first;
        if ( outcount.find( target ) == outcount.end() )
        {
            zero.insert( target );
        }
        else
        {
            nonzero.insert( target );
        }
    }
    
    // Perform moves to nodes with zero outcount.
    while ( zero.size() )
    {
        auto i = zero.begin();
        unsigned target = *i;
        unsigned source = moves.at( target );
        
        // Perform move.
        p->ops.emplace_back( YL_MOV, target, source, 0 );
        
        // Target is no longer in the set.
        zero.erase( i );
        
        // Decrement outcount for source.
        outcount[ source ] -= 1;
        if ( outcount[ source ] == 0
                && nonzero.find( source ) != nonzero.end() )
        {
            nonzero.erase( source );
            zero.insert( source );
        }
    }
    
    // Remaining nodes are in cycles.
    while ( nonzero.size() )
    {
        auto i = nonzero.begin();
        unsigned c = *i;
        unsigned b = moves.at( c );
        unsigned a = moves.at( b );
        
        // Swap.
        p->ops.emplace_back( YL_SWP, a, b, 0 );
        
        if ( c == a )
        {
            // Remove both.
            nonzero.erase( a );
            nonzero.erase( b );
        }
        else
        {
            // Remove B.
            nonzero.erase( b );
            
            // Change source node of C to be A.
            moves[ c ] = a;
        }
    }
}






/*
    Codegen.
*/


static void yssa_codegen_function( ygen_module* m, yssa_function* function );
static size_t yssa_codegen_op(
    ygen_module* m, ygen_program* p, yssa_function* function, size_t index );



yl_invoke yssa_codegen( yssa_module* module )
{
    ygen_module m;
    
    // Generate each function.
    for ( size_t i = 0; i < module->functions.size(); ++i )
    {
        yssa_function* function = module->functions.at( i ).get();
        yssa_codegen_function( &m, function );
    }
    
    // Construct final heap objects.

    

    return yl_invoke();
}


static ygen_program* add_program( ygen_module* m, yssa_function* function )
{
    // Get (or create) program object.
    auto i = m->programs.find( function );
    if ( i != m->programs.end() )
    {
        return i->second.get();
    }
    else
    {
        ygen_program_p program = std::make_unique< ygen_program >();
        program->program = nullptr;
        ygen_program* p = program.get();
        m->programs.emplace( function, std::move( program ) );
        return p;
    }
}


static ygen_string* add_string( ygen_module* m, symkey k )
{
    // Find string.
    auto i = m->strings.find( k );
    if ( i != m->strings.end() )
    {
        return i->second.get();
    }
    else
    {
        ygen_string_p string = std::make_unique< ygen_string >();
        string->text = k.c_str();
        string->size = k.size();
        ygen_string* s = string.get();
        m->strings.emplace( k, std::move( string ) );
        return s;
    }
}


void yssa_codegen_function( ygen_module* m, yssa_function* function )
{
    ygen_program* p = add_program( m, function );
    
    // Extract all constants.
    for ( size_t i = 0; i < function->ops.size(); ++i )
    {
        yssa_opinst* op = function->ops.at( i );
        switch ( op->opcode )
        {
        case YL_GLOBAL:
        case YL_SETGLOBAL:
        case YL_KEY:
        case YL_SETKEY:
        case YL_DELKEY:
        {
            symkey k( op->key );
            auto i = p->strvals.find( k );
            if ( i != p->strvals.end() )
            {
                // Upgrade to key, in case value was used earlier as string.
                ygen_value& value = p->values.at( i->second );
                value.kind = YGEN_KEY;
            }
            else
            {
                size_t index = p->values.size();
                ygen_value value;
                value.kind      = YGEN_KEY;
                value.string    = add_string( m, k );
                value.string->iskey = true;
                p->values.push_back( value );
                p->strvals.emplace( k, index );
            }
            break;
        }
        
        case YL_NUMBER:
        {
            if ( ! p->numvals.count( op->number ) )
            {
                size_t index = p->values.size();
                ygen_value value;
                value.kind      = YGEN_NUMBER;
                value.number    = op->number;
                p->values.push_back( value );
                p->numvals.emplace( op->number, index );
            }
            break;
        }
        
        case YL_STRING:
        {
            symkey k( op->string->string, op->string->length );
            if ( ! p->strvals.count( k ) )
            {
                size_t index = p->values.size();
                ygen_value value;
                value.kind      = YGEN_STRING;
                value.string    = add_string( m, k );
                p->values.push_back( value );
                p->strvals.emplace( k, index );
            }
            break;
        }
        
        case YL_FUNCTION:
        {
            if ( ! p->funvals.count( op->function ) )
            {
                size_t index = p->values.size();
                ygen_value value;
                value.kind      = YGEN_PROGRAM;
                value.program   = add_program( m, op->function );
                p->values.push_back( value );
                p->funvals.emplace( op->function, index );
            }
            break;
        }
        
        default:
            break;
        }
    }
    
    
    // And sort the values.
    std::vector< size_t > sorted;
    sorted.reserve( p->values.size() );
    for ( size_t i = 0; i < p->values.size(); ++i )
    {
        sorted.push_back( i );
    }
    
    std::sort
    (
        sorted.begin(),
        sorted.end(),
        [=]( size_t a, size_t b )
        {
            const ygen_value& aval = p->values.at( a );
            const ygen_value& bval = p->values.at( b );
            
            if ( aval.kind < bval.kind )
                return true;
            if ( aval.kind != bval.kind )
                return false;
            
            switch ( aval.kind )
            {
            case YGEN_KEY:
            case YGEN_STRING:
                return strcmp( aval.string->text, bval.string->text ) < 0;
            
            case YGEN_NUMBER:
                return aval.number < bval.number;

            case YGEN_PROGRAM:
                return a <  b;
            }
            
        }
    );
    
    
    // Construct each block.
    std::vector< size_t > indexes;
    for ( size_t i = 0; i < function->blocks.size(); ++i )
    {
        yssa_block* block = function->blocks.at( i ).get();
    
        // Compile each op in block.
        size_t count = 0;
        for ( size_t i = block->lstart; i < block->lfinal; i += count )
        {
            // Ops should be in sequential block order, with no gaps.
            assert( indexes.size() == i );
            
            // 'count' SSA ops map to one or more VM ops.
            size_t opindex = p->ops.size();
            count = yssa_codegen_op( m, p, function, i );
            indexes.insert( indexes.end(), count, opindex );

        }
    
    }
    
}


static unsigned operand( yssa_opinst* op, size_t index )
{
    if ( index < op->operand_count )
    {
        assert( op->operand[ index ] );
        assert( op->operand[ index ]->r != yl_opinst::NOVAL );
        return op->operand[ index ]->r;
    }
    else
    {
        return 0;
    }
}


static size_t emit_select( ygen_program* p, yssa_function* function, size_t index )
{
    yssa_opinst* op = function->ops.at( index );

    ygen_movgraph results;
    if ( op->r != yl_opinst::NOVAL )
    {
        results.move( op->r, op->stacktop );
    }
    
    size_t n;
    for ( n = 1; index + n < function->ops.size(); ++n )
    {
        yssa_opinst* sel = function->ops.at( index + n );
        if ( sel->opcode == YSSA_SELECT )
        {
            assert( sel->operand_count == 1 );
            assert( sel->operand[ 0 ] == op );
            if ( sel->r != yl_opinst::NOVAL )
            {
                results.move( sel->r, op->stacktop + sel->select );
            }
        }
        else if ( sel->opcode == YSSA_IMPLICIT )
        {
            // Ignore YSSA_IMPLICIT ops.
        }
        else
        {
            break;
        }
    }
    
    results.emit( p );
    
    return n;
}


size_t yssa_codegen_op(
    ygen_module* m, ygen_program* p, yssa_function* function, size_t index )
{
    yssa_opinst* op = function->ops.at( index );
    switch ( op->opcode )
    {
    case YL_NOP:
    case YL_MOV:
    case YL_NULL:
    case YL_NEG:
    case YL_BITNOT:
    case YL_MUL:
    case YL_DIV:
    case YL_MOD:
    case YL_INTDIV:
    case YL_ADD:
    case YL_SUB:
    case YL_LSL:
    case YL_LSR:
    case YL_ASR:
    case YL_BITAND:
    case YL_BITXOR:
    case YL_BITOR:
    case YL_CONCAT:
    case YL_EQ:
    case YL_NE:
    case YL_LT:
    case YL_GT:
    case YL_LE:
    case YL_GE:
    case YL_LNOT:
    case YL_LXOR:
    case YL_OBJECT:
    case YL_SUPER:
    case YL_INKEY:
    case YL_INDEX:
    case YL_DELINKEY:
    case YL_IN:
    case YL_IS:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned a = operand( op, 0 );
            unsigned b = operand( op, 1 );
            p->ops.emplace_back( (yl_opcode)op->opcode, op->r, a, b );
        }
        return 1;
    }
    
    case YL_BOOL:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned a = op->boolean ? 1 : 0;
            p->ops.emplace_back( YL_BOOL, op->r, a, 0 );
        }
        return 1;
    }
    
    case YL_NUMBER:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned c = (unsigned)p->numvals.at( op->number );
            p->ops.emplace_back( YL_NUMBER, op->r, c );
        }
        return 1;
    }
    
    case YL_STRING:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            symkey k( op->string->string, op->string->length );
            unsigned c = (unsigned)p->strvals.at( k );
            p->ops.emplace_back( YL_STRING, op->r, c );
        }
        return 1;
    }
     
    case YL_GLOBAL:
    {
        assert( op->result_count == 1 );
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned b = (unsigned)p->strvals.at( op->key );
            p->ops.emplace_back( YL_GLOBAL, op->r, 0, b );
        }
        return 1;
    }
    
    case YL_SETGLOBAL:
    {
        assert( op->result_count == 0 );
        unsigned r = operand( op, 0 );
        unsigned b = (unsigned)p->strvals.at( op->key );
        p->ops.emplace_back( YL_SETGLOBAL, r, 0, b );
        return 1;
    }

    case YL_FUNCTION:
    {
        if ( op->r != yl_opinst::NOVAL )
        {
            unsigned c = (unsigned)p->funvals.at( op->function );
            p->ops.emplace_back( YL_FUNCTION, op->r, c );
        }
    
        size_t n;
        for ( n = 1; index + n < function->ops.size(); ++n )
        {
            yssa_opinst* up = function->ops.at( index + n );
            if ( up->opcode == YL_UPLOCAL )
            {
                if ( op->r != yl_opinst::NOVAL )
                {
                    unsigned b = operand( up, 0 );
                    p->ops.emplace_back( YL_UPLOCAL, up->r, up->a, b );
                }
            }
            else if ( up->opcode == YL_UPUPVAL )
            {
                if ( op->r != yl_opinst::NOVAL )
                {
                    p->ops.emplace_back( YL_UPUPVAL, up->r, up->a, 0 );
                }
            }
            else
            {
                break;
            }
        }
        return n;
    }
    
    case YL_VARARG:
    case YL_CALL:
    case YL_YCALL:
    case YL_YIELD:
    case YL_RETURN:
    {
        // Get all operands into the correct registers.
        ygen_movgraph arguments;
        for ( unsigned i = 0; i < op->operand_count; ++i )
        {
            assert( op->operand[ i ] );
            assert( op->operand[ i ]->r != yl_opinst::NOVAL );
            arguments.move( op->stacktop + i, op->operand[ i ]->r );
        }

        if ( op->multival )
        {
            assert( op->multival->r == op->stacktop + op->operand_count );
            if ( op->multival->is_call() )
            {
                assert( op->multival->r == op->multival->stacktop );
            }
        }
        
        arguments.emit( p );
        
        // Emit call instruction.
        unsigned a = op->multival ? yl_opinst::MARK : op->operand_count;
        unsigned b = op->result_count;
        p->ops.emplace_back( (yl_opcode)op->opcode, op->stacktop, a, b );
        
        // Move all results into correct registers.
        return emit_select( p, function, index );
    }
     
     
    case YL_ITER:
    case YL_ITERKEY:
    
    case YL_NEXT1:
    case YL_NEXT2:
    case YL_NEXT:
    {
        
        return emit_select( p, function, index );
    }
 
    case YL_GETUP:
    case YL_SETUP:
 
    case YL_CLOSE:
 
    case YL_ARRAY:
    case YL_TABLE:
     
    case YL_KEY:
    case YL_SETKEY:
    case YL_SETINKEY:
    case YL_SETINDEX:
    case YL_DELKEY:
     
    case YL_APPEND:
    case YL_EXTEND:
    case YL_UNPACK:
     
    case YL_THROW:
    case YL_EXCEPT:
    case YL_UNWIND:
        return 1;
 
 
    case YSSA_PARAM:
    {
        // Get all parameters into the correct registers.
        
    }



    case YSSA_IMPLICIT:
    case YSSA_ITEREACH:
    case YSSA_PHI:
    case YSSA_VAR:
    {
        // Ignore these ops.
        return 1;
    }

    case YL_SWP:
    case YL_JMP:
    case YL_JMPT:
    case YL_JMPF:
    case YL_JMPITER:
    case YL_UPLOCAL:
    case YL_UPUPVAL:
    case YSSA_SELECT:
    case YSSA_REF:
    default:
    {
        assert( ! "unexpected SSA op" );
        return 1;
    }

    
    }
}













