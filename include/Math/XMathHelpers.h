#ifndef XMATHHELPERS_H
#define XMATHHELPERS_H

#include "XGlobal.h"
#include "cstdlib"
#include "cmath"

#undef min
#undef max
#include "Math/Eigen/Core"

namespace Eks
{

#define xFuzzyCompare qFuzzyCompare

typedef float Real;
#define EKS_REAL_IS_DOUBLE 0

#if EKS_REAL_IS_DOUBLE
# define X_PI (3.14159265358979323846)
#else
# define X_PI (3.14159265358979323846f)
#endif

xint32 rand( xint32 min, xint32 max );
Real randF();
Real randF( Real min, Real max );
xuint32 factorial( xuint32 in );
xuint32 binomialCoefficient( xuint32 n, xuint32 k );


inline Real degreesToRadians(Real ang)
  {
  return (ang / 360.0f) * 2.0f * X_PI;
  }

inline Real radiansToDegrees(Real ang)
  {
  return (ang / 2.0f * X_PI) * 360.0f;
  }

inline Real cos(Real c)
  {
#if EKS_REAL_IS_DOUBLE
  return cos(c);
#else
  return cosf(c);
#endif
  }

inline Real tan(Real c)
  {
#if EKS_REAL_IS_DOUBLE
  return tan(c);
#else
  return tanf(c);
#endif
  }

inline Real sin(Real c)
  {
#if EKS_REAL_IS_DOUBLE
  return sin(c);
#else
  return sinf(c);
#endif
  }

template <typename T> T clamp( T in, T min, T max )
  {
  if( in < min )
    {
    return min;
    }
  else if( in > max )
    {
    return max;
    }
  return in;
  }

template <typename T> T lerp(T a, T b, Real factor)
  {
  return a + (factor * (b-a));
  }

}

#endif // XMATHHELPERS_H
