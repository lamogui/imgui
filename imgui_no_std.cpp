
#include "imgui_no_std.h"

#ifndef IMGUI_DISABLE
#ifdef IMGUI_NO_STD

// https://stackoverflow.com/questions/1583196/building-visual-c-app-that-doesnt-use-crt-functions-still-references-some
#ifdef __cplusplus
extern "C" {
#endif
    int _fltused = 0; // it should be a single underscore since the double one is the mangled name
#ifdef __cplusplus
}
#endif

static bool eIsDigit( char c ) {
    return ( c >= '0' && c <= '9' );
}

static bool eIsSpace( char _c ) {
    switch ( _c ) {
        case '\t':
        case '\n':
        case '\v':
        case '\f':
        case '\r':
        case ' ':
            return true;
        default:
            return false;
    }

}

static int eStrToInt( const char * _str, const char ** _end ) {
    IM_ASSERT( ImGui::strlen( _str ) > 0 ); // "empty string lol"

    const bool neg = ( _str[ 0 ] == '-' );
    if ( neg ) {
        _str++;
    }
    char c;
    int val = 0;

    while ( ( c = *_str++ ) != '\0' && eIsDigit( c ) )
        val = val * 10 + ( c - '0' );

    if ( _end != nullptr ) {
        *_end = _str;
    }

    return ( neg ? -val : val );
}

static unsigned long long eStrToHex(const char * _str, const char ** _end ) {
    IM_ASSERT( ImGui::strlen( _str ) > 0 ); // empty string mega lol

	char c;
	unsigned long long val = 0;

	while ((c = *_str++) != '\0' && eIsDigit(c) || ((c & 0xDF) >= 'A' && (c & 0xDF) <= 'F')) {
		if (eIsDigit(c)) {
			val = val * 16 + (c - '0');
		}
		else {
			val = val * 16 + ((c & 0xDF) - 'A');
		}
	}
	if (_end != nullptr) {
		*_end = _str;
	}

	return val;

}

float ImAtof( const char * _str ) {
    float val = 0.0f;
    int sign = 1;

    if ( *_str == '+' )
        _str++;
    else if ( *_str == '-' ) {
        sign = -1;
        _str++;
    }

    while ( eIsDigit( *_str ) )
        val = val * 10 + ( *_str++ ) - '0';

    if ( *_str == '.' ) {
        _str++;
        float dec = 1.0f;

        while ( eIsDigit( *_str ) ) {
            dec = dec / 10.0f;
            val = val + ( ( *_str++ ) - '0' ) * dec;
        }
    }
    return val * sign;
}

void * ImGui::memset( void * _pointer, int _value, size_t _count )
{
    __asm
    {
        mov     eax, dword ptr[ _value ]
        mov     ecx, dword ptr[ _count ]
        mov     edi, dword ptr[ _pointer ]
        rep     stosb
    }
}

void * ImGui::memcpy( void * _destination, const void * _source, size_t _size )
{
    __asm
    {
        mov     edi, dword ptr[ _destination ]
        mov     esi, dword ptr[ _source ]
        mov     ecx, dword ptr[ _size ]

        // calculate iteration count
        mov     eax, ecx
        shr     ecx, 6
        mov     edx, ecx
        shl     edx, 6
        sub     eax, edx
        cmp     ecx, 0
        je      done

        copyloop :
        movq    mm0, [ esi ]
            movq    mm1, [ esi + 8 ]
            movq    mm2, [ esi + 16 ]
            movq    mm3, [ esi + 24 ]
            movq    mm4, [ esi + 32 ]
            movq    mm5, [ esi + 40 ]
            movq    mm6, [ esi + 48 ]
            movq    mm7, [ esi + 56 ]

            movq[ edi ], mm0
            movq[ edi + 8 ], mm1
            movq[ edi + 16 ], mm2
            movq[ edi + 24 ], mm3
            movq[ edi + 32 ], mm4
            movq[ edi + 40 ], mm5
            movq[ edi + 48 ], mm6
            movq[ edi + 56 ], mm7

            add     esi, 64
            add     edi, 64
            dec     ecx
            jnz     copyloop

            // copy missing bytes
            done : mov     ecx, eax
            rep     movsb
            emms
    }
}


