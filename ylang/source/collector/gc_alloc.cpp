//
//  gc_alloc.cpp
//
//  Created by Edmund Kapusniak on 21/06/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#include "gc_alloc.h"
#include <sys/mman.h>


/*

    Allocator is based on dlmalloc:

    This is a version (aka dlmalloc) of malloc/free/realloc written by
    Doug Lea and released to the public domain, as explained at
    http://creativecommons.org/publicdomain/zero/1.0/ Send questions,
    comments, complaints, performance data, etc to dl@cs.oswego.edu

*/


/* -----------------------  Chunk representations ------------------------ */

/*
  (The following includes lightly edited explanations by Colin Plumb.)

  The malloc_chunk declaration below is misleading (but accurate and
  necessary).  It declares a "view" into memory allowing access to
  necessary fields at known offsets from a given base.

  Chunks of memory are maintained using a `boundary tag' method as
  originally described by Knuth.  (See the paper by Paul Wilson
  ftp://ftp.cs.utexas.edu/pub/garbage/allocsrv.ps for a survey of such
  techniques.)  Sizes of free chunks are stored both in the front of
  each chunk and at the end.  This makes consolidating fragmented
  chunks into bigger chunks fast.  The head fields also hold bits
  representing whether chunks are free or in use.

  Here are some pictures to make it clearer.  They are "exploded" to
  show that the state of a chunk can be thought of as extending from
  the high 31 bits of the head field of its header through the
  prev_foot and PINUSE_BIT bit of the following chunk header.

  A chunk that's in use looks like:

   chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
           | Size of previous chunk (if P = 0)                             |
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |P|
         | Size of this chunk                                         1| +-+
   mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         |                                                               |
         +-                                                             -+
         |                                                               |
         +-                                                             -+
         |                                                               :
         +-      size - sizeof(size_t) available payload bytes          -+
         :                                                               |
 chunk-> +-                                                             -+
         |                                                               |
         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |1|
       | Size of next chunk (may or may not be in use)               | +-+
 mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    And if it's free, it looks like this:

   chunk-> +-                                                             -+
           | User payload (must be in use, or we would have merged!)       |
           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |P|
         | Size of this chunk                                         0| +-+
   mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         | Next pointer                                                  |
         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         | Prev pointer                                                  |
         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         |                                                               :
         +-      size - sizeof(struct chunk) unused bytes               -+
         :                                                               |
 chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         | Size of this chunk                                            |
         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ |0|
       | Size of next chunk (must be in use, or we would have merged)| +-+
 mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                                                               :
       +- User payload                                                -+
       :                                                               |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                                                                     |0|
                                                                     +-+
  Note that since we always merge adjacent free chunks, the chunks
  adjacent to a free chunk must be in use.

  Given a pointer to a chunk (which can be derived trivially from the
  payload pointer) we can, in O(1) time, find out whether the adjacent
  chunks are free, and if so, unlink them from the lists that they
  are on and merge them with the current chunk.

  Chunks always begin on even word boundaries, so the mem portion
  (which is returned to the user) is also on an even word boundary, and
  thus at least double-word aligned.

  The P (PINUSE_BIT) bit, stored in the unused low-order bit of the
  chunk size (which is always a multiple of two words), is an in-use
  bit for the *previous* chunk.  If that bit is *clear*, then the
  word before the current chunk size contains the previous chunk
  size, and can be used to find the front of the previous chunk.
  The very first chunk allocated always has this bit set, preventing
  access to non-existent (or non-owned) memory. If pinuse is set for
  any given chunk, then you CANNOT determine the size of the
  previous chunk, and might even get a memory addressing fault when
  trying to do so.

  The C (CINUSE_BIT) bit, stored in the unused second-lowest bit of
  the chunk size redundantly records whether the current chunk is
  inuse (unless the chunk is mmapped). This redundancy enables usage
  checks within free and realloc, and reduces indirection when freeing
  and consolidating chunks.

  Each freshly allocated chunk must have both cinuse and pinuse set.
  That is, each allocated chunk borders either a previously allocated
  and still in-use chunk, or the base of its memory arena. This is
  ensured by making all allocations from the `lowest' part of any
  found chunk.  Further, no free chunk physically borders another one,
  so each free chunk is known to be preceded and followed by either
  inuse chunks or the ends of memory.

  Note that the `foot' of the current chunk is actually represented
  as the prev_foot of the NEXT chunk. This makes it easier to
  deal with alignments etc but can be very confusing when trying
  to extend or adapt this code.

  The exceptions to all this are

     1. The special chunk `top' is the top-most available chunk (i.e.,
        the one bordering the end of available memory). It is treated
        specially.  Top is never included in any bin, is used only if
        no other chunk is available, and is released back to the
        system if it is very large (see M_TRIM_THRESHOLD).  In effect,
        the top chunk is treated as larger (and thus less well
        fitting) than any other available chunk.  The top chunk
        doesn't update its trailing size field since there is no next
        contiguous chunk that would have to index off it. However,
        space is still allocated for it (TOP_FOOT_SIZE) to enable
        separation or merging when space is extended.

     3. Chunks allocated via mmap, have both cinuse and pinuse bits
        cleared in their head fields.  Because they are allocated
        one-by-one, each must carry its own prev_foot field, which is
        also used to hold the offset this chunk has within its mmapped
        region, which is needed to preserve alignment. Each mmapped
        chunk is trailed by the first two fields of a fake next-chunk
        for sake of usage checks.

*/

