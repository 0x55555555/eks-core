#ifndef XMATRIX4X4_H
#define XMATRIX4X4_H

#include "XGlobal.h"
#include "Math/XMathHelpers.h"
#include "QtCore/QDebug"

namespace Eks
{

using Eigen::Matrix;
typedef Eigen::Matrix<Real, 4, 4> Matrix4x4;
typedef Eigen::Matrix<Real, 3, 3> Matrix3x3;

}

template <typename Derived> QDebug operator <<(QDebug str, const Eigen::DenseBase <Derived> &data)
  {
  xsize cols = data.cols();
  xsize rows = data.rows();

  if(cols == 1)
    {
    str << "[ ";
    for (xsize r = 0; r < rows; ++r)
      {
      str << data(r, 0);
      if((r < rows-1)) // while not last element
        {
        str << ", "; // separate each element with space
        }
      }
    str << " ]";
    }
  else
    {
    for (xsize r = 0; r < rows; ++r)
      {
      str << "[ ";
      for(xsize c = 0; c < cols; ++c)
        {
        str << data(r, c);
        if((c < cols-1)) // while not last element
          {
          str << ", "; // separate each element with space
          }
        }
      str << " ]";
      if(r < (rows-1))
        {
        str << "\n";
        }
      }
    }
  return str;
  }

#if X_QT_INTEROP

Q_DECLARE_METATYPE(Eks::Matrix4x4);
Q_DECLARE_METATYPE(Eks::Matrix3x3);

#endif


#endif
