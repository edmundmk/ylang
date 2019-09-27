//
//  compiler.h
//
//  Created by Edmund Kapusniak on 29/09/2019.
//  Copyright © 2019 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#ifndef YL_COMPILER_H
#define YL_COMPILER_H

#include <string>
#include <vector>

namespace yl
{

/*
    Diagnostics emitted by the compiler.
*/

enum diagnostic_kind
{
    DIAGNOSTIC_ERROR,
};

struct diagnostic
{
    diagnostic_kind kind;
    unsigned line;
    unsigned column;
    std::string message;
};

/*
    The result of compilation.
*/

class compiled_module
{
public:

    compiled_module();
    compiled_module( compiled_module&& m ) = default;
    compiled_module& operator = ( compiled_module&& m ) = default;
    ~compiled_module();

    explicit operator bool () const;

    std::string_view filename() const;
    size_t diagnostic_count() const;
    const struct diagnostic& diagnostic( size_t i ) const;

    const void* data() const;
    size_t size() const;

private:

    std::string _filename;
    std::vector< struct diagnostic > _diagnostics;
    std::vector< char > _data;
};

/*
    Compile source text.
*/

enum
{
    COMPILE_PRINT_AST = 1 << 0,
    COMPILE_PRINT_SSA = 1 << 1,
    COMPILE_PRINT_OPS = 1 << 2,
};

compiled_module compile( std::string_view filename, const char* text, size_t size, unsigned debug = 0 );

}

#endif

