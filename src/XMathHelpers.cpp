#include "Math/XMathHelpers.h"

namespace Eks
{

xint32 rand( xint32 min, xint32 max )
  {
  return min + ( ::rand() % (max-min+1) );
  }

Real randF( )
  {
  return ::rand() / (Real)RAND_MAX;
  }

Real randF( Real min, Real max )
  {
  return min + ( randF() * (max-min) );
  }

xuint32 factorial( xuint32 in )
  {
  // multiply 1 * 2 * 3 * ... * in
  unsigned int ret = 1;
  for( unsigned int x=2; x<=in; x++ )
    {
    ret *= x;
    }
  return ret;
  }

xuint32 binomialCoefficient( xuint32 n, xuint32 k )
  {
  // Find the coefficient of n and k...
  return factorial( n ) / ( ( factorial( n - k ) * factorial( k ) ) );
  }

}
