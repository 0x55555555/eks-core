#ifndef XCURVE_H
#define XCURVE_H

#include "XGlobal.h"
#include "Containers/XVector.h"
#include "Math/XMathVector.h"
#include "Math/XMathMatrix.h"

namespace Eks
{

//--------------------------------------------------------------------------------------------------------------------------------------------
//                                                      AbstractCurve Implementation
//--------------------------------------------------------------------------------------------------------------------------------------------

template <typename RETURN>
class AbstractCurve
  {
public:
  virtual RETURN sample( Real t ) const = 0;
  Real minimumT() const { return 0.0; }
  Real maximumT() const { return 1.0; }
  };

//--------------------------------------------------------------------------------------------------------------------------------------------
//                                                      GenericCurveSegment Definition
//--------------------------------------------------------------------------------------------------------------------------------------------

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE, 1> &), Matrix<Real, SIZE, 1> (*FROMRETURN)(const RETURN &)>
class GenericCurveSegment : public AbstractCurve<RETURN>
  {
public:
  enum BasisMode { Bezier, Hermite };

  GenericCurveSegment( BasisMode=Bezier );

  void setGeometricConstraints( RETURN one, RETURN two, RETURN thr, RETURN fou );
  void setGeometricConstraint( int, RETURN one );

  RETURN geometricConstraint( int ) const;

  void setBasis( BasisMode );
  void setBasis( const Matrix4x4 & );

  RETURN sample( Real t ) const;

private:
  Matrix <Real, 4, 4> _basis;
  Matrix <Real, SIZE, 4> _geom;
  Matrix <Real, SIZE, 4> _preMult;
  };

//--------------------------------------------------------------------------------------------------------------------------------------------
//                                                      GenericCurve Definition
//--------------------------------------------------------------------------------------------------------------------------------------------

template <int SIZE,
          typename RETURN,
          RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &),
          Matrix<Real, SIZE, 1> (*FROMRETURN)(const RETURN &)>
class GenericCurve : public AbstractCurve<RETURN>
  {
public:
  Real minimumT() const;
  Real maximumT() const;
  RETURN sample( Real t ) const;

protected:
  const Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > &segments() const;
  void setSegments( const Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > & );

private:
  Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > _segments;
  };

//--------------------------------------------------------------------------------------------------------------------------------------------
//                                                      KBSpline Definition
//--------------------------------------------------------------------------------------------------------------------------------------------

template <int SIZE,
          typename RETURN,
          RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &),
          Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
class KBSpline : public GenericCurve<SIZE,RETURN,TORETURN,FROMRETURN>
  {
public:
  KBSpline( uint );

  uint size() const;
  void resize( uint );

  void setPosition( uint, RETURN );
  void setT( uint, Real );
  void setC( uint, Real );
  void setB( uint, Real );

private:
  void updateSegment(Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > &, uint segment);
  struct tcb { Real t, c, b; };
  Vector <tcb> _tcb;
  };

//--------------------------------------------------------------------------------------------------------------------------------------------
//                                                      GenericCurveSegment Implementation
//--------------------------------------------------------------------------------------------------------------------------------------------

template <int SIZE,
          typename RETURN,
          RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &),
          Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN>::GenericCurveSegment( BasisMode mode )
  {
  _geom.fill(0.0);
  setBasis( mode );
  }

template <int SIZE,
          typename RETURN,
          RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &),
          Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN>::setGeometricConstraints( RETURN oneGeneric,
                                                                                     RETURN twoGeneric,
                                                                                     RETURN thrGeneric,
                                                                                     RETURN fouGeneric )
  {
  Matrix<Real, SIZE,1> one( FROMRETURN( oneGeneric ) );
  Matrix<Real, SIZE,1> two( FROMRETURN( twoGeneric ) );
  Matrix<Real, SIZE,1> thr( FROMRETURN( thrGeneric ) );
  Matrix<Real, SIZE,1> fou( FROMRETURN( fouGeneric ) );
  for( int x=0; x<SIZE; x++ )
    {
    _geom(0,x) = one(0,x);

    _geom(1,x) = two(0,x);

    _geom(2,x) = thr(0,x);

    _geom(3,x) = fou(0,x);
    }
  _preMult = _basis * _geom;
  }

template <int SIZE,
          typename RETURN,
          RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &),
          Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN>::setGeometricConstraint( int geo, RETURN constraint )
  {
  Matrix<Real, SIZE,1> one( FROMRETURN( constraint ) );
  for( int x=0; x<SIZE; x++ )
    {
    _geom(geo,x) = one(0,x);
    }
  _preMult = _basis * _geom;
  }

