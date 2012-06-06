#include "XVector2D"
#include "XVector3D"
#include "XVector4D"
#include "QTextStream"

EKSCORE_EXPORT QTextStream &operator>>( QTextStream &str, XVector2D &vec )
    {
    double x, y;
    str >> x >> y;
    vec.setX( x );
    vec.setY( y );
    return str;
    }

EKSCORE_EXPORT QTextStream &operator<<( QTextStream &str, const XVector2D &vec )
    {
    str << vec.x() << " " << vec.y();
    return str;
    }

EKSCORE_EXPORT QTextStream &operator>>( QTextStream &str, XVector3D &vec )
    {
    double x, y, z;
    str >> x >> y >> z;
    vec.setX( x );
    vec.setY( y );
    vec.setZ( z );
    return str;
    }

EKSCORE_EXPORT QTextStream &operator<<( QTextStream &str, const XVector3D &vec )
    {
    str << vec.x() << " " << vec.y() << " " << vec.z();
    return str;
    }


EKSCORE_EXPORT QTextStream &operator>>( QTextStream &str, XVector4D &vec )
    {
    double x, y, z, w;
    str >> x >> y >> z >> w;
    vec.setX( x );
    vec.setY( y );
    vec.setZ( z );
    vec.setW( w );
    return str;
    }

EKSCORE_EXPORT QTextStream &operator<<( QTextStream &str, const XVector4D &vec )
    {
    str << vec.x() << " " << vec.y() << " " << vec.z() << " " << vec.w();
    return str;
    }
