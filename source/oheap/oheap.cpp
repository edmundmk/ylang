//
//  oheap.cpp
//
//  Created by Edmund Kapusniak on 23/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include <oheap/oheap.h>
#include <thread>
#include <vector>
#include <list>


namespace oimpl
{

__thread context* current = nullptr;



/*
    Slot buffers remember the values of slots at the previous snapshot point.
*/


class slot_buffer
{
public:

    struct record
    {
        slot*       pslot;
        uintptr_t   oldp;
    };
    
    slot_buffer();
    ~slot_buffer();
    
    const record*   find( slot* pslot ) const;
    bool            add( slot* pslot, uintptr_t oldp );
    void            clear();


private:

    static const size_t RECORD_COUNT = 180;
    
    std::atomic_size_t  size;
    record              records[ RECORD_COUNT ];

};



slot_buffer::slot_buffer()
    :   size( 0 )
{
}

const slot_buffer::record* slot_buffer::find( slot* pslot ) const
{
    // Load the current size with acquire ordering (so that we can safely read
    // records written before size was stored).
    size_t count = size.load( std::memory_order_acquire );
    
    // Do a linear search to find a record of the slot.
    for ( size_t i = 0; i < count; ++i )
    {
        if ( records[ i ].pslot == pslot )
        {
            return records + i;
        }
    }
    
    return nullptr;
}

bool slot_buffer::add( slot* pslot, uintptr_t oldp )
{
    // Check for full buffer.
    size_t count = size.load( std::memory_order_relaxed );
    if ( count >= RECORD_COUNT )
    {
        return false;
    }
    
    // Write the record.
    records[ count ].pslot = pslot;
    records[ count ].oldp  = oldp;
    
    // Update count with release ordering so that the writes above are visible
    // to threads calling find().
    count += 1;
    size.store( count, std::memory_order_release );
    
    return true;
}

void slot_buffer::clear()
{
    size.store( 0, std::memory_order_relaxed );
}







/*
    Garbage collected heap.
*/

class heap
{
public:

    heap();
    ~heap();
    
    oimpl::context* new_context();
    void delete_context( oimpl::context* context );
    
    void root_update( root* proot, void* p );
    void root_delete( root* proot );
    
    void slot_record( slot* pslot, uintptr_t oldp );
    
    void collect( oimpl::context* current );
    void safepoint( oimpl::context* current );


private:

    enum request
    {
        IDLE,       // Don't do anything.
        COLLECT,    // Run a GC collection.
        QUIT,       // Run a GC collection and quit.
    };
    
    enum state
    {
        RUNNING,    // Mutators run as normal.
        PAUSING,    // Mutators begin handshake at next safepoint.
        STOPPED,    // Collector marks roots.
        MARKING,    // Some mutators running, some still marking locals.
    };


    void collector();


    std::thread                 thread;
    std::mutex                  mutex;
    std::condition_variable     condition;

    request                     request;
    state                       state;
    colour                      colour;

    root                        roots;
    std::list< context* >       mutators;
    std::vector< slot_buffer* > slots;
    seglist< void* >            allocs;
    
};








void root_update( root* proot, void* p )
{
}


void root_delete( root* proot )
{
}


void slot_record( slot* pslot, uintptr_t oldp )
{
}


void* alloc( otype* type, size_t length )
{
    return nullptr;
}


}





oheap::oheap()
    :   p( new oimpl::heap() )
{
}

oheap::~oheap()
{
    delete p;
}



oscope::oscope( oheap* heap )
{
    assert( oimpl::current == nullptr );
    oimpl::current = heap->p->new_context();
}

oscope::~oscope()
{
    oimpl::current->heap->delete_context( oimpl::current );
    delete oimpl::current;
}



void ocollect()
{
    oimpl::current->heap->collect( oimpl::current );
}


void osafepoint()
{
    oimpl::current->heap->safepoint( oimpl::current );
}








