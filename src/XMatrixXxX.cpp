#include "XMatrix4x4"
#include "XGenericMatrix"

EKSCORE_EXPORT QTextStream &operator>>( QTextStream &str, XMatrix4x4 &mat )
    {
    for( unsigned int x=0; x<16; x++ )
        {
        double temp;
        str >> temp;
        mat.data()[x] = temp;
        }
    return str;
    }

EKSCORE_EXPORT QTextStream &operator<<( QTextStream &str, const XMatrix4x4 &mat )
    {
    for( unsigned int x=0; x<16; x++ )
        {
        str << mat.data()[x];
        if( x < 15 )
            {
            str << " ";
            }
        }
    return str;
    }


template <int N, int M, typename T>
        EKSCORE_EXPORT QTextStream &operator>>( QTextStream &str, XGenericMatrix<N, M, T> &mat )
    {
    for( int x=0; x<(N*M); x++ )
        {
        str << mat.data()[x];
        if( x < ((N*M)-1) )
            {
            str << " ";
            }
        }
    return str;
    }

template <int N, int M, typename T>
        EKSCORE_EXPORT QTextStream &operator<<( QTextStream &str, const XGenericMatrix<N, M, T> &mat )
    {
    for( unsigned int x=0; x<(N*M); x++ )
        {
        double temp;
        str >> temp;
        mat.data()[x] = temp;
        }
    return str;
    }
