//
//  y_context.h
//
//  Created by Edmund Kapusniak on 04/02/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef Y_CONTEXT_H
#define Y_CONTEXT_H


#include <unordered_map>


class y_object;
class y_struct_layout;
template < typename object_t > class yref;
template < typename object_t > class yroot;
template < typename object_t > class yslot;
template < typename object_t > class yauto;
template < typename element_t > class y_array;


/*
    A managed heap, local to a particular thread.
*/

class y_context
{
public:

    y_context();
    ~y_context();


private:

    friend class y_object;
    template < typename object_t > friend class yroot;
    template < typename object_t > friend class yslot;
    template < typename object_t > friend class yauto;
    template < typename object_t, typename ... arguments_t >
        friend yauto< object_t > ynew( arguments_t&& ... arguments );
    template < typename struct_t > friend class ylocal;
    template < typename struct_t > friend void ylocal_push( y_context* context );
    template < typename struct_t > friend void ylocal_pop( y_context* context );


    static __thread y_context* current;
    
    
    void* alloc( size_t size );
    
    void add_root( y_object* root );
    void del_root( y_object* root );
    
    void add_auto( void* pauto );
    void del_auto( void* pauto );
    
    void write_barrier( y_object* prev );
    
    
    void*                                       _heap;
    std::unordered_map< y_object*, unsigned >   _roots;
    void*                                       _autos;


};




#endif
