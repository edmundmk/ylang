//
// symbol.h
//
// Created 3rd October 2013.
// (c) Edmund Kapusniak 2013.  All rights reserved.
//


#ifndef SYMBOL_H
#define SYMBOL_H


#include <string>
#include <functional>
#include <hash.h>




class symkey
{
public:

    symkey();
    symkey( const char* s );
    symkey( const char* s, size_t size );
    symkey( hash32_t hash, const char* s, size_t size );

    explicit operator bool () const;
    operator std::string () const;
    const char* c_str() const;
    hash32_t hash() const;
    size_t size() const;


private:

    friend struct std::hash< symkey >;
    friend bool operator == ( const symkey& a, const symkey& b );

    hash32_t    shash;
    const char* sname;
    size_t      ssize;
    
};



inline symkey::symkey()
    :   shash( 0 )
    ,   sname( NULL )
    ,   ssize( NULL )
{
}

inline symkey::symkey( const char* s )
    :   shash( hash32( s ) )
    ,   sname( s )
    ,   ssize( strlen( s ) )
{
}

inline symkey::symkey( const char* s, size_t size )
    :   shash( hash32( s, size ) )
    ,   sname( s )
    ,   ssize( size )
{
}

inline symkey::symkey( hash32_t hash, const char* s, size_t size )
    :   shash( hash )
    ,   sname( s )
    ,   ssize( size )
{
}


inline symkey::operator bool () const
{
    return sname != NULL;
}

inline symkey::operator std::string () const
{
    return std::string( sname, ssize );
}

inline const char* symkey::c_str() const
{
    return sname;
}

inline hash32_t symkey::hash() const
{
    return shash;
}

inline size_t symkey::size() const
{
    return ssize;
}



namespace std
{
template <> struct hash< symkey >
{
    inline size_t operator () ( const symkey& s ) const
    {
        return s.shash;
    }
};
}


inline bool operator == ( const symkey& a, const symkey& b )
{
    return a.shash == b.shash && a.ssize == b.ssize
                    && memcmp( a.sname, b.sname, a.ssize ) == 0;
}

inline bool operator != ( const symkey& a, const symkey& b )
{
    return !( a == b );
}



#endif /* SYMBOL_H */

