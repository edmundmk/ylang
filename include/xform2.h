//
//  xform.h
//
//  Created by Edmund Kapusniak on 12/04/2013.
//  Copyright 2013 Edmund Kapusniak 2013. All rights reserved.
//


#ifndef XFORM_H
#define XFORM_H


#include <math3.h>

#ifdef WITH_XOB
#include <Xob/XobAttrib.h>
#else
#define xobclass
#define xobtuple
#endif

#ifdef WITH_BOX2D
#include <Box2D/Box2D.h>
#endif


struct xroto2;
struct xform2;
struct xfmat2;



static const float XF_T_EPSILON = 0.1f;
static const float XF_R_EPSILON = F_TAU / 3600.0f;
static const float XF_S_EPSILON = 0.001f;



/*
    Represents a proper rigid transformation in two dimensions, consisting
    solely of rotations and translations.  Any combination of such
    transformations is also a rigid transformation.  The rotation component r
    is expressed in radians, and is normalized to lie between 0 and tau.
*/

struct xobtuple xroto2
{
    xroto2();
    xroto2( float2 t, float r );
    xroto2( float tx, float ty, float r );

    bool is_identity() const;

    matrix3 matrix3() const;
    matrix4 matrix4() const;

    float2  transform( float2 v ) const;

#ifdef WITH_XOB
    xroto2( xobfixup );
#endif

#ifdef WITH_BOX2D
    xroto2( const b2Transform& xf );
    operator b2Transform () const;
#endif

    float2  t;
    float   r;
};


float2 operator * ( float2 a, const xroto2& b );
xroto2 operator * ( const xroto2& a, const xroto2& b );
float2& operator *= ( float2 a, const xroto2& b );
xroto2& operator *= ( xroto2& a, const xroto2& b );
xroto2 inverse( const xroto2& xr );




/*
    Represents a more general two-dimensional transformation, including
    scale and skew components.  Storing a transformation in this form allows
    for meaningful interpolation between transformations.  The rotation
    component r becomes two independent skew angles - when they are equal the
    transformation represents a pure rotation with no skew.
    
    This type of transform is relatively expensive to multiply together, as
    effectively the new skew angles and scale factors are recovered from the
    matrix representation of the new transformation.
*/

struct xobtuple xform2
{
    xform2();
    xform2( const xroto2& xf );
    xform2( float2 t, float2 r, float2 s );
    xform2( float tx, float ty, float rx, float ry, float sx, float sy );

    bool is_identity() const;

    explicit xform2( const xfmat2& m );
    explicit xform2( const matrix3& m );
    explicit xform2( const matrix4& m );
    
    matrix3 matrix3() const;
    matrix4 matrix4() const;

    float2  transform( float2 v ) const;

#ifdef WITH_XOB
    xform2( xobfixup );
#endif

    float2  t;
    float2  r;
    float2  s;
};


float2 operator * ( float2 a, const xform2& b );
xform2 operator * ( const xform2& a, const xform2& b );
float2& operator *= ( float2 a, const xform2& b );
xform2& operator *= ( xform2& a, const xform2& b );



/*
    Represents a two-dimensional transform - like xform2 - but in matrix form.
    Manipulating an xfmat2 is faster than using a full matrix3.  Multiplying
    using xfmat2 is quicker than using xform2.
*/

struct xobtuple xfmat2
{
public:

    xfmat2();
    xfmat2( const xroto2& xf );
    xfmat2( const xform2& xf );
    xfmat2( float m00, float m10,
                float m01, float m11,
                    float m02, float m12 );
    float3& operator [] ( size_t column );
    const float3& operator [] ( size_t column ) const;

    bool is_identity() const;

    matrix3 matrix3() const;
    matrix4 matrix4() const;

    float2  transform( float2 v ) const;

#ifdef WITH_XOB
    xfmat2( xobfixup );
#endif

private:

    float3  column_a;
    float3  column_b;
    
};


float2 operator * ( float2 a, const xfmat2& b );
xfmat2 operator * ( const xfmat2& a, const xfmat2& b );
float2& operator *= ( float2& a, const xfmat2& b );
xfmat2& operator *= ( xfmat2& a, const xfmat2& b );
xfmat2 inverse( const xfmat2& m );





inline xroto2::xroto2()
    :   t( 0.0f, 0.0f )
    ,   r( 0.0f )
{
}

