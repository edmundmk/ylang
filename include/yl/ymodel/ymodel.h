//
//  ymodel.h
//
//  Created by Edmund Kapusniak on 07/11/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef YMODEL_H
#define YMODEL_H


#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <symkey.h>
#include <seglist.h>


class ymodel;
class yscope;
class yobject;
class yclass;
class ystring;
class ysymbol;
class yexpand;
class ystack;
class yframe;
class yfunction;
class ystandin;


/*
    32-bit machines without fast 64-bit atomics present several problems.
*/

#if defined( __x86_64__ ) || defined( __arm64__ )
#define Y64BIT 1
#endif


/*
    Whether to differentiate between references and numbers in yclasses.
*/

#if ! Y64BIT
#define YSLOTS 1
#endif




/*
    The core of ymodel is a concurrent, tri-colour garbage collector.
    Mutators are stopped only to find their roots.  Mutators run concurrently
    with both a mark and a sweep thread.  All references in heap objects
    must be write-barriered, visited by a mark routine, and accessible using
    only a single atomic read.
*/


enum ycolour
{
    Y_GREEN     = 0x00,
    Y_PURPLE    = 0x01,
    Y_ORANGE    = 0x02,
    Y_GREY      = 0x03,
};

static const uintptr_t Y_COLOUR_MASK = (uintptr_t)0x03;

inline ycolour ynextcolour( ycolour colour )
{
    return (ycolour)( ( colour + 1 ) % 3 );
}

typedef seglist< yobject* > yworklist;




/*
    The ymodel is an environment which supports garbage-collected yobjects,
    fast-hash and fast-compare ysymbols, and an implicit class tree for
    yexpands.
    
    Multiple mutator threads can access the ymodel concurrently, but note
    that yexpands (and remember yexpands share prototypes) are _not_
    thread-safe.  Multithreaded access to yexpands, where that expand or its
    prototypes might transition yclass, requires user locking.
*/

class ymodel
{
public:

    ymodel();
    ~ymodel();
    
    yexpand* make_global();
    

private:

    friend class yobject;
    friend class ysymbol;
    friend class yexpand;
    friend class yarray;
    friend class ytable;
    friend class yfunction;
    friend class yguard;
    template < typename object_t > friend class yroot;

    friend void ysafe();
    friend void yunsafe();
    friend void ysafepoint();
    friend void ycollect();

    friend void yexec( size_t, unsigned, unsigned );


    yclass*     empty_class();
#if YSLOTS
    yclass*     expand_class( yclass* klass, ysymbol key, bool is_number );
#else
    yclass*     expand_class( yclass* klass, ysymbol key );
#endif

    yexpand*    object_proto();
    yexpand*    array_proto();
    yexpand*    table_proto();
    ystandin*   boolean_proto();
    ystandin*   number_proto();
    ystandin*   string_proto();
    ystandin*   function_proto();


    ystring*    make_symbol( const char* s );
    ystring*    make_symbol( ystring* s );


    void        add_root( yobject* object );
    void        remove_root( yobject* object );

    void        collect();

    void        mark_grey( yobject* object );
    
    unsigned    lock_guard();
    void        unlock_guard( unsigned guard_epoch );
    
    bool        check_epoch( unsigned epoch );
    void        safe();
    void        unsafe();
    void        safepoint();
    
    
private:

    void        make_expands();

    void        mark_locals( std::unique_lock< std::mutex >& lock, yscope* scope );

    void        mark();
    void        sweep();

    std::pair< yobject*, yobject* > get_objects();
    void        add_objects( yobject* head, yobject* last );


    std::mutex  expand_mutex;
    yclass*     expand_empty_class;
    yexpand*    expand_object_proto;
    yexpand*    expand_array_proto;
    yexpand*    expand_table_proto;
    ystandin*   expand_boolean_proto;
    ystandin*   expand_number_proto;
    ystandin*   expand_string_proto;
    ystandin*   expand_function_proto;
    
    std::mutex  symbols_mutex;
    std::unordered_map< symkey, ysymbol > symbols;

    std::mutex  roots_mutex;
    std::unordered_map< yobject*, size_t > roots;

    std::mutex  collector_mutex;
    std::condition_variable collector_condition;
    
    bool        collection_request;
    bool        destroy_request;
    
    std::unordered_set< yscope* > scopes;
    size_t      countdown;
    
    unsigned    guard_epoch;
    unsigned    live_guard_count;
    unsigned    guard_count;

