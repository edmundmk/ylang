//
//  ylang.h
//
//  Created by Edmund Kapusniak on 29/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef YLANG_H
#define YLANG_H

namespace yl
{

/*
    Private structures.
*/

struct runtime;
struct context;
struct object;
struct array;
struct table;
struct function;
struct cothread;
struct native;

/*
    Forward declaration of runtime and context.
*/

struct runtime_handle;
struct context_handle;

/*
    Forward declarations of values.
*/

enum value_kind
{
    VALUE_NULL,
    VALUE_BOOL,
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_OBJECT,
    VALUE_ARRAY,
    VALUE_TABLE,
    VALUE_FUNCTION,
    VALUE_COTHREAD,
    VALUE_NATIVE,
};

struct value;
struct value_list;
struct object_value;
struct array_value;
struct table_value;
struct function_value;
struct cothread_value;
struct native_value;

/*
    Forward declaration of call frame.
*/

enum call_return
{
    CALL_RETURN,
    CALL_YIELD,
};

class call_frame;

/*
    Forward declaration of exception.
*/

struct stack_trace;
class exception;

/*
    Other objects.
*/

class scope;
class key;

/*
    Forward declaration of ref pointer, which keeps values alive.
*/

template < typename T > class ref;
typedef ref< runtime_handle > runtime_ref;
typedef ref< context_handle > context_ref;
typedef ref< value > value_ref;
typedef ref< object_value > object_ref;
typedef ref< array_value > array_ref;
typedef ref< table_value > table_ref;
typedef ref< function_value > function_ref;
typedef ref< cothread_value > cothread_ref;
typedef ref< native_value > native_ref;

/*
    A runtime is a single garbage collected heap and object model.
*/

struct runtime_handle { runtime* r; };
runtime_ref create_runtime();

/*
    A context is a global scope.
*/

struct context_handle { context* c; };
context_ref create_context( const runtime_ref& runtime );

object_value superof( value_kind kind );
object_value superof( value v );
void assign_global( const char* k, value v );
void delete_global( const char* k );

/*
    Values.  Values are opaque handles to any ylang object.
*/

struct value
{
    value();
    value( std::nullptr_t );
    value( bool b );
    value( double n );
    value( std::string_view s );
    value( object_value o );
    value( array_value a );
    value( table_value t );
    value( function_value f );
    value( cothread_value c );
    value( native_value n );

    value( const char* s );
    value( int i );
    value( unsigned u );
    value( long i );
    value( unsigned long u );
    value( long long i );
    value( unsigned long long u );

    value_kind kind() const;
    bool is( value_kind kind ) const;

    bool to_bool() const;
    double to_number() const;
    std::string_view to_string() const;

    object_value to_object() const;
    array_value to_array() const;
    table_value to_table() const;
    function_value to_function() const;
    cothread_value to_cothread() const;
    native_value to_native() const;

    template < typename T > T* cast_native( void* nclass ) const;

    uint64_t v;
};

struct value_list
{
    value operator [] ( size_t i ) const;
    value* values;
    size_t count;
};

/*
    Objects.
*/

struct object;
struct object_value
{
    value lookup( key k ) const;
    value lookup( string_value k ) const;
    void assign( key k, value v );
    void assign( string_value k, value v );

    value_kind kind();
    bool is( value_kind kind ) const;
    array_value to_array() const;
    table_value to_table() const;
    native_value to_native() const;

    template < typename T > T* cast_native( void* nclass ) const;

    object* o;
};

object_ref create_object();
object_ref create_object( object_value prototype );

/*
    Arrays.
*/

struct array;
struct array_value
{
    value lookup( key k ) const;
    value lookup( string_value k ) const;
    void assign( key k, value v );
    void assign( string_value k, value v );

    size_t size() const;
    void clear();
    void resize( size_t size );

    value get( size_t i ) const;
    value set( size_t i, value v );
    value append( value v );

    array* a;
};

/*
    Tables.
*/

struct table;
struct table_value
{
    value lookup( key k ) const;
    value lookup( string_value k ) const;
    void assign( key k, value v );
    void assign( string_value k, value v );

    size_t length() const;
    void clear();

    value get( value k ) const;
    bool get( value k, value* out_v ) const;
    value set( value k, value v );

    table* t;
};

/*
    Functions.
*/

struct function;
struct function_value
{
    std::string_view name() const;
    bool is_generator() const;
    value call( value* arguments, size_t argument_count );
    value_list callm( void* arguments, size_t argument_count );
    template < typename ... A > value call( A ... arguments );
    template < typename ... A > value_list callm( A ... arguments );

    function* f;
};

typedef call_return (*native_function)( void* ndata, call_frame xf );

function_ref create_function( void* data, size_t size );
function_ref create_function( std::string_view name, native_function function, size_t argument_count, void* ndata );

/*
    Cothread.
*/

struct cothread;
struct cothread_value
{
    bool is_completed() const;
    call_frame call_frame();

    cothread* c;
};

cothread_value current_cothread();
cothread_ref create_cothread();

/*
    Native.
*/

struct native;
struct native_value
{
    value lookup( key k ) const;
    void assign( key k, value v );

    void* nclass() const;
    void* object() const;
    template < typename T > T* cast_native( void* nclass );

    native* n;
};

native_ref create_native( object_value prototype, void* nclass, void* object );

/*
    Call frames.
*/

class call_frame
{
public:

    cothread_value cothread() const;

    size_t count() const;
    value operator [] ( size_t i ) const;

    void clear() const;
    void push( value v );
    value_list pushm( size_t count );

    call_return result( value v );

private:

    friend struct cothread_value;
    call_frame( cothread* c );
    cothread* c;
};

/*
    Exception.
*/

struct stack_trace
{
    function_value function;
    int line;
    int column;
};

struct exception_impl;
class exception : public std::exception
{
public:

    exception( const char* format, ... ) PRINTF_FORMAT( 1, 2 );
    exception( exception&& e );
    exception( const exception& e );
    exception& operator = ( exception&& e );
    exception& operator = ( const exception& e );
    ~exception();

    const char* what() const noexcept override;
    size_t trace_count() const noexcept;
    stack_trace trace( size_t i ) const noexcept;

private:

    void* p;
};

/*
    Other objects.
*/

class key
{
public:

    key( std::string_view string );
    ~key();

private:

    void* p;
};

class scope
{
public:

    explicit scope( const context_ref& c );
    ~scope();

private:

    context* c;
    context* prev;
};

/*
    Ref.
*/

template < typename T >
class ref
{
public:

    ref();
    ref( T p );
    ref( ref&& r );
    ref( const ref& r );
    ref& operator = ( ref&& r );
    ref& operator = ( const ref& r );
    ~ref();

    explicit operator bool ();
    T get() const;
    T operator -> () const;

    void reset( T p );

private:

    void incref( T p );
    void decref( T p );

    T p;
};

}

#endif

