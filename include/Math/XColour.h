#ifndef XCOLOUR_H
#define XCOLOUR_H

#include "XGlobal.h"
#include "Utilities/XAssert.h"
#include "Math/XMathVector.h"
#if X_QCOLOUR_COMPATIBILITY
# include "QtGui/QColor"
#endif

namespace Eks
{

#if X_QCOLOUR_COMPATIBILITY
typedef QColor XLDRColour;

inline XLDRColour operator*( const XLDRColour &a, const XLDRColour &b )
  {
  return XLDRColour( a.red() * b.red(), a.green() * b.green(), a.blue() * b.blue(), a.alpha() * b.alpha() );
  }

inline XLDRColour operator+( const XLDRColour &a, const XLDRColour &b )
  {
  return XLDRColour( Eks::clamp( a.red() + b.red(), 0, 255 ), Eks::clamp( a.green() + b.green(), 0, 255 ), Eks::clamp( a.blue() + b.blue(), 0, 255 ), Eks::clamp( a.alpha() + b.alpha(), 0, 255 ) );
  }

inline XLDRColour operator*( Real a, const XLDRColour &b )
  {
  return XLDRColour::fromRgbF( a * b.redF(), a * b.greenF(), a * b.blueF(), a * b.alphaF() );
  }
#endif

class Colour : public Vector4D
  {
public:
  Colour()
    {
    }

  Colour( float *in ) : Vector4D( in[0], in[1], in[2], in[3] )
    {
    }

  Colour( double *in ) : Vector4D( in[0], in[1], in[2], in[3] )
    {
    }

  Colour( Real r, Real g, Real b ) : Vector4D( r, g, b, 1.0 )
    {
    }

  Colour( Real r, Real g, Real b, Real a ) : Vector4D( r, g, b, a )
    {
    }

  Colour( const Vector3D &vec, float a = 1.0f ) : Vector4D( vec.x(), vec.y(), vec.z(), a )
    {
    }

  Colour( const Vector4D &vec ) : Vector4D( vec.x(), vec.y(), vec.z(), vec.w() )
    {
    }

#if X_QCOLOUR_COMPATIBILITY
  Colour( const XLDRColour &col ) : Vector4D( col.redF(), col.greenF(), col.blueF(), col.alphaF() )
    {
    }
#endif

  template <typename Derived>
  Colour(const Eigen::EigenBase<Derived> &oth) : Vector4D(oth)
    {
    }

#if X_QCOLOUR_COMPATIBILITY
  XLDRColour toLDRColour( ) const
    {
    return XLDRColour::fromRgbF( coeff(0), coeff(1), coeff(2), coeff(3) );
    }
#endif

  template <typename Derived>
  Colour &operator=(const Eigen::EigenBase<Derived> &oth)
    {
    Vector4D& ths(*this);
    ths = oth;
    return *this;
    }
  };

typedef Colour ColourTight;
typedef Eks::Vector3DTight ColourRGBTight;
xCompileTimeAssert(sizeof(Real) == sizeof(float));
xCompileTimeAssert(sizeof(Colour) == sizeof(float) * 4);

}


#if X_QT_INTEROP

Q_DECLARE_METATYPE(Eks::Colour);

#endif
#endif // XCOLOUR_H
