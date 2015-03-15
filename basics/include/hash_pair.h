//
//  hash_pair.h
//  basics
//
//  Created by Edmund Kapusniak on 15/03/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef HASH_PAIR_H
#define HASH_PAIR_H


#include <utility>
#include <functional>



namespace std
{
template < typename first_t, typename second_t >
struct hash< std::pair< first_t, second_t > >
{
    inline size_t operator () ( const std::pair< first_t, second_t >& p ) const
    {
        return hash< first_t >()( p.first )
             ^ hash< second_t >()( p.second );
    }
};
}



#endif
