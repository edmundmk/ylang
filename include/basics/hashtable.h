//
//  hashtable.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef HASHTABLE_H
#define HASHTABLE_H


#include <iterator>


/*
    This is a hash table which uses open addressing while ensuring that
    spilled elements do not displace non-colliding elements.  See the design
    of Lua's tables and the following paper:

        http://www.lua.org/source/5.2/ltable.c.html
        http://maths-people.anu.edu.au/~brent/pd/rpb013.pdf

    The interface isn't compatible with std::unordered_map.
*/


template < typename key_t, typename value_t >
class hashtable
{
public:

    struct keyval
    {
        const key_t key;
        value_t     value;
    };
    

private:

    struct bucket : public keyval
    {
        bucket*    next;
    };


    template < typename result_t >
    class basic_iterator
        :   public std::iterator< std::forward_iterator_tag, result_t >
    {
    public:
    
        basic_iterator( const basic_iterator< const keyval >& iterator );
        
        bool operator != ( const basic_iterator& b );
        
        basic_iterator& operator ++ ();
        basic_iterator operator ++ ( int );
        
        result_t& operator * () const;
        result_t* operator -> () const;
        
    private:
    
        friend class hashtable;
        
        explicit basic_iterator( bucket* current, bucket* end );
    
        bucket* current;
        bucket* end;

    };
    
    
public:

    typedef basic_iterator< keyval > iterator;
    typedef basic_iterator< const keyval > const_iterator;

    hashtable();
    hashtable( hashtable&& h );
    hashtable( const hashtable& h );
    hashtable& operator = ( hashtable&& h );
    hashtable& operator = ( const hashtable& h );
    ~hashtable();
    
    bool            empty() const;
    
    const_iterator  cbegin() const;
    const_iterator  begin() const;
    const_iterator  cend() const;
    const_iterator  end() const;

    const keyval*   lookup( const key_t& key ) const;

    iterator        begin();
    iterator        end();

    keyval*         lookup( const key_t& key );

    void            update( const key_t& key, const value_t& value );
    void            remove( const key_t& key );
    void            clear();
    
    void            swap( hashtable& h );


private:

    size_t          bucket_count;
    bucket*         buckets;
    
};




#endif
