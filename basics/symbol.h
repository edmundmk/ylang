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


class symbol
{
public:

    symbol();
    symbol( symbol&& s );
    symbol( const symbol& s );
    symbol( const char* s );
    symbol( const std::string& s );
    symbol& operator = ( symbol&& s );
    symbol& operator = ( const symbol& s );
    symbol& operator = ( const char* s );
    symbol& operator = ( const std::string& s );
    ~symbol();

    explicit operator bool () const;
    operator std::string () const;
    const char* c_str() const;
    hash_t hash() const;


private:

    friend struct std::hash< symbol >;
    friend bool operator == ( const symbol& a, const symbol& b );

    hash_t  shash;
    char*   sname;

};



namespace std
{
template <> struct hash< symbol >
{
    inline size_t operator () ( const symbol& s )
    {
        return s.shash;
    }
};
}


inline bool operator == ( const symbol& a, const symbol& b )
{
    return a.shash == b.shash && strcmp( a.sname, b.sname ) == 0;
}

inline bool operator != ( const symbol& a, const symbol& b )
{
    return !( a == b );
}



#endif /* SYMBOL_H */

