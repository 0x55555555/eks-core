#include "XChildEvent"

XChildEvent::XChildEvent( Type m, XObject *o ) : _type( m ), _object( o )
    {
    }

XChildEvent::Type XChildEvent::type() const
    {
    return _type;
    }

XObject *XChildEvent::object() const
    {
    return _object;
    }