    std::atomic< unsigned > scope_epoch;
    
    ycolour     dead;
    ycolour     unmarked;
    ycolour     marked;
    
    ycolour     sweep_queue[ 2 ];

    std::mutex  objects_mutex;
    yobject*    objects_head;
    yobject*    objects_last;
    
    std::mutex  greylist_mutex;
    yworklist   greylist;
    
    std::thread mark_thread;
    std::thread sweep_thread;
    
};




/*
    All access to a ymodel must be done within the scope of a yscope.
*/

class yscope
{
public:

    explicit yscope( ymodel* model );
    ~yscope();


private:

    friend class ymodel;
    friend class yobject;
    friend class ysymbol;
    friend class yexpand;
    friend class yarray;
    friend class ytable;
    friend class yframe;
    friend class yfunction;
    friend class yguard;

    friend void ysafe();
    friend void yunsafe();
    friend void ysafepoint();
    friend void ycollect();

    friend void yinvoke( yfunction*, yframe& );
    friend void yexec( size_t, unsigned, unsigned );

    static __thread yscope* scope;

    enum markstate
    {
        UNSAFE,
        SAFE,
        GUARD,
        MARKING,
    };
    
    yscope*     previous;
    ymodel*     model;

    unsigned    epoch;
    markstate   state;
    
    ycolour     unmarked;
    ycolour     marked;
    
    yobject*    allocs;
    ystack*     stack;
    
};



/*
    yroot<> is a root that keeps an object alive.
*/

template < typename value_t >
class yroot;



/*
    ylocal<> is a local root.
*/

template < typename value_t >
class ylocal;





/*
    When a thread is at a safepoint (or in a safe period), the thread can
    have its local references marked.  During a safe period local references
    (on the ylang stack and in ylocals) must not be modified.
    
    Leaving a safe period may block if the collector is marking locals.
*/

void ysafe();
void yunsafe();
void ysafepoint();




/*
    Access to references that are visible to more than one thread must occur
    inside a yguard (in addition to any other required locking - a yguard is
    not a mutex).  This allows us to avoid pausing all threads at once while
    guaranteeing that live objects do not slip through the cracks.
    
    Entering a yguard may block if this mutator is behind the epoch.
    
    Entering a yguard is a safepoint.
*/

class yguard
{
public:

    yguard();
    ~yguard();
    

private:

    yguard& operator = ( const yguard& ) = delete;
    
    ymodel*     model;
    unsigned    epoch;
    
};




/*
    Request a collection.
*/

void ycollect();





/*
    ywb< T > is the write barrier for references on the heap.  The
    previous value of a reference is communicated to the garbage collector.
*/

template < typename value_t >
class ywb;


/*
    ywbtraits< T >::wb is the type to use to write-barrier an unknown type.
    Used in situations where generic containers don't know if their contents
    are references or not.
*/

template < typename value_t >
struct ywbtraits
{
    typedef value_t wb;
};



/*
    ymark( wb, work, colour ) marks a write-barried T from the mark thread.
*/

template < typename wb_t >
void ymark( wb_t& wb, yworklist* work, ycolour colour );



/*
    ymarktraits< T > allows partial specialization of ymark.
*/

template < typename wb_t >
struct ymarktraits
{
    static void mark( wb_t& wb, yworklist* work, ycolour colour );
};




/*

*/


/*
    ymodel
*/

inline bool ymodel::check_epoch( unsigned epoch )
{
    unsigned scope_epoch = this->scope_epoch.load( std::memory_order_relaxed );
    return epoch == scope_epoch;
}



/*
    Safepoints.
*/

inline void ysafepoint()
{
    ymodel* model = yscope::scope->model;
    if ( ! model->check_epoch( yscope::scope->epoch ) )
    {
        model->safepoint();
    }
}



/*
    yguard
*/

inline yguard::yguard()
    :   model( yscope::scope->model )
    ,   epoch( model->lock_guard() )
{
}

inline yguard::~yguard()
{
    model->unlock_guard( epoch );
}




/*
    ymark
*/

template < typename wb_t >
void ymark( wb_t& wb, yworklist* work, ycolour colour )
{
    ymarktraits< wb_t >::mark( wb, work, colour );
}

/*
    ymarktraits
*/

template < typename wb_t >
void ymarktraits< wb_t >::mark( wb_t& wb, yworklist* work, ycolour colour )
{
};



#endif