size_t ImGui::strlen( const char * _str )
{
    const char * eos = _str;
    while ( *eos++ );
    return ( size_t ) ( eos - _str - 1 );
}

void * ImGui::memmove( void * _destination, const void * _source, size_t _size )
{
    const unsigned char * psrc = ( const unsigned char * ) _source;
    unsigned char * pdst = ( unsigned char * ) _destination;

    if ( _destination <= _source || pdst >= psrc + _size ) {
        // non-overlapping buffers, so copy from
        // lower addresses to higher addresses
        while ( _size-- )
            *pdst++ = *psrc++;
    }
    else {
        // overlapping buffers, so copy from
        // higher addresses to lower addresses
        pdst = pdst + _size - 1;
        psrc = psrc + _size - 1;

        while ( _size-- )
            *pdst-- = *psrc--;
    }
    return _destination;
}

void ImGui::qsort( void * _array, size_t _elementCount, size_t _elementSize, int ( *_compareFunction )( const void*, const void* ) )
{

}

char * ImGui::strchr( const char * _string, int _searchedChar )
{
    while ( _string[ 0 ] != _searchedChar && _string[ 0 ] != '\0' ) {
        _string++;
    }
    if ( _string[ 0 ] == '\0' ) {
        return nullptr;
    }
    else {
        return (char *) _string;
    }
}


