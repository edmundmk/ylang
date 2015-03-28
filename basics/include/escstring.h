//
//  escstring.h
//
//  Created by Edmund Kapusniak on 28/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//

#ifndef ESCSTRING_H
#define ESCSTRING_H


#include <string>


inline std::string escstring( const std::string& s )
{
    static const char HEX[] = "0123456789ABCDEF";
    std::string result;
    result.reserve( s.size() );
    for ( char c : s )
    {
        if ( c >= 0x20 && c <= 0x7E )
        {
            result.push_back( c );
        }
        else switch ( c )
        {
        case 0x07:  result += "\\a";        break;
        case 0x08:  result += "\\b";        break;
        case 0x09:  result += "\\t";        break;
        case 0x0A:  result += "\\n";        break;
        case 0x0B:  result += "\\v";        break;
        case 0x0C:  result += "\\f";        break;
        case 0x0D:  result += "\\r";        break;
        case '"':   result += "\\\"";       break;
        default:
            result += "\\x";
            result.push_back( HEX[ (unsigned)c >> 4 & 0x0F ] );
            result.push_back( HEX[ (unsigned)c & 0x0F ] );
            break;
        }
    }
    return result;
}


#endif