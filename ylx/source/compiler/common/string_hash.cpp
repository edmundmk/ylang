//
//  string_hash.cpp
//
//  Created by Edmund Kapusniak on 13/08/2017.
//  Copyright © 2017 Edmund Kapusniak.
//
//  Licensed under the MIT License. See LICENSE file in the project root for
//  full license information.
//

#include "string_hash.h"

template class basic_string_hash< char >;
template class basic_string_hash< wchar_t >;
template class basic_string_hash< char16_t >;
template class basic_string_hash< char32_t >;

