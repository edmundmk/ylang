//
//  math3.h
//
//  Created by Edmund Kapusniak on 30/12/2011.
//  Copyright 2011 Edmund Kapusniak 2011. All rights reserved.
//


#ifndef MATH3_H
#define MATH3_H


#include <stddef.h>
#include <math.h>
#include <limits>
#include <assert.h>

#ifdef WITH_XOB
#include <Xob/XobAttrib.h>
#else
#define xobclass
#define xobtuple
#endif

#ifdef WITH_BOX2D
#include <Box2D/Box2D.h>
#endif


#ifdef _MSC_VER

inline float copysignf( float x, float y )
{
    return (float)_copysign( x, y );
}

#endif



static const float F_INFINITY   = std::numeric_limits< float >::infinity();
static const float F_NAN        = std::numeric_limits< float >::quiet_NaN();
static const float F_TAU        = 6.283185307179586f;
static const float F_PI         = F_TAU * 0.5f;


float min( float a, float b );
float max( float a, float b );
float clamp( float v, float min, float max );
float lerp( float a, float b, float alpha );


/*
    v == divp( v, x ) + modp( v, x )
    sign( modp( v, x ) ) == sign( x )
*/

float modp( float v, float divisor );
float divp( float v, float divisor );



/*
    Vector types.
*/


struct xobtuple float2
{
    float2();
    float2( float x, float y );
    
    float& operator [] ( size_t element );
    const float& operator [] ( size_t element ) const;

#ifdef WITH_XOB
    float2( xobfixup );
#endif

#ifdef WITH_BOX2D
    float2( const b2Vec2& v );
    operator b2Vec2 () const;
#endif

    float x, y;
};

float2 operator - ( float2 a );
float2 operator + ( float2 a );

float2 operator + ( float2 a, float2 b );
float2 operator - ( float2 a, float2 b );
float2 operator * ( float2 a, float2 b );
float2 operator * ( float2 a, float b );
float2 operator * ( float a, float2 b );
float2 operator / ( float2 a, float2 b );
float2 operator / ( float2 a, float b );
float2 operator / ( float a, float2 b );

float2& operator += ( float2& a, float2 b );
float2& operator -= ( float2& a, float2 b );
float2& operator *= ( float2& a, float2 b );
float2& operator *= ( float2& a, float b );
float2& operator /= ( float2& a, float2 b );
float2& operator /= ( float2& a, float b );

float dot( float2 a, float2 b );
float length( float2 a );
float lengthsq( float2 a );
float2 normalize( float2 a );
float2 sincos2( float angle );
float2 perpccw2( float2 a );
float2 perpcw2( float2 a );


struct xobtuple float3
{
    float3();
    float3( float x, float y, float z );
    float3( float2 xy, float z );

    const float2& xy() const;
    float2& xy();
    float& operator [] ( size_t element );
    const float& operator [] ( size_t element ) const;
    
#ifdef WITH_XOB
    float3( xobfixup );
#endif
    
    float x, y, z;
};

float3 operator - ( float3 a );
float3 operator + ( float3 a );

float3 operator + ( float3 a, float3 b );
float3 operator - ( float3 a, float3 b );
float3 operator * ( float3 a, float3 b );
float3 operator * ( float3 a, float b );
float3 operator * ( float a, float3 b );
float3 operator / ( float3 a, float3 b );
float3 operator / ( float3 a, float b );
float3 operator / ( float a, float3 b );

float3& operator += ( float3& a, float3 b );
float3& operator -= ( float3& a, float3 b );
float3& operator *= ( float3& a, float3 b );
float3& operator *= ( float3& a, float b );
float3& operator /= ( float3& a, float3 b );
float3& operator /= ( float3& a, float b );

float dot( float3 a, float3 b );
float length( float3 a );
float lengthsq( float3 a );
float3 normalize( float3 a );



struct xobtuple float4
{
    float4();
    float4( float x, float y, float z, float w );
    float4( float2 xy, float z, float w );
    float4( float3 xyz, float w );
    
