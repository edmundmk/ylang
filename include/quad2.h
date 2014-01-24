//
//  quad.h
//
//  Created by Edmund Kapusniak on 07/07/2013.
//  Copyright (c) 2013 Edmund Kapusniak. All rights reserved.
//


#ifndef QUAD_H
#define QUAD_H


#include <math3.h>
#include <xform2.h>

#ifdef WITH_XOB
#include <Xob/XobAttrib.h>
#else
#define xobtuple
#define xobclass
#endif

struct rect2;
struct quad2;



/*
    A rect is an axis-aligned bounding box.
*/

struct xobtuple rect2
{

    rect2();
    rect2( float minx, float miny, float maxx, float maxy );
    
    void    expand( float2 point );
    void    expand( const rect2& rect );
    void    expand( const quad2& quad );
    
    quad2   transform( const xroto2& xr ) const;
    quad2   transform( const xform2& xf ) const;
    quad2   transform( const xfmat2& xf ) const;
    quad2   transform( const matrix3& m ) const;
    
    bool    empty() const;
    float   width() const;
    float   height() const;
    float2  centre() const;
    
    bool    intersect( float2 p ) const;
    
    float2  plane_p( float2 n ) const;
    float2  plane_n( float2 n ) const;

#ifdef WITH_XOB
    rect2( xobfixup );
#endif

    float minx;
    float miny;
    float maxx;
    float maxy;
    
};



/*
    A quad is an irregular (but convex) four-sided shape.  It allows
    representation of an arbitrarily transformed rect.  Point order is
    anticlockwise q0 -> q1 -> q2 -> q3.
*/


struct xobtuple quad2
{

    quad2();
    quad2( const rect2& rect );
    quad2( float2 q0, float2 q1, float2 q2, float2 q3 );
    quad2( float minx, float miny, float maxx, float maxy );

    quad2   transform( const xroto2& xr ) const;
    quad2   transform( const xform2& xf ) const;
    quad2   transform( const xfmat2& xf ) const;
    quad2   transform( const matrix3& m ) const;
    
    bool    intersect( float2 p ) const;
    bool    intersect( const rect2& r ) const;

#ifdef WITH_XOB
    quad2( xobfixup );
#endif
    
    float2 q0;
    float2 q1;
    float2 q2;
    float2 q3;

};




inline rect2::rect2()
    :   minx( F_INFINITY )
    ,   miny( F_INFINITY )
    ,   maxx( -F_INFINITY )
    ,   maxy( -F_INFINITY )
{
}

inline rect2::rect2( float minx, float miny, float maxx, float maxy )
    :   minx( minx )
    ,   miny( miny )
    ,   maxx( maxx )
    ,   maxy( maxy )
{
}
    
inline void rect2::expand( float2 point )
{
    minx = min( minx, point.x );
    miny = min( miny, point.y );
    maxx = max( maxx, point.x );
    maxy = max( maxy, point.y );
}

inline void rect2::expand( const rect2& rect )
{
    minx = min( minx, rect.minx );
    miny = min( miny, rect.miny );
    maxx = max( maxx, rect.maxx );
    maxy = max( maxy, rect.maxy );
}

inline void rect2::expand( const quad2& quad )
{
    expand( quad.q0 );
    expand( quad.q1 );
    expand( quad.q2 );
    expand( quad.q3 );
}


inline quad2 rect2::transform( const xroto2& xr ) const
{
    return quad2( *this ).transform( xr );
}

inline quad2 rect2::transform( const xfmat2& xf ) const
{
    return quad2( *this ).transform( xf );
}

inline quad2 rect2::transform( const xform2& xf ) const
{
    return quad2( *this ).transform( xf );
}

inline quad2 rect2::transform( const matrix3& m ) const
{
    return quad2( *this ).transform( m );
}


inline bool rect2::empty() const
{
    return maxx < minx || maxy < miny;
}

inline float rect2::width() const
{
    return maxx - minx;
}

inline float rect2::height() const
{
    return maxy - miny;
}

inline float2 rect2::centre() const
{
    return float2( ( minx + maxx ) * 0.5f, ( miny + maxy ) * 0.5f );
}


inline bool rect2::intersect( float2 p ) const
{
    return minx <= p.x
        && p.x < maxx
        && miny <= p.y
        && p.y < maxy;
}


