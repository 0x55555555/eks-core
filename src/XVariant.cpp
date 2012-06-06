#if 0

#include "XVariant"
#include "QDataStream"
#include "XColour"

void xInitiateMetaTypes()
  {
  XMetaType::typeOf<XVariantList>();
  XMetaType::typeOf<XVariantVector>();
  XMetaType::typeOf<XVariantMap>();

  XMetaType::typeOf<QString>();
  XMetaType::typeOf<bool>();
  XMetaType::typeOf<xint32>();
  XMetaType::typeOf<xuint32>();
  XMetaType::typeOf<xint64>();
  XMetaType::typeOf<xuint64>();
  XMetaType::typeOf<float>();
  XMetaType::typeOf<double>();

  XMetaType::typeOf<XVector2D>();
  XMetaType::typeOf<XVector3D>();
  XMetaType::typeOf<XVector4D>();
  XMetaType::typeOf<XQuaternion>();
  XMetaType::typeOf<XColour>();
  }

XVariant::XVariant()
  : _d( new XVariantData )
  {
  XVariantData *d = _d.ptr();
  d->type = XMetaType::Null;
  }

XVariant::XVariant(const XVariant &val)
  : _d( new XVariantData )
  {
  XVariantData *d = _d.ptr();
  d->type = XMetaType::Null;

  const XVariantData *otherD = val._d.constPtr();
  if(otherD->type != XMetaType::Null)
    {
    if(XMetaType::sizeOf(otherD->type) < XVariantData::BufferSize)
      {
      initiate(otherD->type, otherD->buffer);
      }
    else
      {
      initiate(otherD->type, otherD->data);
      }
    }
  }

XVariant::~XVariant()
  {
  clear();
  }

bool XVariant::isNull()
  {
  return _d.constPtr()->type == XMetaType::Null;
  }

XMetaType::Type XVariant::type() const
  {
  return _d.constPtr()->type;
  }

bool XVariant::is( XMetaType::Type t ) const
  {
  const XVariantData *d = _d.constPtr();
  return t == d->type;
  }

void XVariant::initiate( XMetaType::Type type, const void *ptr )
  {
  xAssert(type != XMetaType::Null);
  XVariantData *d = _d.ptr();
  if( d->type == type )
    {
    if(XMetaType::sizeOf(type) < XVariantData::BufferSize)
      {
      XMetaType::assign( type, d->buffer, ptr );
      }
    else
      {
      XMetaType::assign( type, d->data, ptr );
      }
    }
  else
    {
    clear();
    if(XMetaType::sizeOf(type) < XVariantData::BufferSize)
      {
      XMetaType::copy( type, ptr, d->buffer );
      }
    else
      {
      d->data = XMetaType::copy( type, ptr );
      }
    d->type = type;
    }
  }

void XVariant::clear()
  {
  XVariantData *d = _d.ptr();
  if( d->type != XMetaType::Null )
    {
    if(XMetaType::sizeOf( d->type ) < XVariantData::BufferSize)
      {
      XMetaType::destroy( d->type, d->buffer, false );
      }
    else
      {
      XMetaType::destroy( d->type, d->data );
      }
    d->type = XMetaType::Null;
    }
  }

QDataStream &operator>>(QDataStream &str, XVariant &var)
  {
  XVariant::XVariantData *d = var._d.ptr();
  XMetaType::Type t;
  str >> t;
  if(t != XMetaType::Null)
    {
    if(XMetaType::sizeOf(t) < XVariant::XVariantData::BufferSize)
      {
      var.initiate(t, 0);
      XMetaType::extract( str, t, d->buffer );
      }
    else
      {
      var.initiate(t, 0);
      XMetaType::extract( str, t, d->data );
      }
    d->type = t;
    }
  else
    {
    var.clear();
    }
  return str;
  }

QDataStream &operator<<(QDataStream &str, const XVariant &var)
  {
  const XVariant::XVariantData *d = var._d.ptr();
  str << d->type;
  if(d->type != XMetaType::Null)
    {
    if(XMetaType::sizeOf(d->type) < XVariant::XVariantData::BufferSize)
      {
      XMetaType::compact( str, d->type, d->buffer );
      }
    else
      {
      XMetaType::compact( str, d->type, d->data );
      }
    }
  return str;
  }

#endif