    const float2& xy() const;
    const float3& xyz() const;
    float2& xy();
    float3& xyz();
    float& operator [] ( size_t element );
    const float& operator [] ( size_t element ) const;

#ifdef WITH_XOB
   float4( xobfixup );
#endif

    float x, y, z, w;
};

float4 operator - ( float4 a );
float4 operator + ( float4 a );

float4 operator + ( float4 a, float4 b );
float4 operator - ( float4 a, float4 b );
float4 operator * ( float4 a, float4 b );
float4 operator * ( float4 a, float b );
float4 operator * ( float a, float4 b );
float4 operator / ( float4 a, float4 b );
float4 operator / ( float4 a, float b );
float4 operator / ( float a, float4 b );

float4& operator += ( float4& a, float4 b );
float4& operator -= ( float4& a, float4 b );
float4& operator *= ( float4& a, float4 b );
float4& operator *= ( float4& a, float b );
float4& operator /= ( float4& a, float4 b );
float4& operator /= ( float4& a, float b );

float dot( float4 a, float4 b );
float length( float4 a );
float lengthsq( float4 a );
float4 normalize( float4 a );




/*
    Matrices are stored and accessed in column-major order.
*/

struct matrix2
{
public:

    matrix2();
    matrix2( float m00, float m10, float m01, float m11 );
    float2& operator [] ( size_t column );
    const float2& operator [] ( size_t column ) const;

#ifdef WITH_XOB
    matrix2( xobfixup );
#endif

private:

    float2 columns[ 2 ];

};

matrix2 operator * ( const matrix2& a, const matrix2& b );
float2 operator * ( float2 a, const matrix2& b );
float2 operator * ( const matrix2& a, float2 b );

matrix2& operator *= ( matrix2& a, const matrix2& b );
float2& operator *= ( float2& a, const matrix2& b );

matrix2 inverse( const matrix2& m );


struct matrix3
{
public:

    matrix3();
    matrix3( float m00, float m10, float m20,
                float m01, float m11, float m21,
                    float m02, float m12, float m22 );
    float3& operator [] ( size_t column );
    const float3& operator [] ( size_t column ) const;

#ifdef WITH_XOB
    matrix3( xobfixup );
#endif

private:

    float3 columns[ 3 ];

};

matrix3 operator * ( const matrix3& a, const matrix3& b );
float3 operator * ( float3 a, const matrix3& b );
float3 operator * ( const matrix3& a, float3 b );

matrix3& operator *= ( matrix3& a, const matrix3& b );
float3& operator *= ( float3& a, const matrix3& b );

matrix3 inverse( const matrix3& m );


struct matrix4
{
public:

    matrix4();
    matrix4( float m00, float m10, float m20, float m30,
                float m01, float m11, float m21, float m31,
                    float m02, float m12, float m22, float m32,
                        float m03, float m13, float m23, float m33 );
    float4& operator [] ( size_t column );
    const float4& operator [] ( size_t column ) const;

#ifdef WITH_XOB
    matrix4( xobfixup );
#endif

private:

    float4 columns[ 4 ];

};

matrix4 operator * ( const matrix4& a, const matrix4& b );
float4 operator * ( float4 a, const matrix4& b );
float4 operator * ( const matrix4& a, float4 b );

matrix4& operator *= ( matrix4& a, const matrix4& b );
float4& operator *= ( float4& a, const matrix4& b );

matrix4 inverse( const matrix4& m );





inline float min( float a, float b )
{
    if ( a < b )
        return a;
    else
        return b;
}

inline float max( float a, float b )
{
    if ( a > b )
        return a;
    else
        return b;
}

inline float clamp( float v, float min, float max )
{
    if ( v < min )
        return min;
    else if ( v > max )
        return max;
    else
        return v;
}

inline float modp( float v, float divisor )
{
    if ( v >= 0.0f )
        return fmodf( v, fabsf( divisor ) );
    else
        return fmodf( v, fabsf( divisor ) ) + fabsf( divisor );
}

