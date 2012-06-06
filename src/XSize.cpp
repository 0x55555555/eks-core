#include "XSize"
#include "cmath"
#include "XVector3D"

XSize::XSize() : _x( 0 ), _y( 0 ), _z( 0 )
    {
    }

XSize::XSize( XVector3D vec ) : _x( vec.x() ), _y( vec.y() ), _z( vec.z() )
    {
    }

XSize::XSize( xReal x, xReal y, xReal z ) : _x( x ), _y( y ), _z( z )
    {
    }

XVector3D XSize::toVector3D()
    {
    return XVector3D( _x, _y, _z );
    }

bool XSize::isValid() const
    {
    return _x >= 0 && _y >= 0 && _z >= 0;
    }

bool XSize::isEmpty() const
    {
    return qFuzzyCompare( _x, 0 ) || qFuzzyCompare( _y, 0 ) || qFuzzyCompare( _z, 0 );
    }

XSize XSize::toValid() const
    {
    return XSize( fabs( _x ), fabs( _y ), fabs( _z ) );
    }

XVector3D operator+( const XVector3D &vec, const XSize &siz )
    {
    return XVector3D( vec.x() + siz.x(), vec.y() + siz.y(), vec.z() + siz.z() );
    }

XSize &operator*=( XSize &size, float in )
    {
    size.setX( size.x() * in );
    size.setY( size.y() * in );
    size.setZ( size.z() * in );
    return size;
    }
