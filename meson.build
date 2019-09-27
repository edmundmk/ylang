#
#  ylang
#
#  Created by Edmund Kapusniak on 27/09/2019.
#
#  Licensed under the MIT License. See LICENSE file in the project root for
#  full license information.
#

project( 'ylang', 'cpp', default_options : [ 'cpp_std=c++17' ] )

lemon_program = find_program( 'lemon' )
lemon_buildsh = find_program( 'lemon.sh' )
lemon = generator( lemon_buildsh, output : [ '@BASENAME@.cpp', '@BASENAME@.h' ], arguments : [ lemon_program.path(), '@INPUT@', '@OUTPUT0@', '@OUTPUT1@' ] )

sources = [
    'ylx/source/compiler/common/alloc_region.cpp',
    'ylx/source/compiler/common/string_hash.cpp',
    'ylx/source/compiler/parser/ast.cpp',
    'ylx/source/compiler/parser/diagnostics.cpp',
]

# sources += lemon.process( 'compiler/ph_grammar.ly' )

yl = library(
    'yl',
    sources : sources,
    include_directories : include_directories( 'ylx/include' ),
    install : true
)