inline float divp( float v, float divisor )
{
    return v - modp( v, divisor );
}

inline float lerp( float a, float b, float alpha )
{
    return a + alpha * ( b - a );
}




inline float2::float2()
    :   x( 0.0f )
    ,   y( 0.0f )
{
}

inline float2::float2( float x, float y )
    :   x( x )
    ,   y( y )
{
}

inline float& float2::operator [] ( size_t element )
{
    return ( &x )[ element ];
}

inline const float& float2::operator [] ( size_t element ) const
{
    return ( &x )[ element ];
}


#ifdef WITH_BOX2D

inline float2::float2( const b2Vec2& v )
    :   x( v.x )
    ,   y( v.y )
{
}

inline float2::operator b2Vec2 () const
{
    return b2Vec2( x, y );
}

#endif


inline float2 operator - ( float2 a )
{
    return float2( -a.x, -a.y );
}

inline float2 operator + ( float2 a )
{
    return a;
}

inline float2 operator + ( float2 a, float2 b )
{
    return float2( a.x + b.x, a.y + b.y );
}

inline float2 operator - ( float2 a, float2 b )
{
    return float2( a.x - b.x, a.y - b.y );
}

inline float2 operator * ( float2 a, float2 b )
{
    return float2( a.x * b.x, a.y * b.y );
}

inline float2 operator * ( float2 a, float b )
{
    return float2( a.x * b, a.y * b );
}

inline float2 operator * ( float a, float2 b )
{
    return float2( a * b.x, a * b.y );
}

inline float2 operator / ( float2 a, float2 b )
{
    return float2( a.x / b.x, a.y / b.y );
}

inline float2 operator / ( float2 a, float b )
{
    return float2( a.x / b, a.y / b );
}

inline float2 operator / ( float a, float2 b )
{
    return float2( a / b.x, a / b.y );
}

inline float2& operator += ( float2& a, float2 b )
{
    a.x += b.x; a.y += b.y;
    return a;
}

inline float2& operator -= ( float2& a, float2 b )
{
    a.x -= b.x; a.y -= b.y;
    return a;
}

inline float2& operator *= ( float2& a, float2 b )
{
    a.x *= b.x; a.y *= b.y;
    return a;
}

inline float2& operator *= ( float2& a, float b )
{
    a.x *= b; a.y *= b;
    return a;
}

inline float2& operator /= ( float2& a, float2 b )
{
    a.x /= b.x; a.y /= b.y;
    return a;
}

inline float2& operator /= ( float2& a, float b )
{
    a.x /= b; a.y /= b;
    return a;
}

inline float dot( float2 a, float2 b )
{
    return a.x * b.x + a.y * b.y;
}

inline float length( float2 a )
{
    return sqrtf( lengthsq( a ) );
}

inline float lengthsq( float2 a )
{
    return dot( a, a );
}

inline float2 normalize( float2 a )
{
    return a / length( a );
}

inline float2 sincos2( float angle )
{
    return float2( cosf( angle ), sinf( angle ) );
}

inline float2 perpccw2( float2 a )
{
    return float2( -a.y, a.x );
}

inline float2 perpcw2( float2 a )
{
    return float2( a.y, -a.x );
}



inline float3::float3()
    :   x( 0.0f )
    ,   y( 0.0f )
    ,   z( 0.0f )
{
}

inline float3::float3( float x, float y, float z )
    :   x( x )
    ,   y( y )
    ,   z( z )
{
}

inline float3::float3( float2 xy, float z )
    :   x( xy.x )
    ,   y( xy.y )
    ,   z( z )
{
}

inline const float2& float3::xy() const
{
    return *(const float2*)this;
}

inline float2& float3::xy()
{
    return *(float2*)this;
}

inline float& float3::operator [] ( size_t element )
{
    return ( &x )[ element ];
}

inline const float& float3::operator [] ( size_t element ) const
{
    return ( &x )[ element ];
}