#include "limits.h" // For INT_MAX
#include <stdarg.h> // Execption for the STD, due to all different calling convention I prefer let visual studio compute the correct address for va_start & va_arg 
static int vsscanf( const char* _str, const char* _format, va_list _args ) {
    const char* f, * s;
    const char point = '.';
    int cnv = 0;

    for ( s = _str, f = _format; *f; ++f ) {
        if ( *f == '%' ) {
            int size = 0;
            int width = 0;
            int do_cnv = 1;

            if ( *++f == '*' )
                ++f, do_cnv = 0;

            for ( ; eIsDigit( *f ); ++f )
                width *= 10, width += *f - '0';

            if ( *f == 'h' || *f == 'l' || *f == 'L' )
                size = *f++;

            if ( *f != '[' && *f != 'c' && *f != 'n' )
                while ( eIsSpace( *s ) )
                    ++s;

#define COPY                         *b++ = *s++, --width
#define MATCH(cond)                  if (width && (cond)) COPY;
#define MATCH_ACTION(cond, action)   if (width && (cond)) { COPY; action; }
#define MATCHES_ACTION(cond, action) while (width && (cond)) { COPY; action; }
            //#define FAIL                         
            switch ( *f ) {
                case 'o':
                    IM_ASSERT( false ); // "TODO: impl octal in eStrToInt"
                    break;
                case 'x': case 'X':
                case 'p':
                {
                    char buf[ 513 ];
                    char* b = buf;
                    int digit = 0;
                    MATCHES_ACTION( ImGui::memchr( "0123456789abcdefABCDEF", *s, 22 ), digit = 1 )
                        unsigned long long data = eStrToHex( buf, nullptr );
                    if ( !digit ) return ( cnv ) ? cnv : -1; //FAIL;
                    *b = '\0';
                    if ( do_cnv ) {
                        if ( size == 'h' ) {
                            //passert( false, "check if this code works" );
                            *va_arg( _args, short* ) = static_cast< short >( data );
                        }
                        else if ( size == 'l' ) {
                            //passert( false, "check if this code works" );
                            *va_arg( _args, long* ) = static_cast< long >( data );
                        }
                        else {
                            *va_arg( _args, int* ) = static_cast< int >( data );
                        }
                    }
                    break;
                }
                case 'd': case 'i': case 'u':
                {
                    //static const char types[] = "diouxXp";
                    //static const int bases[] = { 10, 0, 8, 10, 16, 16, 16 };
                    static const char digitset[] = "0123456789";//abcdefABCDEF";
                    //static const int setsizes[] = { 10, 0, 0, 0, 0, 0, 0, 0, 8, 0, 10, 0, 0, 0, 0, 0, 22 };
                    int base = 10; //bases[ eStrChr( types, *f ) - types ];
                    int setsize;
                    char buf[ 513 ];
                    char* b = buf;
                    int digit = 0;
                    if ( width <= 0 || width > 512 ) width = 512;
                    MATCH( *s && ( *s == '+' || *s == '-' ) )
                        MATCH_ACTION( *s && *s == '0',
                                      digit = 1;
                    MATCH_ACTION( *s && ( *s == 'x' || *s == 'X' ) && ( base == 0 || base == 16 ), IM_ASSERT( false ) /*, "TODO: impl hexadecimal, octal & binary in eStrToInt") /*base = 16*/) /*else passert( false, "TODO: impl hexadecimal, octal & binary in eStrToInt" )base = 8*/;
                    )
                        setsize = 10; //setsizes[ base ];
                    MATCHES_ACTION( ImGui::memchr( digitset, *s, setsize ), digit = 1 )
                        if ( !digit ) return ( cnv ) ? cnv : -1; //FAIL;
                    *b = '\0';
                    if ( do_cnv ) {
                        if ( *f == 'd' || *f == 'i' ) {
                            int data = eStrToInt( buf, nullptr );
                            if ( size == 'h' )
                                *va_arg( _args, short* ) = ( short ) data;
                            else if ( size == 'l' )
                                *va_arg( _args, long* ) = data;
                            else
                                *va_arg( _args, int* ) = ( int ) data;
                        }
                        else {
                            int data = eStrToInt( buf, nullptr );
                            if ( size == 'p' )
                                *va_arg( _args, void** ) = ( void* ) data;
                            else if ( size == 'h' )
                                *va_arg( _args, unsigned short* ) = ( unsigned short ) data;
                            else if ( size == 'l' )
                                *va_arg( _args, unsigned long* ) = data;
                            else
                                *va_arg( _args, unsigned int* ) = ( unsigned int ) data;
                        }
                        ++cnv;
                    }
                    break;
                }

                case 'e': case 'E': case 'f': case 'g': case 'G':
                {
                    char buf[ 513 ];
                    char* b = buf;
                    int digit = 0;
                    if ( width <= 0 || width > 512 ) width = 512;
                    MATCH( *s == '+' || *s == '-' )
                        MATCHES_ACTION( eIsDigit( *s ), digit = 1 )
                        MATCH( *s == point )
                        MATCHES_ACTION( eIsDigit( *s ), digit = 1 )
                        MATCHES_ACTION( digit && ( *s == 'e' || *s == 'E' ),
                                        MATCH( *s == '+' || *s == '-' )
                                        digit = 0;
                    MATCHES_ACTION( eIsDigit( *s ), digit = 1 )
                        )
                        if ( !digit ) return ( cnv ) ? cnv : -1; //FAIL;
                    *b = '\0';
                    if ( do_cnv ) {
                        float data = ImAtof( buf );
                        if ( size == 'l' )
                            *va_arg( _args, double* ) = data;
                        else if ( size == 'L' )
                            *va_arg( _args, long double* ) = ( long double ) data;
                        else
                            *va_arg( _args, float* ) = ( float ) data;
                        ++cnv;
                    }
                    break;
                }

                case 's':
                {
                    char* arg = va_arg( _args, char* );
                    if ( width <= 0 ) width = INT_MAX;
                    while ( width-- && *s && !eIsSpace( *s ) )
                        if ( do_cnv ) *arg++ = *s++;
                    if ( do_cnv ) *arg = '\0', ++cnv;
                    break;
                }

                case '[':
                {
                    IM_ASSERT( false ); // "TODO: implement '[' in sscanf";
                    //char *arg = va_arg( args, char * );
                    //int setcomp = 0;
                    //size_t setsize;
                    //const char *end;
                    //if ( width <= 0 ) width = INT_MAX;
                    //if ( *++f == '^' ) setcomp = 1, ++f;
                    //end = nullptr; //eStrChr( (*f == ']') ? f + 1 : f, ']' );
                    //if ( !end ) return (cnv) ? cnv : -1; // FAIL; /* Could be cnv to match glibc-2.2 */
                    //setsize = end - f;     /* But FAIL matches the C standard */
                    //while ( width-- && *s )
                    //{
                    //	if ( !setcomp && !eMemChr( f, *s, setsize ) ) break;
                    //	if ( setcomp && eMemChr( f, *s, setsize ) ) break;
                    //	if ( do_cnv ) *arg++ = *s++;
                    //}
                    //if ( do_cnv ) *arg = '\0', ++cnv;
                    //f = end;
                    break;
                }

                case 'c':
                {
                    char* arg = va_arg( _args, char* );
                    if ( width <= 0 ) width = 1;
                    while ( width-- ) {
                        if ( !*s ) return ( cnv ) ? cnv : -1; //FAIL;
                        if ( do_cnv ) *arg++ = *s++;
                    }
                    if ( do_cnv ) ++cnv;
                    break;
                }

                case 'n':
                {
                    if ( size == 'h' )
                        *va_arg( _args, short* ) = ( short ) ( s - _str );
                    else if ( size == 'l' )
                        *va_arg( _args, long* ) = ( long ) ( s - _str );
                    else
                        *va_arg( _args, int* ) = ( int ) ( s - _str );
                    break;
                }

                case '%':
                {
                    if ( *s++ != '%' ) return cnv;
                    break;
                }

                default:
                    return ( cnv ) ? cnv : -1; //FAIL;
            }
        }
        else if ( eIsSpace( *f ) ) {
            while ( eIsSpace( f[ 1 ] ) )
                ++f;
            while ( eIsSpace( *s ) )
                ++s;
        }
        else {
            if ( *s++ != *f )
                return cnv;
        }
    }

    return cnv;
}