template <int SIZE,
          typename RETURN,
          RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &),
          Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
RETURN GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN>::geometricConstraint( int geo ) const
  {
  Matrix<Real, SIZE,1> ret;
  for( int x=0; x<SIZE; x++ )
    {
    ret(0,x) = _geom(geo,x);
    }
  return TORETURN( ret );
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN>::setBasis( const Matrix4x4 &in )
  {
  _basis = in;
  _preMult = _basis * _geom;
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN>::setBasis( BasisMode mode )
  {
  if( mode == Bezier )
    {
    Matrix4x4 mat;
    mat.row(0) = Eks::Vector4D( -1, 3, -3, 1 );
    mat.row(1) = Eks::Vector4D( 3, -6, 3, 0 );
    mat.row(2) = Eks::Vector4D( -3, 3, 0, 0 );
    mat.row(3) = Eks::Vector4D( 1, 0, 0, 0 );
    setBasis( mat );
    }
  else if( mode == Hermite )
    {
    Matrix4x4 mat;
    mat.row(0) = Eks::Vector4D( 2, -2, 1, 1 );
    mat.row(1) = Eks::Vector4D( -3, 3, -2, -1 );
    mat.row(2) = Eks::Vector4D( 0, 0, 1, 0 );
    mat.row(3) = Eks::Vector4D( 1, 0, 0, 0 );
    setBasis( mat );
    }
  else
    {
    qWarning() << "Unrecognised Curve Mode";
    }
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
RETURN GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN>::sample( Real t ) const
  {
  float tMult( t );
  Matrix<Real, 4, 1> tMat;
  tMat(0,3) = 1;
  tMat(0,2) = tMult;
  tMult *= t;
  tMat(0,1) = tMult;
  tMult *= t;
  tMat(0,0) = tMult;

  return TORETURN( tMat * _preMult );
  }

//--------------------------------------------------------------------------------------------------------------------------------------------
//                                                      GenericCurve Implementation
//--------------------------------------------------------------------------------------------------------------------------------------------

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
Real GenericCurve<SIZE,RETURN,TORETURN,FROMRETURN>::minimumT() const
  {
  return 0.0;
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
Real GenericCurve<SIZE,RETURN,TORETURN,FROMRETURN>::maximumT() const
  {
  return _segments.size();
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
RETURN GenericCurve<SIZE,RETURN,TORETURN,FROMRETURN>::sample( Real t ) const
  {
  if( t >= 0 && t < _segments.size() )
    {
    return _segments[(int)t].sample( t - (int)t );
    }
  else if( !_segments.isEmpty() )
    {
    if( t <= 0 )
      {
      return _segments.front().sample( 0.0 );
      }
    else
      {
      return _segments.back().sample( 1.0 );
      }
    }

  Matrix<Real, SIZE,1> ret;
  ret.fill( 0.0 );
  return TORETURN( ret );
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
const Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > &GenericCurve<SIZE,RETURN,TORETURN,FROMRETURN>::segments() const
  {
  return _segments;
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void GenericCurve<SIZE,RETURN,TORETURN,FROMRETURN>::setSegments( const Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > &seg )
  {
  _segments = seg;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------
//                                                      KBSpline Implementation
//--------------------------------------------------------------------------------------------------------------------------------------------

template <int SIZE,
          typename RETURN,
          RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &),
          Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
KBSpline<SIZE,RETURN,TORETURN,FROMRETURN>::KBSpline( uint size )
  {
  resize( size );
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
uint KBSpline<SIZE,RETURN,TORETURN,FROMRETURN>::size() const
  {
  return this->segments().size();
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void KBSpline<SIZE,RETURN,TORETURN,FROMRETURN>::resize( uint x )
  {
  int size = x - 1;
  Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > segs( this->segments() );

  while( segs.size() > size && segs.size() > 0 )
    {
    segs.removeLast();
    }
  while( segs.size() < size )
    {
    segs << GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN>( GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN>::Hermite );
    }

  while( (uint)_tcb.size() > x && _tcb.size() > 0 )
    {
    _tcb.removeLast();
    }
  while( (uint)_tcb.size() < x )
    {
    tcb newTCB;
    newTCB.t = 0.0;
    newTCB.c = 0.0;
    newTCB.b = 0.0;
    _tcb << newTCB;
    }

  setSegments( segs );
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void KBSpline<SIZE,RETURN,TORETURN,FROMRETURN>::setPosition( uint index, RETURN pos )
  {
  Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > segs( this->segments() );
  if( index < size() )
    {
    segs[index].setGeometricConstraint( 0, pos );
    }
  if( index > 0 )
    {
    segs[index-1].setGeometricConstraint( 1, pos );
    }
  updateSegment( segs, index-1 );
  updateSegment( segs, index );
  updateSegment( segs, index+1 );
  setSegments( segs );
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void KBSpline<SIZE,RETURN,TORETURN,FROMRETURN>::setT( uint index, Real t )
  {
  _tcb[index].t = t;
  Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > segs( this->segments() );
  updateSegment( segs, index );
  setSegments( segs );
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void KBSpline<SIZE,RETURN,TORETURN,FROMRETURN>::setC( uint index, Real c )
  {
  _tcb[index].c = c;
  Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > segs( this->segments() );
  updateSegment( segs, index );
  setSegments( segs );
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void KBSpline<SIZE,RETURN,TORETURN,FROMRETURN>::setB( uint index, Real b )
  {
  _tcb[index].b = b;
  Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > segs( this->segments() );
  updateSegment( segs, index );
  setSegments( segs );
  }

template <int SIZE, typename RETURN, RETURN (*TORETURN)(const Matrix<Real, SIZE,1> &), Matrix<Real, SIZE,1> (*FROMRETURN)(const RETURN &)>
void KBSpline<SIZE,RETURN,TORETURN,FROMRETURN>::updateSegment( Vector <GenericCurveSegment<SIZE,RETURN,TORETURN,FROMRETURN> > &seg, uint index )
  {
  if( index > (uint)seg.size() )
    {
    return;
    }
  if( index == (uint)seg.size() )
    {
    seg.back().setGeometricConstraint(3,Eks::Vector3D(0,0,0));
    return;
    }
  else if( index == 0 )
    {
    seg.front().setGeometricConstraint(2,Eks::Vector3D(0,0,0));
    return;
    }

  int prevSeg( (int)index - 1 );
  if( (int)index < seg.size() && prevSeg >= 0 )
    {
    RETURN pM1( seg[prevSeg].geometricConstraint(0) );
    RETURN p ( seg[prevSeg].geometricConstraint(1) );
    RETURN pP1( seg[index].geometricConstraint(1) );

    tcb pointTCB( _tcb[index] );

    RETURN source( ( ( 0.5 * (1-pointTCB.t)*(1-pointTCB.c)*(1+pointTCB.b) ) * ( p - pM1 ) ) +
                   ( ( 0.5 * (1-pointTCB.t)*(1+pointTCB.c)*(1-pointTCB.b) ) * ( pP1 - p ) ));

    RETURN dest( ( ( 0.5 * (1-pointTCB.t)*(1+pointTCB.c)*(1+pointTCB.b) ) * ( p - pM1 ) ) +
                 ( ( 0.5 * (1-pointTCB.t)*(1-pointTCB.c)*(1-pointTCB.b) ) * ( pP1 - p ) ));

    seg[prevSeg].setGeometricConstraint( 3, source );
    seg[index].setGeometricConstraint( 2, dest );
    }
  }

Vector3D toVector3D( const Matrix<Real, 3, 1> &vec )
  {
  return Eks::Vector3D( vec(0,0), vec(0,1), vec(0,2) );
  }

Matrix<Real, 3, 1> fromVector3D( const Vector3D &vec )
  {
  Matrix<Real, 3, 1> ret;
  ret(0,0) = vec.x();
  ret(0,1) = vec.y();
  ret(0,2) = vec.z();
  return ret;
  }

Vector2D toVector2D( const Matrix<Real, 2, 1>& vec )
  {
  return Vector2D( vec(0,0), vec(0,1) );
  }

Matrix<Real, 2, 1> fromVector2D( const Vector2D& vec )
  {
  Matrix<Real, 2, 1> ret;
  ret(0,0) = vec.x();
  ret(0,1) = vec.y();
  return ret;
  }
Real toXReal( const Matrix<Real, 1, 1>& vec )
  {
  return vec(0,0);
  }
Matrix<Real, 1, 1> fromXReal( const Real& vec )
  {
  Matrix<Real, 1, 1> ret;
  ret(0,0) = vec;
  return ret;
  }

typedef KBSpline<1,Real,toXReal,fromXReal> KBSpline1D;
typedef KBSpline<2,Vector2D,toVector2D,fromVector2D> KBSpline2D;
typedef KBSpline<3,Vector3D,toVector3D,fromVector3D> KBSpline3D;

}

#endif // XCURVE_H
