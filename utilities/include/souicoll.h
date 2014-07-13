// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __SOUICOLL_H__
#define __SOUICOLL_H__

#pragma once

#include <limits.h>
#include <malloc.h>

#pragma warning(push)
#pragma warning(disable: 4702)  // Unreachable code.  This file will have lots of it, especially without EH enabled.
#pragma warning(disable: 4512)  // assignment operator could not be generated
#pragma warning(disable: 4290)  // C++ Exception Specification ignored
#pragma warning(disable: 4127)  // conditional expression constant
#pragma warning(disable: 4571)  //catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions

// abstract iteration position

#ifndef _AFX_PACKING
#define _AFX_PACKING 4
#endif

#ifndef ASSERT
#define ASSERT(expr) __noop;
#endif // ATLASSERT

#define SThrow(expr) __noop;
#define SASSERT_VALID(x) __noop;

#ifndef SASSUME
#define SASSUME(expr) do { ASSERT(expr); __analysis_assume(!!(expr)); } while(0)
#endif // ATLASSUME

#ifndef SENSURE
#define SENSURE(expr) __noop;
#endif // ATLENSURE


#if !defined(_W64)
#if !defined(__midl) &&    (defined(_X86_)    || defined(_M_IX86))
#define    _W64 __w64
#else
#define    _W64
#endif
#endif


#ifndef _STRY
#define _STRY try
#endif
#define _SCATCHALL() __pragma(warning(push)) __pragma(warning(disable: 4571)) catch( ... ) __pragma(warning(pop))

//copy from <new.h>
#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void *__CRTDECL operator new(size_t, void *_Where)
{return (_Where); }
#if     _MSC_VER >= 1200
inline void __CRTDECL operator delete(void *, void *)
{return; }
#endif
#endif

#pragma pack(push,_ATL_PACKING)
namespace SOUI
{

struct __POSITION
{
};
typedef __POSITION* POSITION;

template<typename T>
class SLimits;

template<>
class SLimits<int _W64>
{
public:
    static const int _Min=INT_MIN;
    static const int _Max=INT_MAX;
};

template<>
class SLimits<unsigned int _W64>
{
public:
    static const unsigned int _Min=0;
    static const unsigned int _Max=UINT_MAX;
};

template<>
class SLimits<long _W64>
{
public:
    static const long _Min=LONG_MIN;
    static const long _Max=LONG_MAX;
};

template<>
class SLimits<unsigned long _W64>
{
public:
    static const unsigned long _Min=0;
    static const unsigned long _Max=ULONG_MAX;
};

template<>
class SLimits<long long>
{
public:
    static const long long _Min=LLONG_MIN;
    static const long long _Max=LLONG_MAX;
};

template<>
class SLimits<unsigned long long>
{
public:
    static const unsigned long long _Min=0;
    static const unsigned long long _Max=ULLONG_MAX;
};

/* generic version */
template<typename T>
inline HRESULT SAdd(T* ptResult, T tLeft, T tRight)
{
    if(SLimits<T>::_Max-tLeft < tRight)
    {
        return E_INVALIDARG;
    }
    *ptResult= tLeft + tRight;
    return S_OK;
}

/* generic but compariatively slow version */
template<typename T>
inline HRESULT SMultiply(T* ptResult,    T tLeft, T tRight)
{
    /* avoid divide 0 */
    if(tLeft==0)
    {
        *ptResult=0;
        return S_OK;
    }
    if(SLimits<T>::_Max/tLeft < tRight)
    {
        return E_INVALIDARG;
    }
    *ptResult= tLeft * tRight;
    return S_OK;
}

/* fast    version    for    32 bit integers    */
template<>
inline HRESULT SMultiply(int _W64    *piResult, int _W64    iLeft, int _W64 iRight)
{
    __int64 i64Result=static_cast<__int64>(iLeft) * static_cast<__int64>(iRight);
    if(i64Result>INT_MAX || i64Result < INT_MIN)
    {
        return E_INVALIDARG;
    }
    *piResult=static_cast<int _W64>(i64Result);
    return S_OK;
}

template<>
inline HRESULT SMultiply(unsigned int    _W64 *piResult, unsigned int _W64 iLeft, unsigned int _W64 iRight)
{
    unsigned __int64 i64Result=static_cast<unsigned __int64>(iLeft) * static_cast<unsigned __int64>(iRight);
    if(i64Result>UINT_MAX)
    {
        return E_INVALIDARG;
    }
    *piResult=static_cast<unsigned int _W64>(i64Result);
    return S_OK;
}

template<>
inline HRESULT SMultiply(long    _W64 *piResult, long _W64 iLeft, long _W64 iRight)
{
    __int64 i64Result=static_cast<__int64>(iLeft) * static_cast<__int64>(iRight);
    if(i64Result>LONG_MAX || i64Result < LONG_MIN)
    {
        return E_INVALIDARG;
    }
    *piResult=static_cast<long _W64>(i64Result);
    return S_OK;
}

template<>
inline HRESULT SMultiply(unsigned long _W64 *piResult, unsigned long _W64 iLeft, unsigned long _W64 iRight)
{
    unsigned __int64 i64Result=static_cast<unsigned __int64>(iLeft) * static_cast<unsigned __int64>(iRight);
    if(i64Result>ULONG_MAX)
    {
        return E_INVALIDARG;
    }
    *piResult=static_cast<unsigned long _W64>(i64Result);
    return S_OK;
}

struct SPlex     // warning variable length structure
{
    SPlex* pNext;
#if (_AFX_PACKING >= 8)
    DWORD dwReserved[1];    // align on 8 byte boundary
#endif
    // BYTE data[maxNum*elementSize];

    void* data()
    {
        return this+1;
    }

    static SPlex* Create(SPlex*& head, size_t nMax, size_t cbElement);
    // like 'calloc' but no zero fill
    // may throw memory exceptions

    void FreeDataChain();       // free this one and links
};

inline SPlex* SPlex::Create( SPlex*& pHead, size_t nMax, size_t nElementSize )
{
    SPlex* pPlex;

    ASSERT( nMax > 0 );
    ASSERT( nElementSize > 0 );

    size_t nBytes=0;
    if(     FAILED(SMultiply(&nBytes, nMax, nElementSize)) ||
            FAILED(SAdd(&nBytes, nBytes, sizeof(SPlex))) )
    {
        return NULL;
    }
    pPlex = static_cast< SPlex* >( malloc( nBytes ) );
    if( pPlex == NULL )
    {
        return( NULL );
    }

    pPlex->pNext = pHead;
    pHead = pPlex;

    return( pPlex );
}

inline void SPlex::FreeDataChain()
{
    SPlex* pPlex;

    pPlex = this;
    while( pPlex != NULL )
    {
        SPlex* pNext;

        pNext = pPlex->pNext;
        free( pPlex );
        pPlex = pNext;
    }
}

template< typename T >
class CElementTraitsBase
{
public:
    typedef const T& INARGTYPE;
    typedef T& OUTARGTYPE;

    static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
    {
        for( size_t iElement = 0; iElement < nElements; iElement++ )
        {
            pDest[iElement] = pSrc[iElement];
        }
    }

    static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
    {
        // A simple memmove works for nearly all types.
        // You'll have to override this for types that have pointers to their
        // own members.
        memmove_s( pDest, nElements*sizeof( T ), pSrc, nElements*sizeof( T ));
    }
};

template< typename T >
class CDefaultHashTraits
{
public:
    static ULONG Hash( const T& element ) throw()
    {
        return( ULONG( ULONG_PTR( element ) ) );
    }
};

template< typename T >
class CDefaultCompareTraits
{
public:
    static bool CompareElements( const T& element1, const T& element2 )
    {
        return( (element1 == element2) != 0 );  // != 0 to handle overloads of operator== that return BOOL instead of bool
    }

