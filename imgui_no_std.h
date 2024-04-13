#pragma once

#ifndef IMGUI_DISABLE
#include <intrin.h>

#ifdef IMGUI_NO_STD
#ifndef IM_ASSERT
#define IM_ASSERT( _exp ) do { if ( ( _exp ) == false ) { __debugbreak(); } } while( 0 )
#endif // !IM_ASSERT
namespace ImGui
{
    void * memset( void * _pointer, int _value, size_t _count );
    void * memcpy( void * _destination, const void * _source, size_t _size );
    size_t strlen( const char * _str );
    void * memmove( void * _destination, const void * _source, size_t _size );
    void qsort( void * _array, size_t _elementCount, size_t _elementSize, int ( *_compareFunction )( const void*, const void* ) );
    char * strchr( const char * _string, int _searchedChar );
    int sscanf( const char * _buffer, const char * format, ... );
    void* memchr( const void* _memoryBlock, int _searchedChar, size_t _size );
    int memcmp( const void* _pointer1, const void* _pointer2, size_t _size );
    int strcmp( const char * _first, const char * _second );
    
    char * strcpy( char * _destination, const char * _source );
    int strncmp( const char * _first, const char * _second, size_t _length );
    const char * strstr( const char * fullString, const char * substring );
}

inline float ImFabs( float _x ) {
    __asm
    {
        fld     dword ptr[ _x ]
        fabs
        fstp    dword ptr[ _x ]
    }
    return _x;
}

inline float ImSqrt( float _x ) {
    __asm
    {
        fld     dword ptr[ _x ]
        fsqrt
        fstp    dword ptr[ _x ]
    }

    return _x;
}

inline float ImFmod( float _a, float _b ) {
    float x = 0.0f;

    __asm
    {
        fld     dword ptr[ _b ]
        fld     dword ptr[ _a ]
        fprem
        fstp    st( 1 )
        fstp    dword ptr[ x ]
    }

    return x;
}

inline float ImCos( float _x ) {
    __asm
    {
        fld     dword ptr[ _x ]
        fcos
        fstp    dword ptr[ _x ]
    }

    return _x;
}

inline float ImSin( float _x ) {
    __asm
    {
        fld     dword ptr[ _x ]
        fsin
        fstp    dword ptr[ _x ]
    }

    return _x;
}

inline float ImAcos( float _x ) {
    __asm
    {
        fld     dword ptr[ _x ]
        fld     st( 0 )
        fld     st( 0 )
        fmul
        fld1
        fsubr
        fsqrt
        fxch
        fpatan
        fstp    dword ptr[ _x ]
    }
    return _x;
}

inline float ImAtan2( float _y, float _x ) {
    __asm
    {
        fld     dword ptr[ _y ]
        fld     dword ptr[ _x ]
        fpatan
        fstp    dword ptr[ _x ]
    }

    return _x;
}

float ImAtof( const char * _str );

inline float  ImCeil( float _x ) {
    int holder, setter, res;

    __asm
    {
        fld     dword ptr[ _x ]
        fnstcw  dword ptr[ holder ]
        movzx   eax, [ holder ]
        and eax, 0xfffff3ff
        or eax, 0x00000800
        mov     dword ptr[ setter ], eax
        fldcw   dword ptr[ setter ]
        fistp   dword ptr[ res ]
        fldcw   dword ptr[ holder ]
    }

    return res;
}

inline double ImPow( double _a, double _b ) {
    __asm
    {
        fld     qword ptr[ _b ]
        fld     qword ptr[ _a ]

        ftst
        fstsw   ax
        sahf
        jz      zero

        fyl2x
        fist    dword ptr[ _a ]
        sub     esp, 12
        mov     dword ptr[ esp ], 0
        mov     dword ptr[ esp + 4 ], 0x80000000
        fisub   dword ptr[ _a ]
        mov     eax, dword ptr[ _a ]
        add     eax, 0x3fff
        mov[ esp + 8 ], eax
        jle     underflow
        cmp     eax, 0x8000
        jge     overflow
        f2xm1
        fld1
        fadd
        fld     tbyte ptr[ esp ]
        add     esp, 12
        fmul
        jmp     end

        underflow :
        fstp    st
            fldz
            add     esp, 12
            jmp     end

            overflow :
        push    0x7f800000
            fstp    st
            fld     dword ptr[ esp ]
            add     esp, 16
            jmp     end

            zero :
        fstp    st( 1 )

            end :
    }
}

inline float  ImPow( float _a, float _b ) {
    return (float)ImPow( (double) _a, (double) _b );
}

inline float  ImLog( float _x ) {
    __asm
    {
        fld1
        fld     dword ptr[ _x ]
        fyl2x
        fldl2e
        fdiv
        fstp    dword ptr[ _x ]
    }

    return _x;
}

inline double ImLog( double _x ) {
    return (double) ImLog( (float) _x );
}

inline int    ImAbs( int _x ) { return _x < 0 ? -_x : _x; }
inline float  ImAbs( float _x ) {
    __asm
    {
        fld     dword ptr[ _x ]
        fabs
        fstp    dword ptr[ _x ]
    }

    return _x;
}

inline double ImAbs( double _x ) {
    return (double) ImAbs( (float ) _x );
}
inline float  ImSign( float _x ) { return ( _x < 0.0f ) ? -1.0f : ( _x > 0.0f ) ? 1.0f : 0.0f; } // Sign operator - returns -1, 0 or 1 based on sign of argument
inline double ImSign( double _x ) { return ( _x < 0.0 ) ? -1.0 : ( _x > 0.0 ) ? 1.0 : 0.0; }
inline float  ImRsqrt( float _x ) { return _mm_cvtss_f32( _mm_rsqrt_ss( _mm_set_ss( _x ) ) ); }
inline double ImRsqrt( double _x ) {
    return ( double ) ImRsqrt( ( float ) _x  );
}

#define IMGUI_STD_FUNC_NAMESPACE ImGui
#else
#define IMGUI_STD_FUNC_NAMESPACE
#endif // IMGUI_NO_STD

#endif // !IMGUI_DISABLE