inline float2 rect2::plane_p( float2 n ) const
{
    // Given the normal of a plane, returns the corner of the rectangle
    // which is farthest away from the plane in the direction of the
    // normal.
    return float2
    (
        n.x < 0.0f ? minx : maxx,
        n.y < 0.0f ? miny : maxy
    );
}

inline float2 rect2::plane_n( float2 n ) const
{
    // Similarly but return the corner nearest the plane.
    return float2
    (
        n.x > 0.0f ? minx : maxx,
        n.y > 0.0f ? miny : maxy
    );
}





inline quad2::quad2()
    :   q0( F_NAN, F_NAN )
    ,   q1( F_NAN, F_NAN )
    ,   q2( F_NAN, F_NAN )
    ,   q3( F_NAN, F_NAN )
{
}

inline quad2::quad2( const rect2& rect )
    :   q0( rect.minx, rect.miny )
    ,   q1( rect.maxx, rect.miny )
    ,   q2( rect.maxx, rect.maxy )
    ,   q3( rect.minx, rect.maxy )
{
}

inline quad2::quad2( float2 q0, float2 q1, float2 q2, float2 q3 )
    :   q0( q0 )
    ,   q1( q1 )
    ,   q2( q2 )
    ,   q3( q3 )
{
}

inline quad2::quad2( float minx, float miny, float maxx, float maxy )
    :   q0( minx, miny )
    ,   q1( maxx, miny )
    ,   q2( maxx, maxy )
    ,   q3( minx, maxy )
{
}


inline quad2 quad2::transform( const xroto2& xr ) const
{
    return quad2
    (
        xr.transform( q0 ),
        xr.transform( q1 ),
        xr.transform( q2 ),
        xr.transform( q3 )
    );
}

inline quad2 quad2::transform( const xform2& xf ) const
{
    return quad2
    (
        xf.transform( q0 ),
        xf.transform( q1 ),
        xf.transform( q2 ),
        xf.transform( q3 )
    );
}

inline quad2 quad2::transform( const xfmat2& xf ) const
{
    return quad2
    (
        xf.transform( q0 ),
        xf.transform( q1 ),
        xf.transform( q2 ),
        xf.transform( q3 )
    );
}

inline quad2 quad2::transform( const matrix3& m ) const
{
    return quad2
    (
        ( float3( q0, 1.0f ) * m ).xy(),
        ( float3( q1, 1.0f ) * m ).xy(),
        ( float3( q2, 1.0f ) * m ).xy(),
        ( float3( q3, 1.0f ) * m ).xy()
    );
}


inline bool quad2::intersect( float2 p ) const
{
    // It's a convex hull with the vertices in anticlockwise order.
    // q0 -> q1 is an edge, n0 is the normal to that edge found by
    // rotating 90 degrees clockwise.  A point is inside the edge
    // if n0 dot ( p - q0 ) < 0.  A point must be inside all edges.
    
    float2 n0 = perpcw2( q1 - q0 );
    float2 n1 = perpcw2( q2 - q1 );
    float2 n2 = perpcw2( q3 - q2 );
    float2 n3 = perpcw2( q0 - q3 );
    
    return dot( n0, p - q0 ) <= 0.0f
        && dot( n1, p - q1 ) <= 0.0f
        && dot( n2, p - q2 ) <= 0.0f
        && dot( n3, p - q3 ) <= 0.0f;
}


inline bool quad2::intersect( const rect2& r ) const
{
    // It's a convex hull with the vertices in anticlockwise order.
    // q0 -> q1 is an edge, n0 is the normal to that edge found by
    // rotating 90 degrees clockwise.  If the rectangle lies outside
    // any of the edges then we do not intersect.

    float2 n0 = perpcw2( q1 - q0 );
    float2 n1 = perpcw2( q2 - q1 );
    float2 n2 = perpcw2( q3 - q2 );
    float2 n3 = perpcw2( q0 - q3 );

    bool sep = dot( n0, r.plane_n( n0 ) - q0 ) > 0.0f
            || dot( n1, r.plane_n( n1 ) - q1 ) > 0.0f
            || dot( n2, r.plane_n( n2 ) - q2 ) > 0.0f
            || dot( n3, r.plane_n( n3 ) - q3 ) > 0.0f;
    
    return ! sep;
}




#endif /* QUAD_H */