struct gc_alloc::chunk
{
    static inline chunk* from_link( chunk_link* link )
    {
        return (chunk*)( (char*)link - offsetof( chunk, link ) );
    }
    
    static inline chunk* from_payload( void* p )
    {
        return (chunk*)( (char*)p - offsetof( chunk, link ) );
    }
    
    static inline chunk* offset( chunk* c, ptrdiff_t offset )
    {
        return (chunk*)( (char*)c + offset );
    }
    
    static inline void* payload( chunk* c )
    {
        return (char*)c + offsetof( chunk, link );
    }

    size_t                  prev_foot;  /* Size of previous chunk (if free).    */
    std::atomic< size_t >   head;       /* Size and inuse bits.                 */
    chunk_link              link;       /* double links - used only if free.    */
};



/* ---------------------- Overlaid data structures ----------------------- */

/*
  When chunks are not in use, they are treated as nodes of either
  lists or trees.

  "Small"  chunks are stored in circular doubly-linked lists, and look
  like this:

    chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of previous chunk                            |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `head:' |             Size of chunk, in bytes                         |P|
      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Forward pointer to next chunk in list             |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Back pointer to previous chunk in list            |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Unused space (may be 0 bytes long)                .
            .                                                               .
            .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `foot:' |             Size of chunk, in bytes                           |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  Larger chunks are kept in a form of bitwise digital trees (aka
  tries) keyed on chunksizes.  Because malloc_tree_chunks are only for
  free chunks greater than 256 bytes, their size doesn't impose any
  constraints on user chunk sizes.  Each node looks like:

    chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Size of previous chunk                            |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `head:' |             Size of chunk, in bytes                         |P|
      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Forward pointer to next chunk of same size        |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Back pointer to previous chunk of same size       |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Pointer to left child (child[0])                  |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Pointer to right child (child[1])                 |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Pointer to parent                                 |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             bin index of this chunk                           |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |             Unused space                                      .
            .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `foot:' |             Size of chunk, in bytes                           |
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  Each tree holding treenodes is a tree of unique chunk sizes.  Chunks
  of the same size are arranged in a circularly-linked list, with only
  the oldest chunk (the next to be used, in our FIFO ordering)
  actually in the tree.  (Tree members are distinguished by a non-null
  parent pointer.)  If a chunk with the same size an an existing node
  is inserted, it is linked off the existing node using pointers that
  work in the same way as fd/bk pointers of small chunks.

  Each tree contains a power of 2 sized range of chunk sizes (the
  smallest is 0x100 <= x < 0x180), which is is divided in half at each
  tree level, with the chunks in the smaller half of the range (0x100
  <= x < 0x140 for the top nose) in the left subtree and the larger
  half (0x140 <= x < 0x180) in the right subtree.  This is, of course,
  done by inspecting individual bits.

  Using these rules, each node's left subtree contains all smaller
  sizes than its right subtree.  However, the node at the root of each
  subtree has no particular ordering relationship to either.  (The
  dividing line between the subtree sizes is based on trie relation.)
  If we remove the last chunk of a given size from the interior of the
  tree, we need to replace it with a leaf node.  The tree ordering
  rules permit a node to be replaced by any leaf below it.

  The smallest chunk in a tree (a common operation in a best-fit
  allocator) can be found by walking a path to the leftmost leaf in
  the tree.  Unlike a usual binary tree, where we follow left child
  pointers until we reach a null, here we follow the right child
  pointer any time the left one is null, until we reach a leaf with
  both child pointers null. The smallest chunk in the tree will be
  somewhere along that path.

  The worst case number of steps to add, find, or remove a node is
  bounded by the number of bits differentiating chunks within
  bins. Under current bin calculations, this ranges from 6 up to 21
  (for 32 bit sizes) or up to 53 (for 64 bit sizes). The typical case
  is of course much better.
*/

struct gc_alloc::tree_chunk : public gc_alloc::chunk
{
    static inline tree_chunk* from_link( chunk_link* link )
    {
        return (tree_chunk*)( (char*)link - offsetof( chunk, link ) );
    }

    tree_chunk*             child[ 2 ];
    tree_chunk*             parent;
    uint32_t                index;
};



/*

*/


static const size_t CHUNK_OVERHEAD      = sizeof( size_t );
static const size_t SMALLBIN_SHIFT      = 3;
static const size_t SMALLBIN_WIDTH      = 1 << SMALLBIN_SHIFT;
static const size_t TREEBIN_SHIFT       = 8;
static const size_t MIN_LARGE_SIZE      = 1 << TREEBIN_SHIFT;
static const size_t MAX_SMALL_SIZE      = MIN_LARGE_SIZE - 1;

