#include "XMetaType"
#include "XAssert"
#include "QString"
#include "QMap"
#include "QHash"

class XMetaTypeData
  {
public:
  XMetaType::Constructor ctor;
  XMetaType::Destructor dtor;
  XMetaType::Assigner assign;
  XMetaType::Compactor compact;
  XMetaType::Extractor extract;
  XMetaType::Convertor convert;
  QString type;
  size_t size;
  XMetaType::Type id;
  };

QMap <XMetaType::Type, XMetaTypeData> _types;

XMetaType::XMetaType()
  {
  }

void *XMetaType::construct(Type t)
  {
  XAssert ass(X_CURRENT_CODE_LOCATION, "", "message");

  xAssert(_types.contains(t));
  return _types[t].ctor(0,0);
  }

void *XMetaType::copy(Type t, const void *cpy)
  {
  xAssert(_types.contains(t));
  return _types[t].ctor(cpy, 0);
  }

void *XMetaType::copy(Type t, const void *cpy, void *dst)
  {
  xAssert(_types.contains(t));
  return _types[t].ctor(cpy, dst);
  }

void XMetaType::assign(Type t, void *dst, const void *ptr)
  {
  xAssert(_types.contains(t));
  _types[t].assign(dst, ptr);
  }

void XMetaType::destroy(Type t, void *del, bool shouldDel)
  {
  xAssert(_types.contains(t));
  return _types[t].dtor(del, shouldDel);
  }

void *XMetaType::convert(Type t, const void *ptr, Type dstType)
  {
  xAssert(_types.contains(t));
  xAssert(_types[t].convert);
  void *output = 0;
  _types[t].convert(ptr, dstType, &output);
  return output;
  }

void XMetaType::convert(Type t, const void *ptr, Type dstType, void *output)
  {
  xAssert(_types.contains(t));
  xAssert(_types[t].convert);
  _types[t].convert( ptr, dstType, &output );
  }
void XMetaType::compact(QDataStream &str, Type t, const void *data)
  {
  xAssert(_types.contains(t));
  xAssert(_types[t].compact);
  return _types[t].compact(str, data);
  }

void XMetaType::extract(QDataStream &str, Type t, void *data)
  {
  xAssert(_types.contains(t));
  xAssert(_types[t].extract);
  return _types[t].extract(str, data);
  }

XMetaType::Type XMetaType::typeOf(const char *name)
  {
  return qHash(name);
  }

size_t XMetaType::sizeOf(Type t)
  {
  return _types[t].size;
  }

size_t XMetaType::sizeOf(const char *name)
  {
  return _types[qHash(name)].size;
  }

XMetaType::Type XMetaType::addType(const char *name,
                                   size_t size,
                                   Constructor ctor,
                                   Destructor dtor,
                                   Assigner ass,
                                   Compactor compact,
                                   Extractor extract,
                                   Convertor convert)
  {
  Type type = qHash(QString(name));

  if(!_types.contains(type))
    {
    XMetaTypeData data;
    data.ctor = ctor;
    data.dtor = dtor;
    data.assign = ass;
    data.compact = compact;
    data.extract = extract;
    data.convert = convert;
    data.type = name;
    data.size = size;
    data.id = type;

    _types[data.id] = data;

    xAssert(_types.contains(type));
    }

  return type;
  }

void xMetaConvert(const QString &str, XMetaType::Type outType, void *out)
  {
  if(outType == XMetaType::typeOf<xint32>())
    {
    XMetaType::set<xint32>(out, str.toInt());
    }
  else if(outType == XMetaType::typeOf<float>())
    {
    XMetaType::set<float>(out, str.toFloat());
    }
  else if(outType == XMetaType::typeOf<double>())
    {
    XMetaType::set<double>(out, str.toDouble());
    }
  }

#define TRY_TYPE(input, type, out, dst) if(XMetaType::typeOf<type>() == out) { XMetaType::set<type>(dst, input); return; }

void xMetaConvert(bool input, XMetaType::Type out, void *dst)
  {
  TRY_TYPE(input, xint32, out, dst);
  TRY_TYPE(input, xuint32, out, dst);
  TRY_TYPE(input, xint64, out, dst);
  TRY_TYPE(input, xuint64, out, dst);
  TRY_TYPE(input, float, out, dst);
  TRY_TYPE(input, double, out, dst);
  }

void xMetaConvert(xint32 input, XMetaType::Type out, void *dst)
  {
  TRY_TYPE(input, bool, out, dst);
  TRY_TYPE(input, xuint32, out, dst);
  TRY_TYPE(input, xint64, out, dst);
  TRY_TYPE(input, xuint64, out, dst);
  TRY_TYPE(input, float, out, dst);
  TRY_TYPE(input, double, out, dst);
  }

void xMetaConvert(xuint32 input, XMetaType::Type out, void *dst)
  {
  TRY_TYPE(input, bool, out, dst);
  TRY_TYPE(input, xuint32, out, dst);
  TRY_TYPE(input, xint64, out, dst);
  TRY_TYPE(input, xuint64, out, dst);
  TRY_TYPE(input, float, out, dst);
  TRY_TYPE(input, double, out, dst);
  }

void xMetaConvert(xint64 input, XMetaType::Type out, void *dst)
  {
  TRY_TYPE(input, bool, out, dst);
  TRY_TYPE(input, xint32, out, dst);
  TRY_TYPE(input, xuint32, out, dst);
  TRY_TYPE(input, xuint64, out, dst);
  TRY_TYPE(input, float, out, dst);
  TRY_TYPE(input, double, out, dst);
  }

void xMetaConvert(xuint64 input, XMetaType::Type out, void *dst)
  {
  TRY_TYPE(input, bool, out, dst);
  TRY_TYPE(input, xint32, out, dst);
  TRY_TYPE(input, xuint32, out, dst);
  TRY_TYPE(input, xint64, out, dst);
  TRY_TYPE(input, float, out, dst);
  TRY_TYPE(input, double, out, dst);
  }

void xMetaConvert(float input, XMetaType::Type out, void *dst)
  {
  TRY_TYPE(input, bool, out, dst);
  TRY_TYPE(input, xint32, out, dst);
  TRY_TYPE(input, xuint32, out, dst);
  TRY_TYPE(input, xint64, out, dst);
  TRY_TYPE(input, xuint64, out, dst);
  TRY_TYPE(input, double, out, dst);
  }

void xMetaConvert(double input, XMetaType::Type out, void *dst)
  {
  TRY_TYPE(input, bool, out, dst);
  TRY_TYPE(input, xint32, out, dst);
  TRY_TYPE(input, xuint32, out, dst);
  TRY_TYPE(input, xint64, out, dst);
  TRY_TYPE(input, xuint64, out, dst);
  TRY_TYPE(input, float, out, dst);
  }