inline xroto2::xroto2( float2 t, float r )
    :   t( t )
    ,   r( r )
{
}

inline xroto2::xroto2( float tx, float ty, float r )
    :   t( tx, ty )
    ,   r( r )
{
}

inline bool xroto2::is_identity() const
{
    return fabsf( t.x ) < XF_T_EPSILON
        && fabsf( t.y ) < XF_T_EPSILON
        && fabsf( r ) < XF_R_EPSILON;
}

inline float2 xroto2::transform( float2 v ) const
{
    float sin_r = sinf( r );
    float cos_r = cosf( r );

    return float2
    (
        v.x * cos_r - v.y * sin_r + t.x,
        v.x * sin_r + v.y * cos_r + t.y
    );
}

inline matrix3 xroto2::matrix3() const
{
    float sin_r = sinf( r );
    float cos_r = cosf( r );

    return ::matrix3
    (
        cos_r,  sin_r,  0.0f,
        -sin_r, cos_r,  0.0f,
        t.x,    t.y,    1.0f
    );
}

inline matrix4 xroto2::matrix4() const
{
    float sin_r = sinf( r );
    float cos_r = cosf( r );

    return ::matrix4
    (
        cos_r,  sin_r,  0.0f,   0.0f,
        -sin_r, cos_r,  0.0f,   0.0f,
        0.0f,   0.0f,   1.0f,   0.0f,
        t.x,    t.y,    0.0f,   1.0f
    );
}


#ifdef WITH_BOX2D

inline xroto2::xroto2( const b2Transform& xf )
    :   t( xf.p )
    ,   r( xf.q.GetAngle() )
{
}

inline xroto2::operator b2Transform () const
{
    return b2Transform( t, b2Rot( r ) );
}

#endif

inline float2 operator * ( float2 a, const xroto2& b )
{
    return b.transform( a );
}

inline xroto2 operator * ( const xroto2& a, const xroto2& b )
{
    return xroto2( a.t * b, a.r + b.r );
}

inline float2& operator *= ( float2 a, const xroto2& b )
{ 
    return a = b.transform( a );
}

inline xroto2& operator *= ( xroto2& a, const xroto2& b )
{
    return a = a * b;
}

inline xroto2 inverse( const xroto2& xr )
{
    return xroto2( -xr.t, 0.0f ) * xroto2( 0.0f, 0.0f, -xr.r );
}




inline xform2::xform2()
    :   t( 0.0f, 0.0f )
    ,   r( 0.0f, 0.0f )
    ,   s( 1.0f, 1.0f )
{
}

inline xform2::xform2( const xroto2& xf )
    :   t( xf.t )
    ,   r( xf.r, xf.r )
    ,   s( 1.0f, 1.0f )
{
}

inline xform2::xform2( float2 t, float2 r, float2 s )
    :   t( t )
    ,   r( modp( r.x, F_TAU ), modp( r.y, F_TAU ) )
    ,   s( s )
{
}

inline xform2::xform2( float tx, float ty, float rx, float ry, float sx, float sy )
    :   t( tx, ty )
    ,   r( modp( rx, F_TAU ), modp( ry, F_TAU ) )
    ,   s( sx, sy )
{
}

inline xform2::xform2( const xfmat2& m )
    :   t( m[ 0 ][ 2 ], m[ 1 ][ 2 ] )
    ,   r( modp( atan2f( - m[ 0 ][ 1 ], m[ 1 ][ 1 ] ), F_TAU ), modp( atan2f( m[ 1 ][ 0 ], m[ 0 ][ 0 ] ), F_TAU ) )
    ,   s( length( float2( m[ 0 ][ 0 ], m[ 1 ][ 0 ] ) ), length( float2( m[ 0 ][ 1 ], m[ 1 ][ 1 ] ) ) )
{
}

inline xform2::xform2( const struct matrix3& m )
    :   t( m[ 0 ][ 2 ], m[ 1 ][ 2 ] )
    ,   r( modp( atan2f( - m[ 0 ][ 1 ], m[ 1 ][ 1 ] ), F_TAU ), modp( atan2f( m[ 1 ][ 0 ], m[ 0 ][ 0 ] ), F_TAU ) )
    ,   s( length( float2( m[ 0 ][ 0 ], m[ 1 ][ 0 ] ) ), length( float2( m[ 0 ][ 1 ], m[ 1 ][ 1 ] ) ) )
{
}