    static int CompareElementsOrdered( const T& element1, const T& element2 )
    {
        if( element1 < element2 )
        {
            return( -1 );
        }
        else if( element1 == element2 )
        {
            return( 0 );
        }
        else
        {
            ASSERT( element1 > element2 );
            return( 1 );
        }
    }
};

template< typename T >
class CDefaultElementTraits :
    public CElementTraitsBase< T >,
    public CDefaultHashTraits< T >,
    public CDefaultCompareTraits< T >
{
};

template< typename T >
class CElementTraits :
    public CDefaultElementTraits< T >
{
};

template<>
class CElementTraits< GUID > :
    public CElementTraitsBase< GUID >
{
public:
    static ULONG Hash( INARGTYPE guid )
    {
        const DWORD* pdwData = reinterpret_cast< const DWORD* >( &guid );

        return( pdwData[0]^pdwData[1]^pdwData[2]^pdwData[3] );
    }

    static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
    {
        return( (element1 == element2) != 0 );  // != 0 to handle overloads of operator== that return BOOL instead of bool
    }

    static int CompareElementsOrdered( INARGTYPE element1, INARGTYPE element2 )
    {
        const DWORD* pdwData1 = reinterpret_cast< const DWORD* >( &element1 );
        const DWORD* pdwData2 = reinterpret_cast< const DWORD* >( &element2 );

        for( int iDWORD = 3; iDWORD >= 0; iDWORD-- )
        {
            if( pdwData1[iDWORD] > pdwData2[iDWORD] )
            {
                return( 1 );
            }
            else if( pdwData1[iDWORD] < pdwData2[iDWORD] )
            {
                return( -1 );
            }
        }

        return( 0 );
    }
};

template < typename T >
class CDefaultCharTraits
{
};

template <>
class CDefaultCharTraits<char>
{
public:
    static char CharToUpper(char x)
    {
        return (char)toupper(x);
    }

    static char CharToLower(char x)
    {
        return (char)tolower(x);
    }
};

template <>
class CDefaultCharTraits<wchar_t>
{
public:
    static wchar_t CharToUpper(wchar_t x)
    {
        return (wchar_t)towupper(x);
    }

    static wchar_t CharToLower(wchar_t x)
    {
        return (wchar_t)towlower(x);
    }
};

template< typename T, class CharTraits = CDefaultCharTraits<T::XCHAR> >
class SStringElementTraitsI :
    public CElementTraitsBase< T >
{
public:
    typedef typename T::PCXSTR INARGTYPE;
    typedef T& OUTARGTYPE;

    static ULONG Hash( INARGTYPE str )
    {
        ULONG nHash = 0;

        const T::XCHAR* pch = str;

        SENSURE( pch != NULL );

        while( *pch != 0 )
        {
            nHash = (nHash<<5)+nHash+CharTraits::CharToUpper(*pch);
            pch++;
        }

        return( nHash );
    }

    static bool CompareElements( INARGTYPE str1, INARGTYPE str2 ) throw()
    {
        return( T::StrTraits::StringCompareIgnore( str1, str2 ) == 0 );
    }

    static int CompareElementsOrdered( INARGTYPE str1, INARGTYPE str2 ) throw()
    {
        return( T::StrTraits::StringCompareIgnore( str1, str2 ) );
    }
};

template< typename T >
class CDuiStringRefElementTraits :
    public CElementTraitsBase< T >
{
public:
    static ULONG Hash( INARGTYPE str )
    {
        ULONG nHash = 0;

        const T::XCHAR* pch = str;

        SENSURE( pch != NULL );

        while( *pch != 0 )
        {
            nHash = (nHash<<5)+nHash+(*pch);
            pch++;
        }

        return( nHash );
    }

    static bool CompareElements( INARGTYPE element1, INARGTYPE element2 ) throw()
    {
        return( element1 == element2 );
    }

    static int CompareElementsOrdered( INARGTYPE str1, INARGTYPE str2 ) throw()
    {
        return( str1.Compare( str2 ) );
    }
};

template< typename T >
class CPrimitiveElementTraits :
    public CDefaultElementTraits< T >
{
public:
    typedef T INARGTYPE;
    typedef T& OUTARGTYPE;
};

#define _DECLARE_PRIMITIVE_TRAITS( T ) \
    template<> \
    class CElementTraits< T > : \
    public CPrimitiveElementTraits< T > \
    { \
    };

_DECLARE_PRIMITIVE_TRAITS( unsigned char )
_DECLARE_PRIMITIVE_TRAITS( unsigned short )
_DECLARE_PRIMITIVE_TRAITS( unsigned int )
_DECLARE_PRIMITIVE_TRAITS( unsigned long )
_DECLARE_PRIMITIVE_TRAITS( unsigned __int64 )
_DECLARE_PRIMITIVE_TRAITS( signed char )
_DECLARE_PRIMITIVE_TRAITS( char )
_DECLARE_PRIMITIVE_TRAITS( short )
_DECLARE_PRIMITIVE_TRAITS( int )
_DECLARE_PRIMITIVE_TRAITS( long )
_DECLARE_PRIMITIVE_TRAITS( __int64 )
_DECLARE_PRIMITIVE_TRAITS( float )
_DECLARE_PRIMITIVE_TRAITS( double )
_DECLARE_PRIMITIVE_TRAITS( bool )
#ifdef _NATIVE_WCHAR_T_DEFINED
_DECLARE_PRIMITIVE_TRAITS( wchar_t )
#endif
_DECLARE_PRIMITIVE_TRAITS( void* )

template< typename E, class ETraits = CElementTraits< E > >
class SArray
{
public:
    typedef typename ETraits::INARGTYPE INARGTYPE;
    typedef typename ETraits::OUTARGTYPE OUTARGTYPE;

public:
    SArray() throw();

    size_t GetCount() const throw();
    bool IsEmpty() const throw();
    bool SetCount( size_t nNewSize, int nGrowBy = -1 );

    void FreeExtra() throw();
    void RemoveAll() throw();

    const E& GetAt( size_t iElement ) const;
    void SetAt( size_t iElement, INARGTYPE element );
    E& GetAt( size_t iElement );

    const E* GetData() const throw();
    E* GetData() throw();

    void SetAtGrow( size_t iElement, INARGTYPE element );
    // Add an empty element to the end of the array
    size_t Add();
    // Add an element to the end of the array
    size_t Add( INARGTYPE element );
    size_t Append( const SArray< E, ETraits >& aSrc );
    void Copy( const SArray< E, ETraits >& aSrc );

    const E& operator[]( size_t iElement ) const;
    E& operator[]( size_t iElement );

    void InsertAt( size_t iElement, INARGTYPE element, size_t nCount = 1 );
    void InsertArrayAt( size_t iStart, const SArray< E, ETraits >* paNew );
    void RemoveAt( size_t iElement, size_t nCount = 1 );

#ifdef _DEBUG
    void AssertValid() const;
#endif  // _DEBUG

private:
    bool GrowBuffer( size_t nNewSize );

