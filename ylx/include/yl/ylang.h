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

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct yl_runtime yl_runtime;
typedef struct yl_context yl_context;
typedef struct { uint64_t value; } yl_value;
typedef struct { const char* text; size_t size; } yl_string_view;
typedef struct { const char* key; } yl_key;
typedef struct { yl_value* values; size_t count; } yl_value_unpack;

/*
    A runtime is a single garbage collector and object model.
*/

yl_runtime* yl_runtime_create();
yl_runtime* yl_runtime_retain( yl_runtime* yr );
void yl_runtime_release( yl_runtime* yr );

/*
    A context is a global scope.
*/

yl_context* yl_context_create( yl_runtime* yr );
yl_context* yl_context_retain( yl_context* yc );
void yl_context_release( yl_context* yc );
void yl_context_set_current( yl_context* yc );

/*
    Globals.
*/

bool yl_assign_global( const char* key, yl_value value );
bool yl_delete_global( const char* key );

/*
    Key registration.
*/

bool yl_register_key( yl_key* key );

/*
    Values.
*/

enum yl_value_kind
{
    YL_VALUE_NULL,
    YL_VALUE_BOOL,
    YL_VALUE_NUMBER,
    YL_VALUE_STRING,
    YL_VALUE_OBJECT,
    YL_VALUE_ARRAY,
    YL_VALUE_TABLE,
    YL_VALUE_FUNCTION,
    YL_VALUE_COTHREAD,
    YL_VALUE_NATIVE_OBJECT,
    YL_VALUE_NATIVE_FUNCTION,
};

yl_value_kind yl_value_get_kind( yl_value value );
yl_value yl_value_retain( yl_value value );
void yl_value_release( yl_value value );

/*
    Get builtin prototypes.
*/

yl_value yl_prototype_object();
yl_value yl_prototype_bool();
yl_value yl_prototype_number();
yl_value yl_prototype_string();
yl_value yl_prototype_object();
yl_value yl_prototype_array();
yl_value yl_prototype_table();
yl_value yl_prototype_function();
yl_value yl_prototype_cothread();

/*
    Primitive values.
*/

yl_value yl_value_null();
yl_value yl_value_bool( bool b );
yl_value yl_value_number( double n );
yl_value yl_value_string( const char* text, size_t size );

bool yl_value_to_bool( yl_value v );
double yl_value_to_number( yl_value v );
yl_string_view yl_value_to_string( yl_value v );

/*
    Objects.
*/

yl_value yl_object_create( yl_value prototype );
yl_value yl_object_prototype( yl_value object );
yl_value yl_object_lookup( yl_value object, yl_key key );
bool yl_object_assign( yl_value object, yl_key key, yl_value value );

/*
    Arrays.
*/

yl_value yl_array_create( yl_value prototype );
size_t yl_array_size( yl_value array );
yl_value yl_array_get( yl_value array, size_t index );
bool yl_array_set( yl_value array, size_t index, yl_value value );
bool yl_array_append( yl_value array, yl_value value );
bool yl_array_resize( yl_value array, size_t size );

/*
    Tables.
*/

yl_value yl_table_create( yl_value prototype );
size_t yl_table_size( yl_value table );
bool yl_table_get( yl_value table, yl_value key, yl_value* out_value );
bool yl_table_set( yl_value table, yl_value key, yl_value value );

/*
    Functions.
*/

enum yl_call_result
{
    YL_CALL_DONE,       // call completed.
    YL_CALL_YIELD,      // call constructed cothread, or cothread has yielded.
    YL_CALL_SUSPEND,    // call suspended in native code.
    YL_CALL_EXCEPTION,  // call threw exception.
};

// HMMM.

yl_value yl_function_create( void* code, size_t size );
yl_value yl_function_call( yl_value function, yl_value* arguments, size_t argcount );
yl_value_unpack yl_function_call_unpack( yl_value function, yl_value* arguments, size_t argcount );

/*
    Cothreads.
*/

yl_value yl_cothread_call( yl_value cothread, yl_value* arguments, size_t argcount );
yl_value_unpack yl_cothread_call_unpack( yl_value cothread, yl_value* arguments, size_t argcount );
bool yl_cothread_finished( yl_value cothread ); 

/*
    Native objects.
*/

yl_value yl_native_object_create( yl_value prototype, void* native );
void* yl_native_object_get( yl_value v );
void yl_native_object_set( yl_value v, void* native );

/*
    Native functions.
*/

// TODO.  native functions need to be able to yield, or suspend entirely.


/*
    Errors.
*/

// TODO.


#ifdef __cplusplus
}
#endif

#endif