inline xform2::xform2( const struct matrix4& m )
    :   t( m[ 0 ][ 3 ], m[ 1 ][ 3 ] )
    ,   r( modp( atan2f( - m[ 0 ][ 1 ], m[ 1 ][ 1 ] ), F_TAU ), modp( atan2f( m[ 1 ][ 0 ], m[ 0 ][ 0 ] ), F_TAU ) )
    ,   s( length( float2( m[ 0 ][ 0 ], m[ 1 ][ 0 ] ) ), length( float2( m[ 0 ][ 1 ], m[ 1 ][ 1 ] ) ) )
{
}

inline bool xform2::is_identity() const
{
    return fabsf( t.x ) < XF_T_EPSILON
        && fabsf( t.y ) < XF_T_EPSILON
        && fabsf( r.x ) < XF_R_EPSILON
        && fabsf( r.y ) < XF_R_EPSILON
        && fabsf( s.x - 1.0f ) < XF_S_EPSILON
        && fabsf( s.y - 1.0f ) < XF_S_EPSILON;
}


inline float2 xform2::transform( float2 v ) const
{
    return ( float3( v, 1.0f ) * matrix3() ).xy();
}

inline matrix3 xform2::matrix3() const
{
    float sin_rx = sinf( r.x );
    float cos_rx = cosf( r.x );
    float sin_ry = sinf( r.y );
    float cos_ry = cosf( r.y );
    
    return ::matrix3
    (
        s.x * cos_ry,  s.x * sin_ry,  0.0f,
        s.y * -sin_rx, s.y * cos_rx,  0.0f,
        t.x,           t.y,           1.0f
    );
}

inline matrix4 xform2::matrix4() const
{
    float sin_rx = sinf( r.x );
    float cos_rx = cosf( r.x );
    float sin_ry = sinf( r.y );
    float cos_ry = cosf( r.y );
    
    return ::matrix4
    (
        s.x * cos_ry,  s.x * sin_ry,  0.0f, 0.0f,
        s.y * -sin_rx, s.y * cos_rx,  0.0f, 0.0f,
        0.0f,          0.0f,          1.0f, 0.0f,
        t.x,           t.y,           0.0f, 1.0f
    );
}




inline float2 operator * ( float2 a, const xform2& b )
{
    return b.transform( a );
}

inline xform2 operator * ( const xform2& a, const xform2& b )
{
    return xform2( a.matrix3() * b.matrix3() );
}

inline float2& operator *= ( float2 a, const xform2& b )
{
    return a = b.transform( a );
}

inline xform2& operator *= ( xform2& a, const xform2& b )
{
    return a = a * b;
}





inline xfmat2::xfmat2()
{
    column_a = float3( 1.0f, 0.0f, 0.0f );
    column_b = float3( 0.0f, 1.0f, 0.0f );
}

inline xfmat2::xfmat2( const xroto2& xf )
{
    float sin_r = sinf( xf.r );
    float cos_r = cosf( xf.r );

    column_a = float3( cos_r, -sin_r, xf.t.x );
    column_b = float3( sin_r, cos_r, xf.t.y );
}

inline xfmat2::xfmat2( const xform2& xf )
{
    float sin_rx = sinf( xf.r.x );
    float cos_rx = cosf( xf.r.x );
    float sin_ry = sinf( xf.r.y );
    float cos_ry = cosf( xf.r.y );

    column_a = float3( xf.s.x * cos_ry, xf.s.y * -sin_rx, xf.t.x );
    column_b = float3( xf.s.x * sin_ry, xf.s.y * cos_rx, xf.t.y );
}

inline xfmat2::xfmat2( float m00, float m10,
                            float m01, float m11,
                                float m02, float m12 )
{
    column_a = float3( m00, m01, m02 );
    column_b = float3( m10, m11, m12 );
}

inline float3& xfmat2::operator [] ( size_t column )
{
    switch ( column )
    {
    case 0:     return column_a;
    case 1:     return column_b;
    default:    assert( ! "invalid index" );
    }
}

inline const float3& xfmat2::operator [] ( size_t column ) const
{
    switch ( column )
    {
    case 0:     return column_a;
    case 1:     return column_b;
    default:    assert( ! "invalid index" );
    }
}