inline float3 operator - ( float3 a )
{
    return float3( -a.x, -a.y, -a.z );
}

inline float3 operator + ( float3 a )
{
    return a;
}

inline float3 operator + ( float3 a, float3 b )
{
    return float3( a.x + b.x, a.y + b.y, a.z + b.z );
}

inline float3 operator - ( float3 a, float3 b )
{
    return float3( a.x - b.x, a.y - b.y, a.z - b.z );
}

inline float3 operator * ( float3 a, float3 b )
{
    return float3( a.x * b.x, a.y * b.y, a.z * b.z );
}

inline float3 operator * ( float3 a, float b )
{
    return float3( a.x * b, a.y * b, a.z * b );
}

inline float3 operator * ( float a, float3 b )
{
    return float3( a * b.x, a * b.y, a * b.z );
}

inline float3 operator / ( float3 a, float3 b )
{
    return float3( a.x / b.x, a.y / b.y, a.z / b.z );
}

inline float3 operator / ( float3 a, float b )
{
    return float3( a.x / b, a.y / b, a.z / b );
}

inline float3 operator / ( float a, float3 b )
{
    return float3( a / b.x, a / b.y, a / b.z );
}

inline float3& operator += ( float3& a, float3 b )
{
    a.x += b.x; a.y += b.y; a.z += b.z;
    return a;
}

inline float3& operator -= ( float3& a, float3 b )
{
    a.x -= b.x; a.y -= b.y; a.z -= b.z;
    return a;
}

inline float3& operator *= ( float3& a, float3 b )
{
    a.x *= b.x; a.y *= b.y; a.z *= b.z;
    return a;
}

inline float3& operator *= ( float3& a, float b )
{
    a.x *= b; a.y *= b; a.z *= b;
    return a;
}

inline float3& operator /= ( float3& a, float3 b )
{
    a.x /= b.x; a.y /= b.y; a.z /= b.z;
    return a;
}

inline float3& operator /= ( float3& a, float b )
{
    a.x /= b; a.y /= b; a.z /= b;
    return a;
}

