#include "XFrustrum.h"
#include "XTransform.h"
#include "XCuboid.h"

XFrustrum::XFrustrum()
  {
  }

XFrustrum::XFrustrum(const XTransform &)
  {
  }

bool XFrustrum::intersects(const XCuboid &) const
  {
  return true;
  }