static const size_t CINUSE_BIT = 0x01;
static const size_t PINUSE_BIT = 0x02;

static inline bool cinuse( size_t head ) { return ( head & CINUSE_BIT ) != 0; }
static inline bool pinuse( size_t head ) { return ( head & PINUSE_BIT ) != 0; }
static inline size_t chunksize( size_t head ) { return head & ~(CINUSE_BIT|PINUSE_BIT); }


/*

*/


gc_alloc::gc_alloc()
{
    for ( size_t i = 0; i < NSMALLBINS; ++i )
    {
        _smallbins[ i ].next = &_smallbins[ i ];
        _smallbins[ i ].prev = &_smallbins[ i ];
    }
    
    for ( size_t i = 0; i < NTREEBINS; ++i )
    {
        _treebins[ i ] = nullptr;
    }
    
    _dv     = nullptr;
    _dvsize = 0;
    _top    = nullptr;
    
}

gc_alloc::~gc_alloc()
{
    for ( const auto& segment : _segments )
    {
        munmap( segment.base, segment.size );
    }
}
    

/*
 Basic algorithm:
 If a small request (< 256 bytes minus per-chunk overhead):
   1. If one exists, use a remainderless chunk in associated smallbin.
      (Remainderless means that there are too few excess bytes to
      represent as a chunk.)
   2. If it is big enough, use the dv chunk, which is normally the
      chunk adjacent to the one used for the most recent small request.
   3. If one exists, split the smallest available chunk in a bin,
      saving remainder in dv.
   4. If it is big enough, use the top chunk.
   5. If available, get memory from system and use it
 Otherwise, for a large request:
   1. Find the smallest available binned chunk that fits, and use it
      if it is better fitting than dv chunk, splitting if necessary.
   2. If better fitting than any binned chunk, use the dv chunk.
   3. If it is big enough, use the top chunk.
   4. If request size >= mmap threshold, try to directly mmap this chunk.
   5. If available, get memory from system and use it

*/

void* gc_alloc::malloc( uint8_t colour, size_t size )
{

    

}


void gc_alloc::free( void* p )
{
}




/*
    The sweeper avoids locking as much as possible.  It works on a segment at
    a time.  Its view of a segment is at the level of individual chunks.
 
    During the sweep of a segment, the following things can happen:
    
      -  The allocator splits a free chunk into an allocated chunk and (if
         any) a free remainder.  In this case the write that updates the inuse
         bits in the chunk header must be a store-release, ensuring that the
         creation of the new chunk and initialization of the allocated object
         are visible to the sweeper.
         
      -  The allocator could have merged the segment we are sweeping with an
         adjacent segment or segments.  To avoid this, we take a copy of the
         allocator's segments at the start of the sweep.  Segment merging can
         resize free chunks, but it can only make them bigger, and only those
         at the start or end of a segment.  I think we are safe.
 
*/

gc_sweep::gc_sweep( gc_alloc* alloc )
    :   _alloc( alloc )
    ,   _segindex( 0 )
    ,   _chunk( nullptr )
    ,   _limit( nullptr )
    ,   _p( nullptr )
{
    // Take a copy of the allocator's segments.
    std::lock_guard< std::mutex > lock( _alloc->_mutex );
    for ( const auto& segment : _alloc->_segments )
    {
        sweep_segment s;
        s.chunk = (gc_alloc::chunk*)segment.base;
        s.limit = (char*)segment.base + segment.size;
        
        // Skip over free chunks at the start of the segment.
        while ( s.chunk < s.limit )
        {
            size_t head = s.chunk->head.load( std::memory_order_relaxed );
            if ( cinuse( head ) )
            {
                break;
            }
            s.chunk = gc_alloc::chunk::offset( s.chunk, chunksize( head ) );
        }
        
        if ( s.chunk < s.limit )
        {
            _segments.push_back( s );
        }
    }
}

gc_sweep::~gc_sweep()
{
}


void* gc_sweep::next()
{
    while ( true )
    {
        // Check if we've reached the end of the segment.
        if ( _chunk >= _limit )
        {
            if ( _segindex < _segments.size() )
            {
                const sweep_segment& segment = _segments.at( _segindex );
                _chunk = segment.chunk;
                _limit = segment.limit;
                _segindex += 1;
            }
            else
            {
                _p = nullptr;
                return _p;
            }
        }
    
        // Read head of the chunk.
        size_t head = _chunk->head.load( std::memory_order_acquire );

        // Move to next chunk.
        _p = gc_alloc::chunk::payload( _chunk );
        _chunk = gc_alloc::chunk::offset( _chunk, chunksize( head ) );
        
        // Return chunks that are in use.
        if ( cinuse( head ) )
        {
            return _p;
        }
    }
}

void* gc_sweep::free()
{
    void* free_p = _p;
    void* next_p = next();
    _alloc->free( free_p );
    return next_p;
}