inline float dot( float3 a, float3 b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float length( float3 a )
{
    return sqrtf( lengthsq( a ) );
}

inline float lengthsq( float3 a )
{
    return dot( a, a );
}

inline float3 normalize( float3 a )
{
    return a / length( a );
}





inline float4::float4()
    :   x( 0.0f )
    ,   y( 0.0f )
    ,   z( 0.0f )
    ,   w( 0.0f )
{
}

inline float4::float4( float x, float y, float z, float w )
    :   x( x )
    ,   y( y )
    ,   z( z )
    ,   w( w )
{
}

inline float4::float4( float2 xy, float z, float w )
    :   x( xy.x )
    ,   y( xy.y )
    ,   z( z )
    ,   w( w )
{
}

inline float4::float4( float3 xyz, float w )
    :   x( xyz.x )
    ,   y( xyz.y )
    ,   z( xyz.z )
    ,   w( w )
{
}

inline const float2& float4::xy() const
{
    return *(const float2*)this;
}

inline float2& float4::xy()
{
    return *(float2*)this;
}

inline const float3& float4::xyz() const
{
    return *(const float3*)this;
}

inline float3& float4::xyz()
{
    return *(float3*)this;
}

inline float& float4::operator [] ( size_t element )
{
    return ( &x )[ element ];
}

inline const float& float4::operator [] ( size_t element ) const
{
    return ( &x )[ element ];
}

inline float4 operator - ( float4 a )
{
    return float4( -a.x, -a.y, -a.z, -a.w );
}

inline float4 operator + ( float4 a )
{
    return a;
}

inline float4 operator + ( float4 a, float4 b )
{
    return float4( a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w );
}

inline float4 operator - ( float4 a, float4 b )
{
    return float4( a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w );
}

inline float4 operator * ( float4 a, float4 b )
{
    return float4( a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w );
}

inline float4 operator * ( float4 a, float b )
{
    return float4( a.x * b, a.y * b, a.z * b, a.w * b );
}

inline float4 operator * ( float a, float4 b )
{
    return float4( a * b.x, a * b.y, a * b.z, a * b.w );
}

inline float4 operator / ( float4 a, float4 b )
{
    return float4( a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w );
}

inline float4 operator / ( float4 a, float b )
{
    return float4( a.x / b, a.y / b, a.z / b, a.w / b );
}

inline float4 operator / ( float a, float4 b )
{
    return float4( a / b.x, a / b.y, a / b.z, a / b.w );
}

inline float4& operator += ( float4& a, float4 b )
{
    a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
    return a;
}

inline float4& operator -= ( float4& a, float4 b )
{
    a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
    return a;
}

inline float4& operator *= ( float4& a, float4 b )
{
    a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;
    return a;
}

inline float4& operator *= ( float4& a, float b )
{
    a.x *= b; a.y *= b; a.z *= b; a.w *= b;
    return a;
}

inline float4& operator /= ( float4& a, float4 b )
{
    a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w;
    return a;
}

inline float4& operator /= ( float4& a, float b )
{
    a.x /= b; a.y /= b; a.z /= b; a.w /= b;
    return a;
}

inline float dot( float4 a, float4 b )
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float length( float4 a )
{
    return sqrtf( lengthsq( a ) );
}

inline float lengthsq( float4 a )
{
    return dot( a, a );
}

inline float4 normalize( float4 a )
{
    return a / length( a );
}




inline matrix2::matrix2()
{
    columns[ 0 ] = float2( 1.0f, 0.0f );
    columns[ 1 ] = float2( 0.0f, 1.0f );
}

inline matrix2::matrix2( float m00, float m10, float m01, float m11 )
{
    columns[ 0 ] = float2( m00, m01 );
    columns[ 1 ] = float2( m10, m11 );
}

inline float2& matrix2::operator [] ( size_t column )
{
    return columns[ column ];
}

inline const float2& matrix2::operator [] ( size_t column ) const
{
    return columns[ column ];
}

inline matrix2 operator * ( const matrix2& a, const matrix2& b )
{
    float2 row0 = float2( a[ 0 ][ 0 ], a[ 1 ][ 0 ] );
    float2 row1 = float2( a[ 0 ][ 1 ], a[ 1 ][ 1 ] );
    return matrix2(
        dot( row0, b[ 0 ] ), dot( row0, b[ 1 ] ),
        dot( row1, b[ 0 ] ), dot( row1, b[ 1 ] ) );
}

inline float2 operator * ( float2 a, const matrix2& b )
{
    return float2( dot( a, b[ 0 ] ), dot( a, b[ 1 ] ) );
}

inline float2 operator * ( const matrix2& a, float2 b )
{
    float2 row0 = float2( a[ 0 ][ 0 ], a[ 1 ][ 0 ] );
    float2 row1 = float2( a[ 0 ][ 1 ], a[ 1 ][ 1 ] );
    return float2( dot( row0, b ), dot( row1, b ) );
}

inline matrix2& operator *= ( matrix2& a, const matrix2& b )
{
    a = a * b;
    return a;
}

inline float2& operator *= ( float2& a, const matrix2& b )
{
    a = a * b;
    return a;
}

inline matrix2 inverse( const matrix2& m )
{
    float a = m[ 0 ][ 0 ];
    float b = m[ 1 ][ 0 ];
    float c = m[ 0 ][ 1 ];
    float d = m[ 1 ][ 1 ];
    float det = 1.0f / ( a * d - b * c );
    return matrix2
    (
         d * det, -b * det,
        -c * det,  a * det
    );
}





inline matrix3::matrix3()
{
    columns[ 0 ] = float3( 1.0f, 0.0f, 0.0f );
    columns[ 1 ] = float3( 0.0f, 1.0f, 0.0f );
    columns[ 2 ] = float3( 0.0f, 0.0f, 1.0f );
}

inline matrix3::matrix3(
            float m00, float m10, float m20,
                float m01, float m11, float m21,
                    float m02, float m12, float m22 )
{
    columns[ 0 ] = float3( m00, m01, m02 );
    columns[ 1 ] = float3( m10, m11, m12 );
    columns[ 2 ] = float3( m20, m21, m22 );
}

inline float3& matrix3::operator [] ( size_t column )
{
    return columns[ column ];
}

inline const float3& matrix3::operator [] ( size_t column ) const
{
    return columns[ column ];
}

inline matrix3 operator * ( const matrix3& a, const matrix3& b )
{
    float3 row0 = float3( a[ 0 ][ 0 ], a[ 1 ][ 0 ], a[ 2 ][ 0 ] );
    float3 row1 = float3( a[ 0 ][ 1 ], a[ 1 ][ 1 ], a[ 2 ][ 1 ] );
    float3 row2 = float3( a[ 0 ][ 2 ], a[ 1 ][ 2 ], a[ 2 ][ 2 ] );
    return matrix3(
        dot( row0, b[ 0 ] ), dot( row0, b[ 1 ] ), dot( row0, b[ 2 ] ),
        dot( row1, b[ 0 ] ), dot( row1, b[ 1 ] ), dot( row1, b[ 2 ] ),
        dot( row2, b[ 0 ] ), dot( row2, b[ 1 ] ), dot( row2, b[ 2 ] ) );
}

inline float3 operator * ( float3 a, const matrix3& b )
{
    return float3( dot( a, b[ 0 ] ), dot( a, b[ 1 ] ), dot( a, b[ 2 ] ) );
}

inline float3 operator * ( const matrix3& a, float3 b )
{
    float3 row0 = float3( a[ 0 ][ 0 ], a[ 1 ][ 0 ], a[ 2 ][ 0 ] );
    float3 row1 = float3( a[ 0 ][ 1 ], a[ 1 ][ 1 ], a[ 2 ][ 1 ] );
    float3 row2 = float3( a[ 0 ][ 2 ], a[ 1 ][ 2 ], a[ 2 ][ 2 ] );
    return float3( dot( row0, b ), dot( row1, b ), dot( row2, b ) );
}

inline matrix3& operator *= ( matrix3& a, const matrix3& b )
{
    a = a * b;
    return a;
}

inline float3& operator *= ( float3& a, const matrix3& b )
{
    a = a * b;
    return a;
}

inline matrix3 inverse( const matrix3& m )
{
    if ( m[ 2 ][ 0 ] == 0.0f && m[ 2 ][ 1 ] == 0.0f && m[ 2 ][ 2 ] == 1.0f )
    {
        // This is a 2D transformation matrix.  It can be decomposed into
        // a rotation part R and a translation part T.  M = R * T.  Therefore
        // the inverse M' = T' * R', since M * M' = R * T * T' * R' = I.
        
        matrix2 r = matrix2
        (
            m[ 0 ][ 0 ], m[ 1 ][ 0 ],
            m[ 0 ][ 1 ], m[ 1 ][ 1 ]
        );
        matrix2 r_inverse = inverse( r );
        float2 t_inverse = -float2( m[ 0 ][ 2 ], m[ 1 ][ 2 ] );
        
        
        // And instead of doing a full 3x3 multiply to combine T' * R', we
        // can exploit the fact that a lot of the elements are 0.
        
        float a = r_inverse[ 0 ][ 0 ];
        float b = r_inverse[ 1 ][ 0 ];
        float c = r_inverse[ 0 ][ 1 ];
        float d = r_inverse[ 1 ][ 1 ];
        float p = a * t_inverse.x + c * t_inverse.y;
        float q = b * t_inverse.x + d * t_inverse.y;
        
        return matrix3
        (
            a, b, 0.0f,
            c, d, 0.0f,
            p, q, 1.0f
        );
    }
    else
    {
        assert( ! "general matrix3 inverse not implemented" );
        return matrix3();
    }
}



inline matrix4::matrix4()
{
    columns[ 0 ] = float4( 1.0f, 0.0f, 0.0f, 0.0f );
    columns[ 1 ] = float4( 0.0f, 1.0f, 0.0f, 0.0f );
    columns[ 2 ] = float4( 0.0f, 0.0f, 1.0f, 0.0f );
    columns[ 3 ] = float4( 0.0f, 0.0f, 0.0f, 1.0f );
}

inline matrix4::matrix4(
            float m00, float m10, float m20, float m30,
                float m01, float m11, float m21, float m31,
                    float m02, float m12, float m22, float m32,
                        float m03, float m13, float m23, float m33 )
{
    columns[ 0 ] = float4( m00, m01, m02, m03 );
    columns[ 1 ] = float4( m10, m11, m12, m13 );
    columns[ 2 ] = float4( m20, m21, m22, m23 );
    columns[ 3 ] = float4( m30, m31, m32, m33 );
}

inline float4& matrix4::operator [] ( size_t column )
{
    return columns[ column ];
}

inline const float4& matrix4::operator [] ( size_t column ) const
{
    return columns[ column ];
}

inline matrix4 operator * ( const matrix4& a, const matrix4& b )
{
    float4 row0 = float4( a[ 0 ][ 0 ], a[ 1 ][ 0 ], a[ 2 ][ 0 ], a[ 3 ][ 0 ] );
    float4 row1 = float4( a[ 0 ][ 1 ], a[ 1 ][ 1 ], a[ 2 ][ 1 ], a[ 3 ][ 1 ] );
    float4 row2 = float4( a[ 0 ][ 2 ], a[ 1 ][ 2 ], a[ 2 ][ 2 ], a[ 3 ][ 2 ] );
    float4 row3 = float4( a[ 0 ][ 3 ], a[ 1 ][ 3 ], a[ 2 ][ 3 ], a[ 3 ][ 3 ] );
    return matrix4(
        dot( row0, b[ 0 ] ), dot( row0, b[ 1 ] ), dot( row0, b[ 2 ] ), dot( row0, b[ 3 ] ),
        dot( row1, b[ 0 ] ), dot( row1, b[ 1 ] ), dot( row1, b[ 2 ] ), dot( row1, b[ 3 ] ),
        dot( row2, b[ 0 ] ), dot( row2, b[ 1 ] ), dot( row2, b[ 2 ] ), dot( row2, b[ 3 ] ),
        dot( row3, b[ 0 ] ), dot( row3, b[ 1 ] ), dot( row3, b[ 2 ] ), dot( row3, b[ 3 ] ) );
}

inline float4 operator * ( float4 a, const matrix4& b )
{
    return float4( dot( a, b[ 0 ] ), dot( a, b[ 1 ] ), dot( a, b[ 2 ] ), dot ( a, b[ 3 ] ) );
}

inline float4 operator * ( const matrix4& a, float4 b )
{
    float4 row0 = float4( a[ 0 ][ 0 ], a[ 1 ][ 0 ], a[ 2 ][ 0 ], a[ 3 ][ 0 ] );
    float4 row1 = float4( a[ 0 ][ 1 ], a[ 1 ][ 1 ], a[ 2 ][ 1 ], a[ 3 ][ 1 ] );
    float4 row2 = float4( a[ 0 ][ 2 ], a[ 1 ][ 2 ], a[ 2 ][ 2 ], a[ 3 ][ 2 ] );
    float4 row3 = float4( a[ 0 ][ 3 ], a[ 1 ][ 3 ], a[ 2 ][ 3 ], a[ 3 ][ 3 ] );
    return float4( dot( row0, b ), dot( row1, b ), dot( row2, b ), dot( row3, b ) );
}

inline matrix4& operator *= ( matrix4& a, const matrix4& b )
{
    a = a * b;
    return a;
}

inline float4& operator *= ( float4& a, const matrix4& b )
{
    a = a * b;
    return a;
}




#endif /* MATH3_H */