    // Implementation
private:
    E* m_pData;
    size_t m_nSize;
    size_t m_nMaxSize;
    int m_nGrowBy;

private:
    static void CallConstructors( E* pElements, size_t nElements );
    static void CallDestructors( E* pElements, size_t nElements ) throw();

public:
    ~SArray() throw();

};

template< typename E, class ETraits >
inline size_t SArray< E, ETraits >::GetCount() const throw()
{
    return( m_nSize );
}

template< typename E, class ETraits >
inline bool SArray< E, ETraits >::IsEmpty() const throw()
{
    return( m_nSize == 0 );
}

template< typename E, class ETraits >
inline void SArray< E, ETraits >::RemoveAll() throw()
{
    SetCount( 0, -1 );
}

template< typename E, class ETraits >
inline const E& SArray< E, ETraits >::GetAt( size_t iElement ) const
{
    ASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline void SArray< E, ETraits >::SetAt( size_t iElement, INARGTYPE element )
{
    ASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    m_pData[iElement] = element;
}

template< typename E, class ETraits >
inline E& SArray< E, ETraits >::GetAt( size_t iElement )
{
    ASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline const E* SArray< E, ETraits >::GetData() const throw()
{
    return( m_pData );
}

template< typename E, class ETraits >
inline E* SArray< E, ETraits >::GetData() throw()
{
    return( m_pData );
}

template< typename E, class ETraits >
inline size_t SArray< E, ETraits >::Add()
{
    size_t iElement;

    iElement = m_nSize;
    bool bSuccess=SetCount( m_nSize+1 );
    if( !bSuccess )
    {
        SThrow( E_OUTOFMEMORY );
    }

    return( iElement );
}

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
inline size_t SArray< E, ETraits >::Add( INARGTYPE element )
{
    size_t iElement;

    iElement = m_nSize;
    if( iElement >= m_nMaxSize )
    {
        bool bSuccess = GrowBuffer( iElement+1 );
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
    }
    ::new( m_pData+iElement ) E( element );
    m_nSize++;

    return( iElement );
}

#pragma pop_macro("new")

template< typename E, class ETraits >
inline const E& SArray< E, ETraits >::operator[]( size_t iElement ) const
{
    ASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline E& SArray< E, ETraits >::operator[]( size_t iElement )
{
    ASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    return( m_pData[iElement] );
}

template< typename E, class ETraits >
SArray< E, ETraits >::SArray()  throw():
    m_pData( NULL ),
    m_nSize( 0 ),
    m_nMaxSize( 0 ),
    m_nGrowBy( 0 )
{
}

template< typename E, class ETraits >
SArray< E, ETraits >::~SArray() throw()
{
    if( m_pData != NULL )
    {
        CallDestructors( m_pData, m_nSize );
        free( m_pData );
    }
}

template< typename E, class ETraits >
bool SArray< E, ETraits >::GrowBuffer( size_t nNewSize )
{
    if( nNewSize > m_nMaxSize )
    {
        if( m_pData == NULL )
        {
            size_t nAllocSize =  size_t( m_nGrowBy ) > nNewSize ? size_t( m_nGrowBy ) : nNewSize ;
            m_pData = static_cast< E* >( calloc( nAllocSize,sizeof( E ) ) );
            if( m_pData == NULL )
            {
                return( false );
            }
            m_nMaxSize = nAllocSize;
        }
        else
        {
            // otherwise, grow array
            size_t nGrowBy = m_nGrowBy;
            if( nGrowBy == 0 )
            {
                // heuristically determine growth when nGrowBy == 0
                //  (this avoids heap fragmentation in many situations)
                nGrowBy = m_nSize/8;
                nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
            }
            size_t nNewMax;
            if( nNewSize < (m_nMaxSize+nGrowBy) )
                nNewMax = m_nMaxSize+nGrowBy;  // granularity
            else
                nNewMax = nNewSize;  // no slush

            ASSERT( nNewMax >= m_nMaxSize );  // no wrap around
#ifdef SIZE_T_MAX
            ASSERT( nNewMax <= SIZE_T_MAX/sizeof( E ) ); // no overflow
#endif
            E* pNewData = static_cast< E* >( calloc( nNewMax,sizeof( E ) ) );
            if( pNewData == NULL )
            {
                return false;
            }

            // copy new data from old
            ETraits::RelocateElements( pNewData, m_pData, m_nSize );

            // get rid of old stuff (note: no destructors called)
            free( m_pData );
            m_pData = pNewData;
            m_nMaxSize = nNewMax;
        }
    }

    return true;
}

template< typename E, class ETraits >
bool SArray< E, ETraits >::SetCount( size_t nNewSize, int nGrowBy )
{
    SASSERT_VALID(this);

    if( nGrowBy != -1 )
    {
        m_nGrowBy = nGrowBy;  // set new size
    }

    if( nNewSize == 0 )
    {
        // shrink to nothing
        if( m_pData != NULL )
        {
            CallDestructors( m_pData, m_nSize );
            free( m_pData );
            m_pData = NULL;
        }
        m_nSize = 0;
        m_nMaxSize = 0;
    }
    else if( nNewSize <= m_nMaxSize )
    {
        // it fits
        if( nNewSize > m_nSize )
        {
            // initialize the new elements
            CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );
        }
        else if( m_nSize > nNewSize )
        {
            // destroy the old elements
            CallDestructors( m_pData+nNewSize, m_nSize-nNewSize );
        }
        m_nSize = nNewSize;
    }
    else
    {
        bool bSuccess;

        bSuccess = GrowBuffer( nNewSize );
        if( !bSuccess )
        {
            return( false );
        }

        // construct new elements
        ASSERT( nNewSize > m_nSize );
        CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );

        m_nSize = nNewSize;
    }

    return true;
}

template< typename E, class ETraits >
size_t SArray< E, ETraits >::Append( const SArray< E, ETraits >& aSrc )
{
    SASSERT_VALID(this);
    ASSERT( this != &aSrc );   // cannot append to itself

    size_t nOldSize = m_nSize;
    bool bSuccess=SetCount( m_nSize+aSrc.m_nSize );
    if( !bSuccess )
    {
        SThrow( E_OUTOFMEMORY );
    }

    ETraits::CopyElements( m_pData+nOldSize, aSrc.m_pData, aSrc.m_nSize );

    return( nOldSize );
}

template< typename E, class ETraits >
void SArray< E, ETraits >::Copy( const SArray< E, ETraits >& aSrc )
{
    SASSERT_VALID(this);
    ASSERT( this != &aSrc );   // cannot append to itself

    bool bSuccess=SetCount( aSrc.m_nSize );
    if( !bSuccess )
    {
        SThrow( E_OUTOFMEMORY );
    }

    ETraits::CopyElements( m_pData, aSrc.m_pData, aSrc.m_nSize );
}

template< typename E, class ETraits >
void SArray< E, ETraits >::FreeExtra() throw()
{
    SASSERT_VALID(this);

    if( m_nSize != m_nMaxSize )
    {
        // shrink to desired size
#ifdef SIZE_T_MAX
        SASSUME( m_nSize <= (SIZE_T_MAX/sizeof( E )) ); // no overflow
#endif
        E* pNewData = NULL;
        if( m_nSize != 0 )
        {
            pNewData = (E*)calloc( m_nSize,sizeof( E ) );
            if( pNewData == NULL )
            {
                return;
            }

            // copy new data from old
            ETraits::RelocateElements( pNewData, m_pData, m_nSize );
        }

        // get rid of old stuff (note: no destructors called)
        free( m_pData );
        m_pData = pNewData;
        m_nMaxSize = m_nSize;
    }
}

template< typename E, class ETraits >
void SArray< E, ETraits >::SetAtGrow( size_t iElement, INARGTYPE element )
{
    SASSERT_VALID(this);
    size_t nOldSize;

    nOldSize = m_nSize;
    if( iElement >= m_nSize )
    {
        bool bSuccess=SetCount( iElement+1, -1 );
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
    }

    _STRY
    {
        m_pData[iElement] = element;
    }
    _SCATCHALL()
    {
        if( m_nSize != nOldSize )
        {
            SetCount( nOldSize, -1 );
        }
    }
}

template< typename E, class ETraits >
void SArray< E, ETraits >::InsertAt( size_t iElement, INARGTYPE element, size_t nElements /*=1*/)
{
    SASSERT_VALID(this);
    ASSERT( nElements > 0 );     // zero size not allowed

    if( iElement >= m_nSize )
    {
        // adding after the end of the array
        bool bSuccess=SetCount( iElement+nElements, -1 );   // grow so nIndex is valid
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
    }
    else
    {
        // inserting in the middle of the array
        size_t nOldSize = m_nSize;
        bool bSuccess=SetCount( m_nSize+nElements, -1 );  // grow it to new size
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
        // destroy intial data before copying over it
        CallDestructors( m_pData+nOldSize, nElements );
        // shift old data up to fill gap
        ETraits::RelocateElements( m_pData+(iElement+nElements), m_pData+iElement,
                                   nOldSize-iElement );

        _STRY
        {
            // re-init slots we copied from
            CallConstructors( m_pData+iElement, nElements );
        }
        _SCATCHALL()
        {
            ETraits::RelocateElements( m_pData+iElement, m_pData+(iElement+nElements),
                                       nOldSize-iElement );
            SetCount( nOldSize, -1 );
        }
    }

    // insert new value in the gap
    ASSERT( (iElement+nElements) <= m_nSize );
    for( size_t iNewElement = iElement; iNewElement < (iElement+nElements); iNewElement++ )
    {
        m_pData[iNewElement] = element;
    }
}

template< typename E, class ETraits >
void SArray< E, ETraits >::RemoveAt( size_t iElement, size_t nElements )
{
    SASSERT_VALID(this);
    ASSERT( (iElement+nElements) <= m_nSize );

    size_t newCount = iElement+nElements;
    if ((newCount < iElement) || (newCount < nElements) || (newCount > m_nSize))
        SThrow(E_INVALIDARG);

    // just remove a range
    size_t nMoveCount = m_nSize-(newCount);
    CallDestructors( m_pData+iElement, nElements );
    if( nMoveCount > 0 )
    {
        ETraits::RelocateElements( m_pData+iElement, m_pData+(newCount),
                                   nMoveCount );
    }
    m_nSize -= nElements;
}

template< typename E, class ETraits >
void SArray< E, ETraits >::InsertArrayAt( size_t iStartElement,
        const SArray< E, ETraits >* paNew )
{
    SASSERT_VALID( this );
    SENSURE( paNew != NULL );
    SASSERT_VALID( paNew );

    if( paNew->GetCount() > 0 )
    {
        InsertAt( iStartElement, paNew->GetAt( 0 ), paNew->GetCount() );
        for( size_t iElement = 0; iElement < paNew->GetCount(); iElement++ )
        {
            SetAt( iStartElement+iElement, paNew->GetAt( iElement ) );
        }
    }
}

#ifdef _DEBUG
template< typename E, class ETraits >
void SArray< E, ETraits >::AssertValid() const
{
    if( m_pData == NULL )
    {
        SASSUME( m_nSize == 0 );
        SASSUME( m_nMaxSize == 0 );
    }
    else
    {
        SASSUME( m_nSize <= m_nMaxSize );
    }
}
#endif

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
void SArray< E, ETraits >::CallConstructors( E* pElements, size_t nElements )
{
    size_t iElement = 0;

    _STRY
    {
        for( iElement = 0; iElement < nElements; iElement++ )
        {
            ::new( pElements+iElement ) E;
        }
    }
    _SCATCHALL()
    {
        while( iElement > 0 )
        {
            iElement--;
            pElements[iElement].~E();
        }

    }
}

#pragma pop_macro("new")

template< typename E, class ETraits >
void SArray< E, ETraits >::CallDestructors( E* pElements, size_t nElements ) throw()
{
    (void)pElements;

    for( size_t iElement = 0; iElement < nElements; iElement++ )
    {
        pElements[iElement].~E();
    }
}


template< typename E, class ETraits = CElementTraits< E > >
class SList
{
public:
    typedef typename ETraits::INARGTYPE INARGTYPE;

private:
    class CNode :
        public __POSITION
    {
    public:
        CNode()
        {
        }
        CNode( INARGTYPE element ) :
            m_element( element )
        {
        }
        ~CNode() throw()
        {
        }

    public:
        CNode* m_pNext;
        CNode* m_pPrev;
        E m_element;

    private:
        CNode( const CNode& ) throw();
    };

public:
    SList( UINT nBlockSize = 10 ) throw();

    size_t GetCount() const throw();
    bool IsEmpty() const throw();

    E& GetHead();
    const E& GetHead() const;
    E& GetTail();
    const E& GetTail() const;

    E RemoveHead();
    E RemoveTail();
    void RemoveHeadNoReturn() throw();
    void RemoveTailNoReturn() throw();

    POSITION AddHead();
    POSITION AddHead( INARGTYPE element );
    void AddHeadList( const SList< E, ETraits >* plNew );
    POSITION AddTail();
    POSITION AddTail( INARGTYPE element );
    void AddTailList( const SList< E, ETraits >* plNew );

    void RemoveAll() throw();

    POSITION GetHeadPosition() const throw();
    POSITION GetTailPosition() const throw();
    E& GetNext( POSITION& pos );
    const E& GetNext( POSITION& pos ) const;
    E& GetPrev( POSITION& pos );
    const E& GetPrev( POSITION& pos ) const throw();

    E& GetAt( POSITION pos );
    const E& GetAt( POSITION pos ) const;
    void SetAt( POSITION pos, INARGTYPE element );
    void RemoveAt( POSITION pos ) throw();

    POSITION InsertBefore( POSITION pos, INARGTYPE element );
    POSITION InsertAfter( POSITION pos, INARGTYPE element );

    POSITION Find( INARGTYPE element, POSITION posStartAfter = NULL ) const throw();
    POSITION FindIndex( size_t iElement ) const throw();

    void MoveToHead( POSITION pos );
    void MoveToTail( POSITION pos );
    void SwapElements( POSITION pos1, POSITION pos2 ) throw();

#ifdef _DEBUG
    void AssertValid() const;
#endif  // _DEBUG

    // Implementation
private:
    CNode* m_pHead;
    CNode* m_pTail;
    size_t m_nElements;
    SPlex* m_pBlocks;
    CNode* m_pFree;
    UINT m_nBlockSize;

private:
    void GetFreeNode();
    CNode* NewNode( CNode* pPrev, CNode* pNext );
    CNode* NewNode( INARGTYPE element, CNode* pPrev, CNode* pNext );
    void FreeNode( CNode* pNode ) throw();

public:
    ~SList() throw();

private:
    // Private to prevent use
    SList( const SList& ) throw();
    SList& operator=( const SList& ) throw();
};

template< typename E, class ETraits >
inline size_t SList< E, ETraits >::GetCount() const throw()
{
    return( m_nElements );
}

template< typename E, class ETraits >
inline bool SList< E, ETraits >::IsEmpty() const throw()
{
    return( m_nElements == 0 );
}

template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetHead()
{
    SENSURE( m_pHead != NULL );
    return( m_pHead->m_element );
}

template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetHead() const
{
    SENSURE( m_pHead != NULL );
    return( m_pHead->m_element );
}

template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetTail()
{
    SENSURE( m_pTail != NULL );
    return( m_pTail->m_element );
}

template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetTail() const
{
    SENSURE( m_pTail != NULL );
    return( m_pTail->m_element );
}

template< typename E, class ETraits >
inline POSITION SList< E, ETraits >::GetHeadPosition() const throw()
{
    return( POSITION( m_pHead ) );
}

template< typename E, class ETraits >
inline POSITION SList< E, ETraits >::GetTailPosition() const throw()
{
    return( POSITION( m_pTail ) );
}

template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetNext( POSITION& pos )
{
    CNode* pNode;

    SENSURE( pos != NULL );
    pNode = (CNode*)pos;
    pos = POSITION( pNode->m_pNext );

    return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetNext( POSITION& pos ) const
{
    CNode* pNode;

    SENSURE( pos != NULL );
    pNode = (CNode*)pos;
    pos = POSITION( pNode->m_pNext );

    return( pNode->m_element );
}

template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetPrev( POSITION& pos )
{
    CNode* pNode;

    SENSURE( pos != NULL );
    pNode = (CNode*)pos;
    pos = POSITION( pNode->m_pPrev );

    return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetPrev( POSITION& pos ) const throw()
{
    CNode* pNode;

    ASSERT( pos != NULL );
    pNode = (CNode*)pos;
    pos = POSITION( pNode->m_pPrev );

    return( pNode->m_element );
}

template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetAt( POSITION pos )
{
    SENSURE( pos != NULL );
    CNode* pNode = (CNode*)pos;
    return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetAt( POSITION pos ) const
{
    SENSURE( pos != NULL );
    CNode* pNode = (CNode*)pos;
    return( pNode->m_element );
}

template< typename E, class ETraits >
inline void SList< E, ETraits >::SetAt( POSITION pos, INARGTYPE element )
{
    SENSURE( pos != NULL );
    CNode* pNode = (CNode*)pos;
    pNode->m_element = element;
}

template< typename E, class ETraits >
SList< E, ETraits >::SList( UINT nBlockSize ) throw() :
    m_nElements( 0 ),
    m_pHead( NULL ),
    m_pTail( NULL ),
    m_nBlockSize( nBlockSize ),
    m_pBlocks( NULL ),
    m_pFree( NULL )
{
    ASSERT( nBlockSize > 0 );
}

template< typename E, class ETraits >
void SList< E, ETraits >::RemoveAll()
{
    while( m_nElements > 0 )
    {
        CNode* pKill = m_pHead;
        SENSURE( pKill != NULL );

        m_pHead = m_pHead->m_pNext;
        FreeNode( pKill );
    }

    SASSUME( m_nElements == 0 );
    m_pHead = NULL;
    m_pTail = NULL;
    m_pFree = NULL;

    if( m_pBlocks != NULL )
    {
        m_pBlocks->FreeDataChain();
        m_pBlocks = NULL;
    }
}

template< typename E, class ETraits >
SList< E, ETraits >::~SList() throw()
{
    RemoveAll();
    SASSUME( m_nElements == 0 );
}

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
void SList< E, ETraits >::GetFreeNode()
{
    if( m_pFree == NULL )
    {
        SPlex* pPlex;
        CNode* pNode;

        pPlex = SPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
        if( pPlex == NULL )
        {
            SThrow( E_OUTOFMEMORY );
        }
        pNode = (CNode*)pPlex->data();
        pNode += m_nBlockSize-1;
        for( int iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
        {
            pNode->m_pNext = m_pFree;
            m_pFree = pNode;
            pNode--;
        }
    }
    SASSUME( m_pFree != NULL );
}

template< typename E, class ETraits >
typename SList< E, ETraits >::CNode* SList< E, ETraits >::NewNode( CNode* pPrev, CNode* pNext )
{
    GetFreeNode();

    CNode* pNewNode = m_pFree;
    CNode* pNextFree = m_pFree->m_pNext;

    ::new( pNewNode ) CNode;

    m_pFree = pNextFree;
    pNewNode->m_pPrev = pPrev;
    pNewNode->m_pNext = pNext;
    m_nElements++;
    SASSUME( m_nElements > 0 );

    return( pNewNode );
}

template< typename E, class ETraits >
typename SList< E, ETraits >::CNode* SList< E, ETraits >::NewNode( INARGTYPE element, CNode* pPrev,
        CNode* pNext )
{
    GetFreeNode();

    CNode* pNewNode = m_pFree;
    CNode* pNextFree = m_pFree->m_pNext;

    ::new( pNewNode ) CNode( element );

    m_pFree = pNextFree;
    pNewNode->m_pPrev = pPrev;
    pNewNode->m_pNext = pNext;
    m_nElements++;
    SASSUME( m_nElements > 0 );

    return( pNewNode );
}

#pragma pop_macro("new")

template< typename E, class ETraits >
void SList< E, ETraits >::FreeNode( CNode* pNode ) throw()
{
    pNode->~CNode();
    pNode->m_pNext = m_pFree;
    m_pFree = pNode;
    SASSUME( m_nElements > 0 );
    m_nElements--;
    if( m_nElements == 0 )
    {
        RemoveAll();
    }
}

template< typename E, class ETraits >
POSITION SList< E, ETraits >::AddHead()
{
    CNode* pNode = NewNode( NULL, m_pHead );
    if( m_pHead != NULL )
    {
        m_pHead->m_pPrev = pNode;
    }
    else
    {
        m_pTail = pNode;
    }
    m_pHead = pNode;

    return( POSITION( pNode ) );
}

template< typename E, class ETraits >
POSITION SList< E, ETraits >::AddHead( INARGTYPE element )
{
    CNode* pNode;

    pNode = NewNode( element, NULL, m_pHead );

    if( m_pHead != NULL )
    {
        m_pHead->m_pPrev = pNode;
    }
    else
    {
        m_pTail = pNode;
    }
    m_pHead = pNode;

    return( POSITION( pNode ) );
}

template< typename E, class ETraits >
POSITION SList< E, ETraits >::AddTail()
{
    CNode* pNode = NewNode( m_pTail, NULL );
    if( m_pTail != NULL )
    {
        m_pTail->m_pNext = pNode;
    }
    else
    {
        m_pHead = pNode;
    }
    m_pTail = pNode;

    return( POSITION( pNode ) );
}

template< typename E, class ETraits >
POSITION SList< E, ETraits >::AddTail( INARGTYPE element )
{
    CNode* pNode;

    pNode = NewNode( element, m_pTail, NULL );

    if( m_pTail != NULL )
    {
        m_pTail->m_pNext = pNode;
    }
    else
    {
        m_pHead = pNode;
    }
    m_pTail = pNode;

    return( POSITION( pNode ) );
}

template< typename E, class ETraits >
void SList< E, ETraits >::AddHeadList( const SList< E, ETraits >* plNew )
{
    SENSURE( plNew != NULL );

    POSITION pos = plNew->GetTailPosition();
    while( pos != NULL )
    {
        INARGTYPE element = plNew->GetPrev( pos );
        AddHead( element );
    }
}

template< typename E, class ETraits >
void SList< E, ETraits >::AddTailList( const SList< E, ETraits >* plNew )
{
    SENSURE( plNew != NULL );

    POSITION pos = plNew->GetHeadPosition();
    while( pos != NULL )
    {
        INARGTYPE element = plNew->GetNext( pos );
        AddTail( element );
    }
}

template< typename E, class ETraits >
E SList< E, ETraits >::RemoveHead()
{
    SENSURE( m_pHead != NULL );

    CNode* pNode = m_pHead;
    E element( pNode->m_element );

    m_pHead = pNode->m_pNext;
    if( m_pHead != NULL )
    {
        m_pHead->m_pPrev = NULL;
    }
    else
    {
        m_pTail = NULL;
    }
    FreeNode( pNode );

    return( element );
}

template< typename E, class ETraits >
void SList< E, ETraits >::RemoveHeadNoReturn()
{
    SENSURE( m_pHead != NULL );

    CNode* pNode = m_pHead;

    m_pHead = pNode->m_pNext;
    if( m_pHead != NULL )
    {
        m_pHead->m_pPrev = NULL;
    }
    else
    {
        m_pTail = NULL;
    }
    FreeNode( pNode );
}

template< typename E, class ETraits >
E SList< E, ETraits >::RemoveTail()
{
    SENSURE( m_pTail != NULL );

    CNode* pNode = m_pTail;

    E element( pNode->m_element );

    m_pTail = pNode->m_pPrev;
    if( m_pTail != NULL )
    {
        m_pTail->m_pNext = NULL;
    }
    else
    {
        m_pHead = NULL;
    }
    FreeNode( pNode );

    return( element );
}

template< typename E, class ETraits >
void SList< E, ETraits >::RemoveTailNoReturn()
{
    SENSURE( m_pTail != NULL );

    CNode* pNode = m_pTail;

    m_pTail = pNode->m_pPrev;
    if( m_pTail != NULL )
    {
        m_pTail->m_pNext = NULL;
    }
    else
    {
        m_pHead = NULL;
    }
    FreeNode( pNode );
}

template< typename E, class ETraits >
POSITION SList< E, ETraits >::InsertBefore( POSITION pos, INARGTYPE element )
{
    SASSERT_VALID(this);

    if( pos == NULL )
        return AddHead( element ); // insert before nothing -> head of the list

    // Insert it before position
    CNode* pOldNode = (CNode*)pos;
    CNode* pNewNode = NewNode( element, pOldNode->m_pPrev, pOldNode );

    if( pOldNode->m_pPrev != NULL )
    {
        pOldNode->m_pPrev->m_pNext = pNewNode;
    }
    else
    {
        ASSERT( pOldNode == m_pHead );
        m_pHead = pNewNode;
    }
    pOldNode->m_pPrev = pNewNode;

    return( POSITION( pNewNode ) );
}

template< typename E, class ETraits >
POSITION SList< E, ETraits >::InsertAfter( POSITION pos, INARGTYPE element )
{
    SASSERT_VALID(this);

    if( pos == NULL )
        return AddTail( element ); // insert after nothing -> tail of the list

    // Insert it after position
    CNode* pOldNode = (CNode*)pos;
    CNode* pNewNode = NewNode( element, pOldNode, pOldNode->m_pNext );

    if( pOldNode->m_pNext != NULL )
    {
        pOldNode->m_pNext->m_pPrev = pNewNode;
    }
    else
    {
        ASSERT( pOldNode == m_pTail );
        m_pTail = pNewNode;
    }
    pOldNode->m_pNext = pNewNode;

    return( POSITION( pNewNode ) );
}

template< typename E, class ETraits >
void SList< E, ETraits >::RemoveAt( POSITION pos )
{
    SASSERT_VALID(this);
    SENSURE( pos != NULL );

    CNode* pOldNode = (CNode*)pos;

    // remove pOldNode from list
    if( pOldNode == m_pHead )
    {
        m_pHead = pOldNode->m_pNext;
    }
    else
    {
        pOldNode->m_pPrev->m_pNext = pOldNode->m_pNext;
    }
    if( pOldNode == m_pTail )
    {
        m_pTail = pOldNode->m_pPrev;
    }
    else
    {
        pOldNode->m_pNext->m_pPrev = pOldNode->m_pPrev;
    }
    FreeNode( pOldNode );
}

template< typename E, class ETraits >
POSITION SList< E, ETraits >::FindIndex( size_t iElement ) const throw()
{
    SASSERT_VALID(this);

    if( iElement >= m_nElements )
        return NULL;  // went too far

    if(m_pHead == NULL)
        return NULL;

    CNode* pNode = m_pHead;
    for( size_t iSearch = 0; iSearch < iElement; iSearch++ )
    {
        pNode = pNode->m_pNext;
    }

    return( POSITION( pNode ) );
}

template< typename E, class ETraits >
void SList< E, ETraits >::MoveToHead( POSITION pos )
{
    SENSURE( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );

    if( pNode == m_pHead )
    {
        // Already at the head
        return;
    }

    if( pNode->m_pNext == NULL )
    {
        ASSERT( pNode == m_pTail );
        m_pTail = pNode->m_pPrev;
    }
    else
    {
        pNode->m_pNext->m_pPrev = pNode->m_pPrev;
    }

    ASSERT( pNode->m_pPrev != NULL );  // This node can't be the head, since we already checked that case
    pNode->m_pPrev->m_pNext = pNode->m_pNext;

    m_pHead->m_pPrev = pNode;
    pNode->m_pNext = m_pHead;
    pNode->m_pPrev = NULL;
    m_pHead = pNode;
}

template< typename E, class ETraits >
void SList< E, ETraits >::MoveToTail( POSITION pos )
{
    SENSURE( pos != NULL );
    CNode* pNode = static_cast< CNode* >( pos );

    if( pNode == m_pTail )
    {
        // Already at the tail
        return;
    }

    if( pNode->m_pPrev == NULL )
    {
        SENSURE( pNode == m_pHead );
        m_pHead = pNode->m_pNext;
    }
    else
    {
        pNode->m_pPrev->m_pNext = pNode->m_pNext;
    }

    pNode->m_pNext->m_pPrev = pNode->m_pPrev;

    m_pTail->m_pNext = pNode;
    pNode->m_pPrev = m_pTail;
    pNode->m_pNext = NULL;
    m_pTail = pNode;
}

template< typename E, class ETraits >
void SList< E, ETraits >::SwapElements( POSITION pos1, POSITION pos2 ) throw()
{
    ASSERT( pos1 != NULL );
    ASSERT( pos2 != NULL );

    if( pos1 == pos2 )
    {
        // Nothing to do
        return;
    }

    CNode* pNode1 = static_cast< CNode* >( pos1 );
    CNode* pNode2 = static_cast< CNode* >( pos2 );
    if( pNode2->m_pNext == pNode1 )
    {
        // Swap pNode2 and pNode1 so that the next case works
        CNode* pNodeTemp = pNode1;
        pNode1 = pNode2;
        pNode2 = pNodeTemp;
    }
    if( pNode1->m_pNext == pNode2 )
    {
        // Node1 and Node2 are adjacent
        pNode2->m_pPrev = pNode1->m_pPrev;
        if( pNode1->m_pPrev != NULL )
        {
            pNode1->m_pPrev->m_pNext = pNode2;
        }
        else
        {
            SASSUME( m_pHead == pNode1 );
            m_pHead = pNode2;
        }
        pNode1->m_pNext = pNode2->m_pNext;
        if( pNode2->m_pNext != NULL )
        {
            pNode2->m_pNext->m_pPrev = pNode1;
        }
        else
        {
            SASSUME( m_pTail == pNode2 );
            m_pTail = pNode1;
        }
        pNode2->m_pNext = pNode1;
        pNode1->m_pPrev = pNode2;
    }
    else
    {
        // The two nodes are not adjacent
        CNode* pNodeTemp;

        pNodeTemp = pNode1->m_pPrev;
        pNode1->m_pPrev = pNode2->m_pPrev;
        pNode2->m_pPrev = pNodeTemp;

        pNodeTemp = pNode1->m_pNext;
        pNode1->m_pNext = pNode2->m_pNext;
        pNode2->m_pNext = pNodeTemp;

        if( pNode1->m_pNext != NULL )
        {
            pNode1->m_pNext->m_pPrev = pNode1;
        }
        else
        {
            SASSUME( m_pTail == pNode2 );
            m_pTail = pNode1;
        }
        if( pNode1->m_pPrev != NULL )
        {
            pNode1->m_pPrev->m_pNext = pNode1;
        }
        else
        {
            SASSUME( m_pHead == pNode2 );
            m_pHead = pNode1;
        }
        if( pNode2->m_pNext != NULL )
        {
            pNode2->m_pNext->m_pPrev = pNode2;
        }
        else
        {
            SASSUME( m_pTail == pNode1 );
            m_pTail = pNode2;
        }
        if( pNode2->m_pPrev != NULL )
        {
            pNode2->m_pPrev->m_pNext = pNode2;
        }
        else
        {
            SASSUME( m_pHead == pNode1 );
            m_pHead = pNode2;
        }
    }
}

template< typename E, class ETraits >
POSITION SList< E, ETraits >::Find( INARGTYPE element, POSITION posStartAfter ) const throw()
{
    SASSERT_VALID(this);

    CNode* pNode = (CNode*)posStartAfter;
    if( pNode == NULL )
    {
        pNode = m_pHead;  // start at head
    }
    else
    {
        pNode = pNode->m_pNext;  // start after the one specified
    }

    for( ; pNode != NULL; pNode = pNode->m_pNext )
    {
        if( ETraits::CompareElements( pNode->m_element, element ) )
            return( POSITION( pNode ) );
    }

    return( NULL );
}

#ifdef _DEBUG
template< typename E, class ETraits >
void SList< E, ETraits >::AssertValid() const
{
    if( IsEmpty() )
    {
        // empty list
        SASSUME(m_pHead == NULL);
        SASSUME(m_pTail == NULL);
    }
    else
    {
        // non-empty list
    }
}
#endif

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class SMap
{
public:
    typedef typename KTraits::INARGTYPE KINARGTYPE;
    typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
    typedef typename VTraits::INARGTYPE VINARGTYPE;
    typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

    class CPair :
        public __POSITION
    {
    protected:
        CPair( KINARGTYPE key ) :
            m_key( key )
        {
        }

    public:
        const K m_key;
        V m_value;
    };

private:
    class CNode :
        public CPair
    {
    public:
        CNode( KINARGTYPE key, UINT nHash ) :
            CPair( key ),
            m_nHash( nHash )
        {
        }

    public:
        UINT GetHash() const throw()
        {
            return( m_nHash );
        }

    public:
        CNode* m_pNext;
        UINT m_nHash;
    };

public:
    SMap( UINT nBins = 17, float fOptimalLoad = 0.75f,
             float fLoThreshold = 0.25f, float fHiThreshold = 2.25f, UINT nBlockSize = 10 ) throw();

    size_t GetCount() const throw();
    bool IsEmpty() const throw();

    bool Lookup( KINARGTYPE key, VOUTARGTYPE value ) const;
    const CPair* Lookup( KINARGTYPE key ) const throw();
    CPair* Lookup( KINARGTYPE key ) throw();
    V& operator[]( KINARGTYPE key ) throw(...);

    POSITION SetAt( KINARGTYPE key, VINARGTYPE value );
    void SetValueAt( POSITION pos, VINARGTYPE value );

    bool RemoveKey( KINARGTYPE key ) throw();
    void RemoveAll();
    void RemoveAtPos( POSITION pos ) throw();

    POSITION GetStartPosition() const throw();
    void GetNextAssoc( POSITION& pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
    const CPair* GetNext( POSITION& pos ) const throw();
    CPair* GetNext( POSITION& pos ) throw();
    const K& GetNextKey( POSITION& pos ) const;
    const V& GetNextValue( POSITION& pos ) const;
    V& GetNextValue( POSITION& pos );
    void GetAt( POSITION pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
    CPair* GetAt( POSITION pos ) throw();
    const CPair* GetAt( POSITION pos ) const throw();
    const K& GetKeyAt( POSITION pos ) const;
    const V& GetValueAt( POSITION pos ) const;
    V& GetValueAt( POSITION pos );

    UINT GetHashTableSize() const throw();
    bool InitHashTable( UINT nBins, bool bAllocNow = true );
    void EnableAutoRehash() throw();
    void DisableAutoRehash() throw();
    void Rehash( UINT nBins = 0 );
    void SetOptimalLoad( float fOptimalLoad, float fLoThreshold, float fHiThreshold,
                         bool bRehashNow = false );

#ifdef _DEBUG
    void AssertValid() const;
#endif  // _DEBUG

    // Implementation
private:
    CNode** m_ppBins;
    size_t m_nElements;
    UINT m_nBins;
    float m_fOptimalLoad;
    float m_fLoThreshold;
    float m_fHiThreshold;
    size_t m_nHiRehashThreshold;
    size_t m_nLoRehashThreshold;
    ULONG m_nLockCount;
    UINT m_nBlockSize;
    SPlex* m_pBlocks;
    CNode* m_pFree;

private:
    bool IsLocked() const throw();
    UINT PickSize( size_t nElements ) const throw();
    CNode* NewNode( KINARGTYPE key, UINT iBin, UINT nHash );
    void FreeNode( CNode* pNode );
    void FreePlexes() throw();
    CNode* GetNode( KINARGTYPE key, UINT& iBin, UINT& nHash, CNode*& pPrev ) const throw();
    CNode* CreateNode( KINARGTYPE key, UINT iBin, UINT nHash ) throw(...);
    void RemoveNode( CNode* pNode, CNode* pPrev ) throw();
    CNode* FindNextNode( CNode* pNode ) const throw();
    void UpdateRehashThresholds() throw();

public:
    ~SMap() throw();

private:
    // Private to prevent use
    SMap( const SMap& ) throw();
    SMap& operator=( const SMap& ) throw();
};


template< typename K, typename V, class KTraits, class VTraits >
inline size_t SMap< K, V, KTraits, VTraits >::GetCount() const throw()
{
    return( m_nElements );
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool SMap< K, V, KTraits, VTraits >::IsEmpty() const throw()
{
    return( m_nElements == 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& SMap< K, V, KTraits, VTraits >::operator[]( KINARGTYPE key ) throw(...)
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );
    if( pNode == NULL )
    {
        pNode = CreateNode( key, iBin, nHash );
    }

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline UINT SMap< K, V, KTraits, VTraits >::GetHashTableSize() const throw()
{
    return( m_nBins );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void SMap< K, V, KTraits, VTraits >::GetAt( POSITION pos, KOUTARGTYPE key, VOUTARGTYPE value ) const
{
    SENSURE( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );

    key = pNode->m_key;
    value = pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
inline typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::GetAt( POSITION pos ) throw()
{
    ASSERT( pos != NULL );

    return( static_cast< CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::GetAt( POSITION pos ) const throw()
{
    ASSERT( pos != NULL );

    return( static_cast< const CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const K& SMap< K, V, KTraits, VTraits >::GetKeyAt( POSITION pos ) const
{
    SENSURE( pos != NULL );

    CNode* pNode = (CNode*)pos;

    return( pNode->m_key );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const V& SMap< K, V, KTraits, VTraits >::GetValueAt( POSITION pos ) const
{
    SENSURE( pos != NULL );

    CNode* pNode = (CNode*)pos;

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& SMap< K, V, KTraits, VTraits >::GetValueAt( POSITION pos )
{
    SENSURE( pos != NULL );

    CNode* pNode = (CNode*)pos;

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void SMap< K, V, KTraits, VTraits >::DisableAutoRehash() throw()
{
    m_nLockCount++;
}

template< typename K, typename V, class KTraits, class VTraits >
inline void SMap< K, V, KTraits, VTraits >::EnableAutoRehash() throw()
{
    SASSUME( m_nLockCount > 0 );
    m_nLockCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool SMap< K, V, KTraits, VTraits >::IsLocked() const throw()
{
    return( m_nLockCount != 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
UINT SMap< K, V, KTraits, VTraits >::PickSize( size_t nElements ) const throw()
{
    // List of primes such that s_anPrimes[i] is the smallest prime greater than 2^(5+i/3)
    static const UINT s_anPrimes[] =
    {
        17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821,
        1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331,
        13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033,
        131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563,
        832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319,
        5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
    };

    size_t nBins = (size_t)(nElements/m_fOptimalLoad);
    UINT nBinsEstimate = UINT(  UINT_MAX < nBins ? UINT_MAX : nBins );

    // Find the smallest prime greater than our estimate
    int iPrime = 0;
    while( nBinsEstimate > s_anPrimes[iPrime] )
    {
        iPrime++;
    }

    if( s_anPrimes[iPrime] == UINT_MAX )
    {
        return( nBinsEstimate );
    }
    else
    {
        return( s_anPrimes[iPrime] );
    }
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CNode* SMap< K, V, KTraits, VTraits >::CreateNode(
    KINARGTYPE key, UINT iBin, UINT nHash ) throw(...)
{
    CNode* pNode;

    if( m_ppBins == NULL )
    {
        bool bSuccess;

        bSuccess = InitHashTable( m_nBins );
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
    }

    pNode = NewNode( key, iBin, nHash );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION SMap< K, V, KTraits, VTraits >::GetStartPosition() const throw()
{
    if( IsEmpty() )
    {
        return( NULL );
    }

    for( UINT iBin = 0; iBin < m_nBins; iBin++ )
    {
        if( m_ppBins[iBin] != NULL )
        {
            return( POSITION( m_ppBins[iBin] ) );
        }
    }
    ASSERT( false );

    return( NULL );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION SMap< K, V, KTraits, VTraits >::SetAt( KINARGTYPE key, VINARGTYPE value )
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );
    if( pNode == NULL )
    {
        pNode = CreateNode( key, iBin, nHash );
        _STRY
        {
            pNode->m_value = value;
        }
        _SCATCHALL()
        {
            RemoveAtPos( POSITION( pNode ) );
        }
    }
    else
    {
        pNode->m_value = value;
    }

    return( POSITION( pNode ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::SetValueAt( POSITION pos, VINARGTYPE value )
{
    ASSERT( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );

    pNode->m_value = value;
}

template< typename K, typename V, class KTraits, class VTraits >
SMap< K, V, KTraits, VTraits >::SMap( UINT nBins, float fOptimalLoad,
        float fLoThreshold, float fHiThreshold, UINT nBlockSize ) throw() :
    m_ppBins( NULL ),
    m_nBins( nBins ),
    m_nElements( 0 ),
    m_nLockCount( 0 ),  // Start unlocked
    m_fOptimalLoad( fOptimalLoad ),
    m_fLoThreshold( fLoThreshold ),
    m_fHiThreshold( fHiThreshold ),
    m_nHiRehashThreshold( UINT_MAX ),
    m_nLoRehashThreshold( 0 ),
    m_pBlocks( NULL ),
    m_pFree( NULL ),
    m_nBlockSize( nBlockSize )
{
    ASSERT( nBins > 0 );
    ASSERT( nBlockSize > 0 );

    SetOptimalLoad( fOptimalLoad, fLoThreshold, fHiThreshold, false );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::SetOptimalLoad( float fOptimalLoad, float fLoThreshold,
        float fHiThreshold, bool bRehashNow )
{
    ASSERT( fOptimalLoad > 0 );
    ASSERT( (fLoThreshold >= 0) && (fLoThreshold < fOptimalLoad) );
    ASSERT( fHiThreshold > fOptimalLoad );

    m_fOptimalLoad = fOptimalLoad;
    m_fLoThreshold = fLoThreshold;
    m_fHiThreshold = fHiThreshold;

    UpdateRehashThresholds();

    if( bRehashNow && ((m_nElements > m_nHiRehashThreshold) ||
                       (m_nElements < m_nLoRehashThreshold)) )
    {
        Rehash( PickSize( m_nElements ) );
    }
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::UpdateRehashThresholds() throw()
{
    m_nHiRehashThreshold = size_t( m_fHiThreshold*m_nBins );
    m_nLoRehashThreshold = size_t( m_fLoThreshold*m_nBins );
    if( m_nLoRehashThreshold < 17 )
    {
        m_nLoRehashThreshold = 0;
    }
}

template< typename K, typename V, class KTraits, class VTraits >
bool SMap< K, V, KTraits, VTraits >::InitHashTable( UINT nBins, bool bAllocNow )
{
    SASSUME( m_nElements == 0 );
    ASSERT( nBins > 0 );

    if( m_ppBins != NULL )
    {
        delete[] m_ppBins;
        m_ppBins = NULL;
    }

    if( bAllocNow )
    {
        //hjx            ATLTRY( m_ppBins = new CNode*[nBins] );
        m_ppBins = new CNode*[nBins];
        if( m_ppBins == NULL )
        {
            return false;
        }

        SENSURE( UINT_MAX / sizeof( CNode* ) >= nBins );
        memset( m_ppBins, 0, sizeof( CNode* )*nBins );
    }
    m_nBins = nBins;

    UpdateRehashThresholds();

    return true;
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::RemoveAll()
{
    DisableAutoRehash();
    if( m_ppBins != NULL )
    {
        for( UINT iBin = 0; iBin < m_nBins; iBin++ )
        {
            CNode* pNext;

            pNext = m_ppBins[iBin];
            while( pNext != NULL )
            {
                CNode* pKill;

                pKill = pNext;
                pNext = pNext->m_pNext;
                FreeNode( pKill );
            }
        }
    }

    delete[] m_ppBins;
    m_ppBins = NULL;
    m_nElements = 0;

    if( !IsLocked() )
    {
        InitHashTable( PickSize( m_nElements ), false );
    }

    FreePlexes();
    EnableAutoRehash();
}

template< typename K, typename V, class KTraits, class VTraits >
SMap< K, V, KTraits, VTraits >::~SMap() throw()
{
    _STRY
    {
        RemoveAll();
    }
    _SCATCHALL()
    {
        ASSERT(false);
    }
}

#pragma push_macro("new")
#undef new

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CNode* SMap< K, V, KTraits, VTraits >::NewNode(
    KINARGTYPE key, UINT iBin, UINT nHash )
{
    CNode* pNewNode;

    if( m_pFree == NULL )
    {
        SPlex* pPlex;
        CNode* pNode;

        pPlex = SPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
        if( pPlex == NULL )
        {
            SThrow( E_OUTOFMEMORY );
        }
        pNode = (CNode*)pPlex->data();
        pNode += m_nBlockSize-1;
        for( int iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
        {
            pNode->m_pNext = m_pFree;
            m_pFree = pNode;
            pNode--;
        }
    }
    SENSURE(m_pFree != NULL );
    pNewNode = m_pFree;
    m_pFree = pNewNode->m_pNext;

    _STRY
    {
        ::new( pNewNode ) CNode( key, nHash );
    }
    _SCATCHALL()
    {
        pNewNode->m_pNext = m_pFree;
        m_pFree = pNewNode;

    }
    m_nElements++;

    pNewNode->m_pNext = m_ppBins[iBin];
    m_ppBins[iBin] = pNewNode;

    if( (m_nElements > m_nHiRehashThreshold) && !IsLocked() )
    {
        Rehash( PickSize( m_nElements ) );
    }

    return( pNewNode );
}

#pragma pop_macro("new")

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::FreeNode( CNode* pNode )
{
    SENSURE( pNode != NULL );

    pNode->~CNode();
    pNode->m_pNext = m_pFree;
    m_pFree = pNode;

    SASSUME( m_nElements > 0 );
    m_nElements--;

    if( (m_nElements < m_nLoRehashThreshold) && !IsLocked() )
    {
        Rehash( PickSize( m_nElements ) );
    }

    if( m_nElements == 0 )
    {
        FreePlexes();
    }
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::FreePlexes() throw()
{
    m_pFree = NULL;
    if( m_pBlocks != NULL )
    {
        m_pBlocks->FreeDataChain();
        m_pBlocks = NULL;
    }
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CNode* SMap< K, V, KTraits, VTraits >::GetNode(
    KINARGTYPE key, UINT& iBin, UINT& nHash, CNode*& pPrev ) const throw()
{
    CNode* pFollow;

    nHash = KTraits::Hash( key );
    iBin = nHash%m_nBins;

    if( m_ppBins == NULL )
    {
        return( NULL );
    }

    pFollow = NULL;
    pPrev = NULL;
    for( CNode* pNode = m_ppBins[iBin]; pNode != NULL; pNode = pNode->m_pNext )
    {
        if( (pNode->GetHash() == nHash) && KTraits::CompareElements( pNode->m_key, key ) )
        {
            pPrev = pFollow;
            return( pNode );
        }
        pFollow = pNode;
    }

    return( NULL );
}

template< typename K, typename V, class KTraits, class VTraits >
bool SMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key, VOUTARGTYPE value ) const
{
    UINT iBin;
    UINT nHash;
    CNode* pNode;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );
    if( pNode == NULL )
    {
        return( false );
    }

    value = pNode->m_value;

    return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
const typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key ) const throw()
{
    UINT iBin;
    UINT nHash;
    CNode* pNode;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key ) throw()
{
    UINT iBin;
    UINT nHash;
    CNode* pNode;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
bool SMap< K, V, KTraits, VTraits >::RemoveKey( KINARGTYPE key ) throw()
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    pPrev = NULL;
    pNode = GetNode( key, iBin, nHash, pPrev );
    if( pNode == NULL )
    {
        return( false );
    }

    RemoveNode( pNode, pPrev );

    return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::RemoveNode( CNode* pNode, CNode* pPrev )
{
    SENSURE( pNode != NULL );

    UINT iBin = pNode->GetHash() % m_nBins;

    if( pPrev == NULL )
    {
        SASSUME( m_ppBins[iBin] == pNode );
        m_ppBins[iBin] = pNode->m_pNext;
    }
    else
    {
        ASSERT( pPrev->m_pNext == pNode );
        pPrev->m_pNext = pNode->m_pNext;
    }
    FreeNode( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::RemoveAtPos( POSITION pos )
{
    SENSURE( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );
    CNode* pPrev = NULL;
    UINT iBin = pNode->GetHash() % m_nBins;

    SASSUME( m_ppBins[iBin] != NULL );
    if( pNode == m_ppBins[iBin] )
    {
        pPrev = NULL;
    }
    else
    {
        pPrev = m_ppBins[iBin];
        while( pPrev->m_pNext != pNode )
        {
            pPrev = pPrev->m_pNext;
            ASSERT( pPrev != NULL );
        }
    }
    RemoveNode( pNode, pPrev );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::Rehash( UINT nBins )
{
    CNode** ppBins = NULL;

    if( nBins == 0 )
    {
        nBins = PickSize( m_nElements );
    }

    if( nBins == m_nBins )
    {
        return;
    }

    //hjx         ATLTRACE(atlTraceMap, 2, _T("Rehash: %u bins\n"), nBins );

    if( m_ppBins == NULL )
    {
        // Just set the new number of bins
        InitHashTable( nBins, false );
        return;
    }

    //hjx        ATLTRY(ppBins = new CNode*[nBins]);
    ppBins = new CNode*[nBins];
    if (ppBins == NULL)
    {
        SThrow( E_OUTOFMEMORY );
    }

    SENSURE( UINT_MAX / sizeof( CNode* ) >= nBins );
    memset( ppBins, 0, nBins*sizeof( CNode* ) );

    // Nothing gets copied.  We just rewire the old nodes
    // into the new bins.
    for( UINT iSrcBin = 0; iSrcBin < m_nBins; iSrcBin++ )
    {
        CNode* pNode;

        pNode = m_ppBins[iSrcBin];
        while( pNode != NULL )
        {
            CNode* pNext;
            UINT iDestBin;

            pNext = pNode->m_pNext;  // Save so we don't trash it
            iDestBin = pNode->GetHash()%nBins;
            pNode->m_pNext = ppBins[iDestBin];
            ppBins[iDestBin] = pNode;

            pNode = pNext;
        }
    }

    delete[] m_ppBins;
    m_ppBins = ppBins;
    m_nBins = nBins;

    UpdateRehashThresholds();
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::GetNextAssoc( POSITION& pos, KOUTARGTYPE key,
        VOUTARGTYPE value ) const
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    SENSURE( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = POSITION( pNext );
    key = pNode->m_key;
    value = pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
const typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::GetNext( POSITION& pos ) const throw()
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    ASSERT( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = POSITION( pNext );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::GetNext(
    POSITION& pos ) throw()
{
    SASSUME( m_ppBins != NULL );
    ASSERT( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );
    CNode* pNext = FindNextNode( pNode );

    pos = POSITION( pNext );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
const K& SMap< K, V, KTraits, VTraits >::GetNextKey( POSITION& pos ) const
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    SENSURE( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = POSITION( pNext );

    return( pNode->m_key );
}

template< typename K, typename V, class KTraits, class VTraits >
const V& SMap< K, V, KTraits, VTraits >::GetNextValue( POSITION& pos ) const
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    SENSURE( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = POSITION( pNext );

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
V& SMap< K, V, KTraits, VTraits >::GetNextValue( POSITION& pos )
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    SENSURE( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = POSITION( pNext );

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CNode* SMap< K, V, KTraits, VTraits >::FindNextNode( CNode* pNode ) const throw()
{
    CNode* pNext;

    if(pNode == NULL)
    {
        ASSERT(FALSE);
        return NULL;
    }

    if( pNode->m_pNext != NULL )
    {
        pNext = pNode->m_pNext;
    }
    else
    {
        UINT iBin;

        pNext = NULL;
        iBin = (pNode->GetHash()%m_nBins)+1;
        while( (pNext == NULL) && (iBin < m_nBins) )
        {
            if( m_ppBins[iBin] != NULL )
            {
                pNext = m_ppBins[iBin];
            }

            iBin++;
        }
    }

    return( pNext );
}

#ifdef _DEBUG
template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::AssertValid() const
{
    SASSUME( m_nBins > 0 );
    // non-empty map should have hash table
    ASSERT( IsEmpty() || (m_ppBins != NULL) );
}
#endif

}; // namespace ATL
#pragma pack(pop,_ATL_PACKING)

#pragma warning(pop)

#endif  // __SOUICOLL_H__