inline bool xfmat2::is_identity() const
{
    const xfmat2& m = *this;
    const float SIN_XF_R_EPSILON = sinf( XF_R_EPSILON );
    return fabsf( m[ 0 ][ 0 ] - 1.0f ) < SIN_XF_R_EPSILON
        && fabsf( m[ 1 ][ 0 ] ) < SIN_XF_R_EPSILON
        && fabsf( m[ 0 ][ 1 ] ) < SIN_XF_R_EPSILON
        && fabsf( m[ 1 ][ 1 ] - 1.0f ) < SIN_XF_R_EPSILON
        && fabsf( m[ 0 ][ 2 ] ) < XF_T_EPSILON
        && fabsf( m[ 1 ][ 2 ] ) < XF_T_EPSILON;
}

inline matrix3 xfmat2::matrix3() const
{
    const xfmat2& m = *this;
    return ::matrix3
    (
        m[ 0 ][ 0 ], m[ 1 ][ 0 ], 0.0f,
        m[ 0 ][ 1 ], m[ 1 ][ 1 ], 0.0f,
        m[ 0 ][ 2 ], m[ 1 ][ 2 ], 1.0f
    );
}

inline matrix4 xfmat2::matrix4() const
{
    const xfmat2& m = *this;
    return ::matrix4
    (
        m[ 0 ][ 0 ], m[ 1 ][ 0 ], 0.0f, 0.0f,
        m[ 0 ][ 1 ], m[ 1 ][ 1 ], 0.0f, 0.0f,
        0.0f,        0.0f,        1.0f, 0.0f,
        m[ 0 ][ 2 ], m[ 1 ][ 2 ], 0.0f, 1.0f
    );
}

inline float2 xfmat2::transform( float2 v ) const
{
    return float2
    (
        dot( v, column_a.xy() ) + column_a.z,
        dot( v, column_b.xy() ) + column_b.z
    );
}

inline float2 operator * ( float2 a, const xfmat2& b )
{
    return b.transform( a );
}

inline xfmat2 operator * ( const xfmat2& a, const xfmat2& b )
{
    float2 row0 = float2( a[ 0 ][ 0 ], a[ 1 ][ 0 ] );
    float2 row1 = float2( a[ 0 ][ 1 ], a[ 1 ][ 1 ] );
    float2 row2 = float2( a[ 0 ][ 2 ], a[ 1 ][ 2 ] );
    return xfmat2
    (
        dot( row0, b[ 0 ].xy() ),
        dot( row0, b[ 1 ].xy() ),
     
        dot( row1, b[ 0 ].xy() ),
        dot( row1, b[ 1 ].xy() ),
     
        dot( row2, b[ 0 ].xy() ) + b[ 0 ].z,
        dot( row2, b[ 1 ].xy() ) + b[ 1 ].z
    );
}

inline float2& operator *= ( float2& a, const xfmat2& b )
{
    return a = a * b;
}

inline xfmat2& operator *= ( xfmat2& a, const xfmat2& b )
{
    return a = a * b;
}

inline xfmat2 inverse( const xfmat2& m )
{
    /*
        An xfmat2 can be decomposed into a rotation part and a translation
        part:
        
                          M = R * T
                       M M' = I
            R * T * T' * R' = I
                         M' = T' * R'
        
        The rotation part is a 2x2 matrix, while the translation part is
        simply the translation vector.  The inverse of a 2x2 matrix is
        relatively simple to calculate.  The inverse of a translation is
        found by negating the vector.
        
        Multiplying T' and R' is simple as most of the elements are 0.
        
            1 0 0     a b 0     a b 0
            0 1 0  *  c d 0  =  c d 0
            x y 1     0 0 1     p q 1
            
        Where:
        
            p = x * a + y * c
            q = x * b + y * d
     
    */

    float x = -m[ 0 ][ 2 ];
    float y = -m[ 1 ][ 2 ];

    float det = 1.0f /
            ( m[ 0 ][ 0 ] * m[ 1 ][ 1 ] - m[ 1 ][ 0 ] * m[ 0 ][ 1 ] );
    
    float a = m[ 1 ][ 1 ] * det;
    float b = -m[ 1 ][ 0 ] * det;
    float c = -m[ 0 ][ 1 ] * det;
    float d = m[ 0 ][ 0 ] * det;
    float p = x * a + y * c;
    float q = x * b + y * d;
    
    return xfmat2
    (
        a, b,
        c, d,
        p, q
    );
}






#endif /* XFORM_H */