int ImGui::sscanf( const char * _str, const char * _format, ... ) {
    va_list args;
    va_start( args, _format );
    int n = vsscanf( _str, _format, args );
    va_end( args );
    return n;
}


void * ImGui::memchr( const void * _memoryBlock, int _searchedChar, size_t _size )
{
	const unsigned char * src = (const unsigned char *) _memoryBlock;

	while ( _size-- > 0 )
	{
		if ( *src == _searchedChar )
			return (void *)src;
		src++;
	}
	return nullptr;
}

int ImGui::strcmp( const char * str0, const char * str1 )
{
    int res = 0;

    while ( !( res = *( const unsigned char * ) str0 - *( const unsigned char * ) str0 ) && *str1 )
    {
        str0++;
        str1++;
    }

    if ( res < 0 )
        res = -1;
    else if ( res > 0 )
        res = 1;

    return res;
}

int ImGui::memcmp( const void * _pointer1, const void * _pointer2, size_t _size )
{
	register const unsigned char * s1 = (const unsigned char *) _pointer1;
	register const unsigned char * s2 = (const unsigned char *) _pointer2;

	while ( _size-- > 0 )
	{
		if ( *s1++ != *s2++ )
			return s1[ -1 ] < s2[ -1 ] ? -1 : 1;
	}
	return 0;
}

char * ImGui::strcpy( char * _destination, const char * _source )
{
    while ( *_destination++ = *_source++ );
    return _destination;
}

int ImGui::strncmp( const char * _first, const char * _second, size_t _length )
{
    register unsigned char u1, u2;

    while ( _length-- > 0 ) {
        u1 = ( unsigned char ) *_first++;
        u2 = ( unsigned char ) *_second++;
        if ( u1 != u2 )
            return u1 - u2;
        if ( u1 == '\0' )
            return 0;
    }
    return 0;
}

#endif // IMGUI_NO_STD
#endif // !IMGUI_DISABLE
